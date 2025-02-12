/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <sal/config.h>

#include <memory>
#include <string_view>

#include <unotest/filters-test.hxx>
#include <test/bootstrapfixture.hxx>
#include <osl/file.hxx>
#include <config_features.h>

#include <vcl/svapp.hxx>
#include <sfx2/docfilt.hxx>
#include <sfx2/docfile.hxx>
#include <svl/stritem.hxx>
#include <svl/zformat.hxx>
#include <svx/svdograf.hxx>
#include <svx/svxids.hrc>

#include <drwlayer.hxx>
#include <svx/svdpage.hxx>
#include <svx/svdoole2.hxx>
#include <editeng/eeitem.hxx>
#include <editeng/wghtitem.hxx>
#include <editeng/postitem.hxx>
#include <editeng/crossedoutitem.hxx>
#include <editeng/udlnitem.hxx>
#include <editeng/editobj.hxx>
#include <editeng/borderline.hxx>
#include <editeng/boxitem.hxx>
#include <editeng/fhgtitem.hxx>
#include <editeng/brushitem.hxx>
#include <editeng/fontitem.hxx>
#include <editeng/flditem.hxx>
#include <editeng/justifyitem.hxx>
#include <editeng/lineitem.hxx>
#include <editeng/colritem.hxx>
#include <dbdata.hxx>
#include <validat.hxx>
#include <formulacell.hxx>
#include <formulaopt.hxx>
#include <userdat.hxx>
#include <stlsheet.hxx>
#include <docfunc.hxx>
#include <markdata.hxx>
#include <colorscale.hxx>
#include <olinetab.hxx>
#include <patattr.hxx>
#include <scitems.hxx>
#include <docsh.hxx>
#include <editutil.hxx>
#include <cellvalue.hxx>
#include <attrib.hxx>
#include <fillinfo.hxx>
#include <scopetools.hxx>
#include <columnspanset.hxx>
#include <tokenstringcontext.hxx>
#include <formula/errorcodes.hxx>
#include <externalrefmgr.hxx>
#include <stlpool.hxx>
#include <hints.hxx>
#include <detfunc.hxx>
#include <cellmergeoption.hxx>
#include <undoblk.hxx>

#include <orcusfilters.hxx>
#include <filter.hxx>

#include <com/sun/star/drawing/XDrawPageSupplier.hpp>
#include <com/sun/star/drawing/XControlShape.hpp>
#include <com/sun/star/sheet/XSpreadsheetDocument.hpp>
#include <com/sun/star/container/XIndexAccess.hpp>
#include <com/sun/star/text/textfield/Type.hpp>

#include <comphelper/scopeguard.hxx>
#include <unotools/syslocaleoptions.hxx>
#include "helper/qahelper.hxx"
#include "helper/shared_test_impl.hxx"

namespace com::sun::star::frame { class XModel; }

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;

/* Implementation of Filters test */

class ScFiltersTest
    : public test::FiltersTest
    , public ScBootstrapFixture
{
public:
    ScFiltersTest();

    virtual bool load( const OUString &rFilter, const OUString &rURL,
        const OUString &rUserData, SfxFilterFlags nFilterFlags,
        SotClipboardFormatId nClipboardID, unsigned int nFilterVersion) override;

    virtual void setUp() override;
    virtual void tearDown() override;

    //ods, xls, xlsx filter tests
    void testCondFormatBeginsAndEndsWithXLSX();
    void testExtCondFormatXLSX();
    void testUpdateCircleInMergedCellODS();
    void testDeleteCircleInMergedCellODS();
    void testBooleanFormatXLSX();
    void testBasicCellContentODS();
    void testRangeNameXLS();
    void testRangeNameLocalXLS();
    void testRangeNameXLSX();
    void testHyperlinksXLSX();
    void testHardRecalcODS();
    void testFunctionsODS();
    void testFunctionsExcel2010();
    void testCeilingFloorXLSX();
    void testCachedFormulaResultsODS();
    void testCachedMatrixFormulaResultsODS();
    void testFormulaDepAcrossSheetsODS();
    void testFormulaDepDeleteContentsODS();
    void testDatabaseRangesODS();
    void testDatabaseRangesXLS();
    void testDatabaseRangesXLSX();
    void testFormatsODS();
    // void testFormatsXLS();
    // void testFormatsXLSX();
    void testMatrixODS();
    void testMatrixXLS();
    void testDoubleThinBorder();
    void testBorderODS();
    void testBordersOoo33();
    void testBugFixesODS();
    void testBugFixesXLS();
    void testBugFixesXLSX();
    void testBrokenQuotesCSV();
    void testMergedCellsODS();
    void testRepeatedColumnsODS();
    void testDataValidityODS();
    void testDataValidityXLSX();
    void testDataTableMortgageXLS();
    void testDataTableOneVarXLSX();
    void testDataTableMultiTableXLSX();

    void testDataBarODS();
    void testDataBarXLSX();
    void testColorScaleODS();
    void testColorScaleXLSX();
    void testNewCondFormatODS();
    void testNewCondFormatXLSX();
    void testCondFormatThemeColorXLSX();
    void testCondFormatImportCellIs();
    void testCondFormatThemeColor2XLSX(); // negative bar color and axis color
    void testCondFormatThemeColor3XLSX(); // theme index 2 and 3 are switched
    void testComplexIconSetsXLSX();
    void testCondFormatParentXLSX();
    void testColorScaleNumWithRefXLSX();
    void testCondFormatXLSB();

    void testOrcusODSStyleInterface();

    void testLiteralInFormulaXLS();

    //change this test file only in excel and not in calc
    void testCellValueXLSX();

    /**
     * Test importing of xlsx document that previously had its row index off
     * by one. (fdo#76032)
     */
    void testRowIndex1BasedXLSX();
    void testErrorOnExternalReferences();

    //misc tests unrelated to the import filters
    void testPasswordNew();
    void testPasswordOld();
    void testPasswordWrongSHA();

    //test shape import
    void testControlImport();
    void testActiveXOptionButtonGroup();
    void testChartImportODS();
#if HAVE_MORE_FONTS
    void testChartImportXLS();
#endif

    void testNumberFormatHTML();
    void testNumberFormatCSV();

    void testCellAnchoredShapesODS();
    void testCellAnchoredHiddenShapesXLSX();

    void testFormulaDependency();

    void testRowHeightODS();
    void testRichTextContentODS();
    void testMiscRowHeights();
    void testOptimalHeightReset();
    void testCustomNumFormatHybridCellODS();
    void testTdf121040();
    void testTdf118624();
    void testTdf124454();

    void testPrintRangeODS();
    void testOutlineODS();

    void testColumnStyleXLSX();
    void testColumnStyleAutoFilterXLSX();

    void testSharedFormulaHorizontalXLS();
    void testSharedFormulaWrappedRefsXLS();
    void testSharedFormulaBIFF5();
    void testSharedFormulaXLSB();
    void testSharedFormulaXLS();
    void testSharedFormulaColumnLabelsODS();
    void testSharedFormulaColumnRowLabelsODS();
    void testExternalRefCacheXLSX();
    void testExternalRefCacheODS();
    void testHybridSharedStringODS();
    void testCopyMergedNumberFormats();
    void testVBAUserFunctionXLSM();
    void testEmbeddedImageXLS();
    void testTdf44076();
    void testEditEngStrikeThroughXLSX();
    void testRefStringXLSX();
    void testHiddenSheetsXLSX();
    void testRelFormulaValidationXLS();
    void testTdf130132();
    void testTdf133327();
    void testColumnStyle2XLSX();
    void testAutofilterXLSX();

    void testBnc762542();

    void testTdf136364();
    void testTdf103734();
    void testTdf98844();
    void testTdf100458();
    void testTdf118561();
    void testTdf125099();
    void testTdf134455();
    void testTdf119533();
    void testTdf127982();
    void testTdf109409();
    void testTdf132105();
    void testTdf131424();
    void testTdf100709XLSX();
    void testTdf97598XLSX();
    void testTdf110440XLSX();
    void testTdf132278();
    void testTdf130959();
    void testTdf129410();
    void testTdf138507();
    void testTdf131380();
    void testTdf129681();
    void testTdf111974XLSM();
    void testTdf83672XLSX();

    void testPageScalingXLSX();
    void testActiveXCheckboxXLSX();
    void testtdf120301_xmlSpaceParsingXLSX();
#ifdef UNX
    void testUnicodeFileNameGnumeric();
#endif
    void testCondFormatFormulaListenerXLSX();

    void testMergedCellsXLSXML();
    void testBackgroundColorStandardXLSXML();
    void testTdf131536();
    void testTdf130583();
    void testTdf85617();
    void testTdf134234();
    void testNamedExpressionsXLSXML();
    void testEmptyRowsXLSXML();
    void testBorderDirectionsXLSXML();
    void testBorderColorsXLSXML();
    void testHiddenRowsColumnsXLSXML();
    void testColumnWidthRowHeightXLSXML();
    void testCharacterSetXLSXML();
    void testTdf137091();
    void testTdf62268();
    void testTdf137453();
    void testTdf35636();
    void testVBAMacroFunctionODS();
    void testAutoheight2Rows();
    void testXLSDefColWidth();
    void testPreviewMissingObjLink();
    void testShapeRotationImport();
    void testShapeDisplacementOnRotationImport();
    void testTextBoxBodyUpright();
    void testTextLengthDataValidityXLSX();
    void testDeleteCircles();
    void testDrawCircleInMergeCells();
    void testDeleteCirclesInRowAndCol();

    CPPUNIT_TEST_SUITE(ScFiltersTest);
    CPPUNIT_TEST(testCondFormatBeginsAndEndsWithXLSX);
    CPPUNIT_TEST(testExtCondFormatXLSX);
    CPPUNIT_TEST(testUpdateCircleInMergedCellODS);
    CPPUNIT_TEST(testDeleteCircleInMergedCellODS);
    CPPUNIT_TEST(testBooleanFormatXLSX);
    CPPUNIT_TEST(testBasicCellContentODS);
    CPPUNIT_TEST(testRangeNameXLS);
    CPPUNIT_TEST(testRangeNameLocalXLS);
    CPPUNIT_TEST(testRangeNameXLSX);
    CPPUNIT_TEST(testHyperlinksXLSX);
    CPPUNIT_TEST(testHardRecalcODS);
    CPPUNIT_TEST(testFunctionsODS);
    CPPUNIT_TEST(testFunctionsExcel2010);
    CPPUNIT_TEST(testCeilingFloorXLSX);
    CPPUNIT_TEST(testCachedFormulaResultsODS);
    CPPUNIT_TEST(testFormulaDepAcrossSheetsODS);
    CPPUNIT_TEST(testFormulaDepDeleteContentsODS);
    CPPUNIT_TEST(testCachedMatrixFormulaResultsODS);
    CPPUNIT_TEST(testDatabaseRangesODS);
    CPPUNIT_TEST(testDatabaseRangesXLS);
    CPPUNIT_TEST(testDatabaseRangesXLSX);
    CPPUNIT_TEST(testFormatsODS);
//  CPPUNIT_TEST(testFormatsXLS); TODO: Fix this
//  CPPUNIT_TEST(testFormatsXLSX); TODO: Fix this
    CPPUNIT_TEST(testMatrixODS);
    CPPUNIT_TEST(testMatrixXLS);
    CPPUNIT_TEST(testDoubleThinBorder);
    CPPUNIT_TEST(testBorderODS);
    CPPUNIT_TEST(testBordersOoo33);
    CPPUNIT_TEST(testBugFixesODS);
    CPPUNIT_TEST(testBugFixesXLS);
    CPPUNIT_TEST(testBugFixesXLSX);
    CPPUNIT_TEST(testMergedCellsODS);
    CPPUNIT_TEST(testRepeatedColumnsODS);
    CPPUNIT_TEST(testDataValidityODS);
    CPPUNIT_TEST(testDataValidityXLSX);
    CPPUNIT_TEST(testDataTableMortgageXLS);
    CPPUNIT_TEST(testDataTableOneVarXLSX);
    CPPUNIT_TEST(testDataTableMultiTableXLSX);
    CPPUNIT_TEST(testBrokenQuotesCSV);
    CPPUNIT_TEST(testCellValueXLSX);
    CPPUNIT_TEST(testRowIndex1BasedXLSX);
    CPPUNIT_TEST(testControlImport);
    CPPUNIT_TEST(testActiveXOptionButtonGroup);
    CPPUNIT_TEST(testChartImportODS);
#if HAVE_MORE_FONTS
    CPPUNIT_TEST(testChartImportXLS);
#endif

    CPPUNIT_TEST(testDataBarODS);
    CPPUNIT_TEST(testDataBarXLSX);
    CPPUNIT_TEST(testColorScaleODS);
    CPPUNIT_TEST(testColorScaleXLSX);
    CPPUNIT_TEST(testNewCondFormatODS);
    CPPUNIT_TEST(testNewCondFormatXLSX);
    CPPUNIT_TEST(testCondFormatThemeColorXLSX);
    CPPUNIT_TEST(testCondFormatImportCellIs);
    CPPUNIT_TEST(testCondFormatThemeColor2XLSX);
    CPPUNIT_TEST(testCondFormatThemeColor3XLSX);
    CPPUNIT_TEST(testComplexIconSetsXLSX);
    CPPUNIT_TEST(testCondFormatParentXLSX);
    CPPUNIT_TEST(testColorScaleNumWithRefXLSX);
    CPPUNIT_TEST(testCondFormatXLSB);

    CPPUNIT_TEST(testOrcusODSStyleInterface);

    CPPUNIT_TEST(testLiteralInFormulaXLS);

    CPPUNIT_TEST(testNumberFormatHTML);
    CPPUNIT_TEST(testNumberFormatCSV);

    CPPUNIT_TEST(testCellAnchoredShapesODS);
    CPPUNIT_TEST(testCellAnchoredHiddenShapesXLSX);

    CPPUNIT_TEST(testRowHeightODS);
    CPPUNIT_TEST(testFormulaDependency);
    CPPUNIT_TEST(testRichTextContentODS);

    //disable testPassword on MacOSX due to problems with libsqlite3
    //also crashes on DragonFly due to problems with nss/nspr headers
    CPPUNIT_TEST(testPasswordWrongSHA);
    CPPUNIT_TEST(testPasswordOld);
    CPPUNIT_TEST(testPasswordNew);

    CPPUNIT_TEST(testMiscRowHeights);
    CPPUNIT_TEST(testOptimalHeightReset);
    CPPUNIT_TEST(testCustomNumFormatHybridCellODS);
    CPPUNIT_TEST(testTdf121040);
    CPPUNIT_TEST(testTdf118624);
    CPPUNIT_TEST(testTdf124454);
    CPPUNIT_TEST(testPrintRangeODS);
    CPPUNIT_TEST(testOutlineODS);
    CPPUNIT_TEST(testColumnStyleXLSX);
    CPPUNIT_TEST(testColumnStyleAutoFilterXLSX);
    CPPUNIT_TEST(testSharedFormulaHorizontalXLS);
    CPPUNIT_TEST(testSharedFormulaWrappedRefsXLS);
    CPPUNIT_TEST(testSharedFormulaBIFF5);
    CPPUNIT_TEST(testSharedFormulaXLSB);
    CPPUNIT_TEST(testSharedFormulaXLS);
    CPPUNIT_TEST(testSharedFormulaColumnLabelsODS);
    CPPUNIT_TEST(testSharedFormulaColumnRowLabelsODS);
    CPPUNIT_TEST(testExternalRefCacheXLSX);
    CPPUNIT_TEST(testExternalRefCacheODS);
    CPPUNIT_TEST(testHybridSharedStringODS);
    CPPUNIT_TEST(testCopyMergedNumberFormats);
    CPPUNIT_TEST(testVBAUserFunctionXLSM);
    CPPUNIT_TEST(testEmbeddedImageXLS);
    CPPUNIT_TEST(testErrorOnExternalReferences);
    CPPUNIT_TEST(testTdf44076);
    CPPUNIT_TEST(testEditEngStrikeThroughXLSX);
    CPPUNIT_TEST(testRefStringXLSX);
    CPPUNIT_TEST(testRelFormulaValidationXLS);
    CPPUNIT_TEST(testTdf130132);
    CPPUNIT_TEST(testTdf133327);
    CPPUNIT_TEST(testColumnStyle2XLSX);
    CPPUNIT_TEST(testAutofilterXLSX);

    CPPUNIT_TEST(testBnc762542);

    CPPUNIT_TEST(testHiddenSheetsXLSX);

    CPPUNIT_TEST(testTdf136364);
    CPPUNIT_TEST(testTdf103734);
    CPPUNIT_TEST(testTdf98844);
    CPPUNIT_TEST(testTdf100458);
    CPPUNIT_TEST(testTdf118561);
    CPPUNIT_TEST(testTdf125099);
    CPPUNIT_TEST(testTdf134455);
    CPPUNIT_TEST(testTdf119533);
    CPPUNIT_TEST(testTdf127982);
    CPPUNIT_TEST(testTdf109409);
    CPPUNIT_TEST(testTdf132105);
    CPPUNIT_TEST(testTdf131424);
    CPPUNIT_TEST(testTdf100709XLSX);
    CPPUNIT_TEST(testTdf97598XLSX);
    CPPUNIT_TEST(testTdf110440XLSX);
    CPPUNIT_TEST(testTdf132278);
    CPPUNIT_TEST(testTdf130959);
    CPPUNIT_TEST(testTdf129410);
    CPPUNIT_TEST(testTdf138507);
    CPPUNIT_TEST(testTdf131380);
    CPPUNIT_TEST(testTdf129681);
    CPPUNIT_TEST(testTdf111974XLSM);
    CPPUNIT_TEST(testTdf83672XLSX);

    CPPUNIT_TEST(testPageScalingXLSX);
    CPPUNIT_TEST(testActiveXCheckboxXLSX);
    CPPUNIT_TEST(testtdf120301_xmlSpaceParsingXLSX);
#ifdef UNX
    CPPUNIT_TEST(testUnicodeFileNameGnumeric);
#endif
    CPPUNIT_TEST(testMergedCellsXLSXML);
    CPPUNIT_TEST(testBackgroundColorStandardXLSXML);
    CPPUNIT_TEST(testTdf131536);
    CPPUNIT_TEST(testTdf130583);
    CPPUNIT_TEST(testTdf85617);
    CPPUNIT_TEST(testTdf134234);
    CPPUNIT_TEST(testNamedExpressionsXLSXML);
    CPPUNIT_TEST(testEmptyRowsXLSXML);
    CPPUNIT_TEST(testBorderDirectionsXLSXML);
    CPPUNIT_TEST(testBorderColorsXLSXML);
    CPPUNIT_TEST(testHiddenRowsColumnsXLSXML);
    CPPUNIT_TEST(testColumnWidthRowHeightXLSXML);
    CPPUNIT_TEST(testCharacterSetXLSXML);
    CPPUNIT_TEST(testCondFormatFormulaListenerXLSX);
    CPPUNIT_TEST(testTdf137091);
    CPPUNIT_TEST(testTdf62268);
    CPPUNIT_TEST(testTdf137453);
    CPPUNIT_TEST(testTdf35636);
    CPPUNIT_TEST(testVBAMacroFunctionODS);
    CPPUNIT_TEST(testAutoheight2Rows);
    CPPUNIT_TEST(testXLSDefColWidth);
    CPPUNIT_TEST(testPreviewMissingObjLink);
    CPPUNIT_TEST(testShapeRotationImport);
    CPPUNIT_TEST(testShapeDisplacementOnRotationImport);
    CPPUNIT_TEST(testTextBoxBodyUpright);
    CPPUNIT_TEST(testTextLengthDataValidityXLSX);
    CPPUNIT_TEST(testDeleteCircles);
    CPPUNIT_TEST(testDrawCircleInMergeCells);
    CPPUNIT_TEST(testDeleteCirclesInRowAndCol);

    CPPUNIT_TEST_SUITE_END();

private:
    void testPassword_Impl(std::u16string_view rFileNameBase);

    uno::Reference<uno::XInterface> m_xCalcComponent;
};

bool ScFiltersTest::load(const OUString &rFilter, const OUString &rURL,
    const OUString &rUserData, SfxFilterFlags nFilterFlags,
        SotClipboardFormatId nClipboardID, unsigned int nFilterVersion)
{
    ScDocShellRef xDocShRef = ScBootstrapFixture::load( rURL, rFilter, rUserData,
        OUString(), nFilterFlags, nClipboardID, nFilterVersion);
    bool bLoaded = xDocShRef.is();
    //reference counting of ScDocShellRef is very confused.
    if (bLoaded)
        xDocShRef->DoClose();
    return bLoaded;
}

namespace {

void testRangeNameImpl(const ScDocument& rDoc)
{
    //check one range data per sheet and one global more detailed
    //add some more checks here
    ScRangeData* pRangeData = rDoc.GetRangeName()->findByUpperName(OUString("GLOBAL1"));
    CPPUNIT_ASSERT_MESSAGE("range name Global1 not found", pRangeData);
    double aValue;
    rDoc.GetValue(1,0,0,aValue);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("range name Global1 should reference Sheet1.A1", 1.0, aValue);
    pRangeData = rDoc.GetRangeName(0)->findByUpperName(OUString("LOCAL1"));
    CPPUNIT_ASSERT_MESSAGE("range name Sheet1.Local1 not found", pRangeData);
    rDoc.GetValue(1,2,0,aValue);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("range name Sheet1.Local1 should reference Sheet1.A3", 3.0, aValue);
    pRangeData = rDoc.GetRangeName(1)->findByUpperName(OUString("LOCAL2"));
    CPPUNIT_ASSERT_MESSAGE("range name Sheet2.Local2 not found", pRangeData);
    rDoc.GetValue(1,1,1,aValue);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("range name Sheet2.Local2 should reference Sheet2.A2", 7.0, aValue);
    //check for correct results for the remaining formulas
    rDoc.GetValue(1,1,0, aValue);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("=global2 should be 2", 2.0, aValue);
    rDoc.GetValue(1,3,0, aValue);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("=local2 should be 4", 4.0, aValue);
    rDoc.GetValue(2,0,0, aValue);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("=SUM(global3) should be 10", 10.0, aValue);
    rDoc.GetValue(1,0,1,aValue);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("range name Sheet2.local1 should reference Sheet1.A5", 5.0, aValue);
    // Test if Global5 ( which depends on Global6 ) is evaluated
    rDoc.GetValue(0,5,1, aValue);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("formula Global5 should reference Global6 ( which is evaluated as local1 )", 5.0, aValue);
}

}

void ScFiltersTest::testCondFormatBeginsAndEndsWithXLSX()
{
    ScDocShellRef xDocSh = loadDoc(u"tdf120749.", FORMAT_XLSX);
    CPPUNIT_ASSERT_MESSAGE("Failed to load tdf120749.xlsx", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();

    // begins with and ends with conditions
    ScConditionalFormat* pFormatA1 = rDoc.GetCondFormat(0, 0, 0);
    CPPUNIT_ASSERT(pFormatA1);
    ScConditionalFormat* pFormatA2 = rDoc.GetCondFormat(0, 1, 0);
    CPPUNIT_ASSERT(pFormatA2);
    ScConditionalFormat* pFormatA3 = rDoc.GetCondFormat(0, 2, 0);
    CPPUNIT_ASSERT(pFormatA3);
    ScConditionalFormat* pFormatA4 = rDoc.GetCondFormat(0, 3, 0);
    CPPUNIT_ASSERT(pFormatA4);

    ScRefCellValue aCellA1(rDoc, ScAddress(0, 0, 0));
    OUString aCellStyleA1 = pFormatA1->GetCellStyle(aCellA1, ScAddress(0, 0, 0));
    CPPUNIT_ASSERT(!aCellStyleA1.isEmpty());

    ScRefCellValue aCellA2(rDoc, ScAddress(0, 1, 0));
    OUString aCellStyleA2 = pFormatA2->GetCellStyle(aCellA2, ScAddress(0, 1, 0));
    CPPUNIT_ASSERT(!aCellStyleA2.isEmpty());

    ScRefCellValue aCellA3(rDoc, ScAddress(0, 2, 0));
    OUString aCellStyleA3 = pFormatA3->GetCellStyle(aCellA3, ScAddress(0, 2, 0));
    CPPUNIT_ASSERT(!aCellStyleA3.isEmpty());

    ScRefCellValue aCellA4(rDoc, ScAddress(0, 3, 0));
    OUString aCellStyleA4 = pFormatA4->GetCellStyle(aCellA4, ScAddress(0, 3, 0));
    CPPUNIT_ASSERT(!aCellStyleA4.isEmpty());

    xDocSh->DoClose();
}

void ScFiltersTest::testExtCondFormatXLSX()
{
    ScDocShellRef xDocSh = loadDoc(u"tdf122102.", FORMAT_XLSX);
    CPPUNIT_ASSERT_MESSAGE("Failed to load tdf122102.xlsx", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();

    // contains text and not contains text conditions
    ScConditionalFormat* pFormatA1 = rDoc.GetCondFormat(0, 0, 0);
    CPPUNIT_ASSERT(pFormatA1);
    ScConditionalFormat* pFormatA2 = rDoc.GetCondFormat(0, 1, 0);
    CPPUNIT_ASSERT(pFormatA2);
    ScConditionalFormat* pFormatA3 = rDoc.GetCondFormat(0, 2, 0);
    CPPUNIT_ASSERT(pFormatA3);
    ScConditionalFormat* pFormatA4 = rDoc.GetCondFormat(0, 3, 0);
    CPPUNIT_ASSERT(pFormatA4);

    ScRefCellValue aCellA1(rDoc, ScAddress(0, 0, 0));
    OUString aCellStyleA1 = pFormatA1->GetCellStyle(aCellA1, ScAddress(0, 0, 0));
    CPPUNIT_ASSERT(!aCellStyleA1.isEmpty());

    ScRefCellValue aCellA2(rDoc, ScAddress(0, 1, 0));
    OUString aCellStyleA2 = pFormatA2->GetCellStyle(aCellA2, ScAddress(0, 1, 0));
    CPPUNIT_ASSERT(!aCellStyleA2.isEmpty());

    ScRefCellValue aCellA3(rDoc, ScAddress(0, 2, 0));
    OUString aCellStyleA3 = pFormatA3->GetCellStyle(aCellA3, ScAddress(0, 2, 0));
    CPPUNIT_ASSERT(!aCellStyleA3.isEmpty());

    ScRefCellValue aCellA4(rDoc, ScAddress(0, 3, 0));
    OUString aCellStyleA4 = pFormatA4->GetCellStyle(aCellA4, ScAddress(0, 3, 0));
    CPPUNIT_ASSERT(!aCellStyleA4.isEmpty());

    xDocSh->DoClose();
}

void ScFiltersTest::testUpdateCircleInMergedCellODS()
{
    ScDocShellRef xDocSh = loadDoc(u"updateCircleInMergedCell.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to load updateCircleInMergedCell.ods", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();
    rDoc.EnableChangeReadOnly(true);

    ScDrawLayer* pDrawLayer = rDoc.GetDrawLayer();
    SdrPage* pPage = pDrawLayer->GetPage(0);
    CPPUNIT_ASSERT_MESSAGE("draw page for sheet 1 should exist.", pPage);

    // There should be four circle objects!
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), pPage->GetObjCount());

    ScCellMergeOption aCellMergeOption(0,0,1,1); // A1:B2
    aCellMergeOption.maTabs.insert(0);
    xDocSh->GetDocFunc().MergeCells(aCellMergeOption, false, true, true, false);

    // There should be a circle object!
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pPage->GetObjCount());

    xDocSh->GetDocFunc().UnmergeCells(aCellMergeOption, true, nullptr);

    // There should be four circle objects!
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), pPage->GetObjCount());

    xDocSh->DoClose();
}

void ScFiltersTest::testDeleteCircleInMergedCellODS()
{
    ScDocShellRef xDocSh = loadDoc(u"deleteCircleInMergedCell.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to load deleteCircleInMergedCell.ods", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();

    ScDrawLayer* pDrawLayer = rDoc.GetDrawLayer();
    SdrPage* pPage = pDrawLayer->GetPage(0);
    CPPUNIT_ASSERT_MESSAGE("draw page for sheet 1 should exist.", pPage);

    // There should be a circle object!
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pPage->GetObjCount());

    ScRefCellValue aMergedCell;
    ScAddress aPosMergedCell(0, 0, 0);
    aMergedCell.assign(rDoc, aPosMergedCell);

    // The value of merged cell change to 6.
    aMergedCell.mfValue = 6;

    // Check that the data is valid.(True if the value = 6)
    const ScValidationData* pData = rDoc.GetValidationEntry(1);
    bool bValidA1 = pData->IsDataValid(aMergedCell, aPosMergedCell);
    // if valid, delete circle.
    if (bValidA1)
        ScDetectiveFunc(rDoc, 0).DeleteCirclesAt(aPosMergedCell.Col(), aPosMergedCell.Row());

    // There should not be a circle object!
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), pPage->GetObjCount());

    xDocSh->DoClose();
}

void ScFiltersTest::testBasicCellContentODS()
{
    ScDocShellRef xDocSh = loadDoc(u"basic-cell-content.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to load basic-cell-content.ods", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();
    OUString aStr = rDoc.GetString(1, 1, 0); // B2
    CPPUNIT_ASSERT_EQUAL(OUString("LibreOffice Calc"), aStr);
    double fVal = rDoc.GetValue(1, 2, 0); // B3
    CPPUNIT_ASSERT_EQUAL(12345.0, fVal);
    aStr = rDoc.GetString(1, 3, 0); // B4
    CPPUNIT_ASSERT_EQUAL(OUString("A < B"), aStr);

    // Numeric value of 0.
    ScRefCellValue aCell;
    aCell.assign(rDoc, ScAddress(1,4,0)); // B5
    CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "This cell must be numeric.", CELLTYPE_VALUE, aCell.meType);
    CPPUNIT_ASSERT_EQUAL(0.0, aCell.mfValue);

    xDocSh->DoClose();
}

void ScFiltersTest::testBooleanFormatXLSX()
{
    ScDocShellRef xDocSh = loadDoc(u"check-boolean.", FORMAT_XLSX);
    ScDocument& rDoc = xDocSh->GetDocument();
    SvNumberFormatter* pNumFormatter = rDoc.GetFormatTable();
    // Saved as >"TRUE";"TRUE";"FALSE"< but reading converted back to >BOOLEAN<
    const OUString aBooleanTypeStr = "BOOLEAN";

    CPPUNIT_ASSERT_MESSAGE("Failed to load check-boolean.xlsx", xDocSh.is());
    sal_uInt32 nNumberFormat;

    for (SCROW i = 0; i <= 1; i++)
    {
        rDoc.GetNumberFormat(0, i, 0, nNumberFormat);
        const SvNumberformat* pNumberFormat = pNumFormatter->GetEntry(nNumberFormat);
        const OUString& rFormatStr = pNumberFormat->GetFormatstring();
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Number format != boolean", aBooleanTypeStr, rFormatStr);
    }

    xDocSh->DoClose();
}

void ScFiltersTest::testRangeNameXLS()
{
    ScDocShellRef xDocSh = loadDoc(u"named-ranges-global.", FORMAT_XLS);
    xDocSh->DoHardRecalc();

    ScDocument& rDoc = xDocSh->GetDocument();
    testRangeNameImpl(rDoc);

    OUString aCSVPath;
    createCSVPath( "rangeExp_Sheet2.", aCSVPath );
    // fdo#44587
    testFile( aCSVPath, rDoc, 1);

    xDocSh->DoClose();
}

void ScFiltersTest::testRangeNameLocalXLS()
{
    ScDocShellRef xDocSh = loadDoc(u"named-ranges-local.", FORMAT_XLS);
    xDocSh->DoHardRecalc();

    ScDocument& rDoc = xDocSh->GetDocument();
    ScRangeName* pRangeName = rDoc.GetRangeName(0);
    CPPUNIT_ASSERT(pRangeName);
    CPPUNIT_ASSERT_EQUAL(size_t(2), pRangeName->size());

    OUString aFormula;
    rDoc.GetFormula(3, 11, 0, aFormula);
    CPPUNIT_ASSERT_EQUAL(OUString("=SUM(local_name2)"), aFormula);
    ASSERT_DOUBLES_EQUAL(14.0, rDoc.GetValue(3, 11, 0));

    rDoc.GetFormula(6, 4, 0, aFormula);
    CPPUNIT_ASSERT_EQUAL(OUString("=local_name1"), aFormula);

    xDocSh->DoClose();
}

void ScFiltersTest::testRangeNameXLSX()
{
    ScDocShellRef xDocSh = loadDoc(u"named-ranges-global.", FORMAT_XLSX);
    xDocSh->DoHardRecalc();

    ScDocument& rDoc = xDocSh->GetDocument();
    testRangeNameImpl(rDoc);

    xDocSh->DoClose();
}

void ScFiltersTest::testHyperlinksXLSX()
{
    ScDocShellRef xDocSh = loadDoc(u"hyperlinks.", FORMAT_XLSX);
    ScDocument& rDoc = xDocSh->GetDocument();

    CPPUNIT_ASSERT_EQUAL(OUString("10:ABC10"), rDoc.GetString(ScAddress(0,1,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("10:ABC11"), rDoc.GetString(ScAddress(0,2,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("10:ABC12"), rDoc.GetString(ScAddress(0,3,0)));

    xDocSh->DoClose();
}

void ScFiltersTest::testHardRecalcODS()
{
    ScDocShellRef xDocSh = loadDoc(u"hard-recalc.", FORMAT_ODS);
    xDocSh->DoHardRecalc();

    CPPUNIT_ASSERT_MESSAGE("Failed to load hard-recalc.*", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();
    OUString aCSVFileName;

    //test hard recalc: document has an incorrect cached formula result
    //hard recalc should have updated to the correct result
    createCSVPath("hard-recalc.", aCSVFileName);
    testFile(aCSVFileName, rDoc, 0);

    xDocSh->DoClose();
}

void ScFiltersTest::testFunctionsODS()
{
    ScDocShellRef xDocSh = loadDoc(u"functions.", FORMAT_ODS);
    xDocSh->DoHardRecalc();

    CPPUNIT_ASSERT_MESSAGE("Failed to load functions.*", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();
    OUString aCSVFileName;

    //test logical functions
    createCSVPath("logical-functions.", aCSVFileName);
    testFile(aCSVFileName, rDoc, 0);
    //test spreadsheet functions
    createCSVPath("spreadsheet-functions.", aCSVFileName);
    testFile(aCSVFileName, rDoc, 1);
    //test mathematical functions
    createCSVPath("mathematical-functions.", aCSVFileName);
    testFile(aCSVFileName, rDoc, 2, StringType::PureString);
    //test information functions
    createCSVPath("information-functions.", aCSVFileName);
    testFile(aCSVFileName, rDoc, 3);
    // text functions
    createCSVPath("text-functions.", aCSVFileName);
    testFile(aCSVFileName, rDoc, 4, StringType::PureString);
    // statistical functions
    createCSVPath("statistical-functions.", aCSVFileName);
    testFile(aCSVFileName, rDoc, 5);
    // financial functions
    createCSVPath("financial-functions.", aCSVFileName);
    testFile(aCSVFileName, rDoc, 6);

    xDocSh->DoClose();

    xDocSh = loadDoc(u"database-functions.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to load functions.*", xDocSh.is());
    xDocSh->DoHardRecalc();
    ScDocument& rDoc2 = xDocSh->GetDocument();

    createCSVPath("database-functions.", aCSVFileName);
    testFile(aCSVFileName, rDoc2, 0);

    xDocSh->DoClose();

    xDocSh = loadDoc(u"date-time-functions.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to load functions.*", xDocSh.is());
    xDocSh->DoHardRecalc();
    ScDocument& rDoc3 = xDocSh->GetDocument();
    createCSVPath("date-time-functions.", aCSVFileName);
    testFile(aCSVFileName, rDoc3, 0, StringType::PureString);

    xDocSh->DoClose();

    // crashes at exit while unloading StarBasic code
    // xDocSh = loadDoc("user-defined-function.", FORMAT_ODS);
    // xDocSh->DoHardRecalc();
    // ScDocument& rDocUserDef = xDocSh->GetDocument();
    // createCSVPath("user-defined-function.", aCSVFileName);
    // testFile(aCSVFileName, rDocUserDef, 0);
}

void ScFiltersTest::testFunctionsExcel2010()
{
    ScDocShellRef xDocSh = loadDoc(u"functions-excel-2010.", FORMAT_XLSX);
    CPPUNIT_ASSERT_MESSAGE("Failed to load the document.", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();
    rDoc.CalcAll(); // perform hard re-calculation.

    testFunctionsExcel2010_Impl(rDoc);

    xDocSh->DoClose();
}

void ScFiltersTest::testCeilingFloorXLSX()
{
    ScDocShellRef xDocSh = loadDoc(u"ceiling-floor.", FORMAT_XLSX);
    CPPUNIT_ASSERT_MESSAGE("Failed to load the document.", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();
    rDoc.CalcAll(); // perform hard re-calculation.

    testCeilingFloor_Impl(rDoc);

    xDocSh->DoClose();
}

void ScFiltersTest::testCachedFormulaResultsODS()
{
    {
        ScDocShellRef xDocSh = loadDoc(u"functions.", FORMAT_ODS);
        CPPUNIT_ASSERT_MESSAGE("Failed to load functions.*", xDocSh.is());

        ScDocument& rDoc = xDocSh->GetDocument();
        OUString aCSVFileName;

        //test cached formula results of logical functions
        createCSVPath("logical-functions.", aCSVFileName);
        testFile(aCSVFileName, rDoc, 0);
        //test cached formula results of spreadsheet functions
        createCSVPath("spreadsheet-functions.", aCSVFileName);
        testFile(aCSVFileName, rDoc, 1);
        //test cached formula results of mathematical functions
        createCSVPath("mathematical-functions.", aCSVFileName);
        testFile(aCSVFileName, rDoc, 2, StringType::PureString);
        //test cached formula results of information functions
        createCSVPath("information-functions.", aCSVFileName);
        testFile(aCSVFileName, rDoc, 3);
        // text functions
        createCSVPath("text-functions.", aCSVFileName);
        testFile(aCSVFileName, rDoc, 4, StringType::PureString);

        xDocSh->DoClose();
    }

    {
        ScDocShellRef xDocSh = loadDoc(u"cachedValue.", FORMAT_ODS);
        CPPUNIT_ASSERT_MESSAGE("Failed to load cachedValue.*", xDocSh.is());

        ScDocument& rDoc = xDocSh->GetDocument();
        OUString aCSVFileName;
        createCSVPath("cachedValue.", aCSVFileName);
        testFile(aCSVFileName, rDoc, 0);

        //we want to me sure that volatile functions are always recalculated
        //regardless of cached results.  if you update the ods file, you must
        //update the values here.
        //if NOW() is recalculated, then it should never equal "01/25/13 01:06 PM"
        OUString sTodayRecalc(rDoc.GetString(0,0,1));

        CPPUNIT_ASSERT("01/25/13 01:06 PM" != sTodayRecalc);

        OUString sTodayRecalcRef(rDoc.GetString(1,0,1));
        CPPUNIT_ASSERT_EQUAL(sTodayRecalc, sTodayRecalcRef);

        // make sure that error values are not being treated as string values
        for(SCCOL nCol = 0; nCol < 4; ++nCol)
        {
            for(SCROW nRow = 0; nRow < 2; ++nRow)
            {
                OUString aFormula = "=ISERROR(" +
                    OUStringChar(static_cast<char>('A'+nCol)) + OUString::number(nRow) +
                    ")";
                rDoc.SetString(nCol, nRow + 2, 2, aFormula);
                CPPUNIT_ASSERT_EQUAL_MESSAGE(OUStringToOString(aFormula, RTL_TEXTENCODING_UTF8).getStr(), OUString("TRUE"), rDoc.GetString(nCol, nRow +2, 2));

                OUString aIsTextFormula = "=ISTEXT(" +
                    OUString::number(static_cast<char>('A'+nCol))+ OUString::number(nRow) +
                    ")";
                rDoc.SetString(nCol, nRow + 4, 2, aIsTextFormula);
                CPPUNIT_ASSERT_EQUAL(OUString("FALSE"), rDoc.GetString(nCol, nRow +4, 2));
            }
        }

        xDocSh->DoClose();
    }
}

void ScFiltersTest::testCachedMatrixFormulaResultsODS()
{
    ScDocShellRef xDocSh = loadDoc(u"matrix.", FORMAT_ODS);

    CPPUNIT_ASSERT_MESSAGE("Failed to load matrix.*", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    //test matrix
    OUString aCSVFileName;
    createCSVPath("matrix.", aCSVFileName);
    testFile(aCSVFileName, rDoc, 0);
    //test matrices with special cases
    createCSVPath("matrix2.", aCSVFileName);
    testFile(aCSVFileName, rDoc, 1);
    createCSVPath("matrix3.", aCSVFileName);
    testFile(aCSVFileName, rDoc, 2);
    //The above testFile() does not catch the below case.
    //If a matrix formula has a matrix reference cell that is intended to have
    //a blank text result, the matrix reference cell is actually saved(export)
    //as a float cell with 0 as the value and an empty <text:p/>.
    //Import works around this by setting these cells as text cells so that
    //the blank text is used for display instead of the number 0.
    //If this is working properly, the following cell should NOT have value data.
    CPPUNIT_ASSERT_EQUAL(OUString(), rDoc.GetString(3,0,2));

    // fdo#59293 with cached value import error formulas require special
    // treatment
    rDoc.SetString(2, 5, 2, "=ISERROR(A6)");
    double nVal = rDoc.GetValue(2,5,2);
    CPPUNIT_ASSERT_EQUAL(1.0, nVal);

    xDocSh->DoClose();
}

void ScFiltersTest::testFormulaDepAcrossSheetsODS()
{
    ScDocShellRef xDocSh = loadDoc(u"formula-across-sheets.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to load the file.", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    sc::AutoCalcSwitch aACSwitch(rDoc, true); // Make sure auto calc is turned on.

    // Save the original values of A4:C4.
    double fA4 = rDoc.GetValue(ScAddress(0,3,2));
    double fB4 = rDoc.GetValue(ScAddress(1,3,2));
    double fC4 = rDoc.GetValue(ScAddress(2,3,2));

    // Change the value of D4. This should trigger A4:C4 to be recalculated.
    double fD4 = rDoc.GetValue(ScAddress(3,3,2));
    rDoc.SetValue(ScAddress(3,3,2), fD4+1.0);

    CPPUNIT_ASSERT_MESSAGE("The value must differ from the original.", fA4 != rDoc.GetValue(ScAddress(0,3,2)));
    CPPUNIT_ASSERT_MESSAGE("The value must differ from the original.", fB4 != rDoc.GetValue(ScAddress(1,3,2)));
    CPPUNIT_ASSERT_MESSAGE("The value must differ from the original.", fC4 != rDoc.GetValue(ScAddress(2,3,2)));

    xDocSh->DoClose();
}

void ScFiltersTest::testFormulaDepDeleteContentsODS()
{
    ScDocShellRef xDocSh = loadDoc(u"formula-delete-contents.", FORMAT_ODS, true);
    CPPUNIT_ASSERT_MESSAGE("Failed to load the file.", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    sc::UndoSwitch aUndoSwitch(rDoc, true); // Enable undo.
    sc::AutoCalcSwitch aACSwitch(rDoc, true); // Make sure auto calc is turned on.

    CPPUNIT_ASSERT_EQUAL(195.0, rDoc.GetValue(ScAddress(3,15,0))); // formula in D16

    // Delete D2:D5.
    ScDocFunc& rFunc = xDocSh->GetDocFunc();
    ScRange aRange(3,1,0,3,4,0);
    ScMarkData aMark(rDoc.GetSheetLimits());
    aMark.SetMarkArea(aRange);
    aMark.MarkToMulti();
    bool bGood = rFunc.DeleteContents(aMark, InsertDeleteFlags::ALL, true, true);
    CPPUNIT_ASSERT(bGood);
    CPPUNIT_ASSERT_EQUAL(0.0, rDoc.GetValue(ScAddress(3,1,0)));
    CPPUNIT_ASSERT_EQUAL(0.0, rDoc.GetValue(ScAddress(3,2,0)));
    CPPUNIT_ASSERT_EQUAL(0.0, rDoc.GetValue(ScAddress(3,3,0)));
    CPPUNIT_ASSERT_EQUAL(0.0, rDoc.GetValue(ScAddress(3,4,0)));

    CPPUNIT_ASSERT_EQUAL(94.0, rDoc.GetValue(ScAddress(3,15,0))); // formula in D16

    SfxUndoManager* pUndoMgr = rDoc.GetUndoManager();
    CPPUNIT_ASSERT(pUndoMgr);
    pUndoMgr->Undo();
    CPPUNIT_ASSERT_EQUAL(195.0, rDoc.GetValue(ScAddress(3,15,0))); // formula in D16

    xDocSh->DoClose();
}

namespace {

void testDBRanges_Impl(ScDocument& rDoc, sal_Int32 nFormat)
{
    ScDBCollection* pDBCollection = rDoc.GetDBCollection();
    CPPUNIT_ASSERT_MESSAGE("no database collection", pDBCollection);

    ScDBData* pAnonDBData = rDoc.GetAnonymousDBData(0);
    CPPUNIT_ASSERT_MESSAGE("missing anonymous DB data in sheet 1", pAnonDBData);
    //control hidden rows
    bool bHidden;
    SCROW nRow1, nRow2;
    bHidden = rDoc.RowHidden(0, 0, &nRow1, &nRow2);
    CPPUNIT_ASSERT_MESSAGE("Sheet1: row 0 should be visible", !bHidden && nRow1 == 0 && nRow2 == 0);
    bHidden = rDoc.RowHidden(1, 0, &nRow1, &nRow2);
    CPPUNIT_ASSERT_MESSAGE("Sheet1: rows 1-2 should be hidden", bHidden && nRow1 == 1 && nRow2 == 2);
    bHidden = rDoc.RowHidden(3, 0, &nRow1, &nRow2);
    CPPUNIT_ASSERT_MESSAGE("Sheet1: row 3 should be visible", !bHidden && nRow1 == 3 && nRow2 == 3);
    bHidden = rDoc.RowHidden(4, 0, &nRow1, &nRow2);
    CPPUNIT_ASSERT_MESSAGE("Sheet1: row 4-5 should be hidden", bHidden && nRow1 == 4 && nRow2 == 5);
    bHidden = rDoc.RowHidden(6, 0, &nRow1, &nRow2);
    CPPUNIT_ASSERT_MESSAGE("Sheet1: row 6-end should be visible", !bHidden && nRow1 == 6 && nRow2 == MAXROW);
    if (nFormat == FORMAT_ODS) //excel doesn't support named db ranges
    {
        double aValue;
        rDoc.GetValue(0,10,1, aValue);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Sheet2: A11: formula result is incorrect", 4.0, aValue);
        rDoc.GetValue(1, 10, 1, aValue);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Sheet2: B11: formula result is incorrect", 2.0, aValue);
    }
    double aValue;
    rDoc.GetValue(3,10,1, aValue);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Sheet2: D11: formula result is incorrect", 4.0, aValue);
    rDoc.GetValue(4, 10, 1, aValue);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Sheet2: E11: formula result is incorrect", 2.0, aValue);
}

}

void ScFiltersTest::testDatabaseRangesODS()
{
    ScDocShellRef xDocSh = loadDoc(u"database.", FORMAT_ODS);
    xDocSh->DoHardRecalc();

    ScDocument& rDoc = xDocSh->GetDocument();

    testDBRanges_Impl(rDoc, FORMAT_ODS);
    xDocSh->DoClose();
}

void ScFiltersTest::testDatabaseRangesXLS()
{
    ScDocShellRef xDocSh = loadDoc(u"database.", FORMAT_XLS);
    xDocSh->DoHardRecalc();

    ScDocument& rDoc = xDocSh->GetDocument();

    testDBRanges_Impl(rDoc, FORMAT_XLS);
    xDocSh->DoClose();
}

void ScFiltersTest::testDatabaseRangesXLSX()
{
    ScDocShellRef xDocSh = loadDoc(u"database.", FORMAT_XLSX);
    xDocSh->DoHardRecalc();

    ScDocument& rDoc = xDocSh->GetDocument();

    testDBRanges_Impl(rDoc, FORMAT_XLSX);
    xDocSh->DoClose();
}

void ScFiltersTest::testFormatsODS()
{
    ScDocShellRef xDocSh = loadDoc(u"formats.", FORMAT_ODS);
    xDocSh->DoHardRecalc();

    ScDocument& rDoc = xDocSh->GetDocument();

    testFormats(this, &rDoc, FORMAT_ODS);
    xDocSh->DoClose();
}

// void ScFiltersTest::testFormatsXLS()
// {
//     ScDocShellRef xDocSh = loadDoc("formats.", FORMAT_XLS);
//     xDocSh->DoHardRecalc();
//
//     ScDocument& rDoc = xDocSh->GetDocument();
//
//     testFormats(this, rDoc, FORMAT_XLS);
//     xDocSh->DoClose();
// }

// void ScFiltersTest::testFormatsXLSX()
// {
//     ScDocShellRef xDocSh = loadDoc("formats.", FORMAT_XLSX);
//     xDocSh->DoHardRecalc();
//
//     ScDocument& rDoc = xDocSh->GetDocument();
//
//     testFormats(this, rDoc, FORMAT_XLSX);
//     xDocSh->DoClose();
// }

void ScFiltersTest::testMatrixODS()
{
    ScDocShellRef xDocSh = loadDoc(u"matrix.", FORMAT_ODS);
    xDocSh->DoHardRecalc();

    ScDocument& rDoc = xDocSh->GetDocument();

    OUString aCSVFileName;
    createCSVPath("matrix.", aCSVFileName);
    testFile(aCSVFileName, rDoc, 0);

    xDocSh->DoClose();
}

void ScFiltersTest::testMatrixXLS()
{
    ScDocShellRef xDocSh = loadDoc(u"matrix.", FORMAT_XLS);
    xDocSh->DoHardRecalc();

    CPPUNIT_ASSERT_MESSAGE("Failed to load matrix.*", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    OUString aCSVFileName;
    createCSVPath("matrix.", aCSVFileName);
    testFile(aCSVFileName, rDoc, 0);

    xDocSh->DoClose();
}

void ScFiltersTest::testDoubleThinBorder()
{
// double-thin borders created with size less than 1.15 where invisible (and subsequently lost) on round-trips.
    ScDocShellRef xDocSh = loadDoc(u"tdf88827_borderDoubleThin.", FORMAT_ODS);

    CPPUNIT_ASSERT_MESSAGE("Failed to load tdf88827_borderDoubleThin.*", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    const editeng::SvxBorderLine* pLeft = nullptr;
    const editeng::SvxBorderLine* pTop = nullptr;
    const editeng::SvxBorderLine* pRight = nullptr;
    const editeng::SvxBorderLine* pBottom = nullptr;

    rDoc.GetBorderLines( 2, 2, 0, &pLeft, &pTop, &pRight, &pBottom );
    CPPUNIT_ASSERT(pTop);
    CPPUNIT_ASSERT(pRight);
    CPPUNIT_ASSERT_EQUAL( SvxBorderLineStyle::DOUBLE_THIN, pRight->GetBorderLineStyle() );
    xDocSh->DoClose();
}

void ScFiltersTest::testBorderODS()
{
    ScDocShellRef xDocSh = loadDoc(u"border.", FORMAT_ODS);

    CPPUNIT_ASSERT_MESSAGE("Failed to load border.*", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    const editeng::SvxBorderLine* pLeft = nullptr;
    const editeng::SvxBorderLine* pTop = nullptr;
    const editeng::SvxBorderLine* pRight = nullptr;
    const editeng::SvxBorderLine* pBottom = nullptr;

    rDoc.GetBorderLines( 0, 1, 0, &pLeft, &pTop, &pRight, &pBottom );
    CPPUNIT_ASSERT(!pLeft);
    CPPUNIT_ASSERT(!pTop);
    CPPUNIT_ASSERT(!pBottom);
    CPPUNIT_ASSERT(pRight);
    CPPUNIT_ASSERT_EQUAL(SvxBorderLineStyle::SOLID, pRight->GetBorderLineStyle());

    rDoc.GetBorderLines( 2, 1, 0, &pLeft, &pTop, &pRight, &pBottom );
    CPPUNIT_ASSERT(!pLeft);
    CPPUNIT_ASSERT(!pTop);
    CPPUNIT_ASSERT(!pBottom);

    CPPUNIT_ASSERT(pRight);
    CPPUNIT_ASSERT_EQUAL(SvxBorderLineStyle::SOLID, pRight->GetBorderLineStyle());
    CPPUNIT_ASSERT_EQUAL(tools::Long(20), pRight->GetWidth());

    rDoc.GetBorderLines( 2, 8, 0, &pLeft, &pTop, &pRight, &pBottom );

    CPPUNIT_ASSERT(pLeft);
    CPPUNIT_ASSERT(pTop);
    CPPUNIT_ASSERT(pBottom);
    CPPUNIT_ASSERT(pRight);
    CPPUNIT_ASSERT_EQUAL(SvxBorderLineStyle::SOLID, pRight->GetBorderLineStyle());
    CPPUNIT_ASSERT_EQUAL(tools::Long(5), pRight->GetWidth());
    CPPUNIT_ASSERT_EQUAL(COL_BLUE, pRight->GetColor());

    xDocSh->DoClose();
}

namespace {

struct Border
{
    sal_Int16 column;
    sal_Int32 row;
    tools::Long leftWidth;
    tools::Long topWidth;
    tools::Long rightWidth;
    tools::Long bottomWidth;
    sal_uInt16 lOutWidth;
    sal_uInt16 lInWidth;
    sal_uInt16 lDistance;
    sal_uInt16 tOutWidth;
    sal_uInt16 tInWidth;
    sal_uInt16 tDistance;
    sal_uInt16 rOutWidth;
    sal_uInt16 rInWidth;
    sal_uInt16 rDistance;
    sal_uInt16 bOutWidth;
    sal_uInt16 bInWidth;
    sal_uInt16 bDistance;
    SvxBorderLineStyle lStyle;
    SvxBorderLineStyle tStyle;
    SvxBorderLineStyle rStyle;
    SvxBorderLineStyle bStyle;
    // that's a monster
    Border(sal_Int16 col, sal_Int32 r, sal_Int32 lW, sal_Int32 tW, sal_Int32 rW, sal_Int32 bW, sal_uInt16 lOutW, sal_uInt16 lInW,
        sal_uInt16 lDist, sal_uInt16 tOutW, sal_uInt16 tInW, sal_uInt16 tDist, sal_uInt16 rOutW, sal_uInt16 rInW, sal_uInt16 rDist,
        sal_uInt16 bOutW, sal_uInt16 bInW, sal_uInt16 bDist, sal_Int16 lSt, sal_Int16 tSt, sal_Int16 rSt, sal_Int16 bSt):
    column(col), row(r), leftWidth(lW), topWidth(tW), rightWidth(rW), bottomWidth(bW), lOutWidth(lOutW), lInWidth(lInW), lDistance(lDist),
    tOutWidth(tOutW), tInWidth(tInW), tDistance(tDist), rOutWidth(rOutW), rInWidth(rInW), rDistance(rDist), bOutWidth(bOutW), bInWidth(bInW),
    bDistance(bDist),
    lStyle(static_cast<SvxBorderLineStyle>(lSt)), tStyle(static_cast<SvxBorderLineStyle>(tSt)), rStyle(static_cast<SvxBorderLineStyle>(rSt)), bStyle(static_cast<SvxBorderLineStyle>(bSt)) {};
};

}

void ScFiltersTest::testBordersOoo33()
{
    std::vector<Border> borders;
    borders.emplace_back(1, 1, 22, 22, 22, 22, 1, 1, 20, 1, 1, 20, 1, 1, 20, 1, 1, 20, 3, 3, 3, 3);
    borders.emplace_back(1, 3, 52, 52, 52, 52, 1, 1, 50, 1, 1, 50, 1, 1, 50, 1, 1, 50, 3, 3, 3, 3);
    borders.emplace_back(1, 5, 60, 60, 60, 60, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 3, 3, 3, 3);
    borders.emplace_back(1, 7, 150, 150, 150, 150, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 3, 3, 3, 3);
    borders.emplace_back(1, 9, 71, 71, 71, 71, 20, 1, 50, 20, 1, 50, 20, 1, 50, 20, 1, 50, 3, 3, 3, 3);
    borders.emplace_back(1, 11, 101, 101, 101, 101, 50, 1, 50, 50, 1, 50, 50, 1, 50, 50, 1, 50, 3, 3, 3, 3);
    borders.emplace_back(1, 13, 131, 131, 131, 131, 80, 1, 50, 80, 1, 50, 80, 1, 50, 80, 1, 50, 3, 3, 3, 3);
    borders.emplace_back(1, 15, 120, 120, 120, 120, 50, 20, 50, 50, 20, 50, 50, 20, 50, 50, 20, 50, 3, 3, 3, 3);
    borders.emplace_back(1, 17, 90, 90, 90, 90, 20, 50, 20, 20, 50, 20, 20, 50, 20, 20, 50, 20, 3, 3, 3, 3);
    borders.emplace_back(1, 19, 180, 180, 180, 180, 80, 50, 50, 80, 50, 50, 80, 50, 50, 80, 50, 50, 3, 3, 3, 3);
    borders.emplace_back(1, 21, 180, 180, 180, 180, 50, 80, 50, 50, 80, 50, 50, 80, 50, 50, 80, 50, 3, 3, 3, 3);
    borders.emplace_back(4, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0);
    borders.emplace_back(4, 3, 10, 10, 10, 10, 10, 0, 0, 10, 0, 0, 10, 0, 0, 10, 0, 0, 0, 0, 0, 0);
    borders.emplace_back(4, 5, 20, 20, 20, 20, 20, 0, 0, 20, 0, 0, 20, 0, 0, 20, 0, 0, 0, 0, 0, 0);
    borders.emplace_back(4, 7, 50, 50, 50, 50, 50, 0, 0, 50, 0, 0, 50, 0, 0, 50, 0, 0, 0, 0, 0, 0);
    borders.emplace_back(4, 9, 80, 80, 80, 80, 80, 0, 0, 80, 0, 0, 80, 0, 0, 80, 0, 0, 0, 0, 0, 0);
    borders.emplace_back(4, 11, 100, 100, 100, 100, 100, 0, 0, 100, 0, 0, 100, 0, 0, 100, 0, 0, 0, 0, 0, 0);

    ScDocShellRef xDocSh = loadDoc(u"borders_ooo33.", FORMAT_ODS);

    CPPUNIT_ASSERT_MESSAGE("Failed to load borders_ooo33.*", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    const editeng::SvxBorderLine* pLeft = nullptr;
    const editeng::SvxBorderLine* pTop = nullptr;
    const editeng::SvxBorderLine* pRight = nullptr;
    const editeng::SvxBorderLine* pBottom = nullptr;
    sal_Int16 temp = 0;
    for(sal_Int16 i = 0; i<6; ++i)
    {
        for(sal_Int32 j = 0; j<22; ++j)
        {
            rDoc.GetBorderLines( i, j, 0, &pLeft, &pTop, &pRight, &pBottom );
            if(pLeft!=nullptr && pTop!=nullptr && pRight!=nullptr && pBottom!=nullptr)
            {
                CPPUNIT_ASSERT_EQUAL(borders[temp].column, i);
                CPPUNIT_ASSERT_EQUAL(borders[temp].row, j);
                CPPUNIT_ASSERT_EQUAL(borders[temp].leftWidth, pLeft->GetWidth());
                CPPUNIT_ASSERT_EQUAL(borders[temp].topWidth, pTop->GetWidth());
                CPPUNIT_ASSERT_EQUAL(borders[temp].rightWidth, pRight->GetWidth());
                CPPUNIT_ASSERT_EQUAL(borders[temp].bottomWidth, pBottom->GetWidth());
                CPPUNIT_ASSERT_EQUAL(borders[temp].lOutWidth, pLeft->GetOutWidth());
                CPPUNIT_ASSERT_EQUAL(borders[temp].lInWidth, pLeft->GetInWidth());
                CPPUNIT_ASSERT_EQUAL(borders[temp].lDistance, pLeft->GetDistance());
                CPPUNIT_ASSERT_EQUAL(borders[temp].tOutWidth, pTop->GetOutWidth());
                CPPUNIT_ASSERT_EQUAL(borders[temp].tInWidth, pTop->GetInWidth());
                CPPUNIT_ASSERT_EQUAL(borders[temp].tDistance, pTop->GetDistance());
                CPPUNIT_ASSERT_EQUAL(borders[temp].rOutWidth, pRight->GetOutWidth());
                CPPUNIT_ASSERT_EQUAL(borders[temp].rInWidth, pRight->GetInWidth());
                CPPUNIT_ASSERT_EQUAL(borders[temp].rDistance, pRight->GetDistance());
                CPPUNIT_ASSERT_EQUAL(borders[temp].bOutWidth, pBottom->GetOutWidth());
                CPPUNIT_ASSERT_EQUAL(borders[temp].bInWidth, pBottom->GetInWidth());
                CPPUNIT_ASSERT_EQUAL(borders[temp].bDistance, pBottom->GetDistance());
                SvxBorderLineStyle tempStyle = pLeft->GetBorderLineStyle();
                CPPUNIT_ASSERT_EQUAL(borders[temp].lStyle, tempStyle);
                tempStyle = pTop->GetBorderLineStyle();
                CPPUNIT_ASSERT_EQUAL(borders[temp].tStyle, tempStyle);
                tempStyle = pRight->GetBorderLineStyle();
                CPPUNIT_ASSERT_EQUAL(borders[temp].rStyle, tempStyle);
                tempStyle = pBottom->GetBorderLineStyle();
                CPPUNIT_ASSERT_EQUAL(borders[temp].bStyle, tempStyle);
                ++temp;
            }
        }
    }

    xDocSh->DoClose();
}

void ScFiltersTest::testBugFixesODS()
{
    ScDocShellRef xDocSh = loadDoc(u"bug-fixes.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to load bugFixes.ods", xDocSh.is());

    xDocSh->DoHardRecalc();
    ScDocument& rDoc = xDocSh->GetDocument();

    {
        // fdo#40967
        OUString aCSVFileName;
        createCSVPath("bugFix_Sheet2.", aCSVFileName);
        testFile(aCSVFileName, rDoc, 1);
    }

    {
        // fdo#40426
        ScDBData* pDBData = rDoc.GetDBCollection()->getNamedDBs().findByUpperName("DBRANGE1");
        CPPUNIT_ASSERT(pDBData);
        CPPUNIT_ASSERT(pDBData->HasHeader());
        // no header
        pDBData = rDoc.GetDBCollection()->getNamedDBs().findByUpperName("DBRANGE2");
        CPPUNIT_ASSERT(pDBData);
        CPPUNIT_ASSERT(!pDBData->HasHeader());
    }

    {
        // fdo#59240
        OUString aCSVFileName;
        createCSVPath("bugFix_Sheet4.", aCSVFileName);
        testFile(aCSVFileName, rDoc, 3);
    }

    xDocSh->DoClose();
}

void ScFiltersTest::testBugFixesXLS()
{
    ScDocShellRef xDocSh = loadDoc(u"bug-fixes.", FORMAT_XLS);
    CPPUNIT_ASSERT_MESSAGE("Failed to load bugFixes.xls", xDocSh.is());

    xDocSh->DoHardRecalc();
    xDocSh->DoClose();
}

void ScFiltersTest::testBugFixesXLSX()
{
    ScDocShellRef xDocSh = loadDoc(u"bug-fixes.", FORMAT_XLSX);
    CPPUNIT_ASSERT_MESSAGE("Failed to load bugFixes.xls", xDocSh.is());

    xDocSh->DoHardRecalc();
    xDocSh->DoClose();
}

namespace {

void checkMergedCells( ScDocument& rDoc, const ScAddress& rStartAddress,
                       const ScAddress& rExpectedEndAddress )
{
    SCCOL nActualEndCol = rStartAddress.Col();
    SCROW nActualEndRow = rStartAddress.Row();
    rDoc.ExtendMerge( rStartAddress.Col(), rStartAddress.Row(),
                       nActualEndCol, nActualEndRow, rStartAddress.Tab() );
    OString sTab = OString::number( rStartAddress.Tab() + 1 );
    OString msg = "Merged cells are not correctly imported on sheet" + sTab;
    OString msgCol = msg + "; end col";
    OString msgRow = msg + "; end row";
    CPPUNIT_ASSERT_EQUAL_MESSAGE( msgCol.pData->buffer, rExpectedEndAddress.Col(), nActualEndCol );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( msgRow.pData->buffer, rExpectedEndAddress.Row(), nActualEndRow );
}

}

void ScFiltersTest::testMergedCellsODS()
{
    ScDocShellRef xDocSh = loadDoc(u"merged.", FORMAT_ODS);
    ScDocument& rDoc = xDocSh->GetDocument();

    //check sheet1 content
    OUString aCSVFileName1;
    createCSVPath("merged1.", aCSVFileName1);
    testFile(aCSVFileName1, rDoc, 0);

    //check sheet1 merged cells
    checkMergedCells( rDoc, ScAddress( 0, 0, 0 ),  ScAddress( 5, 11, 0 ) );
    checkMergedCells( rDoc, ScAddress( 7, 2, 0 ),  ScAddress( 9, 12, 0 ) );
    checkMergedCells( rDoc, ScAddress( 3, 15, 0 ),  ScAddress( 7, 23, 0 ) );

    //check sheet2 content
    OUString aCSVFileName2;
    createCSVPath("merged2.", aCSVFileName2);
    testFile(aCSVFileName2, rDoc, 1);

    //check sheet2 merged cells
    checkMergedCells( rDoc, ScAddress( 4, 3, 1 ),  ScAddress( 6, 15, 1 ) );

    xDocSh->DoClose();
}

void ScFiltersTest::testRepeatedColumnsODS()
{
    ScDocShellRef xDocSh = loadDoc(u"repeatedColumns.", FORMAT_ODS);
    ScDocument& rDoc = xDocSh->GetDocument();

    //text
    OUString aCSVFileName1;
    createCSVPath("repeatedColumns1.", aCSVFileName1);
    testFile(aCSVFileName1, rDoc, 0);

    //numbers
    OUString aCSVFileName2;
    createCSVPath("repeatedColumns2.", aCSVFileName2);
    testFile(aCSVFileName2, rDoc, 1);

    xDocSh->DoClose();
}

namespace {

//for cleaner passing of parameters
struct ValDataTestParams
{
    ScValidationMode eValMode;
    ScConditionMode eCondOp;
    OUString aStrVal1;
    OUString aStrVal2;
    ScDocument& rDocument;
    ScAddress aPosition;
    OUString aErrorTitle;
    OUString aErrorMessage;
    ScValidErrorStyle eErrorStyle;
    sal_uLong nExpectedIndex;

    ValDataTestParams( ScValidationMode eMode, ScConditionMode eOp,
                       const OUString& aExpr1, const OUString& aExpr2, ScDocument& rDoc,
                       const ScAddress& aPos, const OUString& aETitle, const OUString& aEMsg,
                       ScValidErrorStyle eEStyle, sal_uLong nIndex ):
                            eValMode(eMode), eCondOp(eOp), aStrVal1(aExpr1),
                            aStrVal2(aExpr2), rDocument(rDoc), aPosition(aPos),
                            aErrorTitle(aETitle), aErrorMessage(aEMsg),
                            eErrorStyle(eEStyle), nExpectedIndex(nIndex) { };
};

void checkValiditationEntries( const ValDataTestParams& rVDTParams )
{
    ScDocument& rDoc = rVDTParams.rDocument;

    //create expected data validation entry
    ScValidationData aValData(
        rVDTParams.eValMode, rVDTParams.eCondOp, rVDTParams.aStrVal1,
        rVDTParams.aStrVal2, rDoc, rVDTParams.aPosition, EMPTY_OUSTRING,
        EMPTY_OUSTRING, rDoc.GetStorageGrammar(), rDoc.GetStorageGrammar()
    );
    aValData.SetIgnoreBlank( true );
    aValData.SetListType( 1 );
    aValData.ResetInput();
    aValData.SetError( rVDTParams.aErrorTitle, rVDTParams.aErrorMessage, rVDTParams.eErrorStyle );
    aValData.SetSrcString( EMPTY_OUSTRING );

    //get actual data validation entry from document
    const ScValidationData* pValDataTest = rDoc.GetValidationEntry( rVDTParams.nExpectedIndex );

    sal_Int32 nCol( static_cast<sal_Int32>(rVDTParams.aPosition.Col()) );
    sal_Int32 nRow( static_cast<sal_Int32>(rVDTParams.aPosition.Row()) );
    sal_Int32 nTab( static_cast<sal_Int32>(rVDTParams.aPosition.Tab()) );
    OString aMsgPrefix = "Data Validation Entry with base-cell-address: (" +
        OString::number(nCol) + "," + OString::number(nRow) + "," + OString::number(nTab) + ") ";

    OString aMsg = aMsgPrefix + "did not get imported at all.";
    CPPUNIT_ASSERT_MESSAGE(aMsg.getStr(), pValDataTest);

    //check if expected and actual data validation entries are equal
    if (!aValData.EqualEntries(*pValDataTest))
    {
        aMsg = aMsgPrefix + "got imported incorrectly.";
        CPPUNIT_FAIL(aMsg.getStr());
    }
}

void checkCellValidity( const ScAddress& rValBaseAddr, const ScRange& rRange, const ScDocument& rDoc )
{
    SCCOL nBCol( rValBaseAddr.Col() );
    SCROW nBRow( rValBaseAddr.Row() );
    SCTAB nTab( static_cast<sal_Int32>(rValBaseAddr.Tab()) );
    //get from the document the data validation entry we are checking against
    const SfxUInt32Item* pItem = rDoc.GetAttr(nBCol, nBRow, nTab, ATTR_VALIDDATA);
    const ScValidationData* pValData = rDoc.GetValidationEntry( pItem->GetValue() );
    CPPUNIT_ASSERT(pValData);

    //check that each cell in the expected range is associated with the data validation entry
    for(SCCOL i = rRange.aStart.Col(); i <= rRange.aEnd.Col(); ++i)
    {
        for(SCROW j = rRange.aStart.Row(); j <= rRange.aEnd.Row(); ++j)
        {
            const SfxUInt32Item* pItemTest = rDoc.GetAttr(i, j, nTab, ATTR_VALIDDATA);
            const ScValidationData* pValDataTest = rDoc.GetValidationEntry( pItemTest->GetValue() );
            //prevent string operations for occurring unnecessarily
            if(!(pValDataTest && pValData->GetKey() == pValDataTest->GetKey()))
            {
                sal_Int32 nCol = static_cast<sal_Int32>(i);
                sal_Int32 nRow = static_cast<sal_Int32>(j);
                sal_Int32 nTab32 = static_cast<sal_Int32>(nTab);
                OString sMsg = "\nData validation entry base-cell-address: (" +
                    OString::number( static_cast<sal_Int32>(nBCol) ) + "," +
                    OString::number( static_cast<sal_Int32>(nBRow) ) + "," +
                    OString::number( nTab32 ) + ")\n"
                    "Cell: (" + OString::number(nCol) + "," +
                    OString::number(nRow) + "," +
                    OString::number(nTab32) + ")";
                sal_uInt32 expectedKey(pValData->GetKey());
                sal_uInt32 actualKey(0);
                if(pValDataTest)
                    actualKey = pValDataTest->GetKey();
                CPPUNIT_ASSERT_EQUAL_MESSAGE(sMsg.getStr(), expectedKey, actualKey);
            }
        }
    }
}

}

void ScFiltersTest::testDataValidityODS()
{
    ScDocShellRef xDocSh = loadDoc(u"dataValidity.", FORMAT_ODS);
    ScDocument& rDoc = xDocSh->GetDocument();

    ScAddress aValBaseAddr1( 2,6,0 ); //sheet1
    ScAddress aValBaseAddr2( 2,3,1 ); //sheet2
    ScAddress aValBaseAddr3( 2,2,2 ); //sheet3

    //sheet1's expected Data Validation Entry values
    ValDataTestParams aVDTParams1(
        SC_VALID_DECIMAL, ScConditionMode::Greater, "3.14", EMPTY_OUSTRING, rDoc,
        aValBaseAddr1, "Too small",
        "The number you are trying to enter is not greater than 3.14! Are you sure you want to enter it anyway?",
        SC_VALERR_WARNING, 1
    );
    //sheet2's expected Data Validation Entry values
    ValDataTestParams aVDTParams2(
        SC_VALID_WHOLE, ScConditionMode::Between, "1", "10", rDoc,
        aValBaseAddr2, "Error sheet 2",
        "Must be a whole number between 1 and 10.",
        SC_VALERR_STOP, 2
    );
    //sheet3's expected Data Validation Entry values
    ValDataTestParams aVDTParams3(
        SC_VALID_CUSTOM, ScConditionMode::Direct, "ISTEXT(C3)", EMPTY_OUSTRING, rDoc,
        aValBaseAddr3, "Error sheet 3",
        "Must not be a numerical value.",
        SC_VALERR_STOP, 3
    );
    //check each sheet's Data Validation Entries
    checkValiditationEntries( aVDTParams1 );
    checkValiditationEntries( aVDTParams2 );
    checkValiditationEntries( aVDTParams3 );

    //expected ranges to be associated with data validity
    ScRange aRange1( 2,2,0, 2,6,0 ); //sheet1
    ScRange aRange2( 2,3,1, 6,7,1 ); //sheet2
    ScRange aRange3( 2,2,2, 2,6,2 ); //sheet3

    //check each sheet's cells for data validity
    checkCellValidity( aValBaseAddr1, aRange1, rDoc );
    checkCellValidity( aValBaseAddr2, aRange2, rDoc );
    checkCellValidity( aValBaseAddr3, aRange3, rDoc );

    //check each sheet's content
    OUString aCSVFileName1;
    createCSVPath("dataValidity1.", aCSVFileName1);
    testFile(aCSVFileName1, rDoc, 0);

    OUString aCSVFileName2;
    createCSVPath("dataValidity2.", aCSVFileName2);
    testFile(aCSVFileName2, rDoc, 1);

    OUString aCSVFileName3;
    createCSVPath("dataValidity3.", aCSVFileName3);
    testFile(aCSVFileName3, rDoc, 2);

    xDocSh->DoClose();
}

void ScFiltersTest::testDataValidityXLSX()
{
    ScDocShellRef xDocSh = loadDoc(u"dataValidity.", FORMAT_XLSX);
    ScDocument& rDoc = xDocSh->GetDocument();

    ScAddress aValBaseAddr1( 2,6,0 ); //sheet1
    ScAddress aValBaseAddr2( 2,3,1 ); //sheet2
    ScAddress aValBaseAddr3( 2,2,2 ); //sheet3

    //expected ranges to be associated with data validity
    ScRange aRange1( 2,2,0, 2,6,0 ); //sheet1
    ScRange aRange2( 2,3,1, 6,7,1 ); //sheet2
    ScRange aRange3( 2,2,2, 2,6,2 ); //sheet3

    //check each sheet's cells for data validity
    checkCellValidity( aValBaseAddr1, aRange1, rDoc );
    checkCellValidity( aValBaseAddr2, aRange2, rDoc );
    checkCellValidity( aValBaseAddr3, aRange3, rDoc );

    xDocSh->DoClose();
}

void ScFiltersTest::testDataTableMortgageXLS()
{
    ScDocShellRef xDocSh = loadDoc(u"data-table/mortgage.", FORMAT_XLS);
    CPPUNIT_ASSERT_MESSAGE("Failed to load the document.", xDocSh.is());

    ScFormulaOptions aOptions;
    aOptions.SetFormulaSepArg(",");
    aOptions.SetFormulaSepArrayCol(",");
    aOptions.SetFormulaSepArrayRow(";");
    xDocSh->SetFormulaOptions(aOptions);

    ScDocument& rDoc = xDocSh->GetDocument();

    // One-variable table

    ASSERT_FORMULA_EQUAL(rDoc, ScAddress(3,1,0), "PMT(B3/12,B4,-B5)", "Wrong formula!");
    ASSERT_FORMULA_EQUAL(rDoc, ScAddress(3,2,0), "MULTIPLE.OPERATIONS(D$2,$B$3,$C3)", "Wrong formula!");
    ASSERT_FORMULA_EQUAL(rDoc, ScAddress(3,3,0), "MULTIPLE.OPERATIONS(D$2,$B$3,$C4)", "Wrong formula!");
    ASSERT_FORMULA_EQUAL(rDoc, ScAddress(3,4,0), "MULTIPLE.OPERATIONS(D$2,$B$3,$C5)", "Wrong formula!");

    // Two-variable table

    ASSERT_FORMULA_EQUAL(rDoc, ScAddress(2,7,0), "PMT(B9/12,B10,-B11)", "Wrong formula!");
    ASSERT_FORMULA_EQUAL(rDoc, ScAddress(3,8,0), "MULTIPLE.OPERATIONS($C$8,$B$9,$C9,$B$10,D$8)", "Wrong formula!");
    ASSERT_FORMULA_EQUAL(rDoc, ScAddress(3,9,0), "MULTIPLE.OPERATIONS($C$8,$B$9,$C10,$B$10,D$8)", "Wrong formula!");
    ASSERT_FORMULA_EQUAL(rDoc, ScAddress(3,10,0), "MULTIPLE.OPERATIONS($C$8,$B$9,$C11,$B$10,D$8)", "Wrong formula!");
    ASSERT_FORMULA_EQUAL(rDoc, ScAddress(4,8,0), "MULTIPLE.OPERATIONS($C$8,$B$9,$C9,$B$10,E$8)", "Wrong formula!");
    ASSERT_FORMULA_EQUAL(rDoc, ScAddress(4,9,0), "MULTIPLE.OPERATIONS($C$8,$B$9,$C10,$B$10,E$8)", "Wrong formula!");
    ASSERT_FORMULA_EQUAL(rDoc, ScAddress(4,10,0), "MULTIPLE.OPERATIONS($C$8,$B$9,$C11,$B$10,E$8)", "Wrong formula!");

    xDocSh->DoClose();
}

void ScFiltersTest::testDataTableOneVarXLSX()
{
    ScDocShellRef xDocSh = loadDoc(u"data-table/one-variable.", FORMAT_XLSX);
    CPPUNIT_ASSERT_MESSAGE("Failed to load the document.", xDocSh.is());

    ScFormulaOptions aOptions;
    aOptions.SetFormulaSepArg(",");
    aOptions.SetFormulaSepArrayCol(",");
    aOptions.SetFormulaSepArrayRow(";");
    xDocSh->SetFormulaOptions(aOptions);

    ScDocument& rDoc = xDocSh->GetDocument();

    // Right now, we have a bug that prevents Calc from re-calculating these
    // cells automatically upon file load. We can remove this call if/when we
    // fix the aforementioned bug.
    rDoc.CalcAll();

    // B5:B11 should have multiple operations formula cells.  Just check the
    // top and bottom cells.

    ASSERT_FORMULA_EQUAL(rDoc, ScAddress(1,4,0), "MULTIPLE.OPERATIONS(B$4,$A$2,$A5)", "Wrong formula!");

    CPPUNIT_ASSERT_EQUAL(2.0, rDoc.GetValue(ScAddress(1,4,0)));

    ASSERT_FORMULA_EQUAL(rDoc, ScAddress(1,10,0), "MULTIPLE.OPERATIONS(B$4,$A$2,$A11)", "Wrong formula!");

    CPPUNIT_ASSERT_EQUAL(14.0, rDoc.GetValue(ScAddress(1,10,0)));

    // Likewise, E5:I5 should have multiple operations formula cells.  Just
    // check the left- and right-most cells.

    ASSERT_FORMULA_EQUAL(rDoc, ScAddress(4,4,0), "MULTIPLE.OPERATIONS($D5,$B$2,E$4)", "Wrong formula!");

    CPPUNIT_ASSERT_EQUAL(10.0, rDoc.GetValue(ScAddress(4,4,0)));

    ASSERT_FORMULA_EQUAL(rDoc, ScAddress(8,4,0), "MULTIPLE.OPERATIONS($D5,$B$2,I$4)", "Wrong formula!");

    CPPUNIT_ASSERT_EQUAL(50.0, rDoc.GetValue(ScAddress(8,4,0)));

    xDocSh->DoClose();
}

void ScFiltersTest::testDataTableMultiTableXLSX()
{
    ScDocShellRef xDocSh = loadDoc(u"data-table/multi-table.", FORMAT_XLSX);
    CPPUNIT_ASSERT_MESSAGE("Failed to load the document.", xDocSh.is());

    ScFormulaOptions aOptions;
    aOptions.SetFormulaSepArg(",");
    aOptions.SetFormulaSepArrayCol(",");
    aOptions.SetFormulaSepArrayRow(";");
    xDocSh->SetFormulaOptions(aOptions);

    ScDocument& rDoc = xDocSh->GetDocument();

    // Right now, we have a bug that prevents Calc from re-calculating these
    // cells automatically upon file load. We can remove this call if/when we
    // fix the aforementioned bug.
    rDoc.CalcAll();

    // B4:M15 should have multiple operations formula cells.  We'll just check
    // the top-left and bottom-right ones.

    ASSERT_FORMULA_EQUAL(rDoc, ScAddress(1,3,0), "MULTIPLE.OPERATIONS($A$3,$E$1,$A4,$D$1,B$3)", "Wrong formula!");

    CPPUNIT_ASSERT_EQUAL(1.0, rDoc.GetValue(ScAddress(1,3,0)));

    ASSERT_FORMULA_EQUAL(rDoc, ScAddress(12,14,0), "MULTIPLE.OPERATIONS($A$3,$E$1,$A15,$D$1,M$3)", "Wrong formula!");

    CPPUNIT_ASSERT_EQUAL(144.0, rDoc.GetValue(ScAddress(12,14,0)));

    xDocSh->DoClose();
}

void ScFiltersTest::testBrokenQuotesCSV()
{
    OUString aFileExtension(getFileFormats()[FORMAT_CSV].pName, strlen(getFileFormats()[FORMAT_CSV].pName), RTL_TEXTENCODING_UTF8 );
    OUString aFilterName(getFileFormats()[FORMAT_CSV].pFilterName, strlen(getFileFormats()[FORMAT_CSV].pFilterName), RTL_TEXTENCODING_UTF8) ;
    OUString aFileName;
    createFileURL(u"fdo48621_broken_quotes.", aFileExtension, aFileName);
    OUString aFilterType(getFileFormats()[FORMAT_CSV].pTypeName, strlen(getFileFormats()[FORMAT_CSV].pTypeName), RTL_TEXTENCODING_UTF8);
    std::cout << getFileFormats()[FORMAT_CSV].pName << " Test" << std::endl;

    SfxFilterFlags nFormatType = getFileFormats()[FORMAT_CSV].nFormatType;
    SotClipboardFormatId nClipboardId = bool(nFormatType) ? SotClipboardFormatId::STARCALC_8 : SotClipboardFormatId::NONE;
    ScDocShellRef xDocSh = ScBootstrapFixture::load(aFileName, aFilterName, OUString(), aFilterType,
        nFormatType, nClipboardId);

    CPPUNIT_ASSERT_MESSAGE("Failed to load fdo48621_broken_quotes.csv", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    OUString aCSVPath;
    createCSVPath( "fdo48621_broken_quotes_exported.", aCSVPath );
    // fdo#48621
    testFile( aCSVPath, rDoc, 0, StringType::PureString);

    xDocSh->DoClose();
}

void ScFiltersTest::testCellValueXLSX()
{
    const OUString aFileNameBase("cell-value.");
    OUString aFileExtension(getFileFormats()[FORMAT_XLSX].pName, strlen(getFileFormats()[FORMAT_XLSX].pName), RTL_TEXTENCODING_UTF8 );
    OUString aFilterName(getFileFormats()[FORMAT_XLSX].pFilterName, strlen(getFileFormats()[FORMAT_XLSX].pFilterName), RTL_TEXTENCODING_UTF8) ;
    OUString aFileName;
    createFileURL(aFileNameBase, aFileExtension, aFileName);
    OUString aFilterType(getFileFormats()[FORMAT_XLSX].pTypeName, strlen(getFileFormats()[FORMAT_XLSX].pTypeName), RTL_TEXTENCODING_UTF8);
    std::cout << getFileFormats()[FORMAT_XLSX].pName << " Test" << std::endl;

    SfxFilterFlags nFormatType = getFileFormats()[FORMAT_XLSX].nFormatType;
    SotClipboardFormatId nClipboardId = bool(nFormatType) ? SotClipboardFormatId::STARCALC_8 : SotClipboardFormatId::NONE;
    ScDocShellRef xDocSh = ScBootstrapFixture::load( aFileName, aFilterName, OUString(), aFilterType,
        nFormatType, nClipboardId);

    CPPUNIT_ASSERT_MESSAGE("Failed to load cell-value.xlsx", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    OUString aCSVPath;
    createCSVPath( aFileNameBase, aCSVPath );
    testFile( aCSVPath, rDoc, 0 );

    xDocSh->DoClose();
}

void ScFiltersTest::testRowIndex1BasedXLSX()
{
    ScDocShellRef xDocSh = loadDoc(u"row-index-1-based.", FORMAT_XLSX);
    CPPUNIT_ASSERT(xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    // A1
    OUString aStr = rDoc.GetString(ScAddress(0,0,0));
    CPPUNIT_ASSERT_EQUAL(OUString("Action Plan.Name"), aStr);

    // B1
    aStr = rDoc.GetString(ScAddress(1,0,0));
    CPPUNIT_ASSERT_EQUAL(OUString("Action Plan.Description"), aStr);

    // A2
    aStr = rDoc.GetString(ScAddress(0,1,0));
    CPPUNIT_ASSERT_EQUAL(OUString("Jerry"), aStr);

    // B2 - multi-line text.
    const EditTextObject* pText = rDoc.GetEditText(ScAddress(1,1,0));
    CPPUNIT_ASSERT(pText);
    CPPUNIT_ASSERT_EQUAL(static_cast<sal_Int32>(3), pText->GetParagraphCount());
    aStr = pText->GetText(0);
    CPPUNIT_ASSERT_EQUAL(OUString("This is a longer Text."), aStr);
    aStr = pText->GetText(1);
    CPPUNIT_ASSERT_EQUAL(OUString("Second line."), aStr);
    aStr = pText->GetText(2);
    CPPUNIT_ASSERT_EQUAL(OUString("Third line."), aStr);

    xDocSh->DoClose();
}

void ScFiltersTest::testPassword_Impl(std::u16string_view aFileNameBase)
{
    OUString aFileExtension(getFileFormats()[0].pName, strlen(getFileFormats()[0].pName), RTL_TEXTENCODING_UTF8 );
    OUString aFilterName(getFileFormats()[0].pFilterName, strlen(getFileFormats()[0].pFilterName), RTL_TEXTENCODING_UTF8) ;
    OUString aFileName;
    createFileURL(aFileNameBase, aFileExtension, aFileName);
    OUString aFilterType(getFileFormats()[0].pTypeName, strlen(getFileFormats()[0].pTypeName), RTL_TEXTENCODING_UTF8);

    auto pFilter = std::make_shared<SfxFilter>(
        aFilterName,
        OUString(), getFileFormats()[0].nFormatType,
        SotClipboardFormatId::STARCALC_8,
        aFilterType, OUString(),
        OUString(), "private:factory/scalc*" );
    pFilter->SetVersion(SOFFICE_FILEFORMAT_CURRENT);

    ScDocShellRef xDocSh = new ScDocShell;
    SfxMedium* pMedium = new SfxMedium(aFileName, StreamMode::STD_READWRITE);
    SfxItemSet* pSet = pMedium->GetItemSet();
    pSet->Put(SfxStringItem(SID_PASSWORD, "test"));
    pMedium->SetFilter(pFilter);
    if (!xDocSh->DoLoad(pMedium))
    {
        xDocSh->DoClose();
        // load failed.
        xDocSh.clear();
    }

    CPPUNIT_ASSERT_MESSAGE("Failed to load password.ods", xDocSh.is());
    xDocSh->DoClose();
}

void ScFiltersTest::testPasswordNew()
{
    //tests opening a file with new password algorithm
    testPassword_Impl(u"password.");
}

void ScFiltersTest::testPasswordOld()
{
    //tests opening a file with old password algorithm
    testPassword_Impl(u"passwordOld.");
}

void ScFiltersTest::testPasswordWrongSHA()
{
    //tests opening a file wrongly using the new password algorithm
    //in a sxc with the key algorithm missing
    testPassword_Impl(u"passwordWrongSHA.");
}

void ScFiltersTest::testControlImport()
{
    ScDocShellRef xDocSh = loadDoc(u"singlecontrol.", FORMAT_XLSX);
    CPPUNIT_ASSERT_MESSAGE("Failed to load singlecontrol.xlsx", xDocSh.is());

    uno::Reference< frame::XModel > xModel = xDocSh->GetModel();
    uno::Reference< sheet::XSpreadsheetDocument > xDoc(xModel, UNO_QUERY_THROW);
    uno::Reference< container::XIndexAccess > xIA(xDoc->getSheets(), UNO_QUERY_THROW);
    uno::Reference< drawing::XDrawPageSupplier > xDrawPageSupplier( xIA->getByIndex(0), UNO_QUERY_THROW);
    uno::Reference< container::XIndexAccess > xIA_DrawPage(xDrawPageSupplier->getDrawPage(), UNO_QUERY_THROW);
    uno::Reference< drawing::XControlShape > xControlShape(xIA_DrawPage->getByIndex(0), UNO_QUERY_THROW);

    xDocSh->DoClose();
}

void ScFiltersTest::testActiveXOptionButtonGroup()
{
    ScDocShellRef xDocSh = loadDoc(u"tdf111980_radioButtons.", FORMAT_XLSX);
    CPPUNIT_ASSERT_MESSAGE("Failed to load tdf111980_radioButtons.xlsx", xDocSh.is());
    uno::Reference< frame::XModel > xModel = xDocSh->GetModel();
    uno::Reference< sheet::XSpreadsheetDocument > xDoc(xModel, UNO_QUERY_THROW);
    uno::Reference< container::XIndexAccess > xIA(xDoc->getSheets(), UNO_QUERY_THROW);
    uno::Reference< drawing::XDrawPageSupplier > xDrawPageSupplier( xIA->getByIndex(0), UNO_QUERY_THROW);
    uno::Reference< container::XIndexAccess > xIA_DrawPage(xDrawPageSupplier->getDrawPage(), UNO_QUERY_THROW);

    OUString sGroupName;
    uno::Reference< drawing::XControlShape > xControlShape(xIA_DrawPage->getByIndex(0), UNO_QUERY_THROW);
    uno::Reference<beans::XPropertySet> xPropertySet(xControlShape->getControl(), uno::UNO_QUERY_THROW);
    xPropertySet->getPropertyValue("GroupName") >>= sGroupName;
    CPPUNIT_ASSERT_EQUAL(OUString("Sheet1"), sGroupName);

    // Optionbuttons (without Group names) were not grouped.
    // The two optionbuttons should have the same auto-generated group name.
    OUString sGroupName2; //ActiveX controls
    xControlShape.set(xIA_DrawPage->getByIndex(2), uno::UNO_QUERY_THROW);
    xPropertySet.set(xControlShape->getControl(), uno::UNO_QUERY_THROW);
    xPropertySet->getPropertyValue("GroupName") >>= sGroupName2;
    CPPUNIT_ASSERT_EQUAL( false, sGroupName2.isEmpty() );

    OUString sGroupName3;
    xControlShape.set(xIA_DrawPage->getByIndex(3), uno::UNO_QUERY_THROW);
    xPropertySet.set(xControlShape->getControl(), uno::UNO_QUERY_THROW);
    xPropertySet->getPropertyValue("GroupName") >>= sGroupName3;
    CPPUNIT_ASSERT_EQUAL( sGroupName2, sGroupName3 );
    CPPUNIT_ASSERT( sGroupName != sGroupName3 );

    OUString sGroupName4; //Form controls
    xControlShape.set(xIA_DrawPage->getByIndex(4), uno::UNO_QUERY_THROW);
    xPropertySet.set(xControlShape->getControl(), uno::UNO_QUERY_THROW);
    xPropertySet->getPropertyValue("GroupName") >>= sGroupName4;
    CPPUNIT_ASSERT_EQUAL( false, sGroupName4.isEmpty() );

    OUString sGroupName5;
    xControlShape.set(xIA_DrawPage->getByIndex(5), uno::UNO_QUERY_THROW);
    xPropertySet.set(xControlShape->getControl(), uno::UNO_QUERY_THROW);
    xPropertySet->getPropertyValue("GroupName") >>= sGroupName5;
    CPPUNIT_ASSERT_EQUAL( sGroupName4, sGroupName5 );
    CPPUNIT_ASSERT( sGroupName2 != sGroupName5 );
    CPPUNIT_ASSERT( sGroupName != sGroupName5 );

    OUString sGroupName7; //Form radiobutton autogrouped by GroupBox
    xControlShape.set(xIA_DrawPage->getByIndex(7), uno::UNO_QUERY_THROW);
    xPropertySet.set(xControlShape->getControl(), uno::UNO_QUERY_THROW);
    xPropertySet->getPropertyValue("GroupName") >>= sGroupName7;
    CPPUNIT_ASSERT_EQUAL( OUString("autoGroup_Group Box 7"), sGroupName7 );

    OUString sGroupName8;
    xControlShape.set(xIA_DrawPage->getByIndex(8), uno::UNO_QUERY_THROW);
    xPropertySet.set(xControlShape->getControl(), uno::UNO_QUERY_THROW);
    xPropertySet->getPropertyValue("GroupName") >>= sGroupName8;
    CPPUNIT_ASSERT_EQUAL( sGroupName7, sGroupName8 );
    CPPUNIT_ASSERT( sGroupName4 != sGroupName8 );
    CPPUNIT_ASSERT( sGroupName2 != sGroupName8 );
    CPPUNIT_ASSERT( sGroupName != sGroupName8 );

    OUString sGroupName9; //Form radiobutton not fully inside GroupBox
    xControlShape.set(xIA_DrawPage->getByIndex(9), uno::UNO_QUERY_THROW);
    xPropertySet.set(xControlShape->getControl(), uno::UNO_QUERY_THROW);
    xPropertySet->getPropertyValue("GroupName") >>= sGroupName9;
    CPPUNIT_ASSERT_EQUAL( sGroupName4, sGroupName9 );

    OUString sGroupName10; //ActiveX unaffected by GroupBox
    xControlShape.set(xIA_DrawPage->getByIndex(10), uno::UNO_QUERY_THROW);
    xPropertySet.set(xControlShape->getControl(), uno::UNO_QUERY_THROW);
    xPropertySet->getPropertyValue("GroupName") >>= sGroupName10;
    CPPUNIT_ASSERT_EQUAL( sGroupName, sGroupName10 );

    xDocSh->DoClose();
}

void ScFiltersTest::testChartImportODS()
{
    ScDocShellRef xDocSh = loadDoc(u"chart-import-basic.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to load chart-import-basic.ods.", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();

    // Ensure that the document contains "Empty", "Chart", "Data" and "Title" sheets in this exact order.
    CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "There should be 4 sheets in this document.", sal_Int16(4),
        rDoc.GetTableCount());
    OUString aName;
    rDoc.GetName(0, aName);
    CPPUNIT_ASSERT_EQUAL(OUString("Empty"), aName);
    rDoc.GetName(1, aName);
    CPPUNIT_ASSERT_EQUAL(OUString("Chart"), aName);
    rDoc.GetName(2, aName);
    CPPUNIT_ASSERT_EQUAL(OUString("Data"), aName);
    rDoc.GetName(3, aName);
    CPPUNIT_ASSERT_EQUAL(OUString("Title"), aName);

    // Retrieve the chart object instance from the 2nd page (for the 2nd sheet).
    const SdrOle2Obj* pOleObj = getSingleChartObject(rDoc, 1);
    CPPUNIT_ASSERT_MESSAGE("Failed to retrieve a chart object from the 2nd sheet.", pOleObj);

    ScRangeList aRanges = getChartRanges(rDoc, *pOleObj);

    CPPUNIT_ASSERT_MESSAGE("Data series title cell not found.", aRanges.In(ScAddress(1,0,3))); // B1 on Title
    CPPUNIT_ASSERT_MESSAGE("Data series label range not found.", aRanges.In(ScRange(0,1,2,0,3,2))); // A2:A4 on Data
    CPPUNIT_ASSERT_MESSAGE("Data series value range not found.", aRanges.In(ScRange(1,1,2,1,3,2))); // B2:B4 on Data

    xDocSh->DoClose();
}

#if HAVE_MORE_FONTS
void ScFiltersTest::testChartImportXLS()
{
    ScDocShellRef xDocSh = loadDoc(u"chartx.", FORMAT_XLS);
    CPPUNIT_ASSERT_MESSAGE("Failed to load chartx.xls.", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();

    // Retrieve the chart object instance from the 2nd page (for the 2nd sheet).
    const SdrOle2Obj* pOleObj = getSingleChartObject(rDoc, 0);
    CPPUNIT_ASSERT_MESSAGE("Failed to retrieve a chart object from the 2nd sheet.", pOleObj);

    CPPUNIT_ASSERT_EQUAL(tools::Long(11137), pOleObj->GetLogicRect().getWidth());
    CPPUNIT_ASSERT(8640L > pOleObj->GetLogicRect().getHeight());

    xDocSh->DoClose();
}
#endif

void ScFiltersTest::testNumberFormatHTML()
{
    ScDocShellRef xDocSh = loadDoc(u"numberformat.", FORMAT_HTML);
    CPPUNIT_ASSERT_MESSAGE("Failed to load numberformat.html", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();

    // Check the header just in case.
    CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "Cell value is not as expected", OUString("Product"),
        rDoc.GetString(0, 0, 0));
    CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "Cell value is not as expected", OUString("Price"),
        rDoc.GetString(1, 0, 0));
    CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "Cell value is not as expected", OUString("Note"),
        rDoc.GetString(2, 0, 0));

    // B2 should be imported as a value cell.
    bool bHasValue = rDoc.HasValueData(1, 1, 0);
    CPPUNIT_ASSERT_MESSAGE("Fail to import number as a value cell.", bHasValue);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "Incorrect value.", 199.98, rDoc.GetValue(1, 1, 0));

    xDocSh->DoClose();
}

void ScFiltersTest::testNumberFormatCSV()
{
    ScDocShellRef xDocSh = loadDoc(u"numberformat.", FORMAT_CSV);
    CPPUNIT_ASSERT_MESSAGE("Failed to load numberformat.csv", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();

    // Check the header just in case.
    CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "Cell value is not as expected", OUString("Product"),
        rDoc.GetString(0, 0, 0));
    CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "Cell value is not as expected", OUString("Price"),
        rDoc.GetString(1, 0, 0));
    CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "Cell value is not as expected", OUString("Note"),
        rDoc.GetString(2, 0, 0));

    // B2 should be imported as a value cell.
    bool bHasValue = rDoc.HasValueData(1, 1, 0);
    CPPUNIT_ASSERT_MESSAGE("Fail to import number as a value cell.", bHasValue);
    CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "Incorrect value.", 199.98, rDoc.GetValue(1, 1, 0));

    xDocSh->DoClose();
}

void ScFiltersTest::testCellAnchoredShapesODS()
{
    ScDocShellRef xDocSh = loadDoc(u"cell-anchored-shapes.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to load cell-anchored-shapes.ods", xDocSh.is());

    // There are two cell-anchored objects on the first sheet.
    ScDocument& rDoc = xDocSh->GetDocument();

    CPPUNIT_ASSERT_MESSAGE("There should be at least one sheet.", rDoc.GetTableCount() > 0);

    ScDrawLayer* pDrawLayer = rDoc.GetDrawLayer();
    SdrPage* pPage = pDrawLayer->GetPage(0);
    CPPUNIT_ASSERT_MESSAGE("draw page for sheet 1 should exist.", pPage);
    const size_t nCount = pPage->GetObjCount();
    CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "There should be 2 objects.", static_cast<size_t>(2), nCount);
    for (size_t i = 0; i < nCount; ++i)
    {
        SdrObject* pObj = pPage->GetObj(i);
        CPPUNIT_ASSERT_MESSAGE("Failed to get drawing object.", pObj);
        ScDrawObjData* pData = ScDrawLayer::GetObjData(pObj);
        CPPUNIT_ASSERT_MESSAGE("Failed to retrieve user data for this object.", pData);
        CPPUNIT_ASSERT_MESSAGE("Bounding rectangle should have been calculated upon import.", !pData->getShapeRect().IsEmpty());
    }

    xDocSh->DoClose();
}

void ScFiltersTest::testCellAnchoredHiddenShapesXLSX()
{
    ScDocShellRef xDocSh = loadDoc(u"cell-anchored-hidden-shapes.", FORMAT_XLSX);
    CPPUNIT_ASSERT_MESSAGE("Failed to load cell-anchored-shapes.xlsx", xDocSh.is());

    // There are two cell-anchored objects on the first sheet.
    ScDocument& rDoc = xDocSh->GetDocument();

    CPPUNIT_ASSERT_MESSAGE("There should be at least one sheet.", rDoc.GetTableCount() > 0);

    ScDrawLayer* pDrawLayer = rDoc.GetDrawLayer();
    SdrPage* pPage = pDrawLayer->GetPage(0);
    CPPUNIT_ASSERT_MESSAGE("draw page for sheet 1 should exist.", pPage);
    const size_t nCount = pPage->GetObjCount();
    CPPUNIT_ASSERT_MESSAGE("There should be 2 shapes.", !(nCount == 2));

    SdrObject* pObj = pPage->GetObj(1);
    CPPUNIT_ASSERT_MESSAGE("Failed to get drawing object.", pObj);
    CPPUNIT_ASSERT_MESSAGE("The shape having same twocellanchor from and to attribute values, is visible.", !pObj->IsVisible());

    xDocSh->DoClose();
}

void ScFiltersTest::testRowHeightODS()
{
    ScDocShellRef xDocSh = loadDoc(u"row-height-import.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to load row-height-import.ods", xDocSh.is());

    SCTAB nTab = 0;
    SCROW nRow = 0;
    ScDocument& rDoc = xDocSh->GetDocument();

    // The first 3 rows have manual heights.
    int nHeight = rDoc.GetRowHeight(nRow, nTab, false);
    bool bManual = rDoc.IsManualRowHeight(nRow, nTab);
    CPPUNIT_ASSERT_EQUAL(600, nHeight);
    CPPUNIT_ASSERT_MESSAGE("this row should have a manual row height.", bManual);
    nHeight = rDoc.GetRowHeight(++nRow, nTab, false);
    bManual = rDoc.IsManualRowHeight(nRow, nTab);
    CPPUNIT_ASSERT_EQUAL(1200, nHeight);
    CPPUNIT_ASSERT_MESSAGE("this row should have a manual row height.", bManual);
    nHeight = rDoc.GetRowHeight(++nRow, nTab, false);
    bManual = rDoc.IsManualRowHeight(nRow, nTab);
    CPPUNIT_ASSERT_EQUAL(1800, nHeight);
    CPPUNIT_ASSERT_MESSAGE("this row should have a manual row height.", bManual);

    // This one should have an automatic row height.
    bManual = rDoc.IsManualRowHeight(++nRow, nTab);
    CPPUNIT_ASSERT_MESSAGE("Row should have an automatic height.", !bManual);

    // Followed by a row with manual height.
    nHeight = rDoc.GetRowHeight(++nRow, nTab, false);
    bManual = rDoc.IsManualRowHeight(nRow, nTab);
    CPPUNIT_ASSERT_EQUAL(2400, nHeight);
    CPPUNIT_ASSERT_MESSAGE("this row should have a manual row height.", bManual);

    // And all the rest should have automatic heights.
    bManual = rDoc.IsManualRowHeight(++nRow, nTab);
    CPPUNIT_ASSERT_MESSAGE("Row should have an automatic height.", !bManual);

    bManual = rDoc.IsManualRowHeight(MAXROW, nTab);
    CPPUNIT_ASSERT_MESSAGE("Row should have an automatic height.", !bManual);

    xDocSh->DoClose();
}

void ScFiltersTest::testRichTextContentODS()
{
    ScDocShellRef xDocSh = loadDoc(u"rich-text-cells.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to load rich-text-cells.ods", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    OUString aTabName;
    CPPUNIT_ASSERT_MESSAGE("Failed to get the name of the first sheet.", rDoc.GetName(0, aTabName));

    // All tested cells are in the first column.
    ScAddress aPos(0, 0, 0);

    // Normal simple string with no formatting.
    aPos.IncRow();
    CPPUNIT_ASSERT_EQUAL(CELLTYPE_STRING, rDoc.GetCellType(aPos));
    CPPUNIT_ASSERT_EQUAL(OUString("Normal"), rDoc.GetString(aPos.Col(), aPos.Row(), aPos.Tab()));

    // Normal string with bold applied to the whole cell.
    {
        aPos.IncRow();
        CPPUNIT_ASSERT_EQUAL(CELLTYPE_STRING, rDoc.GetCellType(aPos));
        CPPUNIT_ASSERT_EQUAL(OUString("All bold"), rDoc.GetString(aPos.Col(), aPos.Row(), aPos.Tab()));
        const ScPatternAttr* pAttr = rDoc.GetPattern(aPos.Col(), aPos.Row(), aPos.Tab());
        CPPUNIT_ASSERT_MESSAGE("Failed to get cell attribute.", pAttr);
        const SvxWeightItem& rWeightItem = pAttr->GetItem(ATTR_FONT_WEIGHT);
        CPPUNIT_ASSERT_EQUAL(WEIGHT_BOLD, rWeightItem.GetWeight());
    }

    // This cell has an unformatted but multi-line content. Multi-line text is
    // stored in edit cell even if it has no formatting applied.
    aPos.IncRow();
    CPPUNIT_ASSERT_EQUAL(CELLTYPE_EDIT, rDoc.GetCellType(aPos));
    const EditTextObject* pEditText = rDoc.GetEditText(aPos);
    CPPUNIT_ASSERT_MESSAGE("Failed to retrieve edit text object.", pEditText);
    CPPUNIT_ASSERT_EQUAL(static_cast<sal_Int32>(3), pEditText->GetParagraphCount());
    OUString aParaText = pEditText->GetText(0);
    CPPUNIT_ASSERT_EQUAL(OUString("one"), aParaText);
    aParaText = pEditText->GetText(1);
    CPPUNIT_ASSERT_EQUAL(OUString("two"), aParaText);
    aParaText = pEditText->GetText(2);
    CPPUNIT_ASSERT_EQUAL(OUString("three"), aParaText);

    // Cell with sheet name field item.
    aPos.IncRow();
    CPPUNIT_ASSERT_EQUAL(CELLTYPE_EDIT, rDoc.GetCellType(aPos));
    pEditText = rDoc.GetEditText(aPos);
    CPPUNIT_ASSERT_MESSAGE("Failed to retrieve edit text object.", pEditText);
    CPPUNIT_ASSERT_EQUAL(static_cast<sal_Int32>(1), pEditText->GetParagraphCount());
    aParaText = pEditText->GetText(0);
    CPPUNIT_ASSERT_MESSAGE("Unexpected text.", aParaText.startsWith("Sheet name is "));
    CPPUNIT_ASSERT_MESSAGE("Sheet name field item not found.", pEditText->HasField(text::textfield::Type::TABLE));
    CPPUNIT_ASSERT_EQUAL(OUString("Sheet name is Test."), ScEditUtil::GetString(*pEditText, &rDoc));
    CPPUNIT_ASSERT_EQUAL(OUString("Sheet name is ?."), ScEditUtil::GetString(*pEditText, nullptr));

    // Cell with URL field item.
    aPos.IncRow();
    CPPUNIT_ASSERT_EQUAL(CELLTYPE_EDIT, rDoc.GetCellType(aPos));
    pEditText = rDoc.GetEditText(aPos);
    CPPUNIT_ASSERT_MESSAGE("Failed to retrieve edit text object.", pEditText);
    CPPUNIT_ASSERT_EQUAL(static_cast<sal_Int32>(1), pEditText->GetParagraphCount());
    aParaText = pEditText->GetText(0);
    CPPUNIT_ASSERT_MESSAGE("Unexpected text.", aParaText.startsWith("URL: "));
    CPPUNIT_ASSERT_MESSAGE("URL field item not found.", pEditText->HasField(text::textfield::Type::URL));
    CPPUNIT_ASSERT_EQUAL(OUString("URL: http://libreoffice.org"), ScEditUtil::GetString(*pEditText, &rDoc));
    CPPUNIT_ASSERT_EQUAL(OUString("URL: http://libreoffice.org"), ScEditUtil::GetString(*pEditText, nullptr));

    // Cell with Date field item.
    aPos.IncRow();
    CPPUNIT_ASSERT_EQUAL(CELLTYPE_EDIT, rDoc.GetCellType(aPos));
    pEditText = rDoc.GetEditText(aPos);
    CPPUNIT_ASSERT_MESSAGE("Failed to retrieve edit text object.", pEditText);
    CPPUNIT_ASSERT_EQUAL(static_cast<sal_Int32>(1), pEditText->GetParagraphCount());
    aParaText = pEditText->GetText(0);
    CPPUNIT_ASSERT_MESSAGE("Unexpected text.", aParaText.startsWith("Date: "));
    CPPUNIT_ASSERT_MESSAGE("Date field item not found.", pEditText->HasField(text::textfield::Type::DATE));
    CPPUNIT_ASSERT_MESSAGE("Date field not resolved with rDoc.", ScEditUtil::GetString(*pEditText, &rDoc).indexOf("/20") > 0);
    CPPUNIT_ASSERT_MESSAGE("Date field not resolved with NULL.", ScEditUtil::GetString(*pEditText, nullptr).indexOf("/20") > 0);

    // Cell with DocInfo title field item.
    aPos.IncRow();
    CPPUNIT_ASSERT_EQUAL(CELLTYPE_EDIT, rDoc.GetCellType(aPos));
    pEditText = rDoc.GetEditText(aPos);
    CPPUNIT_ASSERT_MESSAGE("Failed to retrieve edit text object.", pEditText);
    CPPUNIT_ASSERT_EQUAL(static_cast<sal_Int32>(1), pEditText->GetParagraphCount());
    aParaText = pEditText->GetText(0);
    CPPUNIT_ASSERT_MESSAGE("Unexpected text.", aParaText.startsWith("Title: "));
    CPPUNIT_ASSERT_MESSAGE("DocInfo title field item not found.", pEditText->HasField(text::textfield::Type::DOCINFO_TITLE));
    CPPUNIT_ASSERT_EQUAL(OUString("Title: Test Document"), ScEditUtil::GetString(*pEditText, &rDoc));
    CPPUNIT_ASSERT_EQUAL(OUString("Title: ?"), ScEditUtil::GetString(*pEditText, nullptr));

    // Cell with sentence with both bold and italic sequences.
    aPos.IncRow();
    CPPUNIT_ASSERT_EQUAL(CELLTYPE_EDIT, rDoc.GetCellType(aPos));
    pEditText = rDoc.GetEditText(aPos);
    CPPUNIT_ASSERT_MESSAGE("Failed to retrieve edit text object.", pEditText);
    CPPUNIT_ASSERT_EQUAL(static_cast<sal_Int32>(1), pEditText->GetParagraphCount());
    aParaText = pEditText->GetText(0);
    CPPUNIT_ASSERT_EQUAL(OUString("Sentence with bold and italic."), aParaText);
    std::vector<EECharAttrib> aAttribs;
    pEditText->GetCharAttribs(0, aAttribs);
    {
        bool bHasBold = false, bHasItalic = false;
        for (const auto& rAttrib : aAttribs)
        {
            OUString aSeg = aParaText.copy(rAttrib.nStart, rAttrib.nEnd - rAttrib.nStart);
            const SfxPoolItem* pAttr = rAttrib.pAttr;
            if (aSeg == "bold" && pAttr->Which() == EE_CHAR_WEIGHT && !bHasBold)
            {
                const SvxWeightItem& rItem = static_cast<const SvxWeightItem&>(*pAttr);
                bHasBold = (rItem.GetWeight() == WEIGHT_BOLD);
            }
            else if (aSeg == "italic" && pAttr->Which() == EE_CHAR_ITALIC && !bHasItalic)
            {
                const SvxPostureItem& rItem = static_cast<const SvxPostureItem&>(*pAttr);
                bHasItalic = (rItem.GetPosture() == ITALIC_NORMAL);
            }
        }
        CPPUNIT_ASSERT_MESSAGE("This sentence is expected to have both bold and italic sequences.", bHasBold && bHasItalic);
    }

    // Cell with multi-line content with formatting applied.
    aPos.IncRow();
    CPPUNIT_ASSERT_EQUAL(CELLTYPE_EDIT, rDoc.GetCellType(aPos));
    pEditText = rDoc.GetEditText(aPos);
    CPPUNIT_ASSERT_MESSAGE("Failed to retrieve edit text object.", pEditText);
    CPPUNIT_ASSERT_EQUAL(static_cast<sal_Int32>(3), pEditText->GetParagraphCount());
    aParaText = pEditText->GetText(0);
    CPPUNIT_ASSERT_EQUAL(OUString("bold"), aParaText);
    aParaText = pEditText->GetText(1);
    CPPUNIT_ASSERT_EQUAL(OUString("italic"), aParaText);
    aParaText = pEditText->GetText(2);
    CPPUNIT_ASSERT_EQUAL(OUString("underlined"), aParaText);

    // first line is bold.
    pEditText->GetCharAttribs(0, aAttribs);
    {
        bool bHasBold = std::any_of(aAttribs.begin(), aAttribs.end(), [](const EECharAttrib& rAttrib) {
            return rAttrib.pAttr->Which() == EE_CHAR_WEIGHT &&
                static_cast<const SvxWeightItem&>(*rAttrib.pAttr).GetWeight() == WEIGHT_BOLD; });
        CPPUNIT_ASSERT_MESSAGE("First line should be bold.", bHasBold);
    }

    // second line is italic.
    pEditText->GetCharAttribs(1, aAttribs);
    bool bHasItalic = std::any_of(aAttribs.begin(), aAttribs.end(), [](const EECharAttrib& rAttrib) {
            return rAttrib.pAttr->Which() == EE_CHAR_ITALIC &&
                static_cast<const SvxPostureItem&>(*rAttrib.pAttr).GetPosture() == ITALIC_NORMAL; });
    CPPUNIT_ASSERT_MESSAGE("Second line should be italic.", bHasItalic);

    // third line is underlined.
    pEditText->GetCharAttribs(2, aAttribs);
    bool bHasUnderline = std::any_of(aAttribs.begin(), aAttribs.end(), [](const EECharAttrib& rAttrib) {
        return rAttrib.pAttr->Which() == EE_CHAR_UNDERLINE &&
            static_cast<const SvxUnderlineItem&>(*rAttrib.pAttr).GetLineStyle() == LINESTYLE_SINGLE; });
    CPPUNIT_ASSERT_MESSAGE("Second line should be underlined.", bHasUnderline);

    // URL with formats applied.  For now, we'll check whether or not the
    // field objects gets imported.  Later we should add checks for the
    // formats.
    aPos.IncRow();
    pEditText = rDoc.GetEditText(aPos);
    CPPUNIT_ASSERT_MESSAGE("Failed to retrieve edit text object.", pEditText);
    CPPUNIT_ASSERT_MESSAGE("URL field item not found.", pEditText->HasField(text::textfield::Type::URL));

    // Sheet name with formats applied.
    aPos.IncRow();
    pEditText = rDoc.GetEditText(aPos);
    CPPUNIT_ASSERT_MESSAGE("Failed to retrieve edit text object.", pEditText);
    CPPUNIT_ASSERT_MESSAGE("Sheet name field item not found.", pEditText->HasField(text::textfield::Type::TABLE));

    // Date with formats applied.
    aPos.IncRow();
    pEditText = rDoc.GetEditText(aPos);
    CPPUNIT_ASSERT_MESSAGE("Failed to retrieve edit text object.", pEditText);
    CPPUNIT_ASSERT_MESSAGE("Date field item not found.", pEditText->HasField(text::textfield::Type::DATE));

    // Document title with formats applied.
    aPos.IncRow();
    pEditText = rDoc.GetEditText(aPos);
    CPPUNIT_ASSERT_MESSAGE("Failed to retrieve edit text object.", pEditText);
    CPPUNIT_ASSERT_MESSAGE("Date field item not found.", pEditText->HasField(text::textfield::Type::DOCINFO_TITLE));

    // URL for a file in the same directory. It should be converted into an absolute URL on import.
    aPos.IncRow();
    pEditText = rDoc.GetEditText(aPos);
    CPPUNIT_ASSERT_MESSAGE("Failed to retrieve edit text object.", pEditText);
    const SvxFieldData* pData = pEditText->GetFieldData(0, 0, text::textfield::Type::URL);
    CPPUNIT_ASSERT_MESSAGE("Failed to get the URL data.", pData && pData->GetClassId() == text::textfield::Type::URL);
    const SvxURLField* pURLData = static_cast<const SvxURLField*>(pData);
    CPPUNIT_ASSERT_MESSAGE("URL is not absolute with respect to the file system.", pURLData->GetURL().startsWith("file:///"));

    // Embedded spaces as <text:s text:c='4' />, normal text
    aPos.IncRow();
    CPPUNIT_ASSERT_EQUAL(CELLTYPE_STRING, rDoc.GetCellType(aPos));
    CPPUNIT_ASSERT_EQUAL(OUString("one     two"), rDoc.GetString(aPos.Col(), aPos.Row(), aPos.Tab()));

    // Leading space as <text:s />.
    aPos.IncRow();
    CPPUNIT_ASSERT_EQUAL(CELLTYPE_STRING, rDoc.GetCellType(aPos));
    CPPUNIT_ASSERT_EQUAL(OUString(" =3+4"), rDoc.GetString(aPos.Col(), aPos.Row(), aPos.Tab()));

    // Embedded spaces with <text:s text:c='4' /> inside a <text:span>, text
    // partly bold.
    aPos.IncRow();
    CPPUNIT_ASSERT_EQUAL(CELLTYPE_EDIT, rDoc.GetCellType(aPos));
    pEditText = rDoc.GetEditText(aPos);
    CPPUNIT_ASSERT_MESSAGE("Failed to retrieve edit text object.", pEditText);
    CPPUNIT_ASSERT_EQUAL(static_cast<sal_Int32>(1), pEditText->GetParagraphCount());
    aParaText = pEditText->GetText(0);
    CPPUNIT_ASSERT_EQUAL(OUString("one     two"), aParaText);
    pEditText->GetCharAttribs(0, aAttribs);
    {
        auto it = std::find_if(aAttribs.begin(), aAttribs.end(), [](const EECharAttrib& rAttrib) {
            return rAttrib.pAttr->Which() == EE_CHAR_WEIGHT &&
                static_cast<const SvxWeightItem&>(*rAttrib.pAttr).GetWeight() == WEIGHT_BOLD; });
        bool bHasBold = (it != aAttribs.end());
        if (bHasBold)
        {
            OUString aSeg = aParaText.copy(it->nStart, it->nEnd - it->nStart);
            CPPUNIT_ASSERT_EQUAL(OUString("e     t"), aSeg);
        }
        CPPUNIT_ASSERT_MESSAGE("Expected a bold sequence.", bHasBold);
    }

    xDocSh->DoClose();
}

void ScFiltersTest::testDataBarODS()
{
    ScDocShellRef xDocSh = loadDoc(u"databar.", FORMAT_ODS);
    CPPUNIT_ASSERT(xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();
    testDataBar_Impl(rDoc);

    xDocSh->DoClose();
}

void ScFiltersTest::testDataBarXLSX()
{
    ScDocShellRef xDocSh = loadDoc(u"databar.", FORMAT_XLSX);
    CPPUNIT_ASSERT(xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();
    testDataBar_Impl(rDoc);

    xDocSh->DoClose();
}

void ScFiltersTest::testColorScaleODS()
{
    ScDocShellRef xDocSh = loadDoc(u"colorscale.", FORMAT_ODS);
    CPPUNIT_ASSERT(xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    testColorScale2Entry_Impl(rDoc);
    testColorScale3Entry_Impl(rDoc);

    xDocSh->DoClose();
}

void ScFiltersTest::testColorScaleXLSX()
{
    ScDocShellRef xDocSh = loadDoc(u"colorscale.", FORMAT_XLSX);
    CPPUNIT_ASSERT(xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    testColorScale2Entry_Impl(rDoc);
    testColorScale3Entry_Impl(rDoc);

    xDocSh->DoClose();
}

void ScFiltersTest::testNewCondFormatODS()
{
    ScDocShellRef xDocSh = ScBootstrapFixture::loadDoc(u"new_cond_format_test.", FORMAT_ODS);

    CPPUNIT_ASSERT_MESSAGE("Failed to load new_cond_format_test.ods", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();

    OUString aCSVPath;
    createCSVPath( "new_cond_format_test.", aCSVPath );
    testCondFile(aCSVPath, &rDoc, 0);

    xDocSh->DoClose();
}

void ScFiltersTest::testNewCondFormatXLSX()
{
    ScDocShellRef xDocSh = ScBootstrapFixture::loadDoc(u"new_cond_format_test.", FORMAT_XLSX);

    CPPUNIT_ASSERT_MESSAGE("Failed to load new_cond_format_test.xlsx", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();

    OUString aCSVPath;
    createCSVPath( "new_cond_format_test.", aCSVPath );
    testCondFile(aCSVPath, &rDoc, 0);

    xDocSh->DoClose();
}

void ScFiltersTest::testCondFormatImportCellIs()
{
    ScDocShellRef xDocSh = ScBootstrapFixture::loadDoc(u"condFormat_cellis.", FORMAT_XLSX);
    CPPUNIT_ASSERT_MESSAGE("Failed to load condFormat_cellis.xlsx", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();
    CPPUNIT_ASSERT_EQUAL(size_t(1), rDoc.GetCondFormList(0)->size());

    ScConditionalFormat* pFormat = rDoc.GetCondFormat(0, 0, 0);
    CPPUNIT_ASSERT(pFormat);

    const ScFormatEntry* pEntry = pFormat->GetEntry(0);
    CPPUNIT_ASSERT(pEntry);
    CPPUNIT_ASSERT_EQUAL(ScFormatEntry::Type::ExtCondition, pEntry->GetType());

    const ScCondFormatEntry* pCondition = static_cast<const ScCondFormatEntry*>(pEntry);
    CPPUNIT_ASSERT_EQUAL( ScConditionMode::Equal,  pCondition->GetOperation());

    OUString aStr = pCondition->GetExpression(ScAddress(0, 0, 0), 0);
    CPPUNIT_ASSERT_EQUAL( OUString("$Sheet2.$A$1"), aStr );

    pEntry = pFormat->GetEntry(1);
    CPPUNIT_ASSERT(pEntry);
    CPPUNIT_ASSERT_EQUAL(ScFormatEntry::Type::ExtCondition, pEntry->GetType());

    pCondition = static_cast<const ScCondFormatEntry*>(pEntry);
    CPPUNIT_ASSERT_EQUAL( ScConditionMode::Equal,  pCondition->GetOperation());

    aStr = pCondition->GetExpression(ScAddress(0, 0, 0), 0);
    CPPUNIT_ASSERT_EQUAL( OUString("$Sheet2.$A$2"), aStr );

    xDocSh->DoClose();
}

void ScFiltersTest::testCondFormatThemeColorXLSX()
{
    ScDocShellRef xDocSh = ScBootstrapFixture::loadDoc(u"condformat_theme_color.", FORMAT_XLSX);

    CPPUNIT_ASSERT_MESSAGE("Failed to load condformat_theme_color.xlsx", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();
    ScConditionalFormat* pFormat = rDoc.GetCondFormat(0, 0, 0);
    const ScFormatEntry* pEntry = pFormat->GetEntry(0);
    CPPUNIT_ASSERT(pEntry);
    CPPUNIT_ASSERT_EQUAL(ScFormatEntry::Type::Databar, pEntry->GetType());
    const ScDataBarFormat* pDataBar = static_cast<const ScDataBarFormat*>(pEntry);
    const ScDataBarFormatData* pDataBarFormatData = pDataBar->GetDataBarData();

    CPPUNIT_ASSERT_EQUAL(Color(157, 195, 230), pDataBarFormatData->maPositiveColor);
    CPPUNIT_ASSERT(pDataBarFormatData->mxNegativeColor);
    CPPUNIT_ASSERT_EQUAL(COL_LIGHTRED, *pDataBarFormatData->mxNegativeColor);

    CPPUNIT_ASSERT_EQUAL(size_t(1), rDoc.GetCondFormList(1)->size());
    pFormat = rDoc.GetCondFormat(0, 0, 1);
    CPPUNIT_ASSERT(pFormat);
    CPPUNIT_ASSERT_EQUAL(size_t(1), pFormat->size());
    pEntry = pFormat->GetEntry(0);
    CPPUNIT_ASSERT(pEntry);
    CPPUNIT_ASSERT_EQUAL(ScFormatEntry::Type::Colorscale, pEntry->GetType());
    const ScColorScaleFormat* pColorScale = static_cast<const ScColorScaleFormat*>(pEntry);
    CPPUNIT_ASSERT_EQUAL(size_t(2), pColorScale->size());
    const ScColorScaleEntry* pColorScaleEntry = pColorScale->GetEntry(0);
    CPPUNIT_ASSERT(pColorScaleEntry);
    CPPUNIT_ASSERT_EQUAL(Color(255, 230, 153), pColorScaleEntry->GetColor());

    pColorScaleEntry = pColorScale->GetEntry(1);
    CPPUNIT_ASSERT(pColorScaleEntry);
    CPPUNIT_ASSERT_EQUAL(Color(157, 195, 230), pColorScaleEntry->GetColor());

    xDocSh->DoClose();
}

void ScFiltersTest::testCondFormatThemeColor2XLSX()
{
    ScDocShellRef xDocSh = ScBootstrapFixture::loadDoc(u"cond_format_theme_color2.", FORMAT_XLSX);

    CPPUNIT_ASSERT_MESSAGE("Failed to load cond_format_theme_color2.xlsx", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();
    ScConditionalFormat* pFormat = rDoc.GetCondFormat(5, 5, 0);
    CPPUNIT_ASSERT(pFormat);
    const ScFormatEntry* pEntry = pFormat->GetEntry(0);
    CPPUNIT_ASSERT(pEntry);
    CPPUNIT_ASSERT_EQUAL(ScFormatEntry::Type::Databar, pEntry->GetType());
    const ScDataBarFormat* pDataBar = static_cast<const ScDataBarFormat*>(pEntry);
    const ScDataBarFormatData* pDataBarFormatData = pDataBar->GetDataBarData();

    CPPUNIT_ASSERT_EQUAL(Color(99, 142, 198), pDataBarFormatData->maPositiveColor);
    CPPUNIT_ASSERT(pDataBarFormatData->mxNegativeColor);
    CPPUNIT_ASSERT_EQUAL(Color(217, 217, 217), *pDataBarFormatData->mxNegativeColor);
    CPPUNIT_ASSERT_EQUAL(Color(197, 90, 17), pDataBarFormatData->maAxisColor);

    xDocSh->DoClose();
}

namespace {

void checkDatabarPositiveColor(const ScConditionalFormat* pFormat, const Color& rColor)
{
    CPPUNIT_ASSERT(pFormat);
    const ScFormatEntry* pEntry = pFormat->GetEntry(0);
    CPPUNIT_ASSERT(pEntry);
    CPPUNIT_ASSERT_EQUAL(ScFormatEntry::Type::Databar, pEntry->GetType());
    const ScDataBarFormat* pDataBar = static_cast<const ScDataBarFormat*>(pEntry);
    const ScDataBarFormatData* pDataBarFormatData = pDataBar->GetDataBarData();

    CPPUNIT_ASSERT_EQUAL(rColor, pDataBarFormatData->maPositiveColor);
}

}

void ScFiltersTest::testCondFormatThemeColor3XLSX()
{
    ScDocShellRef xDocSh = ScBootstrapFixture::loadDoc(u"cond_format_theme_color3.", FORMAT_XLSX);

    CPPUNIT_ASSERT_MESSAGE("Failed to load document", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();
    ScConditionalFormat* pFormat = rDoc.GetCondFormat(1, 3, 0);
    CPPUNIT_ASSERT(pFormat);
    const ScFormatEntry* pEntry = pFormat->GetEntry(0);
    CPPUNIT_ASSERT(pEntry);
    CPPUNIT_ASSERT_EQUAL(ScFormatEntry::Type::Colorscale, pEntry->GetType());
    const ScColorScaleFormat* pColorScale = static_cast<const ScColorScaleFormat*>(pEntry);

    CPPUNIT_ASSERT_EQUAL(size_t(2), pColorScale->size());
    const ScColorScaleEntry* pColorScaleEntry = pColorScale->GetEntry(0);
    CPPUNIT_ASSERT(pColorScaleEntry);
    CPPUNIT_ASSERT_EQUAL(Color(175, 171, 171), pColorScaleEntry->GetColor());

    pColorScaleEntry = pColorScale->GetEntry(1);
    CPPUNIT_ASSERT(pColorScaleEntry);
    CPPUNIT_ASSERT_EQUAL(Color(51, 63, 80), pColorScaleEntry->GetColor());

    pFormat = rDoc.GetCondFormat(3, 3, 0);
    checkDatabarPositiveColor(pFormat, Color(59, 56, 56));

    pFormat = rDoc.GetCondFormat(5, 3, 0);
    checkDatabarPositiveColor(pFormat, Color(173, 185, 202));

    pFormat = rDoc.GetCondFormat(7, 3, 0);
    checkDatabarPositiveColor(pFormat, Color(89, 89, 89));

    pFormat = rDoc.GetCondFormat(9, 3, 0);
    checkDatabarPositiveColor(pFormat, Color(217, 217, 217));

    xDocSh->DoClose();
}

namespace {

void testComplexIconSetsXLSX_Impl(const ScDocument& rDoc, SCCOL nCol, ScIconSetType eType)
{
    ScConditionalFormat* pFormat = rDoc.GetCondFormat(nCol, 1, 0);
    CPPUNIT_ASSERT(pFormat);
    CPPUNIT_ASSERT_EQUAL(size_t(1), pFormat->size());
    const ScFormatEntry* pEntry = pFormat->GetEntry(0);
    CPPUNIT_ASSERT(pEntry);
    CPPUNIT_ASSERT_EQUAL(ScFormatEntry::Type::Iconset, pEntry->GetType());
    const ScIconSetFormat* pIconSet = static_cast<const ScIconSetFormat*>(pEntry);
    CPPUNIT_ASSERT_EQUAL(eType, pIconSet->GetIconSetData()->eIconSetType);
}

void testCustomIconSetsXLSX_Impl(const ScDocument& rDoc, SCCOL nCol, SCROW nRow, ScIconSetType eType, sal_Int32 nIndex)
{
    ScConditionalFormat* pFormat = rDoc.GetCondFormat(nCol, 1, 1);
    CPPUNIT_ASSERT(pFormat);
    CPPUNIT_ASSERT_EQUAL(size_t(1), pFormat->size());
    const ScFormatEntry* pEntry = pFormat->GetEntry(0);
    CPPUNIT_ASSERT(pEntry);
    CPPUNIT_ASSERT_EQUAL(ScFormatEntry::Type::Iconset, pEntry->GetType());
    const ScIconSetFormat* pIconSet = static_cast<const ScIconSetFormat*>(pEntry);
    std::unique_ptr<ScIconSetInfo> pInfo(pIconSet->GetIconSetInfo(ScAddress(nCol, nRow, 1)));
    if (nIndex == -1)
        CPPUNIT_ASSERT(!pInfo);
    else
    {
        CPPUNIT_ASSERT(pInfo);
        CPPUNIT_ASSERT_EQUAL(nIndex, pInfo->nIconIndex);
        CPPUNIT_ASSERT_EQUAL(eType, pInfo->eIconSetType);
    }
}

}

void ScFiltersTest::testComplexIconSetsXLSX()
{
    ScDocShellRef xDocSh = ScBootstrapFixture::loadDoc(u"complex_icon_set.", FORMAT_XLSX);

    CPPUNIT_ASSERT_MESSAGE("Failed to load complex_icon_set.xlsx", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();
    CPPUNIT_ASSERT_EQUAL(size_t(3), rDoc.GetCondFormList(0)->size());
    testComplexIconSetsXLSX_Impl(rDoc, 1, IconSet_3Triangles);
    testComplexIconSetsXLSX_Impl(rDoc, 3, IconSet_3Stars);
    testComplexIconSetsXLSX_Impl(rDoc, 5, IconSet_5Boxes);

    CPPUNIT_ASSERT_EQUAL(size_t(2), rDoc.GetCondFormList(1)->size());
    testCustomIconSetsXLSX_Impl(rDoc, 1, 1, IconSet_3ArrowsGray, 0);
    testCustomIconSetsXLSX_Impl(rDoc, 1, 2, IconSet_3ArrowsGray, -1);
    testCustomIconSetsXLSX_Impl(rDoc, 1, 3, IconSet_3Arrows, 1);
    testCustomIconSetsXLSX_Impl(rDoc, 1, 4, IconSet_3ArrowsGray, -1);
    testCustomIconSetsXLSX_Impl(rDoc, 1, 5, IconSet_3Arrows, 2);

    testCustomIconSetsXLSX_Impl(rDoc, 3, 1, IconSet_4RedToBlack, 3);
    testCustomIconSetsXLSX_Impl(rDoc, 3, 2, IconSet_3TrafficLights1, 1);
    testCustomIconSetsXLSX_Impl(rDoc, 3, 3, IconSet_3Arrows, 2);

    xDocSh->DoClose();
}

void ScFiltersTest::testCondFormatParentXLSX()
{
    ScDocShellRef xDocSh = ScBootstrapFixture::loadDoc(u"cond_parent.", FORMAT_XLSX);

    CPPUNIT_ASSERT_MESSAGE("Failed to load cond_parent.xlsx", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();
    const SfxItemSet* pCondSet = rDoc.GetCondResult(2, 5, 0);
    const ScPatternAttr* pPattern = rDoc.GetPattern(2, 5, 0);
    const SfxPoolItem& rPoolItem = pPattern->GetItem(ATTR_VER_JUSTIFY, pCondSet);
    const SvxVerJustifyItem& rVerJustify = static_cast<const SvxVerJustifyItem&>(rPoolItem);
    CPPUNIT_ASSERT_EQUAL(SvxCellVerJustify::Top, rVerJustify.GetValue());

    xDocSh->DoClose();
}

void ScFiltersTest::testColorScaleNumWithRefXLSX()
{
    ScDocShellRef xDocSh = ScBootstrapFixture::loadDoc(u"colorscale_num_with_ref.", FORMAT_XLSX);

    CPPUNIT_ASSERT_MESSAGE("Failed to load colorscale_num_with_ref.xlsx", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();
    ScConditionalFormatList* pList = rDoc.GetCondFormList(0);
    CPPUNIT_ASSERT(pList);

    CPPUNIT_ASSERT_EQUAL(size_t(1), pList->size());

    ScConditionalFormat* pFormat = pList->begin()->get();
    CPPUNIT_ASSERT(pFormat);

    CPPUNIT_ASSERT_EQUAL(size_t(1), pFormat->size());
    const ScFormatEntry* pEntry = pFormat->GetEntry(0);
    CPPUNIT_ASSERT(pEntry);

    CPPUNIT_ASSERT_EQUAL(ScFormatEntry::Type::Colorscale, pEntry->GetType());

    const ScColorScaleFormat* pColorScale= dynamic_cast<const ScColorScaleFormat*>(pEntry);
    CPPUNIT_ASSERT(pColorScale);

    const ScColorScaleEntry* pColorScaleEntry = pColorScale->GetEntry(1);
    CPPUNIT_ASSERT_EQUAL(OUString("=$A$1"),
            pColorScaleEntry->GetFormula(formula::FormulaGrammar::GRAM_NATIVE));

    xDocSh->DoClose();
}

void ScFiltersTest::testOrcusODSStyleInterface()
{
    ScDocument aDoc;
    OUString aFullUrl = m_directories.getURLFromSrc(u"sc/qa/unit/data/xml/styles.xml");

    /* This loop below trims file:// from the start because orcus doesn't accept such a URL */
    OUString aValidPath;
    osl::FileBase::getSystemPathFromFileURL(aFullUrl, aValidPath);

    ScOrcusFilters* pOrcus = ScFormatFilter::Get().GetOrcusFilters();
    CPPUNIT_ASSERT(pOrcus);

    pOrcus->importODS_Styles(aDoc, aValidPath);
    ScStyleSheetPool* pStyleSheetPool = aDoc.GetStyleSheetPool();

    /* Test cases for Style "Name1"
     * Has Border and Fill.
     */
    ScStyleSheet* pStyleSheet = pStyleSheetPool->FindCaseIns("Name1", SfxStyleFamily::Para);
    const SfxPoolItem* pItem = nullptr;

    CPPUNIT_ASSERT_MESSAGE("Style Name1 : Doesn't have Attribute background, but it should have.",
        pStyleSheet->GetItemSet().HasItem(ATTR_BACKGROUND, &pItem));
    const SvxBrushItem* pBackground = static_cast<const SvxBrushItem*>(pItem);
    CPPUNIT_ASSERT_EQUAL(Color(254, 255, 204), pBackground->GetColor());

    CPPUNIT_ASSERT_MESSAGE("Style Name1 : Doesn't have Attribute border, but it should have.",
        pStyleSheet->GetItemSet().HasItem(ATTR_BORDER, &pItem));
    const SvxBoxItem* pBoxItem = static_cast<const SvxBoxItem*>(pItem);
    CPPUNIT_ASSERT_EQUAL(Color(255, 204, 18), pBoxItem->GetLeft()->GetColor());
    CPPUNIT_ASSERT_EQUAL(Color(255, 204, 18), pBoxItem->GetRight()->GetColor());
    CPPUNIT_ASSERT_EQUAL(Color(255, 204, 18), pBoxItem->GetTop()->GetColor());
    CPPUNIT_ASSERT_EQUAL(Color(255, 204, 18), pBoxItem->GetBottom()->GetColor());
    CPPUNIT_ASSERT_EQUAL(SvxBorderLineStyle::DOTTED, pBoxItem->GetLeft()->GetBorderLineStyle());
    CPPUNIT_ASSERT_EQUAL(SvxBorderLineStyle::DOTTED, pBoxItem->GetRight()->GetBorderLineStyle());
    CPPUNIT_ASSERT_EQUAL(SvxBorderLineStyle::DOTTED, pBoxItem->GetTop()->GetBorderLineStyle());
    CPPUNIT_ASSERT_EQUAL(SvxBorderLineStyle::DOTTED, pBoxItem->GetBottom()->GetBorderLineStyle());
    ASSERT_DOUBLES_EQUAL_MESSAGE("Error with left width", 1, pBoxItem->GetLeft()->GetWidth());
    ASSERT_DOUBLES_EQUAL_MESSAGE("Error with right width", 1, pBoxItem->GetRight()->GetWidth());
    ASSERT_DOUBLES_EQUAL_MESSAGE("Error with top width", 1, pBoxItem->GetTop()->GetWidth());
    ASSERT_DOUBLES_EQUAL_MESSAGE("Error with bottom width", 1, pBoxItem->GetBottom()->GetWidth());

    CPPUNIT_ASSERT_MESSAGE("Style Name1 : Has Attribute Protection, but it shouldn't.",
        !pStyleSheet->GetItemSet().HasItem(ATTR_PROTECTION, &pItem));
    CPPUNIT_ASSERT_MESSAGE("Style Name1 : Has Attribute font, but it shouldn't.",
        !pStyleSheet->GetItemSet().HasItem(ATTR_FONT, &pItem));
    CPPUNIT_ASSERT_MESSAGE("Style Name1 : Has Attribute number format, but it shouldn't.",
        !pStyleSheet->GetItemSet().HasItem(ATTR_VALUE_FORMAT, &pItem));

    /* Test for Style "Name2"
     * Has 4 sided borders + Diagonal borders.
     */
    pStyleSheet = pStyleSheetPool->FindCaseIns("Name2", SfxStyleFamily::Para);

    CPPUNIT_ASSERT_MESSAGE("Style Name2 : Doesn't have Attribute background, but it should have.",
        pStyleSheet->GetItemSet().HasItem(ATTR_BORDER, &pItem));

    pBoxItem = static_cast<const SvxBoxItem*>(pItem);
    CPPUNIT_ASSERT_EQUAL(Color(0, 0, 0), pBoxItem->GetLeft()->GetColor());
    CPPUNIT_ASSERT_EQUAL(Color(255, 0, 0), pBoxItem->GetRight()->GetColor());
    CPPUNIT_ASSERT_EQUAL(SvxBorderLineStyle::SOLID, pBoxItem->GetLeft()->GetBorderLineStyle());
    CPPUNIT_ASSERT_EQUAL(SvxBorderLineStyle::DOTTED, pBoxItem->GetRight()->GetBorderLineStyle());
    ASSERT_DOUBLES_EQUAL_MESSAGE("Error with left width", 0, pBoxItem->GetLeft()->GetWidth());
    ASSERT_DOUBLES_EQUAL_MESSAGE("Error with right width", 14, pBoxItem->GetRight()->GetWidth());

    CPPUNIT_ASSERT_MESSAGE("Style Name2 : Doesn't have Attribute diagonal(tl-br) border, but it should have.",
        pStyleSheet->GetItemSet().HasItem(ATTR_BORDER_TLBR, &pItem));

    const SvxLineItem* pTLBR= static_cast<const SvxLineItem*>(pItem);
    CPPUNIT_ASSERT_EQUAL(Color(18, 0, 0), pTLBR->GetLine()->GetColor());
    CPPUNIT_ASSERT_EQUAL(SvxBorderLineStyle::DASH_DOT, pTLBR->GetLine()->GetBorderLineStyle());
    ASSERT_DOUBLES_EQUAL_MESSAGE("Error with diagonal tl-br width", 14, pTLBR->GetLine()->GetWidth());

    CPPUNIT_ASSERT_MESSAGE("Style Name2 : Doesn't have Attribute diagonal(bl-tr) border, but it should have.",
        pStyleSheet->GetItemSet().HasItem(ATTR_BORDER_BLTR, &pItem));

    const SvxLineItem* pBLTR= static_cast<const SvxLineItem*>(pItem);
    CPPUNIT_ASSERT_EQUAL(Color(255, 204, 238), pBLTR->GetLine()->GetColor());
    CPPUNIT_ASSERT_EQUAL(SvxBorderLineStyle::DASHED, pBLTR->GetLine()->GetBorderLineStyle());
    ASSERT_DOUBLES_EQUAL_MESSAGE("Error with diagonal tl-br width", 34, pBLTR->GetLine()->GetWidth());

    CPPUNIT_ASSERT_MESSAGE("Style Name2 : Has Attribute background, but it shouldn't.",
        !pStyleSheet->GetItemSet().HasItem(ATTR_BACKGROUND, &pItem));
    CPPUNIT_ASSERT_MESSAGE("Style Name2 : Has Attribute font, but it shouldn't.",
        !pStyleSheet->GetItemSet().HasItem(ATTR_FONT, &pItem));
    CPPUNIT_ASSERT_MESSAGE("Style Name2 : Has Attribute number format, but it shouldn't.",
        !pStyleSheet->GetItemSet().HasItem(ATTR_VALUE_FORMAT, &pItem));

    /* Test for Style "Name3"
     * Hidden, protected and content is printed.
     */
    pStyleSheet = pStyleSheetPool->FindCaseIns("Name3", SfxStyleFamily::Para);
    CPPUNIT_ASSERT_MESSAGE("Style Name3 : Doesn't have Attribute Protection, but it should have.",
        pStyleSheet->GetItemSet().HasItem(ATTR_PROTECTION, &pItem));

    CPPUNIT_ASSERT_MESSAGE("Style Name 3 : Error with Protection attribute." ,bool(ScProtectionAttr(true, false, true, true) == *pItem));

    /* Test for Style "Name4"
     * Hidden, protected and content is printed.
     */
    pStyleSheet = pStyleSheetPool->FindCaseIns("Name4", SfxStyleFamily::Para);
    CPPUNIT_ASSERT_MESSAGE("Style Name4 : Doesn't have Attribute Protection, but it should have.",
        pStyleSheet->GetItemSet().HasItem(ATTR_PROTECTION, &pItem));

    CPPUNIT_ASSERT_MESSAGE("Style Name 4 : Error with Protection attribute." ,bool(ScProtectionAttr(true, true, false, false) == *pItem));

    /* Test for Style "Name3"
     * Hidden, protected and content is printed.
     */
    pStyleSheet = pStyleSheetPool->FindCaseIns("Name5", SfxStyleFamily::Para);
    CPPUNIT_ASSERT_MESSAGE("Style Name5 : Doesn't have Attribute Protection, but it should have.",
        pStyleSheet->GetItemSet().HasItem(ATTR_PROTECTION, &pItem));

    CPPUNIT_ASSERT_MESSAGE("Style Name 5 : Error with Protection attribute." ,bool(ScProtectionAttr(false, false, false, true) == *pItem));

    CPPUNIT_ASSERT_MESSAGE("Style Name5 : Has Attribute Border, but it shouldn't.",
        !pStyleSheet->GetItemSet().HasItem(ATTR_BORDER, &pItem));
    CPPUNIT_ASSERT_MESSAGE("Style Name5: Has Attribute background, but it shouldn't.",
        !pStyleSheet->GetItemSet().HasItem(ATTR_BACKGROUND, &pItem));
    CPPUNIT_ASSERT_MESSAGE("Style Name5 : Has Attribute font, but it shouldn't.",
        !pStyleSheet->GetItemSet().HasItem(ATTR_FONT, &pItem));
    CPPUNIT_ASSERT_MESSAGE("Style Name5 : Has Attribute number format, but it shouldn't.",
        !pStyleSheet->GetItemSet().HasItem(ATTR_VALUE_FORMAT, &pItem));

    /* Test for Style "Name6"
     * Has Font name, posture, weight, color, height
     */
    pStyleSheet = pStyleSheetPool->FindCaseIns("Name6", SfxStyleFamily::Para);
    CPPUNIT_ASSERT_MESSAGE("Style Name6 : Doesn't have Attribute Font, but it should have.",
        pStyleSheet->GetItemSet().HasItem(ATTR_FONT, &pItem));

    const SvxFontItem* pFontItem= static_cast<const SvxFontItem*>(pItem);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Style Name6 :Error with Font name", OUString("Liberation Sans"), pFontItem->GetStyleName());

    CPPUNIT_ASSERT_MESSAGE("Style Name6 : Doesn't have Attribute Font Height, but it should have.",
        pStyleSheet->GetItemSet().HasItem(ATTR_FONT_HEIGHT, &pItem));

    const SvxFontHeightItem* pFontHeightItem= static_cast<const SvxFontHeightItem*>(pItem);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Style Name6 :Error with Font Height", static_cast<sal_uInt32>(480), pFontHeightItem->GetHeight());

    CPPUNIT_ASSERT_MESSAGE("Style Name6 : Doesn't have Attribute Font Posture, but it should have.",
        pStyleSheet->GetItemSet().HasItem(ATTR_FONT_POSTURE, &pItem));

    const SvxPostureItem* pFontPostureItem= static_cast<const SvxPostureItem*>(pItem);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Style Name6 :Error with Font Posture", ITALIC_NORMAL, pFontPostureItem->GetPosture());

    CPPUNIT_ASSERT_MESSAGE("Style Name6 : Doesn't have Attribute Font Weight, but it should have.",
        pStyleSheet->GetItemSet().HasItem(ATTR_FONT_WEIGHT, &pItem));

    const SvxWeightItem* pFontWeightItem= static_cast<const SvxWeightItem*>(pItem);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Style Name6 :Error with Font Weight", WEIGHT_BOLD, pFontWeightItem->GetWeight());

    CPPUNIT_ASSERT_MESSAGE("Style Name6 : Doesn't have Attribute Font Color, but it should have.",
        pStyleSheet->GetItemSet().HasItem(ATTR_FONT_COLOR, &pItem));

    const SvxColorItem* pFontColorItem= static_cast<const SvxColorItem*>(pItem);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Style Name6 :Error with Font Color", Color(128, 128, 128), pFontColorItem->GetValue());

    CPPUNIT_ASSERT_MESSAGE("Style Name6 : Doesn't have Attribute Underline, but it should have.",
        pStyleSheet->GetItemSet().HasItem(ATTR_FONT_UNDERLINE, &pItem));

    const SvxUnderlineItem* pUnderlineItem= static_cast<const SvxUnderlineItem*>(pItem);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Style Name6 :Error with Font Underline Style", LINESTYLE_SINGLE, pUnderlineItem->GetLineStyle());
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Style Name6 :Error with Font Underline Color", Color(128, 128, 128), pUnderlineItem->GetColor());

    /* Test for Style Name "7"
     * Has strikethrough single
     */
    pStyleSheet = pStyleSheetPool->FindCaseIns("Name7", SfxStyleFamily::Para);
    CPPUNIT_ASSERT_MESSAGE("Style Name7 : Doesn't have Attribute Strikeout, but it should have.",
        pStyleSheet->GetItemSet().HasItem(ATTR_FONT_CROSSEDOUT, &pItem));

    const SvxCrossedOutItem* pCrossedOutItem = static_cast<const SvxCrossedOutItem*>(pItem);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Style Name7 :Error with Strikeout", STRIKEOUT_SINGLE, pCrossedOutItem->GetStrikeout());

    /* Test for Style Name "8"
     * Has strikethrough bold
     */
    pStyleSheet = pStyleSheetPool->FindCaseIns("Name8", SfxStyleFamily::Para);
    CPPUNIT_ASSERT_MESSAGE("Style Name8 : Doesn't have Attribute Strikeout, but it should have.",
        pStyleSheet->GetItemSet().HasItem(ATTR_FONT_CROSSEDOUT, &pItem));

    pCrossedOutItem = static_cast<const SvxCrossedOutItem*>(pItem);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Style Name7 :Error with Strikeout", STRIKEOUT_BOLD, pCrossedOutItem->GetStrikeout());

    /* Test for Style Name "9"
     * Has strikethrough slash
     */
    pStyleSheet = pStyleSheetPool->FindCaseIns("Name9", SfxStyleFamily::Para);
    CPPUNIT_ASSERT_MESSAGE("Style Name9 : Doesn't have Attribute Strikeout, but it should have.",
        pStyleSheet->GetItemSet().HasItem(ATTR_FONT_CROSSEDOUT, &pItem));

    pCrossedOutItem = static_cast<const SvxCrossedOutItem*>(pItem);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Style Name9 :Error with Strikeout", STRIKEOUT_SLASH, pCrossedOutItem->GetStrikeout());

    /* Test for Style Name "10"
     * Has ver align, and hor align
     */

    pStyleSheet = pStyleSheetPool->FindCaseIns("Name10", SfxStyleFamily::Para);
    CPPUNIT_ASSERT_MESSAGE("Style Name10 : Doesn't have Attribute hor justify, but it should have.",
        pStyleSheet->GetItemSet().HasItem(ATTR_HOR_JUSTIFY, &pItem));

    const SvxHorJustifyItem* pHorJustify = static_cast<const SvxHorJustifyItem*>(pItem);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Style Name10 :Error with hor justify", SvxCellHorJustify::Right, pHorJustify->GetValue());

    pStyleSheet = pStyleSheetPool->FindCaseIns("Name10", SfxStyleFamily::Para);
    CPPUNIT_ASSERT_MESSAGE("Style Name10 : Doesn't have Attribute ver justify, but it should have.",
        pStyleSheet->GetItemSet().HasItem(ATTR_VER_JUSTIFY, &pItem));

    const SvxVerJustifyItem* pVerJustify = static_cast<const SvxVerJustifyItem*>(pItem);
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Style Name10 :Error with ver justify", SvxCellVerJustify::Center, pVerJustify->GetValue());

}

void ScFiltersTest::testLiteralInFormulaXLS()
{
    ScDocShellRef xDocSh = loadDoc(u"shared-string/literal-in-formula.", FORMAT_XLS);
    CPPUNIT_ASSERT(xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();
    rDoc.CalcAll();

    CPPUNIT_ASSERT_EQUAL(2.0, rDoc.GetValue(ScAddress(0,0,0)));

    xDocSh->DoClose();
}

void ScFiltersTest::testFormulaDependency()
{
    ScDocShellRef xDocSh = ScBootstrapFixture::loadDoc(u"dependencyTree.", FORMAT_ODS);

    ScDocument& rDoc = xDocSh->GetDocument();

    // check if formula in A1 changes value
    double nVal = rDoc.GetValue(0,0,0);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(nVal, 1.0, 1e-10);
    rDoc.SetValue(0,1,0, 0.0);
    nVal = rDoc.GetValue(0,0,0);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(nVal, 2.0, 1e-10);

    // check that the number format is implicitly inherited
    // CPPUNIT_ASSERT_EQUAL(rDoc.GetString(0,4,0), rDoc.GetString(0,5,0));

    xDocSh->DoClose();
}

void ScFiltersTest::testMiscRowHeights()
{
    // FIXME: the DPI check should be removed when either (1) the test is fixed to work with
    // non-default DPI; or (2) unit tests on Windows are made to use svp VCL plugin.
    if (!IsDefaultDPI())
        return;

    static const TestParam::RowData DfltRowData[] =
    {
        // check rows at the beginning and end of document
        // and make sure they are reported as the default row
        // height ( indicated by -1 )
        { 2, 4, 0, -1, 0, false  },
        { 1048573, 1048575, 0, -1, 0, false  },
    };

    static const TestParam::RowData MultiLineOptData[] =
    {
        // Row 0 is 12.63 mm and optimal flag is set => 12.36 mm
        { 0, 0, 0, 1236, CHECK_OPTIMAL, true  },
        // Row 1 is 11.99 mm and optimal flag is NOT set
        { 1, 1, 0, 1199, CHECK_OPTIMAL, false  },
    };

    TestParam aTestValues[] =
    {
        /* Checks that a document saved to ods with default rows does indeed
           have default row heights ( there was a problem where the optimal
           height was being calculated after import if no hard height )
        */
        { "alldefaultheights.", FORMAT_ODS, -1, SAL_N_ELEMENTS(DfltRowData), DfltRowData },
        /* Checks the imported height of some multiline input, additionally checks
           that the optimal height flag is set ( or not )
        */
        { "multilineoptimal.", FORMAT_ODS, -1, SAL_N_ELEMENTS(MultiLineOptData), MultiLineOptData },
    };
    miscRowHeightsTest( aTestValues, SAL_N_ELEMENTS(aTestValues) );
}

// regression test at least fdo#59193
// what we want to test here is that when cell contents are deleted
// and the optimal flag is set for that row that the row is actually resized

void ScFiltersTest::testOptimalHeightReset()
{
    // FIXME: the DPI check should be removed when either (1) the test is fixed to work with
    // non-default DPI; or (2) unit tests on Windows are made to use svp VCL plugin.
    if (!IsDefaultDPI())
        return;

    ScDocShellRef xDocSh = loadDoc(u"multilineoptimal.", FORMAT_ODS, true);
    SCTAB nTab = 0;
    SCROW nRow = 0;
    ScDocument& rDoc = xDocSh->GetDocument();
    // open document in read/write mode ( otherwise optimal height stuff won't
    // be triggered ) *and* you can't delete cell contents.
    int nHeight = sc::TwipsToHMM ( rDoc.GetRowHeight(nRow, nTab, false) );
    CPPUNIT_ASSERT_EQUAL(1236, nHeight);

    ScDocFunc &rFunc = xDocSh->GetDocFunc();

    // delete content of A1
    ScRange aDelRange(0,0,0,0,0,0);
    ScMarkData aMark(rDoc.GetSheetLimits());
    aMark.SetMarkArea(aDelRange);
    bool bRet = rFunc.DeleteContents( aMark, InsertDeleteFlags::ALL, false, true );
    CPPUNIT_ASSERT_MESSAGE("DeleteContents failed", bRet);

    // get the new height of A1
    nHeight =  sc::TwipsToHMM( rDoc.GetRowHeight(nRow, nTab, false) );

    // set optimal height for empty row 2
    std::vector<sc::ColRowSpan> aRowArr(1, sc::ColRowSpan(2,2));
    rFunc.SetWidthOrHeight(false, aRowArr, nTab, SC_SIZE_OPTIMAL, 0, true, true);

    // retrieve optimal height
    int nOptimalHeight = sc::TwipsToHMM( rDoc.GetRowHeight(aRowArr[0].mnStart, nTab, false) );

    // check if the new height of A1 ( after delete ) is now the optimal height of an empty cell
    CPPUNIT_ASSERT_EQUAL(nOptimalHeight, nHeight );
    xDocSh->DoClose();
}

void ScFiltersTest::testCustomNumFormatHybridCellODS()
{
    ScDocShellRef xDocSh = loadDoc(u"custom-numfmt-hybrid-cell.", FORMAT_ODS, false);
    ScDocument& rDoc = xDocSh->GetDocument();
    rDoc.SetAutoCalc(true);

    // All of B14, B16 and B18 should be displaying empty strings by virtue
    // of the custom number format being set on those cells.

    for (SCROW nRow : {13, 15, 17})
    {
        ScAddress aPos(1, nRow, 0);
        OUString aStr = rDoc.GetString(aPos);
        CPPUNIT_ASSERT(aStr.isEmpty());
    }

    // Now, set value of 1 to B15.  This should trigger re-calc on B18 and B18
    // should now show a value of 1.
    rDoc.SetValue(ScAddress(1,15,0), 1.0);

    OUString aStr = rDoc.GetString(ScAddress(1,17,0));
    CPPUNIT_ASSERT_EQUAL(OUString("1"), aStr);

    // Make sure the cell doesn't have an error value.
    ScFormulaCell* pFC = rDoc.GetFormulaCell(ScAddress(1,17,0));
    CPPUNIT_ASSERT(pFC);
    CPPUNIT_ASSERT_EQUAL(FormulaError::NONE, pFC->GetErrCode());

    xDocSh->DoClose();
}

void ScFiltersTest::testTdf121040()
{
    ScDocShellRef xDocSh = loadDoc(u"tdf121040.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to load tdf121040.ods", xDocSh.is());

    const SCTAB nTab = 0;
    ScDocument& rDoc = xDocSh->GetDocument();

    // The first 9 rows should have the same height
    const sal_uInt16 nHeight = rDoc.GetRowHeight(0, nTab, false);
    for (SCTAB nRow=1; nRow<9; nRow++)
    {
        CPPUNIT_ASSERT_EQUAL(nHeight, rDoc.GetRowHeight(nRow, nTab, false));
    }

    xDocSh->DoClose();
}

void ScFiltersTest::testTdf118624()
{
    ScDocShellRef xDocSh = loadDoc(u"tdf118624.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to open doc", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();

    CPPUNIT_ASSERT_MESSAGE("RAND() in array/matrix mode shouldn't return the same value",
            rDoc.GetString(ScAddress(0,0,0)) != rDoc.GetString(ScAddress(0,1,0)));

    xDocSh->DoClose();
}

void ScFiltersTest::testTdf124454()
{
    ScDocShellRef xDocSh = loadDoc(u"tdf124454.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to open doc", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();

    CPPUNIT_ASSERT_EQUAL(OUString("1"), rDoc.GetString(ScAddress(1,0,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("1"), rDoc.GetString(ScAddress(2,0,0)));
    // Without the fix in place, double negation with text in array
    // would have returned -1
    CPPUNIT_ASSERT_EQUAL(OUString("1"), rDoc.GetString(ScAddress(3,0,0)));

    xDocSh->DoClose();
}

void ScFiltersTest::testPrintRangeODS()
{
    ScDocShellRef xDocSh = loadDoc(u"print-range.", FORMAT_ODS);
    ScDocument& rDoc = xDocSh->GetDocument();
    const ScRange* pRange = rDoc.GetRepeatRowRange(0);
    CPPUNIT_ASSERT(pRange);
    CPPUNIT_ASSERT_EQUAL(ScRange(0,0,0,0,1,0), *pRange);

    pRange = rDoc.GetRepeatRowRange(1);
    CPPUNIT_ASSERT(pRange);
    CPPUNIT_ASSERT_EQUAL(ScRange(0,2,0,0,4,0), *pRange);

    xDocSh->DoClose();
}

void ScFiltersTest::testOutlineODS()
{
    ScDocShellRef xDocSh = loadDoc(u"outline.", FORMAT_ODS);
    ScDocument& rDoc = xDocSh->GetDocument();

    const ScOutlineTable* pTable = rDoc.GetOutlineTable(0);
    CPPUNIT_ASSERT(pTable);

    const ScOutlineArray& rArr = pTable->GetRowArray();
    size_t nDepth = rArr.GetDepth();
    CPPUNIT_ASSERT_EQUAL(size_t(4), nDepth);

    for(size_t i = 0; i < nDepth; ++i)
    {
        CPPUNIT_ASSERT_EQUAL(size_t(1), rArr.GetCount(i));
    }

    struct OutlineData {
        SCCOLROW nStart;
        SCCOLROW nEnd;
        bool bHidden;
        bool bVisible;

        size_t nDepth;
        size_t nIndex;
    };

    static const OutlineData aRow[] =
    {
        { 1, 29, false, true, 0, 0 },
        { 2, 26, false, true, 1, 0 },
        { 4, 23, false, true, 2, 0 },
        { 6, 20, true, true, 3, 0 }
    };

    for(size_t i = 0; i < SAL_N_ELEMENTS(aRow); ++i)
    {

        const ScOutlineEntry* pEntry = rArr.GetEntry(aRow[i].nDepth, aRow[i].nIndex);
        SCCOLROW nStart = pEntry->GetStart();
        CPPUNIT_ASSERT_EQUAL(aRow[i].nStart, nStart);

        SCCOLROW nEnd = pEntry->GetEnd();
        CPPUNIT_ASSERT_EQUAL(aRow[i].nEnd, nEnd);

        bool bHidden = pEntry->IsHidden();
        CPPUNIT_ASSERT_EQUAL(aRow[i].bHidden, bHidden);

        bool bVisible = pEntry->IsVisible();
        CPPUNIT_ASSERT_EQUAL(aRow[i].bVisible, bVisible);
    }

    xDocSh->DoClose();
}

void ScFiltersTest::testColumnStyleXLSX()
{
    ScDocShellRef xDocSh = loadDoc(u"column-style.", FORMAT_XLSX);
    CPPUNIT_ASSERT(xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    const ScPatternAttr* pPattern = rDoc.GetPattern(0,0,0);
    CPPUNIT_ASSERT(pPattern);

    const ScProtectionAttr& rAttr = pPattern->GetItem(ATTR_PROTECTION);
    CPPUNIT_ASSERT(rAttr.GetProtection());

    pPattern = rDoc.GetPattern(0,1,0);
    CPPUNIT_ASSERT(pPattern);

    const ScProtectionAttr& rAttrNew = pPattern->GetItem(ATTR_PROTECTION);
    CPPUNIT_ASSERT(!rAttrNew.GetProtection());

    xDocSh->DoClose();
}

void ScFiltersTest::testColumnStyleAutoFilterXLSX()
{
    ScDocShellRef xDocSh = loadDoc(u"column-style-autofilter.", FORMAT_XLSX);
    CPPUNIT_ASSERT(xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    const ScPatternAttr* pPattern = rDoc.GetPattern(0, 10, 18);
    CPPUNIT_ASSERT(pPattern);

    const ScMergeFlagAttr& rAttr = pPattern->GetItem(ATTR_MERGE_FLAG);
    CPPUNIT_ASSERT(!rAttr.HasAutoFilter());

    xDocSh->DoClose();
}

void ScFiltersTest::testSharedFormulaHorizontalXLS()
{
    ScDocShellRef xDocSh = loadDoc(u"shared-formula/horizontal.", FORMAT_XLS);
    CPPUNIT_ASSERT(xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    // Make sure K2:S2 on the 2nd sheet are all formula cells.
    ScAddress aPos(0, 1, 1);
    for (SCCOL nCol = 10; nCol <= 18; ++nCol)
    {
        aPos.SetCol(nCol);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Formula cell is expected here.", CELLTYPE_FORMULA, rDoc.GetCellType(aPos));
    }

    // Likewise, B3:J9 all should be formula cells.
    for (SCCOL nCol = 1; nCol <= 9; ++nCol)
    {
        aPos.SetCol(nCol);
        for (SCROW nRow = 2; nRow <= 8; ++nRow)
        {
            aPos.SetRow(nRow);
            CPPUNIT_ASSERT_EQUAL_MESSAGE("Formula cell is expected here.", CELLTYPE_FORMULA, rDoc.GetCellType(aPos));
        }
    }

    // B2:I2 too.
    aPos.SetRow(1);
    for (SCCOL nCol = 1; nCol <= 8; ++nCol)
    {
        aPos.SetCol(nCol);
        CPPUNIT_ASSERT_EQUAL_MESSAGE("Formula cell is expected here.", CELLTYPE_FORMULA, rDoc.GetCellType(aPos));
    }

    // J2 has a string of "MW".
    aPos.SetCol(9);
    CPPUNIT_ASSERT_EQUAL(OUString("MW"), rDoc.GetString(aPos));

    xDocSh->DoClose();
}

void ScFiltersTest::testSharedFormulaWrappedRefsXLS()
{
    ScDocShellRef xDocSh = loadDoc(u"shared-formula/wrapped-refs.", FORMAT_XLS);
    CPPUNIT_ASSERT(xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();
    rDoc.CalcAll();

    // Check the values of H7:H10.
    CPPUNIT_ASSERT_EQUAL(7.0, rDoc.GetValue(ScAddress(7,6,0)));
    CPPUNIT_ASSERT_EQUAL(8.0, rDoc.GetValue(ScAddress(7,7,0)));
    CPPUNIT_ASSERT_EQUAL(9.0, rDoc.GetValue(ScAddress(7,8,0)));
    CPPUNIT_ASSERT_EQUAL(10.0, rDoc.GetValue(ScAddress(7,9,0)));

    // EM7:EM10 should reference H7:H10.
    CPPUNIT_ASSERT_EQUAL(7.0, rDoc.GetValue(ScAddress(142,6,0)));
    CPPUNIT_ASSERT_EQUAL(8.0, rDoc.GetValue(ScAddress(142,7,0)));
    CPPUNIT_ASSERT_EQUAL(9.0, rDoc.GetValue(ScAddress(142,8,0)));
    CPPUNIT_ASSERT_EQUAL(10.0, rDoc.GetValue(ScAddress(142,9,0)));

    // Make sure EM7:EM10 are grouped.
    const ScFormulaCell *pFC = rDoc.GetFormulaCell(ScAddress(142,6,0));
    CPPUNIT_ASSERT(pFC);
    CPPUNIT_ASSERT_EQUAL(static_cast<SCROW>(6), pFC->GetSharedTopRow());
    CPPUNIT_ASSERT_EQUAL(static_cast<SCROW>(4), pFC->GetSharedLength());

    xDocSh->DoClose();
}

void ScFiltersTest::testSharedFormulaBIFF5()
{
    ScDocShellRef xDocSh = loadDoc(u"shared-formula/biff5.", FORMAT_XLS);
    CPPUNIT_ASSERT(xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();
    rDoc.CalcAll();

    // E6:E376 should be all formulas, and they should belong to the same group.
    const ScFormulaCell* pFC = rDoc.GetFormulaCell(ScAddress(4,5,0));
    CPPUNIT_ASSERT(pFC);
    CPPUNIT_ASSERT_EQUAL(static_cast<SCROW>(5), pFC->GetSharedTopRow());
    CPPUNIT_ASSERT_EQUAL(static_cast<SCROW>(371), pFC->GetSharedLength());

    xDocSh->DoClose();
}

void ScFiltersTest::testSharedFormulaXLSB()
{
    ScDocShellRef xDocSh = loadDoc(u"shared_formula.", FORMAT_XLSB);
    CPPUNIT_ASSERT(xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();
    rDoc.CalcAll();

    // A1:A30 should be all formulas, and they should belong to the same group.
    const ScFormulaCell* pFC = rDoc.GetFormulaCell(ScAddress(0,0,0));
    CPPUNIT_ASSERT(pFC);
    CPPUNIT_ASSERT_EQUAL(static_cast<SCROW>(0), pFC->GetSharedTopRow());
    CPPUNIT_ASSERT_EQUAL(static_cast<SCROW>(30), pFC->GetSharedLength());

    for(SCROW nRow = 0; nRow < 30; ++nRow)
    {
        ASSERT_DOUBLES_EQUAL(3.0, rDoc.GetValue(0, nRow, 0));
    }

    xDocSh->DoClose();
}

void ScFiltersTest::testSharedFormulaXLS()
{
    {
        // fdo#80091
        ScDocShellRef xDocSh = loadDoc(u"shared-formula/relative-refs1.", FORMAT_XLS);
        CPPUNIT_ASSERT(xDocSh.is());
        ScDocument& rDoc = xDocSh->GetDocument();
        rDoc.CalcAll();

        // A1:A30 should be all formulas, and they should belong to the same group.
        const ScFormulaCell* pFC = rDoc.GetFormulaCell(ScAddress(0,1,0));
        CPPUNIT_ASSERT(pFC);
        CPPUNIT_ASSERT_EQUAL(static_cast<SCROW>(1), pFC->GetSharedTopRow());
        CPPUNIT_ASSERT_EQUAL(static_cast<SCROW>(29), pFC->GetSharedLength());

        for(SCROW nRow = 0; nRow < 30; ++nRow)
        {
            ASSERT_DOUBLES_EQUAL(double(nRow+1), rDoc.GetValue(0, nRow, 0));
        }

        xDocSh->DoClose();
    }

    {
        // fdo#84556 and some related tests
        ScDocShellRef xDocSh = loadDoc(u"shared-formula/relative-refs2.", FORMAT_XLS);
        CPPUNIT_ASSERT(xDocSh.is());
        ScDocument& rDoc = xDocSh->GetDocument();
        rDoc.CalcAll();

        {
            const ScFormulaCell* pFC = rDoc.GetFormulaCell(ScAddress(2,1,0));
            CPPUNIT_ASSERT(pFC);
            CPPUNIT_ASSERT_EQUAL(static_cast<SCROW>(1), pFC->GetSharedTopRow());
            CPPUNIT_ASSERT_EQUAL(static_cast<SCROW>(10), pFC->GetSharedLength());

            pFC = rDoc.GetFormulaCell(ScAddress(2,10,0));
            CPPUNIT_ASSERT(pFC);
            CPPUNIT_ASSERT_EQUAL(static_cast<SCROW>(1), pFC->GetSharedTopRow());
            CPPUNIT_ASSERT_EQUAL(static_cast<SCROW>(10), pFC->GetSharedLength());

            OUString aFormula;
            rDoc.GetFormula(2, 1, 0, aFormula);
            CPPUNIT_ASSERT_EQUAL(OUString("=SUM(B9:D9)"), aFormula);

            rDoc.GetFormula(2, 10, 0, aFormula);
            CPPUNIT_ASSERT_EQUAL(OUString("=SUM(B18:D18)"), aFormula);
        }

        {
            const ScFormulaCell* pFC = rDoc.GetFormulaCell(ScAddress(4,8,0));
            CPPUNIT_ASSERT(pFC);
            CPPUNIT_ASSERT_EQUAL(static_cast<SCROW>(8), pFC->GetSharedTopRow());
            CPPUNIT_ASSERT_EQUAL(static_cast<SCROW>(10), pFC->GetSharedLength());

            pFC = rDoc.GetFormulaCell(ScAddress(4,17,0));
            CPPUNIT_ASSERT(pFC);
            CPPUNIT_ASSERT_EQUAL(static_cast<SCROW>(8), pFC->GetSharedTopRow());
            CPPUNIT_ASSERT_EQUAL(static_cast<SCROW>(10), pFC->GetSharedLength());

            OUString aFormula;
            rDoc.GetFormula(4, 8, 0, aFormula);
            CPPUNIT_ASSERT_EQUAL(OUString("=SUM(G9:EY9)"), aFormula);

            rDoc.GetFormula(4, 17, 0, aFormula);
            CPPUNIT_ASSERT_EQUAL(OUString("=SUM(G18:EY18)"), aFormula);
        }

        {
            const ScFormulaCell* pFC = rDoc.GetFormulaCell(ScAddress(6,15,0));
            CPPUNIT_ASSERT(pFC);
            CPPUNIT_ASSERT_EQUAL(static_cast<SCROW>(15), pFC->GetSharedTopRow());
            CPPUNIT_ASSERT_EQUAL(static_cast<SCROW>(10), pFC->GetSharedLength());

            pFC = rDoc.GetFormulaCell(ScAddress(6,24,0));
            CPPUNIT_ASSERT(pFC);
            CPPUNIT_ASSERT_EQUAL(static_cast<SCROW>(15), pFC->GetSharedTopRow());
            CPPUNIT_ASSERT_EQUAL(static_cast<SCROW>(10), pFC->GetSharedLength());

            OUString aFormula;
            rDoc.GetFormula(6, 15, 0, aFormula);
            CPPUNIT_ASSERT_EQUAL(OUString("=SUM(A16:A40000)"), aFormula);

            rDoc.GetFormula(6, 24, 0, aFormula);
            CPPUNIT_ASSERT_EQUAL(OUString("=SUM(A25:A40009)"), aFormula);
        }

        xDocSh->DoClose();
    }
}

void ScFiltersTest::testSharedFormulaColumnLabelsODS()
{
    ScDocShellRef xDocSh = loadDoc(u"shared-formula/column-labels.", FORMAT_ODS);

    CPPUNIT_ASSERT(xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();
    rDoc.CalcAll();

    CPPUNIT_ASSERT_EQUAL( 5.0, rDoc.GetValue(ScAddress(2,1,0)));
    CPPUNIT_ASSERT_EQUAL(15.0, rDoc.GetValue(ScAddress(2,2,0)));
    CPPUNIT_ASSERT_EQUAL(30.0, rDoc.GetValue(ScAddress(2,3,0)));
    CPPUNIT_ASSERT_EQUAL(28.0, rDoc.GetValue(ScAddress(2,4,0)));
    CPPUNIT_ASSERT_EQUAL(48.0, rDoc.GetValue(ScAddress(2,5,0)));

    CPPUNIT_ASSERT_EQUAL(  0.0, rDoc.GetValue(ScAddress(3,1,0)));
    CPPUNIT_ASSERT_EQUAL( 50.0, rDoc.GetValue(ScAddress(3,2,0)));
    CPPUNIT_ASSERT_EQUAL(144.0, rDoc.GetValue(ScAddress(3,3,0)));
    CPPUNIT_ASSERT_EQUAL(147.0, rDoc.GetValue(ScAddress(3,4,0)));
    CPPUNIT_ASSERT_EQUAL(320.0, rDoc.GetValue(ScAddress(3,5,0)));

    CPPUNIT_ASSERT_EQUAL(0.0, rDoc.GetValue(ScAddress(4,1,0)));
    CPPUNIT_ASSERT_EQUAL(2.0, rDoc.GetValue(ScAddress(4,2,0)));
    CPPUNIT_ASSERT_EQUAL(4.0, rDoc.GetValue(ScAddress(4,3,0)));
    CPPUNIT_ASSERT_EQUAL(3.0, rDoc.GetValue(ScAddress(4,4,0)));
    CPPUNIT_ASSERT_EQUAL(5.0, rDoc.GetValue(ScAddress(4,5,0)));

    xDocSh->DoClose();
}

void ScFiltersTest::testSharedFormulaColumnRowLabelsODS()
{
    ScDocShellRef xDocSh = loadDoc(u"shared-formula/column-row-labels.", FORMAT_ODS);

    CPPUNIT_ASSERT(xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();
    rDoc.CalcAll();

    // Expected output in each of the three ranges.
    //
    // +---+---+---+
    // | 1 | 4 | 7 |
    // +---+---+---+
    // | 2 | 5 | 8 |
    // +---+---+---+
    // | 3 | 6 | 9 |
    // +---+---+---+

    auto aCheckFunc = [&](SCCOL nStartCol, SCROW nStartRow)
    {
        double fExpected = 1.0;
        for (SCCOL nCol = 0; nCol <= 2; ++nCol)
        {
            for (SCROW nRow = 0; nRow <= 2; ++nRow)
            {
                ScAddress aPos(nStartCol+nCol, nStartRow+nRow, 0);
                CPPUNIT_ASSERT_EQUAL(fExpected, rDoc.GetValue(aPos));
                fExpected += 1.0;
            }
        }
    };

    aCheckFunc(5, 1); // F2:H4
    aCheckFunc(9, 1); // J2:L4
    aCheckFunc(1, 6); // B7:D9

    xDocSh->DoClose();
}

void ScFiltersTest::testExternalRefCacheXLSX()
{
    ScDocShellRef xDocSh = loadDoc(u"external-refs.", FORMAT_XLSX);
    CPPUNIT_ASSERT(xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    // These string values are cached external cell values.
    CPPUNIT_ASSERT_EQUAL(OUString("Name"), rDoc.GetString(ScAddress(0,0,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("Andy"), rDoc.GetString(ScAddress(0,1,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("Bruce"), rDoc.GetString(ScAddress(0,2,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("Charlie"), rDoc.GetString(ScAddress(0,3,0)));

    xDocSh->DoClose();
}

void ScFiltersTest::testExternalRefCacheODS()
{
    ScDocShellRef xDocSh = loadDoc(u"external-ref-cache.", FORMAT_ODS);

    CPPUNIT_ASSERT(xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    // Cells B2:B4 have VLOOKUP with external references which should all show "text".
    CPPUNIT_ASSERT_EQUAL(OUString("text"), rDoc.GetString(ScAddress(1,1,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("text"), rDoc.GetString(ScAddress(1,2,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("text"), rDoc.GetString(ScAddress(1,3,0)));

    // Both cells A6 and A7 should be registered with scExternalRefManager properly
    CPPUNIT_ASSERT_EQUAL(true, rDoc.GetExternalRefManager()->hasCellExternalReference(ScAddress(0, 5, 0)));
    CPPUNIT_ASSERT_EQUAL(true, rDoc.GetExternalRefManager()->hasCellExternalReference(ScAddress(0, 6, 0)));

    xDocSh->DoClose();
}

void ScFiltersTest::testHybridSharedStringODS()
{
    ScDocShellRef xDocSh = loadDoc(u"hybrid-shared-string.", FORMAT_ODS);

    CPPUNIT_ASSERT(xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    // A2 contains formula with MATCH function.  The result must be 2, not #N/A!
    CPPUNIT_ASSERT_EQUAL(2.0, rDoc.GetValue(ScAddress(0,1,0)));

    xDocSh->DoClose();
}

void ScFiltersTest::testCopyMergedNumberFormats()
{
    ScDocShellRef xDocSh = loadDoc(u"copy-merged-number-formats.", FORMAT_ODS);
    CPPUNIT_ASSERT(xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    // Cells B1, C1 and D1 are formatted as dates.
    OUString aStrB1 = rDoc.GetString(ScAddress(1,0,0));
    OUString aStrC1 = rDoc.GetString(ScAddress(2,0,0));
    OUString aStrD1 = rDoc.GetString(ScAddress(3,0,0));

    ScDocument aCopyDoc;
    aCopyDoc.InsertTab(0, "CopyHere");
    rDoc.CopyStaticToDocument(ScRange(1,0,0,3,0,0), 0, aCopyDoc);

    // Make sure the date formats are copied to the new document.
    CPPUNIT_ASSERT_EQUAL(aStrB1, aCopyDoc.GetString(ScAddress(1,0,0)));
    CPPUNIT_ASSERT_EQUAL(aStrC1, aCopyDoc.GetString(ScAddress(2,0,0)));
    CPPUNIT_ASSERT_EQUAL(aStrD1, aCopyDoc.GetString(ScAddress(3,0,0)));

    xDocSh->DoClose();
}

void ScFiltersTest::testVBAUserFunctionXLSM()
{
    ScDocShellRef xDocSh = loadDoc(u"vba-user-function.", FORMAT_XLSM);
    CPPUNIT_ASSERT(xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    // A1 contains formula with user-defined function, and the function is defined in VBA.
    ScFormulaCell* pFC = rDoc.GetFormulaCell(ScAddress(0,0,0));
    CPPUNIT_ASSERT(pFC);

    sc::CompileFormulaContext aCxt(rDoc);
    OUString aFormula = pFC->GetFormula(aCxt);

    CPPUNIT_ASSERT_EQUAL(OUString("=MYFUNC()"), aFormula);

    // Check the formula state after the load.
    FormulaError nErrCode = pFC->GetErrCode();
    CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(nErrCode));

    // Check the result.
    CPPUNIT_ASSERT_EQUAL(42.0, rDoc.GetValue(ScAddress(0,0,0)));

    xDocSh->DoClose();
}

void ScFiltersTest::testEmbeddedImageXLS()
{
    // The document has one embedded image on the first sheet.  Make sure it's
    // imported properly.

    ScDocShellRef xDocSh = loadDoc(u"file-with-png-image.", FORMAT_XLS);
    CPPUNIT_ASSERT(xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    ScDrawLayer* pDL = rDoc.GetDrawLayer();
    CPPUNIT_ASSERT(pDL);
    const SdrPage* pPage = pDL->GetPage(0);
    CPPUNIT_ASSERT(pPage);
    const SdrObject* pObj = pPage->GetObj(0);
    CPPUNIT_ASSERT(pObj);
    const SdrGrafObj* pImageObj = dynamic_cast<const SdrGrafObj*>(pObj);
    CPPUNIT_ASSERT(pImageObj);
    const Graphic& rGrf = pImageObj->GetGraphic();
    BitmapEx aBMP = rGrf.GetBitmapEx();
    CPPUNIT_ASSERT_MESSAGE("Bitmap content should not be empty if the image has been properly imported.", !aBMP.IsEmpty());

    xDocSh->DoClose();
}

void ScFiltersTest::testErrorOnExternalReferences()
{
    ScDocShellRef xDocSh = loadDoc(u"blank.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to open empty doc", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();

    // Test tdf#89330
    rDoc.SetString(ScAddress(0,0,0), "='file:///Path/To/FileA.ods'#$Sheet1.A1A");

    ScFormulaCell* pFC = rDoc.GetFormulaCell(ScAddress(0,0,0));
    CPPUNIT_ASSERT(pFC);
    CPPUNIT_ASSERT_EQUAL(int(FormulaError::NoName), static_cast<int>(pFC->GetErrCode()));

    ASSERT_FORMULA_EQUAL(rDoc, ScAddress(0,0,0), "'file:///Path/To/FileA.ods'#$Sheet1.A1A", "Formula changed");

    xDocSh->DoClose();
}

void ScFiltersTest::testTdf44076()
{
    ScDocShellRef xDocSh = loadDoc(u"blank.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to open empty doc", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();

    rDoc.SetString(ScAddress(0,0,0), "=(-8)^(1/3)");

    CPPUNIT_ASSERT_EQUAL(-2.0, rDoc.GetValue(ScAddress(0,0,0)));

    xDocSh->DoClose();
}

void ScFiltersTest::testEditEngStrikeThroughXLSX()
{
    ScDocShellRef xDocSh = loadDoc(u"strike-through.", FORMAT_XLSX);
    CPPUNIT_ASSERT_MESSAGE("Failed to open doc", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();

    const EditTextObject* pObj = rDoc.GetEditText(ScAddress(0, 0, 0));
    CPPUNIT_ASSERT(pObj);
    CPPUNIT_ASSERT_EQUAL(sal_Int32(1), pObj->GetParagraphCount());
    CPPUNIT_ASSERT_EQUAL(OUString("this is strike through  this not"), pObj->GetText(0));

    std::vector<EECharAttrib> aAttribs;
    pObj->GetCharAttribs(0, aAttribs);
    for (const auto& rAttrib : aAttribs)
    {
        if (rAttrib.pAttr->Which() == EE_CHAR_STRIKEOUT)
        {
            const SvxCrossedOutItem& rItem = static_cast<const SvxCrossedOutItem&>(*rAttrib.pAttr);
            if (rAttrib.nStart == 0)
            {
                CPPUNIT_ASSERT(rItem.GetStrikeout() != STRIKEOUT_NONE);
            }
            else
            {
                CPPUNIT_ASSERT_EQUAL(STRIKEOUT_NONE, rItem.GetStrikeout());
            }
        }
    }

    xDocSh->DoClose();
}

void ScFiltersTest::testRefStringXLSX()
{
    ScDocShellRef xDocSh = loadDoc(u"ref_string.", FORMAT_XLSX);
    CPPUNIT_ASSERT_MESSAGE("Failed to open doc", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();

    double nVal = rDoc.GetValue(2, 2, 0);
    ASSERT_DOUBLES_EQUAL(3.0, nVal);

    const ScCalcConfig& rCalcConfig = rDoc.GetCalcConfig();
    CPPUNIT_ASSERT_EQUAL(formula::FormulaGrammar::CONV_XL_A1, rCalcConfig.meStringRefAddressSyntax);

    xDocSh->DoClose();
}

void ScFiltersTest::testTdf130132()
{
    ScDocShellRef xDocSh = loadDoc(u"tdf130132.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to open doc", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();
    const ScPatternAttr* pAttr = rDoc.GetPattern(434, 0, 0);

    {
        const SfxPoolItem& rItem = pAttr->GetItem(ATTR_BACKGROUND);
        const SvxBrushItem& rBackground = static_cast<const SvxBrushItem&>(rItem);
        const Color& rColor = rBackground.GetColor();
        // background colour is yellow
        CPPUNIT_ASSERT_EQUAL(Color(255, 255, 0), rColor);
    }
}

void ScFiltersTest::testTdf133327()
{
    ScDocShellRef xDocSh = loadDoc(u"tdf133327.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to open doc", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();

    const ScPatternAttr* pAttr = rDoc.GetPattern(250, 1, 0);

    const SfxPoolItem& rItem = pAttr->GetItem(ATTR_BACKGROUND);
    const SvxBrushItem& rBackground = static_cast<const SvxBrushItem&>(rItem);
    const Color& rColor = rBackground.GetColor();

    // Without the fix in place, this test would have failed with
    // - Expected: Color: R:255 G:255 B: 0
    // - Actual  : Color: R:255 G:255 B: 255
    CPPUNIT_ASSERT_EQUAL(Color(255, 255, 0), rColor);
}

void ScFiltersTest::testColumnStyle2XLSX()
{
    ScDocShellRef xDocSh = loadDoc(u"column_style.", FORMAT_XLSX);
    CPPUNIT_ASSERT_MESSAGE("Failed to open doc", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();
    const ScPatternAttr* pAttr = rDoc.GetPattern(1, 1, 0);

    {
        const SfxPoolItem& rItem = pAttr->GetItem(ATTR_BACKGROUND);
        const SvxBrushItem& rBackground = static_cast<const SvxBrushItem&>(rItem);
        const Color& rColor = rBackground.GetColor();
        CPPUNIT_ASSERT_EQUAL(Color(255, 51, 51), rColor);
    }

    {
        const SfxPoolItem& rItem = pAttr->GetItem(ATTR_HOR_JUSTIFY);
        const SvxHorJustifyItem& rJustify = static_cast<const SvxHorJustifyItem&>(rItem);
        CPPUNIT_ASSERT_EQUAL(SvxCellHorJustify::Center, rJustify.GetValue());
    }

    {
        const SfxPoolItem& rItem = pAttr->GetItem(ATTR_FONT_HEIGHT);
        const SvxFontHeightItem& rFontHeight = static_cast<const SvxFontHeightItem&>(rItem);
        sal_uInt16 nHeight = rFontHeight.GetHeight();
        CPPUNIT_ASSERT_EQUAL(sal_uInt16(240), nHeight);
    }

    {
        const SfxPoolItem& rItem = pAttr->GetItem(ATTR_FONT);
        const SvxFontItem& rFont = static_cast<const SvxFontItem&>(rItem);
        OUString aName = rFont.GetFamilyName();
        CPPUNIT_ASSERT_EQUAL(OUString("Linux Biolinum G"), aName);
    }

    xDocSh->DoClose();
}

void ScFiltersTest::testTdf110440XLSX()
{
    ScDocShellRef xDocSh = loadDoc(u"tdf110440.", FORMAT_XLSX);
    CPPUNIT_ASSERT_MESSAGE("Failed to open doc", xDocSh.is());

    uno::Reference<frame::XModel> xModel = xDocSh->GetModel();
    uno::Reference<sheet::XSpreadsheetDocument> xDoc(xModel, uno::UNO_QUERY_THROW);
    uno::Reference<container::XIndexAccess> xIA(xDoc->getSheets(), uno::UNO_QUERY_THROW);
    uno::Reference<drawing::XDrawPageSupplier> xDrawPageSupplier(xIA->getByIndex(0), uno::UNO_QUERY_THROW);
    xIA.set(xDrawPageSupplier->getDrawPage(), uno::UNO_QUERY_THROW);
    uno::Reference<beans::XPropertySet> xShape(xIA->getByIndex(0), uno::UNO_QUERY_THROW);
    bool bVisible = true;
    xShape->getPropertyValue("Visible") >>= bVisible;
    // This failed: group shape's hidden property was lost on import.
    CPPUNIT_ASSERT(!bVisible);

    xDocSh->DoClose();
}

void ScFiltersTest::testTdf132278()
{
    // Would crash without the fix on loading
    ScDocShellRef xDocSh = loadDoc(u"tdf132278.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to open doc", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();
    rDoc.CalcAll(); // perform hard re-calculation.

    xDocSh->DoClose();
}

void ScFiltersTest::testTdf130959()
{
    // Would crash without the fix on loading
    ScDocShellRef xDocSh = loadDoc(u"tdf130959.", FORMAT_XLSX);
    CPPUNIT_ASSERT_MESSAGE("Failed to open doc", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();
    rDoc.CalcAll(); // perform hard re-calculation.

    xDocSh->DoClose();
}

void ScFiltersTest::testTdf129410()
{
    // Would crash without the fix on loading
    ScDocShellRef xDocSh = loadDoc(u"tdf129410.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to open doc", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();
    rDoc.CalcAll(); // perform hard re-calculation.

    xDocSh->DoClose();
}

void ScFiltersTest::testTdf138507()
{
    // Would fail to load without the fix
    ScDocShellRef xDocSh = loadDoc(u"tdf138507.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to open doc", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();
    rDoc.CalcAll(); // perform hard re-calculation.

    xDocSh->DoClose();
}

void ScFiltersTest::testTdf131380()
{
    ScDocShellRef xDocSh = loadDoc(u"tdf131380.", FORMAT_XLSX);
    CPPUNIT_ASSERT_MESSAGE("Failed to open doc", xDocSh.is());

    // Would crash without the fix on recalculating
    ScDocument& rDoc = xDocSh->GetDocument();
    rDoc.CalcAll(); // perform hard re-calculation.

    xDocSh->DoClose();
}

void ScFiltersTest::testTdf129681()
{
    ScDocShellRef xDocSh = loadDoc(u"tdf129681.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to open doc", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();

    CPPUNIT_ASSERT_EQUAL(OUString("Lamb"), rDoc.GetString(ScAddress(4,2,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("Beef"), rDoc.GetString(ScAddress(4,3,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("Pork"), rDoc.GetString(ScAddress(4,4,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("Goat"), rDoc.GetString(ScAddress(4,5,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("Goat"), rDoc.GetString(ScAddress(4,6,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("#VALUE!"), rDoc.GetString(ScAddress(4,7,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("#VALUE!"), rDoc.GetString(ScAddress(4,8,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("Deer"), rDoc.GetString(ScAddress(4,9,0)));

    CPPUNIT_ASSERT_EQUAL(OUString("1"), rDoc.GetString(ScAddress(6,2,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("2"), rDoc.GetString(ScAddress(6,3,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("3"), rDoc.GetString(ScAddress(6,4,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("5"), rDoc.GetString(ScAddress(6,5,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("8"), rDoc.GetString(ScAddress(6,6,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("#VALUE!"), rDoc.GetString(ScAddress(6,7,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("#VALUE!"), rDoc.GetString(ScAddress(6,8,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("#VALUE!"), rDoc.GetString(ScAddress(6,9,0)));

    xDocSh->DoClose();
}

void ScFiltersTest::testTdf111974XLSM()
{
    // Would crash without the fix on loading
    ScDocShellRef xDocSh = loadDoc(u"tdf111974.", FORMAT_XLSM);
    CPPUNIT_ASSERT_MESSAGE("Failed to open doc", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();
    rDoc.CalcAll(); // perform hard re-calculation.

    xDocSh->DoClose();
}

void ScFiltersTest::testBnc762542()
{
    ScDocShellRef xDocSh = loadDoc(u"bnc762542.", FORMAT_XLSX);
    CPPUNIT_ASSERT_MESSAGE("Failed to open doc", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();
    ScDrawLayer* pDrawLayer = rDoc.GetDrawLayer();
    SdrPage* pPage = pDrawLayer->GetPage(0);
    CPPUNIT_ASSERT_MESSAGE("draw page for sheet 1 should exist.", pPage);

    const size_t nCount = pPage->GetObjCount();
    CPPUNIT_ASSERT_EQUAL_MESSAGE("There should be 10 shapes.", static_cast<size_t>(10), nCount);

    // previously, some of the shapes were (incorrectly) rotated by 90 degrees
    for (size_t i : { 1, 2, 4, 5, 7, 9 })
    {
        SdrObject* pObj = pPage->GetObj(i);
        CPPUNIT_ASSERT_MESSAGE("Failed to get drawing object.", pObj);

        tools::Rectangle aRect(pObj->GetCurrentBoundRect());
        CPPUNIT_ASSERT_MESSAGE("Drawing object shouldn't be rotated.", aRect.GetWidth() > aRect.GetHeight());
    }

    xDocSh->DoClose();
}

void ScFiltersTest::testHiddenSheetsXLSX()
{
    ScDocShellRef xDocSh = loadDoc(u"hidden_sheets.", FORMAT_XLSX);
    CPPUNIT_ASSERT_MESSAGE("Failed to open doc", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();
    CPPUNIT_ASSERT_EQUAL_MESSAGE("1st sheet should be hidden", false, rDoc.IsVisible(0));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("2nd sheet should be visible", true, rDoc.IsVisible(1));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("3rd sheet should be hidden", false, rDoc.IsVisible(2));

    xDocSh->DoClose();
}

void ScFiltersTest::testAutofilterXLSX()
{
    ScDocShellRef xDocSh = loadDoc(u"autofilter.", FORMAT_XLSX);
    CPPUNIT_ASSERT_MESSAGE("Failed to open doc", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();
    const ScDBData* pData = rDoc.GetDBCollection()->GetDBNearCursor(0,0,0);
    CPPUNIT_ASSERT(pData);
    ScRange aRange;
    pData->GetArea(aRange);
    CPPUNIT_ASSERT_EQUAL(ScRange(0,0,0,2,4,0), aRange);

    xDocSh->DoClose();
}

namespace {

void checkValidationFormula(const ScAddress& rPos, const ScDocument& rDoc, const OUString& rExpectedFormula)
{
    const SfxUInt32Item* pItem = rDoc.GetAttr(rPos, ATTR_VALIDDATA);
    CPPUNIT_ASSERT(pItem);
    sal_uLong nKey = pItem->GetValue();
    const ScValidationData* pData = rDoc.GetValidationEntry(nKey);
    CPPUNIT_ASSERT(pData);

    OUString aFormula = pData->GetExpression(rPos, 0);
    CPPUNIT_ASSERT_EQUAL(rExpectedFormula, aFormula);
}

}

void ScFiltersTest::testRelFormulaValidationXLS()
{
    ScDocShellRef xDocSh = loadDoc(u"validation.", FORMAT_XLS);
    CPPUNIT_ASSERT_MESSAGE("Failed to open doc", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();

    checkValidationFormula(ScAddress(3, 4, 0), rDoc, "C5");
    checkValidationFormula(ScAddress(5, 8, 0), rDoc, "D7");

    xDocSh->DoClose();
}

void ScFiltersTest::testTdf136364()
{
    ScDocShellRef xDocSh = loadDoc(u"tdf136364.", FORMAT_XLSX);
    CPPUNIT_ASSERT_MESSAGE("Failed to open doc", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();

    // Without the fix in place, it would have failed with
    // - Expected: =SUM((B2:B3~C4:C5~D6:D7))
    // - Actual  : =SUM((B2:B3~C4:C5,D6:D7))
    OUString aFormula;
    rDoc.GetFormula(4, 0, 0, aFormula);
    CPPUNIT_ASSERT_EQUAL(OUString("=SUM((B2:B3~C4:C5~D6:D7))"), aFormula);
    CPPUNIT_ASSERT_EQUAL(27.0, rDoc.GetValue(ScAddress(4,0,0)));

    // - Expected: =SUM((B2~C4~D6))
    // - Actual  : =SUM((B2~C4,D6))
    rDoc.GetFormula(4, 1, 0, aFormula);
    CPPUNIT_ASSERT_EQUAL(OUString("=SUM((B2~C4~D6))"), aFormula);
    CPPUNIT_ASSERT_EQUAL(12.0, rDoc.GetValue(ScAddress(4,1,0)));

    xDocSh->DoClose();
}

void ScFiltersTest::testTdf103734()
{
    ScDocShellRef xDocSh = loadDoc(u"tdf103734.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to open doc", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    // Without the fix in place, MAX() would have returned -1.8E+308
    CPPUNIT_ASSERT_EQUAL(OUString("#N/A"), rDoc.GetString(ScAddress(2,0,0)));

    xDocSh->DoClose();
}

void ScFiltersTest::testTdf98844()
{
    ScDocShellRef xDocSh = loadDoc(u"tdf98844.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to open doc", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    CPPUNIT_ASSERT_EQUAL(47.6227, rDoc.GetValue(ScAddress(0,7,0)));
    CPPUNIT_ASSERT_EQUAL(48.0, rDoc.GetValue(ScAddress(0,8,0)));

    xDocSh->DoHardRecalc();

    // Without the fix in place, SUM() wouldn't have been updated when
    // Precision as shown is enabled
    CPPUNIT_ASSERT_EQUAL(48.0, rDoc.GetValue(ScAddress(0,7,0)));
    CPPUNIT_ASSERT_EQUAL(48.0, rDoc.GetValue(ScAddress(0,8,0)));

    xDocSh->DoClose();
}

void ScFiltersTest::testTdf100458()
{
    ScDocShellRef xDocSh = loadDoc(u"tdf100458_lost_zero_value.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to open doc", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();
    CPPUNIT_ASSERT(rDoc.HasValueData(0, 0, 0));
    CPPUNIT_ASSERT_EQUAL(0.0, rDoc.GetValue(0,0,0));
    CPPUNIT_ASSERT(!rDoc.HasStringData(0, 0, 0));
    xDocSh->DoClose();
}

void ScFiltersTest::testTdf118561()
{
    ScDocShellRef xDocSh = loadDoc(u"tdf118561.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to open doc", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    //Without the fix in place, it would have failed with
    //- Expected: apple
    //- Actual  : Err:502
    CPPUNIT_ASSERT_EQUAL(OUString("apple"), rDoc.GetString(ScAddress(1,1,1)));
    CPPUNIT_ASSERT_EQUAL(OUString("apple"), rDoc.GetString(ScAddress(2,1,1)));
    CPPUNIT_ASSERT_EQUAL(OUString("TRUE"), rDoc.GetString(ScAddress(3,1,1)));
    CPPUNIT_ASSERT_EQUAL(OUString("fruits"), rDoc.GetString(ScAddress(4,1,1)));
    CPPUNIT_ASSERT_EQUAL(OUString("apple"), rDoc.GetString(ScAddress(5,1,1)));
    CPPUNIT_ASSERT_EQUAL(OUString("hat"), rDoc.GetString(ScAddress(6,1,1)));

    xDocSh->DoClose();
}

void ScFiltersTest::testTdf125099()
{
    ScDocShellRef xDocSh = loadDoc(u"tdf125099.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to open doc", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    CPPUNIT_ASSERT_EQUAL(OUString("03:53:46"), rDoc.GetString(ScAddress(0,0,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("03:23:59"), rDoc.GetString(ScAddress(0,1,0)));

    xDocSh->DoHardRecalc();

    CPPUNIT_ASSERT_EQUAL(OUString("03:53:46"), rDoc.GetString(ScAddress(0,0,0)));

    // Without the fix in place, this would have failed with
    // - Expected: 03:24:00
    // - Actual  : 03:23:59
    CPPUNIT_ASSERT_EQUAL(OUString("03:24:00"), rDoc.GetString(ScAddress(0,1,0)));

    xDocSh->DoClose();
}

void ScFiltersTest::testTdf134455()
{
    ScDocShellRef xDocSh = loadDoc(u"tdf134455.", FORMAT_XLSX);
    CPPUNIT_ASSERT_MESSAGE("Failed to open doc", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    CPPUNIT_ASSERT_EQUAL(OUString("00:05"), rDoc.GetString(ScAddress(3,4,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("00:10"), rDoc.GetString(ScAddress(3,5,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("00:59"), rDoc.GetString(ScAddress(3,6,0)));

    // Without the fix in place, TIMEVALUE would have returned Err:502 for values
    // greater than 59
    CPPUNIT_ASSERT_EQUAL(OUString("01:05"), rDoc.GetString(ScAddress(3,7,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("04:00"), rDoc.GetString(ScAddress(3,8,0)));

    xDocSh->DoClose();
}

void ScFiltersTest::testTdf119533()
{
    ScDocShellRef xDocSh = loadDoc(u"tdf119533.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to open doc", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    // Without fix in place, this test would have failed with
    // - Expected: 0.5
    // - Actual  : 0.483333333333333
    CPPUNIT_ASSERT_EQUAL(OUString("0.5"), rDoc.GetString(ScAddress(4,0,0)));

    // Without fix in place, this test would have failed with
    // - Expected: 9.5
    // - Actual  : 9.51666666666667
    CPPUNIT_ASSERT_EQUAL(OUString("9.5"), rDoc.GetString(ScAddress(5,0,0)));

    xDocSh->DoClose();
}

void ScFiltersTest::testTdf127982()
{
    ScDocShellRef xDocSh = loadDoc(u"tdf127982.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to open doc", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    // Without the fix in place, these cells would be empty
    CPPUNIT_ASSERT_EQUAL(OUString("R1"), rDoc.GetString(ScAddress(3,5,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("R6"), rDoc.GetString(ScAddress(3,6,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("R7"), rDoc.GetString(ScAddress(3,7,0)));

    CPPUNIT_ASSERT_EQUAL(OUString("R1"), rDoc.GetString(ScAddress(4,5,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("R6"), rDoc.GetString(ScAddress(4,6,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("R7"), rDoc.GetString(ScAddress(4,7,0)));

    // Without the fix in place, these cells would be empty
    CPPUNIT_ASSERT_EQUAL(OUString("R1"), rDoc.GetString(ScAddress(4,5,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("R6"), rDoc.GetString(ScAddress(4,6,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("R7"), rDoc.GetString(ScAddress(4,7,0)));

    xDocSh->DoClose();
}

void ScFiltersTest::testTdf109409()
{
    ScDocShellRef xDocSh = loadDoc(u"tdf109409.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to open doc", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    // TEXTJOIN
    CPPUNIT_ASSERT_EQUAL(OUString("A1;B1;A2;B2;A3;B3"), rDoc.GetString(ScAddress(3,1,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("A1;B1;A2;B2;A3;B3"), rDoc.GetString(ScAddress(3,2,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("A1;A2;A3;B1;B2;B3"), rDoc.GetString(ScAddress(3,4,0)));

    // Without the fix in place, it would have failed with
    //- Expected: A1;B1;A2;B2;A3;B3
    //- Actual  : A1;A2;A3;B1;B2;B3
    CPPUNIT_ASSERT_EQUAL(OUString("A1;B1;A2;B2;A3;B3"), rDoc.GetString(ScAddress(3,5,0)));

    // CONCAT
    CPPUNIT_ASSERT_EQUAL(OUString("A1B1A2B2A3B3"), rDoc.GetString(ScAddress(6,1,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("A1B1A2B2A3B3"), rDoc.GetString(ScAddress(6,2,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("A1A2A3B1B2B3"), rDoc.GetString(ScAddress(6,4,0)));

    // Without the fix in place, it would have failed with
    //- Expected: A1B1A2B2A3B3
    //- Actual  : A1A2A3B1B2B3
    CPPUNIT_ASSERT_EQUAL(OUString("A1B1A2B2A3B3"), rDoc.GetString(ScAddress(6,5,0)));

    xDocSh->DoClose();
}

void ScFiltersTest::testTdf132105()
{
    ScDocShellRef xDocSh = loadDoc(u"tdf132105.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to open doc", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    // MATCH
    CPPUNIT_ASSERT_EQUAL(OUString("5"), rDoc.GetString(ScAddress(0,1,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("5"), rDoc.GetString(ScAddress(1,1,0)));

    // COUNT
    CPPUNIT_ASSERT_EQUAL(OUString("0"), rDoc.GetString(ScAddress(0,2,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("20"), rDoc.GetString(ScAddress(1,2,0)));

    // COUNTA
    CPPUNIT_ASSERT_EQUAL(OUString("20"), rDoc.GetString(ScAddress(0,3,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("20"), rDoc.GetString(ScAddress(1,3,0)));

    // COUNTBLANK
    // Without the fix in place, it would have failed with
    // - Expected: 0
    //- Actual  : Err:504
    CPPUNIT_ASSERT_EQUAL(OUString("0"), rDoc.GetString(ScAddress(0,4,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("0"), rDoc.GetString(ScAddress(1,4,0)));

    xDocSh->DoClose();
}

void ScFiltersTest::testTdf131424()
{
    ScDocShellRef xDocSh = loadDoc(u"tdf131424.", FORMAT_XLSX);
    CPPUNIT_ASSERT_MESSAGE("Failed to open doc", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    // Without the fix in place, table reference would have failed
    CPPUNIT_ASSERT_EQUAL(35.0, rDoc.GetValue(ScAddress(2,1,0)));
    CPPUNIT_ASSERT_EQUAL(58.0, rDoc.GetValue(ScAddress(2,2,0)));
    CPPUNIT_ASSERT_EQUAL(81.0, rDoc.GetValue(ScAddress(2,3,0)));
    CPPUNIT_ASSERT_EQUAL(104.0, rDoc.GetValue(ScAddress(2,4,0)));

    xDocSh->DoClose();
}

void ScFiltersTest::testTdf100709XLSX()
{
    ScDocShellRef xDocSh = ScBootstrapFixture::loadDoc(u"tdf100709.", FORMAT_XLSX);
    CPPUNIT_ASSERT_MESSAGE("Failed to load tdf100709.xlsx", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Cell B52 should not be formatted with a $", OUString("218"), rDoc.GetString(1, 51, 0));
    CPPUNIT_ASSERT_EQUAL_MESSAGE("Cell A75 should not be formatted as a date", OUString("218"), rDoc.GetString(0, 74, 0));

    xDocSh->DoClose();
}

void ScFiltersTest::testTdf97598XLSX()
{
    ScDocShellRef xDocSh = loadDoc(u"tdf97598_scenarios.", FORMAT_XLSX);
    CPPUNIT_ASSERT_MESSAGE("Failed to load tdf97598_scenarios.xlsx", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();
    OUString aStr = rDoc.GetString(0, 0, 0); // A1
    CPPUNIT_ASSERT_EQUAL(OUString("Cell A1"), aStr);

    xDocSh->DoClose();
}

void ScFiltersTest::testTdf83672XLSX()
{
    ScDocShellRef xDocSh = loadDoc(u"tdf83672.", FORMAT_XLSX);
    CPPUNIT_ASSERT_MESSAGE("Failed to load tdf83672.xlsx", xDocSh.is());
    uno::Reference< drawing::XDrawPagesSupplier > xDoc(
        xDocSh->GetModel(), uno::UNO_QUERY_THROW );
    uno::Reference< drawing::XDrawPage > xPage(
        xDoc->getDrawPages()->getByIndex(0), uno::UNO_QUERY_THROW );
    uno::Reference< drawing::XShape > xShape(
        xPage->getByIndex(0), uno::UNO_QUERY_THROW );
    uno::Reference< beans::XPropertySet > xShapeProperties(
        xShape, uno::UNO_QUERY );
    sal_Int32 nRotate = 0;
    xShapeProperties->getPropertyValue("RotateAngle") >>= nRotate;
    CPPUNIT_ASSERT(nRotate != 0);
    xDocSh->DoClose();
}

#ifdef UNX
void ScFiltersTest::testUnicodeFileNameGnumeric()
{
    // Mapping the LO-internal URL
    // <file:///.../sc/qa/unit/data/gnumeric/t%C3%A4%C3%9Ft.gnumeric> to the
    // repo's file sc/qa/unit/data/gnumeric/t\303\244\303\237t.gnumeric only
    // works when the system encoding is UTF-8:
    if (osl_getThreadTextEncoding() != RTL_TEXTENCODING_UTF8) {
        return;
    }
    ScDocShellRef xDocSh = loadDoc(u"t\u00E4\u00DFt.", FORMAT_GNUMERIC);
    CPPUNIT_ASSERT(xDocSh.is());

    xDocSh->DoClose();
}
#endif

void ScFiltersTest::testMergedCellsXLSXML()
{
    ScDocShellRef xDocSh = loadDoc(u"merged-cells.", FORMAT_XLS_XML);
    CPPUNIT_ASSERT_MESSAGE("Failed to load merged-cells.xml", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    // B1:C1 is merged.
    ScRange aMergedRange(1,0,0); // B1
    rDoc.ExtendTotalMerge(aMergedRange);
    CPPUNIT_ASSERT_EQUAL(ScRange(1,0,0,2,0,0), aMergedRange);

    // D1:F1 is merged.
    aMergedRange = ScRange(3,0,0); // D1
    rDoc.ExtendTotalMerge(aMergedRange);
    CPPUNIT_ASSERT_EQUAL(ScRange(3,0,0,5,0,0), aMergedRange);

    // A2:A3 is merged.
    aMergedRange = ScRange(0,1,0); // A2
    rDoc.ExtendTotalMerge(aMergedRange);
    CPPUNIT_ASSERT_EQUAL(ScRange(0,1,0,0,2,0), aMergedRange);

    // A4:A6 is merged.
    aMergedRange = ScRange(0,3,0); // A4
    rDoc.ExtendTotalMerge(aMergedRange);
    CPPUNIT_ASSERT_EQUAL(ScRange(0,3,0,0,5,0), aMergedRange);

    // C3:F6 is merged.
    aMergedRange = ScRange(2,2,0); // C3
    rDoc.ExtendTotalMerge(aMergedRange);
    CPPUNIT_ASSERT_EQUAL(ScRange(2,2,0,5,5,0), aMergedRange);

    xDocSh->DoClose();
}

void ScFiltersTest::testBackgroundColorStandardXLSXML()
{
    ScDocShellRef xDocSh = loadDoc(u"background-color-standard.", FORMAT_XLS_XML);
    CPPUNIT_ASSERT_MESSAGE("Failed to load background-color-standard.xml", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    struct Check
    {
        OUString aCellValue;
        Color aFontColor;
        Color aBgColor;
    };

    const std::vector<Check> aChecks =
    {
        { OUString("Background Color"), COL_BLACK, COL_TRANSPARENT        },
        { OUString("Dark Red"),         COL_WHITE, Color(192,  0,    0)   },
        { OUString("Red"),              COL_WHITE, Color(255,  0,    0)   },
        { OUString("Orange"),           COL_WHITE, Color(255, 192,   0)   },
        { OUString("Yellow"),           COL_WHITE, Color(255, 255,   0)   },
        { OUString("Light Green"),      COL_WHITE, Color(146, 208,  80)   },
        { OUString("Green"),            COL_WHITE, Color(  0, 176,  80)   },
        { OUString("Light Blue"),       COL_WHITE, Color(  0, 176, 240)   },
        { OUString("Blue"),             COL_WHITE, Color(  0, 112, 192)   },
        { OUString("Dark Blue"),        COL_WHITE, Color(  0,  32,  96)   },
        { OUString("Purple"),           COL_WHITE, Color(112,  48, 160)   },
    };

    for (size_t nRow = 0; nRow < aChecks.size(); ++nRow)
    {
        ScAddress aPos(0, nRow, 0);
        OUString aStr = rDoc.GetString(aPos);
        CPPUNIT_ASSERT_EQUAL(aChecks[nRow].aCellValue, aStr);

        const ScPatternAttr* pPat = rDoc.GetPattern(aPos);
        CPPUNIT_ASSERT(pPat);

        const SvxColorItem& rColor = pPat->GetItem(ATTR_FONT_COLOR);
        CPPUNIT_ASSERT_EQUAL(aChecks[nRow].aFontColor, rColor.GetValue());

        const SvxBrushItem& rBgColor = pPat->GetItem(ATTR_BACKGROUND);
        CPPUNIT_ASSERT_EQUAL(aChecks[nRow].aBgColor, rBgColor.GetColor());
    }

    xDocSh->DoClose();
}

void ScFiltersTest::testTdf131536()
{
    ScDocShellRef xDocSh = loadDoc(u"tdf131536.", FORMAT_XLSX);
    CPPUNIT_ASSERT_MESSAGE("Failed to load the document", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    ScAddress aPos(3,9,0);
    CPPUNIT_ASSERT_EQUAL(1.0, rDoc.GetValue(aPos));
    ASSERT_FORMULA_EQUAL(rDoc, aPos, "IF(D$4=\"-\",\"-\",MID(TEXT(INDEX($Comparison.$I:$J,$Comparison.$A5,$Comparison.D$2),\"\"),2,4)"
                                     "=RIGHT(TEXT(INDEX($Comparison.$L:$Z,$Comparison.$A5,$Comparison.D$4),\"\"),4))", nullptr);

    ScAddress aPos2(4,9,0);
    CPPUNIT_ASSERT_EQUAL(1.0, rDoc.GetValue(aPos2));
    ASSERT_FORMULA_EQUAL(rDoc, aPos2, "IF(D$4=\"-\",\"-\",MID(TEXT(INDEX($Comparison.$I:$J,$Comparison.$A5,$Comparison.D$2),\"0\"),2,4)"
                                      "=RIGHT(TEXT(INDEX($Comparison.$L:$Z,$Comparison.$A5,$Comparison.D$4),\"0\"),4))", nullptr);
}

void ScFiltersTest::testTdf130583()
{
    ScDocShellRef xDocSh = loadDoc(u"tdf130583.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to load the document", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    CPPUNIT_ASSERT_EQUAL(OUString("b"), rDoc.GetString(ScAddress(1,0,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("c"), rDoc.GetString(ScAddress(1,1,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("a"), rDoc.GetString(ScAddress(1,2,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("d"), rDoc.GetString(ScAddress(1,3,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("#N/A"), rDoc.GetString(ScAddress(1,4,0)));

    // Without the fix in place, SWITCH would have returned #VALUE!
    CPPUNIT_ASSERT_EQUAL(OUString("b"), rDoc.GetString(ScAddress(4,0,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("c"), rDoc.GetString(ScAddress(4,1,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("a"), rDoc.GetString(ScAddress(4,2,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("d"), rDoc.GetString(ScAddress(4,3,0)));
    CPPUNIT_ASSERT_EQUAL(OUString("#N/A"), rDoc.GetString(ScAddress(4,4,0)));
}

void ScFiltersTest::testTdf85617()
{
    ScDocShellRef xDocSh = loadDoc(u"tdf85617.", FORMAT_XLSX);
    CPPUNIT_ASSERT_MESSAGE("Failed to load the document", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    ScAddress aPos(2,2,0);
    //Without the fix in place, it would be Err:509
    CPPUNIT_ASSERT_EQUAL(4.5, rDoc.GetValue(aPos));
}

void ScFiltersTest::testTdf134234()
{
    ScDocShellRef xDocSh = loadDoc(u"tdf134234.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to load the document", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    CPPUNIT_ASSERT_EQUAL(3.0, rDoc.GetValue(ScAddress(1,0,1)));

    //Without the fix in place, SUMPRODUCT would have returned 0
    CPPUNIT_ASSERT_EQUAL(36.54, rDoc.GetValue(ScAddress(2,0,1)));
    CPPUNIT_ASSERT_EQUAL(sal_uInt32(833), static_cast<sal_uInt32>(rDoc.GetValue(ScAddress(3,0,1))));
}

void ScFiltersTest::testNamedExpressionsXLSXML()
{
    {
        // global named expressions

        ScDocShellRef xDocSh = loadDoc(u"named-exp-global.", FORMAT_XLS_XML);
        CPPUNIT_ASSERT_MESSAGE("Failed to load named-exp-global.xml", xDocSh.is());
        ScDocument& rDoc = xDocSh->GetDocument();

        // A7
        ScAddress aPos(0,6,0);
        CPPUNIT_ASSERT_EQUAL(15.0, rDoc.GetValue(aPos));
        ASSERT_FORMULA_EQUAL(rDoc, aPos, "SUM(MyRange)", nullptr);

        // B7
        aPos.IncCol();
        CPPUNIT_ASSERT_EQUAL(55.0, rDoc.GetValue(aPos));
        ASSERT_FORMULA_EQUAL(rDoc, aPos, "SUM(MyRange2)", nullptr);

        const ScRangeData* pRD = rDoc.GetRangeName()->findByUpperName("MYRANGE");
        CPPUNIT_ASSERT(pRD);
        pRD = rDoc.GetRangeName()->findByUpperName("MYRANGE2");
        CPPUNIT_ASSERT(pRD);

        xDocSh->DoClose();
    }

    {
        // sheet-local named expressions

        ScDocShellRef xDocSh = loadDoc(u"named-exp-local.", FORMAT_XLS_XML);
        CPPUNIT_ASSERT_MESSAGE("Failed to load named-exp-local.xml", xDocSh.is());
        ScDocument& rDoc = xDocSh->GetDocument();

        // A7 on Sheet1
        ScAddress aPos(0,6,0);
        CPPUNIT_ASSERT_EQUAL(27.0, rDoc.GetValue(aPos));
        ASSERT_FORMULA_EQUAL(rDoc, aPos, "SUM(MyRange)", nullptr);

        // A7 on Sheet2
        aPos.IncTab();
        CPPUNIT_ASSERT_EQUAL(74.0, rDoc.GetValue(aPos));
        ASSERT_FORMULA_EQUAL(rDoc, aPos, "SUM(MyRange)", nullptr);

        const ScRangeName* pRN = rDoc.GetRangeName(0);
        CPPUNIT_ASSERT(pRN);
        const ScRangeData* pRD = pRN->findByUpperName("MYRANGE");
        CPPUNIT_ASSERT(pRD);
        pRN = rDoc.GetRangeName(1);
        CPPUNIT_ASSERT(pRN);
        pRD = pRN->findByUpperName("MYRANGE");
        CPPUNIT_ASSERT(pRD);

        xDocSh->DoClose();
    }
}

void ScFiltersTest::testEmptyRowsXLSXML()
{
    ScDocShellRef xDocSh = loadDoc(u"empty-rows.", FORMAT_XLS_XML);
    CPPUNIT_ASSERT_MESSAGE("Failed to load empty-rows.xml", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    {
        // Expected output table content.  0 = empty cell
        std::vector<std::vector<const char*>> aOutputCheck = {
            { "Top row, followed by 2 empty rows.", nullptr },
            { nullptr, nullptr },
            { nullptr, nullptr },
            { nullptr,     "1" },
            { nullptr,     "2" },
            { nullptr,     "3" },
            { nullptr,     "4" },
            { nullptr,     "5" },
            { nullptr,    "15" },
        };

        ScRange aDataRange;
        aDataRange.Parse("A1:B9", rDoc);
        bool bSuccess = checkOutput(&rDoc, aDataRange, aOutputCheck, "Expected output");
        CPPUNIT_ASSERT_MESSAGE("Table output check failed", bSuccess);
    }

    ScAddress aPos;
    aPos.Parse("B9", rDoc);
    ASSERT_FORMULA_EQUAL(rDoc, aPos, "SUM(B4:B8)", nullptr);

    xDocSh->DoClose();
}

void ScFiltersTest::testBorderDirectionsXLSXML()
{
    ScDocShellRef xDocSh = loadDoc(u"border-directions.", FORMAT_XLS_XML);
    CPPUNIT_ASSERT_MESSAGE("Failed to load border-directions.xml", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    struct Check
    {
        ScAddress aPos;
        bool bTop;
        bool bBottom;
        bool bLeft;
        bool bRight;
        bool bTLtoBR;
        bool bTRtoBL;
    };

    std::vector<Check> aChecks = {
        { { 1,  1, 0 },  true, false, false, false, false, false }, // B2 - top
        { { 1,  3, 0 }, false, false,  true, false, false, false }, // B4 - left
        { { 1,  5, 0 }, false, false, false,  true, false, false }, // B6 - right
        { { 1,  7, 0 }, false,  true, false, false, false, false }, // B8 - bottom
        { { 1,  9, 0 }, false, false, false, false,  true, false }, // B10 - tl to br
        { { 1, 11, 0 }, false, false, false, false, false,  true }, // B12 - tr to bl
        { { 1, 13, 0 }, false, false, false, false,  true,  true }, // B14 - cross-diagonal
    };

    auto funcCheckBorder = []( bool bHasBorder, const editeng::SvxBorderLine* pLine ) -> bool
    {
        if (bHasBorder)
        {
            if (!pLine)
            {
                std::cout << "Border was expected, but not found!" << std::endl;
                return false;
            }

            if (SvxBorderLineStyle::SOLID != pLine->GetBorderLineStyle())
            {
                std::cout << "Border type was expected to be of SOLID, but is not." << std::endl;
                return false;
            }

            if (COL_BLACK != pLine->GetColor())
            {
                std::cout << "Border color was expected to be black, but is not." << std::endl;
                return false;
            }
        }
        else
        {
            if (pLine)
            {
                std::cout << "Border was not expected, but is found!" << std::endl;
                return false;
            }
        }

        return true;
    };

    for (const Check& c : aChecks)
    {
        const ScPatternAttr* pPat = rDoc.GetPattern(c.aPos);
        CPPUNIT_ASSERT(pPat);

        const SvxBoxItem& rBox = pPat->GetItem(ATTR_BORDER);

        const editeng::SvxBorderLine* pLine = rBox.GetTop();
        CPPUNIT_ASSERT(funcCheckBorder(c.bTop, pLine));

        pLine = rBox.GetBottom();
        CPPUNIT_ASSERT(funcCheckBorder(c.bBottom, pLine));

        pLine = rBox.GetLeft();
        CPPUNIT_ASSERT(funcCheckBorder(c.bLeft, pLine));

        pLine = rBox.GetRight();
        CPPUNIT_ASSERT(funcCheckBorder(c.bRight, pLine));

        pLine = pPat->GetItem(ATTR_BORDER_TLBR).GetLine();
        CPPUNIT_ASSERT(funcCheckBorder(c.bTLtoBR, pLine));

        pLine = pPat->GetItem(ATTR_BORDER_BLTR).GetLine();
        CPPUNIT_ASSERT(funcCheckBorder(c.bTRtoBL, pLine));
    }

    xDocSh->DoClose();
}

void ScFiltersTest::testBorderColorsXLSXML()
{
    ScDocShellRef xDocSh = loadDoc(u"border-colors.", FORMAT_XLS_XML);
    CPPUNIT_ASSERT_MESSAGE("Failed to load border-colors.xml", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    // B3 - red
    const ScPatternAttr* pPat = rDoc.GetPattern(ScAddress(1,2,0));
    CPPUNIT_ASSERT(pPat);
    const editeng::SvxBorderLine* pLine = pPat->GetItem(ATTR_BORDER).GetRight();
    CPPUNIT_ASSERT(pLine);
    CPPUNIT_ASSERT_EQUAL(SvxBorderLineStyle::SOLID, pLine->GetBorderLineStyle());
    CPPUNIT_ASSERT_EQUAL(Color(255,0,0), pLine->GetColor());

    // B4 - blue
    pPat = rDoc.GetPattern(ScAddress(1,3,0));
    CPPUNIT_ASSERT(pPat);
    pLine = pPat->GetItem(ATTR_BORDER).GetRight();
    CPPUNIT_ASSERT(pLine);
    CPPUNIT_ASSERT_EQUAL(SvxBorderLineStyle::SOLID, pLine->GetBorderLineStyle());
    CPPUNIT_ASSERT_EQUAL(Color(0,112,192), pLine->GetColor());

    // B5 - green
    pPat = rDoc.GetPattern(ScAddress(1,4,0));
    CPPUNIT_ASSERT(pPat);
    pLine = pPat->GetItem(ATTR_BORDER).GetRight();
    CPPUNIT_ASSERT(pLine);
    CPPUNIT_ASSERT_EQUAL(SvxBorderLineStyle::SOLID, pLine->GetBorderLineStyle());
    CPPUNIT_ASSERT_EQUAL(Color(0,176,80), pLine->GetColor());

    // B7 - yellow (left), purple (right), light blue (cross)
    pPat = rDoc.GetPattern(ScAddress(1,6,0));
    CPPUNIT_ASSERT(pPat);

    pLine = pPat->GetItem(ATTR_BORDER).GetLeft();
    CPPUNIT_ASSERT(pLine);
    CPPUNIT_ASSERT_EQUAL(SvxBorderLineStyle::SOLID, pLine->GetBorderLineStyle());
    CPPUNIT_ASSERT_EQUAL(Color(255,255,0), pLine->GetColor()); // yellow

    pLine = pPat->GetItem(ATTR_BORDER).GetRight();
    CPPUNIT_ASSERT(pLine);
    CPPUNIT_ASSERT_EQUAL(SvxBorderLineStyle::SOLID, pLine->GetBorderLineStyle());
    CPPUNIT_ASSERT_EQUAL(Color(112,48,160), pLine->GetColor()); // purple

    pLine = pPat->GetItem(ATTR_BORDER_TLBR).GetLine();
    CPPUNIT_ASSERT(pLine);
    CPPUNIT_ASSERT_EQUAL(SvxBorderLineStyle::SOLID, pLine->GetBorderLineStyle());
    CPPUNIT_ASSERT_EQUAL(Color(0,176,240), pLine->GetColor()); // light blue

    pLine = pPat->GetItem(ATTR_BORDER_BLTR).GetLine();
    CPPUNIT_ASSERT(pLine);
    CPPUNIT_ASSERT_EQUAL(SvxBorderLineStyle::SOLID, pLine->GetBorderLineStyle());
    CPPUNIT_ASSERT_EQUAL(Color(0,176,240), pLine->GetColor()); // light blue

    xDocSh->DoClose();
}

void ScFiltersTest::testHiddenRowsColumnsXLSXML()
{
    ScDocShellRef xDocSh = loadDoc(u"hidden-rows-columns.", FORMAT_XLS_XML);
    CPPUNIT_ASSERT_MESSAGE("Failed to load hidden-rows-columns.xml", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    struct Check
    {
        SCCOLROW nPos1;
        SCCOLROW nPos2;
        bool bVisible;
    };

    std::vector<Check> aRowChecks = {
        { 0, 0, true  },
        { 1, 2, false },
        { 3, 3, true  },
        { 4, 4, false },
        { 5, 7, true  },
        { 8, 8, false },
        { 9, MAXROW, true },
    };

    for (const Check& c : aRowChecks)
    {
        SCROW nRow1 = -1, nRow2 = -1;
        bool bVisible = !rDoc.RowHidden(c.nPos1, 0, &nRow1, &nRow2);
        CPPUNIT_ASSERT_EQUAL(bVisible, c.bVisible);
        CPPUNIT_ASSERT_EQUAL(c.nPos1, nRow1);
        CPPUNIT_ASSERT_EQUAL(c.nPos2, nRow2);
    }

    std::vector<Check> aColChecks = {
        { 0, 1, true  },
        { 2, 5, false },
        { 6, 9, true  },
        { 10, 10, false },
        { 11, MAXCOL, true },
    };

    for (const Check& c : aColChecks)
    {
        SCCOL nCol1 = -1, nCol2 = -1;
        bool bVisible = !rDoc.ColHidden(c.nPos1, 1, &nCol1, &nCol2);
        CPPUNIT_ASSERT_EQUAL(bVisible, c.bVisible);
        CPPUNIT_ASSERT_EQUAL(c.nPos1, SCCOLROW(nCol1));
        CPPUNIT_ASSERT_EQUAL(c.nPos2, SCCOLROW(nCol2));
    }

    xDocSh->DoClose();
}

void ScFiltersTest::testColumnWidthRowHeightXLSXML()
{
    ScDocShellRef xDocSh = loadDoc(u"column-width-row-height.", FORMAT_XLS_XML);
    CPPUNIT_ASSERT_MESSAGE("Failed to load column-width-row-height.xml", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    struct RowHeight
    {
        SCROW nRow1;
        SCROW nRow2;
        sal_uInt16 nHeight; // in points (1 point == 20 twips)
    };

    std::vector<RowHeight> aRowChecks = {
        {  2,  2, 20 },
        {  3,  3, 30 },
        {  4,  4, 40 },
        {  5,  5, 50 },
        {  7,  9, 25 },
        { 12, 13, 35 },
    };

    for (const RowHeight& rh : aRowChecks)
    {
        for (SCROW i = rh.nRow1; i <= rh.nRow2; ++i)
        {
            sal_uInt16 nHeight = rDoc.GetRowHeight(i, 0);
            CPPUNIT_ASSERT_EQUAL(sal_uInt16(rh.nHeight*20), nHeight);
        }
    }

    struct ColWidth
    {
        SCCOL nCol1;
        SCCOL nCol2;
        sal_uInt16 nWidth; // in points (1 point == 20 twips
    };

    std::vector<ColWidth> aColChecks = {
        {  1,  1,  56 },
        {  2,  2,  83 },
        {  3,  3, 109 },
        {  5,  7,  67 },
        { 10, 11, 119 },
    };

    for (const ColWidth& cw : aColChecks)
    {
        for (SCCOL i = cw.nCol1; i <= cw.nCol2; ++i)
        {
            sal_uInt16 nWidth = rDoc.GetColWidth(i, 0);
            CPPUNIT_ASSERT_EQUAL(sal_uInt16(cw.nWidth*20), nWidth);
        }
    }

    xDocSh->DoClose();
}

void ScFiltersTest::testCharacterSetXLSXML()
{
    ScDocShellRef xDocSh = loadDoc(u"character-set.", FORMAT_XLS_XML);
    CPPUNIT_ASSERT_MESSAGE("Failed to load column-width-row-height.xml", xDocSh.is());
    ScDocument& rDoc = xDocSh->GetDocument();

    CPPUNIT_ASSERT_EQUAL(SCTAB(1), rDoc.GetTableCount());

    OUString aName;
    rDoc.GetName(0, aName);

    // Check the sheet name.  The values are all Cyrillic letters.
    std::vector<sal_Unicode> aBuf = { 0x041b, 0x0438, 0x0441, 0x0442, 0x0031 };
    OUString aExpected(aBuf.data(), aBuf.size());
    CPPUNIT_ASSERT_EQUAL(aExpected, aName);

    // Check the value of I4
    OUString aVal = rDoc.GetString(ScAddress(8,3,0));
    aBuf = { 0x0421, 0x0443, 0x043c, 0x043c, 0x0430 };
    aExpected = OUString(aBuf.data(), aBuf.size());
    CPPUNIT_ASSERT_EQUAL(aExpected, aVal);

    // Check the value of J3
    aVal = rDoc.GetString(ScAddress(9,2,0));
    aBuf = { 0x041e, 0x0441, 0x0442, 0x0430, 0x0442, 0x043e, 0x043a };
    aExpected = OUString(aBuf.data(), aBuf.size());
    CPPUNIT_ASSERT_EQUAL(aExpected, aVal);

    xDocSh->DoClose();
}

void ScFiltersTest::testCondFormatXLSB()
{
    ScDocShellRef xDocSh = loadDoc(u"cond_format.", FORMAT_XLSB);

    ScDocument& rDoc = xDocSh->GetDocument();
    ScConditionalFormatList* pList = rDoc.GetCondFormList(0);
    CPPUNIT_ASSERT(pList);

    CPPUNIT_ASSERT_EQUAL(size_t(1), pList->size());
    ScConditionalFormat* pFormat = pList->begin()->get();
    CPPUNIT_ASSERT(pFormat);
    CPPUNIT_ASSERT_EQUAL(size_t(1), pFormat->size());

    xDocSh->DoClose();
}

void ScFiltersTest::testPageScalingXLSX()
{
    ScDocShellRef xDocSh = loadDoc(u"page_scale.", FORMAT_XLSX);
    ScDocument& rDoc = xDocSh->GetDocument();

    OUString aStyleName = rDoc.GetPageStyle(0);
    ScStyleSheetPool* pStylePool = rDoc.GetStyleSheetPool();
    SfxStyleSheetBase* pStyleSheet = pStylePool->Find( aStyleName, SfxStyleFamily::Page );
    CPPUNIT_ASSERT(pStyleSheet);

    SfxItemSet& rSet = pStyleSheet->GetItemSet();
    sal_uInt16 nVal = rSet.Get(ATTR_PAGE_SCALE).GetValue();
    CPPUNIT_ASSERT_EQUAL(sal_uInt16(90), nVal);

    xDocSh->DoClose();
}

void ScFiltersTest::testActiveXCheckboxXLSX()
{
    ScDocShellRef xDocSh = loadDoc(u"activex_checkbox.", FORMAT_XLSX);
    uno::Reference< frame::XModel > xModel = xDocSh->GetModel();
    uno::Reference< sheet::XSpreadsheetDocument > xDoc(xModel, UNO_QUERY_THROW);
    uno::Reference< container::XIndexAccess > xIA(xDoc->getSheets(), UNO_QUERY_THROW);
    uno::Reference< drawing::XDrawPageSupplier > xDrawPageSupplier(xIA->getByIndex(0), UNO_QUERY_THROW);
    uno::Reference< container::XIndexAccess > xIA_DrawPage(xDrawPageSupplier->getDrawPage(), UNO_QUERY_THROW);
    uno::Reference< drawing::XControlShape > xControlShape(xIA_DrawPage->getByIndex(0), UNO_QUERY_THROW);

    // Check control type
    uno::Reference<beans::XPropertySet> xPropertySet(xControlShape->getControl(), uno::UNO_QUERY);
    uno::Reference<lang::XServiceInfo> xServiceInfo(xPropertySet, uno::UNO_QUERY);
    CPPUNIT_ASSERT_EQUAL(true, bool(xServiceInfo->supportsService("com.sun.star.form.component.CheckBox")));

    // Check custom label
    OUString sLabel;
    xPropertySet->getPropertyValue("Label") >>= sLabel;
    CPPUNIT_ASSERT_EQUAL(OUString("Custom Caption"), sLabel);

    // Check background color (highlight system color)
    sal_Int32 nColor;
    xPropertySet->getPropertyValue("BackgroundColor") >>= nColor;
    CPPUNIT_ASSERT_EQUAL(sal_Int32(0x316AC5), nColor);

    // Check Text color (active border system color)
    xPropertySet->getPropertyValue("TextColor") >>= nColor;
    CPPUNIT_ASSERT_EQUAL(sal_Int32(0xD4D0C8), nColor);

    // Check state of the checkbox
    sal_Int16 nState;
    xPropertySet->getPropertyValue("State") >>= nState;
    CPPUNIT_ASSERT_EQUAL(sal_Int16(1), nState);

    xDocSh->DoClose();
}

void ScFiltersTest::testtdf120301_xmlSpaceParsingXLSX()
{
    ScDocShellRef xDocSh = loadDoc(u"tdf120301_xmlSpaceParsing.", FORMAT_XLSX);
    uno::Reference< frame::XModel > xModel = xDocSh->GetModel();
    uno::Reference< sheet::XSpreadsheetDocument > xDoc(xModel, UNO_QUERY_THROW);
    uno::Reference< container::XIndexAccess > xIA(xDoc->getSheets(), UNO_QUERY_THROW);
    uno::Reference< drawing::XDrawPageSupplier > xDrawPageSupplier( xIA->getByIndex(0), UNO_QUERY_THROW);
    uno::Reference< container::XIndexAccess > xIA_DrawPage(xDrawPageSupplier->getDrawPage(), UNO_QUERY_THROW);

    uno::Reference< drawing::XControlShape > xControlShape(xIA_DrawPage->getByIndex(0), UNO_QUERY_THROW);
    uno::Reference< beans::XPropertySet > XPropSet( xControlShape->getControl(), uno::UNO_QUERY_THROW );
    OUString sCaption;
    XPropSet->getPropertyValue("Label") >>= sCaption;
    CPPUNIT_ASSERT_EQUAL(OUString("Check Box 1"), sCaption);
    xDocSh->DoClose();
}

namespace {

struct PaintListener : public SfxListener
{
    bool mbCalled = false;
    virtual void Notify(SfxBroadcaster& /*rBC*/, const SfxHint& rHint) override
    {
        const ScPaintHint* pPaintHint = dynamic_cast<const ScPaintHint*>(&rHint);
        if (pPaintHint)
        {
            if (pPaintHint->GetStartCol() <= 0 && pPaintHint->GetEndCol() >= 0
                    && pPaintHint->GetStartRow() <= 9 && pPaintHint->GetEndRow() >= 9)
            {
                mbCalled = true;
            }
        }
    }
};

}

void ScFiltersTest::testCondFormatFormulaListenerXLSX()
{
    ScDocShellRef xDocSh = loadDoc(u"cond_format_formula_listener.", FORMAT_XLSX);
    PaintListener aListener;
    aListener.StartListening(*xDocSh);
    ScDocument& rDoc = xDocSh->GetDocument();
    ScConditionalFormatList* pList = rDoc.GetCondFormList(0);
    CPPUNIT_ASSERT(pList);

    CPPUNIT_ASSERT_EQUAL(size_t(1), pList->size());
    ScConditionalFormat* pFormat = pList->begin()->get();
    CPPUNIT_ASSERT(pFormat);
    rDoc.SetDocVisible(true);
    rDoc.SetValue(0, 0, 0, 2.0);

    CPPUNIT_ASSERT(aListener.mbCalled);

    xDocSh->DoClose();
}

void ScFiltersTest::testTdf137091()
{
    // Set the system locale to Turkish
    SvtSysLocaleOptions aOptions;
    OUString sLocaleConfigString = aOptions.GetLanguageTag().getBcp47();
    aOptions.SetLocaleConfigString("tr-TR");
    aOptions.Commit();
    comphelper::ScopeGuard g([&aOptions, &sLocaleConfigString] {
        aOptions.SetLocaleConfigString(sLocaleConfigString);
        aOptions.Commit();
    });

    ScDocShellRef xDocSh = loadDoc(u"tdf137091.", FORMAT_XLSX);
    ScDocument& rDoc = xDocSh->GetDocument();

    // Without the fix in place, this test would have failed with
    // - Expected: 28/4
    // - Actual  : Err:507
    CPPUNIT_ASSERT_EQUAL(OUString("28/4"), rDoc.GetString(ScAddress(2,1,0)));
    xDocSh->DoClose();
}

void ScFiltersTest::testTdf62268()
{
    ScDocShellRef xDocSh = loadDoc(u"tdf62268.", FORMAT_ODS);
    ScDocument& rDoc = xDocSh->GetDocument();
    int nHeight;

    SCTAB nTab = 0;
    nHeight = rDoc.GetRowHeight(0, nTab, false);
    CPPUNIT_ASSERT_LESSEQUAL( 3, abs( 256 - nHeight ) );
    nHeight = rDoc.GetRowHeight(1, nTab, false);
    CPPUNIT_ASSERT_LESSEQUAL( 19, abs( 1905 - nHeight ) );

    xDocSh->DoClose();
}

void ScFiltersTest::testTdf137453()
{
    ScDocShellRef xDocSh = loadDoc(u"tdf137453.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to open doc", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();

    // Without the fix in place, this test would have failed with
    // - Expected: 3333333333/100
    // - Actual  : -961633963/100
    CPPUNIT_ASSERT_EQUAL(OUString("3333333333/100"), rDoc.GetString(ScAddress(0,0,0)));

    xDocSh->DoClose();
}

void ScFiltersTest::testTdf35636()
{
    ScDocShellRef xDocSh = loadDoc(u"tdf35636.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to open doc", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();

    // Without the fix in place, SUMIF would have returned 0.0
    // with empty cells in the criteria
    CPPUNIT_ASSERT_EQUAL(50.0, rDoc.GetValue(ScAddress(1,4,0)));

    xDocSh->DoClose();
}

void ScFiltersTest::testVBAMacroFunctionODS()
{
    ScDocShellRef xDocSh = loadDoc(u"vba_macro_functions.", FORMAT_ODS);
    ScDocument& rDoc = xDocSh->GetDocument();

    OUString aFunction;
    rDoc.GetFormula(2, 0, 0, aFunction);
    std::cout << aFunction << std::endl;
    CPPUNIT_ASSERT_DOUBLES_EQUAL(10.0, rDoc.GetValue(2, 0, 0), 1e-6);

    xDocSh->DoClose();
}

void ScFiltersTest::testAutoheight2Rows()
{
    ScDocShellRef xDocSh = loadDoc(u"autoheight2rows.", FORMAT_ODS);
    ScDocument& rDoc = xDocSh->GetDocument();

    SCTAB nTab = 0;
    int nHeight1 = rDoc.GetRowHeight(0, nTab, false);
    int nHeight2 = rDoc.GetRowHeight(1, nTab, false);
    int nHeight4 = rDoc.GetRowHeight(3, nTab, false);
    int nHeight5 = rDoc.GetRowHeight(4, nTab, false);

    // We will do relative comparison, because calculated autoheight
    // can be different on different platforms
    CPPUNIT_ASSERT_MESSAGE("Row #1 and row #4 must have same height after load & auto-adjust",
                           abs( nHeight1 - nHeight4 ) < 10 );
    CPPUNIT_ASSERT_MESSAGE("Row #2 and row #5 must have same height after load & auto-adjust",
                           abs( nHeight2 - nHeight5 ) < 10 );

    xDocSh->DoClose();
}

void ScFiltersTest::testXLSDefColWidth()
{
    // XLS has only 256 columns; but on import, we need to set default width to all above that limit
    ScDocShellRef xDocSh = loadDoc(u"chartx.", FORMAT_XLS); // just some XLS with narrow columns
    ScDocument& rDoc = xDocSh->GetDocument();

    int nWidth = rDoc.GetColWidth(MAXCOL, 0, false);
    // This was 1280
    CPPUNIT_ASSERT_EQUAL(1005, nWidth);

    xDocSh->DoClose();
}

void ScFiltersTest::testPreviewMissingObjLink()
{
    ScDocShellRef xDocSh = loadDoc(u"keep-preview-missing-obj-link.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to load keep-preview-missing-obj-link.ods.", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();

    // Retrieve the ole object
    const SdrOle2Obj* pOleObj = getSingleOleObject(rDoc, 0);
    CPPUNIT_ASSERT_MESSAGE("Failed to retrieve an ole object from the 2nd sheet.", pOleObj);

    const Graphic* pGraphic = pOleObj->GetGraphic();
    CPPUNIT_ASSERT_MESSAGE("the ole object links to a missing file, but we should retain its preview", pGraphic);

    xDocSh->DoClose();
}

void ScFiltersTest::testShapeRotationImport()
{
    if (!IsDefaultDPI())
        return;
    // tdf#83593 Incorrectly calculated bounding rectangles caused shapes to appear as if there
    // were extra or missing rotations. Hence, we check the sizes of these rectangles.
    ScDocShellRef xDocSh = loadDoc(u"testShapeRotationImport.", FORMAT_XLSX);
    CPPUNIT_ASSERT_MESSAGE("Failed to load testShapeRotationImport.xlsx", xDocSh.is());
    uno::Reference< drawing::XDrawPagesSupplier > xDoc(xDocSh->GetModel(), uno::UNO_QUERY_THROW);
    uno::Reference< drawing::XDrawPage > xPage(xDoc->getDrawPages()->getByIndex(0), uno::UNO_QUERY_THROW);

    // The expected values are in the map below. Note that some of the angles are outside of the set which contains
    // the value of the wrong angles. This is to check the border cases and one value on both sides.
    std::map<sal_Int32, std::map<std::string, sal_Int32>> aExpectedValues
    {
        {  4400, { { "x",  6832 }, { "y", 36893 }, { "width",  8898 }, { "height", 1163 } } },
        {  4500, { { "x",  4490 }, { "y", 36400 }, { "width",  8898 }, { "height", 1163 } } },
        {  4600, { { "x",  1673 }, { "y", 36270 }, { "width",  8862 }, { "height", 1144 } } },
        { 13400, { { "x", 13762 }, { "y", 28403 }, { "width",  8863 }, { "height", 1194 } } },
        { 13500, { { "x", 10817 }, { "y", 27951 }, { "width",  8863 }, { "height", 1170 } } },
        { 13600, { { "x",  8449 }, { "y", 28336 }, { "width",  8897 }, { "height", 1164 } } },
        { 22400, { { "x", 14948 }, { "y", 12978 }, { "width",  8898 }, { "height", 1164 } } },
        { 22500, { { "x", 11765 }, { "y", 12834 }, { "width",  8898 }, { "height", 1164 } } },
        { 22600, { { "x",  8253 }, { "y", 12919 }, { "width",  8863 }, { "height", 1171 } } },
        { 31400, { { "x",  8099 }, { "y",  1160 }, { "width",  9815 }, { "height", 1171 } } },
        { 31500, { { "x",  4427 }, { "y",  1274 }, { "width", 10238 }, { "height", 1171 } } },
        { 31600, { { "x",  1964 }, { "y",  1878 }, { "width", 10307 }, { "height", 1164 } } },
    };

    for (sal_Int32 ind = 0; ind < 12; ++ind)
    {
        uno::Reference< drawing::XShape > xShape(xPage->getByIndex(ind), uno::UNO_QUERY_THROW);

        uno::Reference< beans::XPropertySet > xShapeProperties(xShape, uno::UNO_QUERY);
        uno::Any nRotProp = xShapeProperties->getPropertyValue("RotateAngle");
        sal_Int32 nRot = nRotProp.get<sal_Int32>();

        awt::Point aPosition = xShape->getPosition();
        awt::Size aSize = xShape->getSize();

        CPPUNIT_ASSERT(aExpectedValues.find(nRot) != aExpectedValues.end());
        CPPUNIT_ASSERT_EQUAL(aExpectedValues[nRot]["x"],      aPosition.X);
        CPPUNIT_ASSERT_EQUAL(aExpectedValues[nRot]["y"],      aPosition.Y);
        CPPUNIT_ASSERT_EQUAL(aExpectedValues[nRot]["width"],  aSize.Width);
        CPPUNIT_ASSERT_EQUAL(aExpectedValues[nRot]["height"], aSize.Height);
    }
}

void ScFiltersTest::testShapeDisplacementOnRotationImport()
{
    // tdf#135918 shape is displaced on rotation if it is placed next to the sheets upper/left edges
    ScDocShellRef xDocSh = loadDoc(u"testShapeDisplacementOnRotationImport.", FORMAT_XLSX);
    CPPUNIT_ASSERT_MESSAGE("Failed to load testShapeDisplacementOnRotationImport.xlsx", xDocSh.is());

    uno::Reference<drawing::XDrawPagesSupplier> xDoc(xDocSh->GetModel(), uno::UNO_QUERY_THROW);
    uno::Reference<drawing::XDrawPage> xPage(xDoc->getDrawPages()->getByIndex(0), uno::UNO_QUERY_THROW);
    uno::Reference<drawing::XShape> xShape(xPage->getByIndex(0), uno::UNO_QUERY_THROW);

    uno::Reference<beans::XPropertySet> xShapeProperties(xShape, uno::UNO_QUERY_THROW);
    uno::Any aRectProp = xShapeProperties->getPropertyValue("FrameRect");
    awt::Rectangle aRectangle = aRectProp.get<awt::Rectangle>();
    CPPUNIT_ASSERT_EQUAL(sal_Int32(0), aRectangle.X);
    CPPUNIT_ASSERT_EQUAL(sal_Int32(1), aRectangle.Y);
}

void ScFiltersTest::testTextBoxBodyUpright()
{
    // tdf#106197 We should import the "upright" attribute of txBody.
    ScDocShellRef xDocSh = loadDoc(u"tdf106197_import_upright.", FORMAT_XLSX);
    CPPUNIT_ASSERT_MESSAGE("Failed to load testTextBoxBodyUpright.xlsx", xDocSh.is());

    uno::Reference<drawing::XDrawPagesSupplier> xDoc(xDocSh->GetModel(), uno::UNO_QUERY_THROW);
    uno::Reference<drawing::XDrawPage> xPage(xDoc->getDrawPages()->getByIndex(0), uno::UNO_QUERY_THROW);
    uno::Reference<drawing::XShape> xShape(xPage->getByIndex(0), uno::UNO_QUERY_THROW);
    uno::Reference<beans::XPropertySet> xShapeProperties(xShape, uno::UNO_QUERY_THROW);

    // Check that we imported "Upright".
    bool isUpright = false;
    if (xShapeProperties->getPropertySetInfo()->hasPropertyByName("InteropGrabBag"))
    {
        uno::Sequence<beans::PropertyValue> aGrabBag;
        xShapeProperties->getPropertyValue("InteropGrabBag") >>= aGrabBag;
        for (auto& aProp : aGrabBag)
        {
            if (aProp.Name == "Upright")
            {
                aProp.Value >>= isUpright;
                break;
            }
        }
    }
    CPPUNIT_ASSERT_EQUAL(true, isUpright);

    // Check the new textRotateAngle.
    sal_Int32 nAngle;
    uno::Any aGeom = xShapeProperties->getPropertyValue("CustomShapeGeometry");
    auto aGeomSeq = aGeom.get<Sequence<beans::PropertyValue>>();
    for (const auto& aProp : std::as_const(aGeomSeq))
    {
        if (aProp.Name == "TextPreRotateAngle")
        {
            aProp.Value >>= nAngle;
            break;
        }
    }
    CPPUNIT_ASSERT_EQUAL(sal_Int32(90), nAngle);
}

void ScFiltersTest::testTextLengthDataValidityXLSX()
{
    ScDocShellRef xDocSh = loadDoc(u"textLengthDataValidity.", FORMAT_XLSX);
    CPPUNIT_ASSERT_MESSAGE("Failed to load textLengthDataValidity.xlsx", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();

    const ScValidationData* pData = rDoc.GetValidationEntry(1);

    ScRefCellValue aCellA1; // A1 = 1234(numeric value)
    ScAddress aValBaseAddrA1( 0,0,0 );
    aCellA1.assign(rDoc, aValBaseAddrA1);
    bool bValidA1 = pData->IsDataValid(aCellA1, aValBaseAddrA1);

    ScRefCellValue aCellA2; // A2 = 1234(numeric value format as text)
    ScAddress aValBaseAddrA2( 0,1,0 );
    aCellA2.assign(rDoc, aValBaseAddrA2);
    bool bValidA2 = pData->IsDataValid(aCellA2, aValBaseAddrA2);

    ScRefCellValue aCellA3; // A3 = 1234.00(numeric value)
    ScAddress aValBaseAddrA3( 0,2,0 );
    aCellA3.assign(rDoc, aValBaseAddrA3);
    bool bValidA3 = pData->IsDataValid(aCellA3, aValBaseAddrA3);

    ScRefCellValue aCellA4; // A4 = 12.3(numeric value)
    ScAddress aValBaseAddrA4( 0,3,0 );
    aCellA4.assign(rDoc, aValBaseAddrA4);
    bool bValidA4 = pData->IsDataValid(aCellA4, aValBaseAddrA4);

    // True if text length = 4
    CPPUNIT_ASSERT_EQUAL(true, bValidA1);
    CPPUNIT_ASSERT_EQUAL(true, bValidA2);
    CPPUNIT_ASSERT_EQUAL(true, bValidA3);
    CPPUNIT_ASSERT_EQUAL(true, bValidA4);

    xDocSh->DoClose();
}

void ScFiltersTest::testDeleteCircles()
{
    ScDocShellRef xDocSh = loadDoc(u"testDeleteCircles.", FORMAT_XLSX);
    CPPUNIT_ASSERT_MESSAGE("Failed to load testDeleteCircles.xlsx", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();

    ScDrawLayer* pDrawLayer = rDoc.GetDrawLayer();
    SdrPage* pPage = pDrawLayer->GetPage(0);
    CPPUNIT_ASSERT_MESSAGE("draw page for sheet 1 should exist.", pPage);

    ScRefCellValue aCellA1; // A1 = "Hello"
    ScAddress aPosA1(0, 0, 0);
    aCellA1.assign(rDoc, aPosA1);

    // Mark invalid value
    bool bOverflow;
    bool bMarkInvalid = ScDetectiveFunc(rDoc, 0).MarkInvalid(bOverflow);
    CPPUNIT_ASSERT_EQUAL(true, bMarkInvalid);

    // There should be a circle object!
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pPage->GetObjCount());

    // The value of A1 change to Hello1.
    rDoc.SetString(0, 0, 0, "Hello1");

    // Check that the data is valid.(True if text length = 6)
    const ScValidationData* pData = rDoc.GetValidationEntry(1);
    bool bValidA1 = pData->IsDataValid(aCellA1, aPosA1);
    // if valid, delete circle.
    if (bValidA1)
        ScDetectiveFunc(rDoc, 0).DeleteCirclesAt(aPosA1.Col(), aPosA1.Row());

    // There should not be a circle object!
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), pPage->GetObjCount());

    xDocSh->DoClose();
}

void ScFiltersTest::testDrawCircleInMergeCells()
{
    ScDocShellRef xDocSh = loadDoc(u"testDrawCircleInMergeCells.", FORMAT_XLSX);
    CPPUNIT_ASSERT_MESSAGE("Failed to load testDrawCircleInMergeCells.ods", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();

    ScDrawLayer* pDrawLayer = rDoc.GetDrawLayer();
    SdrPage* pPage = pDrawLayer->GetPage(0);
    CPPUNIT_ASSERT_MESSAGE("draw page for sheet 1 should exist.", pPage);

    // A1:B2 is merged.
    ScRange aMergedRange(0,0,0);
    rDoc.ExtendTotalMerge(aMergedRange);
    CPPUNIT_ASSERT_EQUAL(ScRange(0,0,0,1,1,0), aMergedRange);

    // Mark invalid value
    bool bOverflow;
    bool bMarkInvalid = ScDetectiveFunc(rDoc, 0).MarkInvalid(bOverflow);
    CPPUNIT_ASSERT_EQUAL(true, bMarkInvalid);

    // There should be a circle object!
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), pPage->GetObjCount());

    SdrObject* pObj = pPage->GetObj(0);
    tools::Rectangle aRect(pObj->GetLogicRect());
    Point aStartCircle = aRect.TopLeft();
    Point aEndCircle = aRect.BottomRight();

    tools::Rectangle aCellRect = rDoc.GetMMRect(0,0,1,1,0);
    aCellRect.AdjustLeft( -250 );
    aCellRect.AdjustRight(250 );
    aCellRect.AdjustTop( -70 );
    aCellRect.AdjustBottom(70 );
    Point aStartCell = aCellRect.TopLeft();
    Point aEndCell = aCellRect.BottomRight();

    CPPUNIT_ASSERT_EQUAL(aStartCell.X(), aStartCircle.X());
    CPPUNIT_ASSERT_EQUAL(aEndCell.X(), aEndCircle.X());
    CPPUNIT_ASSERT_EQUAL(aStartCell.Y(), aStartCircle.Y());
    CPPUNIT_ASSERT_EQUAL(aEndCell.Y(), aEndCircle.Y());

    // Change the height of the first row. (556 ~ 1cm)
    rDoc.SetRowHeight(0, 0, 556);
    ScDrawObjData* pData = ScDrawLayer::GetObjData(pObj);
    pDrawLayer->RecalcPos(pObj,*pData,false,false);

    tools::Rectangle aRecalcRect(pObj->GetLogicRect());
    Point aStartRecalcCircle = aRecalcRect.TopLeft();
    Point aEndRecalcCircle = aRecalcRect.BottomRight();

    tools::Rectangle aRecalcCellRect = rDoc.GetMMRect(0,0,1,1,0);
    aRecalcCellRect.AdjustLeft( -250 );
    aRecalcCellRect.AdjustRight(250 );
    aRecalcCellRect.AdjustTop( -70 );
    aRecalcCellRect.AdjustBottom(70 );
    Point aStartRecalcCell = aRecalcCellRect.TopLeft();
    Point aEndRecalcCell1 = aRecalcCellRect.BottomRight();

    CPPUNIT_ASSERT_EQUAL(aStartRecalcCell.X(), aStartRecalcCircle.X());
    CPPUNIT_ASSERT_EQUAL(aEndRecalcCell1.X(), aEndRecalcCircle.X());
    CPPUNIT_ASSERT_EQUAL(aStartRecalcCell.Y(), aStartRecalcCircle.Y());
    CPPUNIT_ASSERT_EQUAL(aEndRecalcCell1.Y(), aEndRecalcCircle.Y());

    xDocSh->DoClose();
}

void ScFiltersTest::testDeleteCirclesInRowAndCol()
{
    ScDocShellRef xDocSh = loadDoc(u"deleteCirclesInRowAndCol.", FORMAT_ODS);
    CPPUNIT_ASSERT_MESSAGE("Failed to load deleteCirclesInRowAndCol.ods", xDocSh.is());

    ScDocument& rDoc = xDocSh->GetDocument();

    ScDrawLayer* pDrawLayer = rDoc.GetDrawLayer();
    SdrPage* pPage = pDrawLayer->GetPage(0);
    CPPUNIT_ASSERT_MESSAGE("draw page for sheet 1 should exist.", pPage);

    // There should be 6 circle objects!
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), pPage->GetObjCount());

    // Delete first row (1023 = MAXCOLS)
    pDrawLayer->DeleteObjectsInArea(0,0,0,1023,0,true);

    // There should be 3 circle objects!
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), pPage->GetObjCount());

    // Delete first col (1048575 = MAXROWS)
    pDrawLayer->DeleteObjectsInArea(0,0,0,0,1048575,true);

    // There should not be a circle object!
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), pPage->GetObjCount());

    xDocSh->DoClose();
}

ScFiltersTest::ScFiltersTest()
      : ScBootstrapFixture( "sc/qa/unit/data" )
{
}

void ScFiltersTest::setUp()
{
    test::BootstrapFixture::setUp();

    // This is a bit of a fudge, we do this to ensure that ScGlobals::ensure,
    // which is a private symbol to us, gets called
    m_xCalcComponent =
        getMultiServiceFactory()->createInstance("com.sun.star.comp.Calc.SpreadsheetDocument");
    CPPUNIT_ASSERT_MESSAGE("no calc component!", m_xCalcComponent.is());
}

void ScFiltersTest::tearDown()
{
    uno::Reference< lang::XComponent >( m_xCalcComponent, UNO_QUERY_THROW )->dispose();
    test::BootstrapFixture::tearDown();
}

CPPUNIT_TEST_SUITE_REGISTRATION(ScFiltersTest);

CPPUNIT_PLUGIN_IMPLEMENT();

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */

