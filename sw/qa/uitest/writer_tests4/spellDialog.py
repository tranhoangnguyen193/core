#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
#

import re
import org.libreoffice.unotest
import pathlib

from uitest.framework import UITestCase
from uitest.uihelper.common import get_state_as_dict

from libreoffice.linguistic.linguservice import get_spellchecker
from com.sun.star.lang import Locale

def get_url_for_data_file(file_name):
    return pathlib.Path(org.libreoffice.unotest.makeCopyFromTDOC(file_name)).as_uri()

class SpellingAndGrammarDialog(UITestCase):

    def is_supported_locale(self, language, country):
        xSpellChecker = get_spellchecker(self.ui_test._xContext)
        locales = xSpellChecker.getLocales()
        for locale in locales:
            if language != None:
                if locale.Language != language:
                    continue

            if country != None:
                if locale.Country != country:
                    continue

            # we found the correct combination
            return True

    def launch_dialog(self):
        self.ui_test.execute_modeless_dialog_through_command(
            ".uno:SpellingAndGrammarDialog")

        return self.xUITest.getTopFocusWindow()

    TDF46852_INPUT = """\
dogg
dogg
catt dogg
frogg frogg
frogg catt dogg
dogg catt
frog, dogg, catt"""

    TDF46852_REGEX = """\
([a-z]+)
\\1
([a-z]+) \\1
([a-z]+) \\3
\\3 \\2 \\1
\\1 \\2
\\3, \\1, \\2"""

    def test_tdf46852(self):
        supported_locale = self.is_supported_locale("en", "US")
        if not supported_locale:
            self.skipTest("no dictionary support for en_US available")
        # This automates the steps described in the bug report tdf#46852

        # Step 1: Create a document with repetitious misspelled words
        self.ui_test.create_doc_in_start_center("writer")
        document = self.ui_test.get_component()
        cursor = document.getCurrentController().getViewCursor()
        # Inserted text must be en_US, so make sure to set language in current location
        cursor.CharLocale = Locale("en", "US", "")
        input_text = self.TDF46852_INPUT.replace('\n', '\r') # \r = para break
        document.Text.insertString(cursor, input_text, False)

        # Step 2: Place cursor on 4th line after second "frogg"
        cursor.goUp(2, False)
        cursor.goLeft(1, False)

        # Step 3: Initiate spellchecking, and make sure "Check grammar" is
        # unchecked
        spell_dialog = self.launch_dialog()
        checkgrammar = spell_dialog.getChild('checkgrammar')
        if get_state_as_dict(checkgrammar)['Selected'] == 'true':
            checkgrammar.executeAction('CLICK', ())
        self.assertTrue(get_state_as_dict(checkgrammar)['Selected'] == 'false')

        # Step 4: Repetitively click on "Correct all" for each misspelling
        #         prompt until end of document is reached.
        changeall = spell_dialog.getChild('changeall')
        changeall.executeAction("CLICK", ())
        changeall.executeAction("CLICK", ())
        # The third time we click on changeall, the click action is going to
        # block while two message boxes are shown, so we need to do this third
        # click specially:
        self.ui_test.execute_blocking_action(
            changeall.executeAction, args=('CLICK', ()),
            # Step 5: Confirm to "Continue check at beginning of document"
            dialog_handler=lambda dialog :
                self.ui_test.execute_blocking_action(
                    dialog.getChild('yes').executeAction, 'ok', ('CLICK', ())
                )
            )

        output_text = document.Text.getString().replace('\r\n', '\n')
        self.assertTrue(re.match(self.TDF46852_REGEX, output_text))

    def test_tdf66043(self):
        writer_doc = self.ui_test.load_file(get_url_for_data_file("tdf66043.fodt"))
        document = self.ui_test.get_component()
        # Step 1: Initiate spellchecking, and make sure "Check grammar" is
        # unchecked
        spell_dialog = self.launch_dialog()
        checkgrammar = spell_dialog.getChild('checkgrammar')
        if get_state_as_dict(checkgrammar)['Selected'] == 'true':
            checkgrammar.executeAction('CLICK', ())
        self.assertTrue(get_state_as_dict(checkgrammar)['Selected'] == 'false')

        # Step 2: Click on "Correct all" for each misspelling
        #         prompt until end of document is reached.
        changeall = spell_dialog.getChild('changeall')
        changeall.executeAction("CLICK", ())

        xCloseBtn = spell_dialog.getChild("close")
        xCloseBtn.executeAction("CLICK", tuple())

        output_text = document.Text.getString().replace('\r\n', '\n')
        # This was "gooodgood baaad eeend" ("goood" is a deletion,
        # "good" is an insertion by fixing the first misspelling),
        # but now "goood" is not a misspelling because it is accepted
        # correctly without the redline containing a deleted "o"
        self.assertEqual(output_text, 'goood baaadbaaed eeend')

