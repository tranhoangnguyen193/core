/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */
#include <sfx2/sidebar/SidebarDockingWindow.hxx>
#include <sfx2/sidebar/SidebarChildWindow.hxx>
#include <sfx2/sidebar/SidebarController.hxx>
#include <sidebar/PanelDescriptor.hxx>

#include <sfx2/strings.hrc>
#include <bitmaps.hlst>
#include <sfx2/sfxresid.hxx>
#include <svl/eitem.hxx>
#include <vcl/settings.hxx>
#include <vcl/splitwin.hxx>

#include <comphelper/dispatchcommand.hxx>
#include <comphelper/processfactory.hxx>
#include <sfx2/bindings.hxx>
#include <sfx2/dispatch.hxx>
#include <sfx2/viewfrm.hxx>
#include <sfx2/viewsh.hxx>
#include <sfx2/titledockwin.hxx>
#include <svtools/acceleratorexecute.hxx>
#include <tools/gen.hxx>
#include <tools/json_writer.hxx>
#include <vcl/event.hxx>
#include <comphelper/lok.hxx>
#include <LibreOfficeKit/LibreOfficeKitEnums.h>
#include <openuriexternally.hxx>
#include <boost/property_tree/json_parser.hpp>

using namespace css;
using namespace css::uno;

namespace sfx2::sidebar {

class SidebarNotifyIdle : public Idle
{
    SidebarDockingWindow& m_rSidebarDockingWin;
    std::string m_LastNotificationMessage;
    vcl::LOKWindowId m_LastLOKWindowId;

public:
    SidebarNotifyIdle(SidebarDockingWindow &rSidebarDockingWin) :
        Idle("Sidebar notify"),
        m_rSidebarDockingWin(rSidebarDockingWin),
        m_LastNotificationMessage(),
        m_LastLOKWindowId(0)
    {
        SetPriority(TaskPriority::POST_PAINT);
    }

    void Invoke() override
    {
        auto pNotifier = m_rSidebarDockingWin.GetLOKNotifier();
        auto pMobileNotifier = SfxViewShell::Current();
        if (!pNotifier || (!pMobileNotifier && !comphelper::LibreOfficeKit::isActive()))
            return;

        try
        {
            if (pMobileNotifier && pMobileNotifier->isLOKMobilePhone())
            {
                // Mobile phone.
                tools::JsonWriter aJsonWriter;
                m_rSidebarDockingWin.DumpAsPropertyTree(aJsonWriter);
                aJsonWriter.put("id", m_rSidebarDockingWin.GetLOKWindowId());
                std::unique_ptr<char[]> data( aJsonWriter.extractData());
                std::string_view message(data.get());
                if (message != m_LastNotificationMessage)
                {
                    m_LastNotificationMessage = message;
                    pMobileNotifier->libreOfficeKitViewCallback(LOK_CALLBACK_JSDIALOG, m_LastNotificationMessage.c_str());
                }
            }

            // Notify the sidebar is created, and its LOKWindowId, which
            // is needed on mobile phones, tablets, and desktop.
            const Point pos(m_rSidebarDockingWin.GetOutOffXPixel(),
                            m_rSidebarDockingWin.GetOutOffYPixel());
            const OString posMessage = pos.toString();
            const OString sizeMessage = m_rSidebarDockingWin.GetSizePixel().toString();

            const std::string message = OString(posMessage + sizeMessage).getStr();
            const vcl::LOKWindowId lokWindowId = m_rSidebarDockingWin.GetLOKWindowId();

            if (lokWindowId != m_LastLOKWindowId || message != m_LastNotificationMessage)
            {
                m_LastLOKWindowId = lokWindowId;
                m_LastNotificationMessage = message;

                std::vector<vcl::LOKPayloadItem> aItems;
                aItems.emplace_back("type", "deck");
                aItems.emplace_back("position", posMessage);
                aItems.emplace_back("size", sizeMessage);
                pNotifier->notifyWindow(lokWindowId, "created", aItems);
            }
        }
        catch (boost::property_tree::json_parser::json_parser_error& rError)
        {
            SAL_WARN("sfx.sidebar", rError.message());
        }
    }
};

SidebarDockingWindow::SidebarDockingWindow(SfxBindings* pSfxBindings, SidebarChildWindow& rChildWindow,
                                           vcl::Window* pParentWindow, WinBits nBits)
    : SfxDockingWindow(pSfxBindings, &rChildWindow, pParentWindow, nBits)
    , mpSidebarController()
    , mbIsReadyToDrag(false)
    , mpIdleNotify(new SidebarNotifyIdle(*this))
{
    // Get the XFrame from the bindings.
    if (pSfxBindings==nullptr || pSfxBindings->GetDispatcher()==nullptr)
    {
        OSL_ASSERT(pSfxBindings!=nullptr);
        OSL_ASSERT(pSfxBindings->GetDispatcher()!=nullptr);
    }
    else
    {
        const SfxViewFrame* pViewFrame = pSfxBindings->GetDispatcher()->GetFrame();
        mpSidebarController.set(sfx2::sidebar::SidebarController::create(this, pViewFrame).get());
    }
}

SidebarDockingWindow::~SidebarDockingWindow()
{
    disposeOnce();
}

void SidebarDockingWindow::dispose()
{
    if (comphelper::LibreOfficeKit::isActive())
        LOKClose();

    Reference<lang::XComponent> xComponent (static_cast<XWeak*>(mpSidebarController.get()), UNO_QUERY);
    mpSidebarController.clear();
    if (xComponent.is())
        xComponent->dispose();

    SfxDockingWindow::dispose();
}

void SidebarDockingWindow::LOKClose()
{
    assert(comphelper::LibreOfficeKit::isActive());
    if (const vcl::ILibreOfficeKitNotifier* pNotifier = GetLOKNotifier())
    {
        mpIdleNotify->Stop();

        pNotifier->notifyWindow(GetLOKWindowId(), "close");
        ReleaseLOKNotifier();
    }
}

void SidebarDockingWindow::GetFocus()
{
    if (mpSidebarController.is())
    {
        mpSidebarController->RequestOpenDeck();
        mpSidebarController->GetFocusManager().GrabFocus();
    }
    else
        SfxDockingWindow::GetFocus();
}

bool SidebarDockingWindow::Close()
{
    if (mpSidebarController.is())
        mpSidebarController->SetFloatingDeckClosed(true);

    return SfxDockingWindow::Close();
}

void SidebarDockingWindow::Resize()
{
    SfxDockingWindow::Resize();

    NotifyResize();
}

void SidebarDockingWindow::SyncUpdate()
{
    if (mpSidebarController.is())
        mpSidebarController->SyncUpdate();
}

void SidebarDockingWindow::NotifyResize()
{
    if (!(comphelper::LibreOfficeKit::isActive() && mpSidebarController.is() && SfxViewShell::Current()))
        return;

    const vcl::ILibreOfficeKitNotifier* pCurrentView = SfxViewShell::Current();
    if (GetLOKNotifier() != pCurrentView)
    {
        // ViewShell not yet set, or has changed. Reset it.
        // Note GetLOKWindowId will return a new value after resetting, so we must notify clients.
        LOKClose();

        SetLOKNotifier(pCurrentView);
    }

    mpIdleNotify->Start();
}

SfxChildAlignment SidebarDockingWindow::CheckAlignment (
    SfxChildAlignment eCurrentAlignment,
    SfxChildAlignment eRequestedAlignment)
{
    switch (eRequestedAlignment)
    {
        case SfxChildAlignment::TOP:
        case SfxChildAlignment::HIGHESTTOP:
        case SfxChildAlignment::LOWESTTOP:
        case SfxChildAlignment::BOTTOM:
        case SfxChildAlignment::LOWESTBOTTOM:
        case SfxChildAlignment::HIGHESTBOTTOM:
            return eCurrentAlignment;

        case SfxChildAlignment::LEFT:
        case SfxChildAlignment::RIGHT:
        case SfxChildAlignment::FIRSTLEFT:
        case SfxChildAlignment::LASTLEFT:
        case SfxChildAlignment::FIRSTRIGHT:
        case SfxChildAlignment::LASTRIGHT:
            return eRequestedAlignment;

        default:
            return eRequestedAlignment;
    }
}

bool SidebarDockingWindow::EventNotify(NotifyEvent& rEvent)
{
    MouseNotifyEvent nType = rEvent.GetType();
    if (MouseNotifyEvent::KEYINPUT == nType)
    {
        const vcl::KeyCode& rKeyCode = rEvent.GetKeyEvent()->GetKeyCode();
        switch (rKeyCode.GetCode())
        {
            case KEY_UP:
            case KEY_DOWN:
            case KEY_PAGEUP:
            case KEY_PAGEDOWN:
            case KEY_HOME:
            case KEY_END:
            case KEY_LEFT:
            case KEY_RIGHT:
            case KEY_BACKSPACE:
            case KEY_DELETE:
            case KEY_INSERT:
            case KEY_RETURN:
            case KEY_ESCAPE:
            {
                return true;
            }
            default:
            break;
        }
        if (!mpAccel)
        {
            mpAccel = svt::AcceleratorExecute::createAcceleratorHelper();
            mpAccel->init(comphelper::getProcessComponentContext(), mpSidebarController->getXFrame());
        }
        const OUString aCommand(mpAccel->findCommand(svt::AcceleratorExecute::st_VCLKey2AWTKey(rKeyCode)));
        if (".uno:DesignerDialog" == aCommand)
        {
            std::shared_ptr<PanelDescriptor> xPanelDescriptor =
                    mpSidebarController->GetResourceManager()->GetPanelDescriptor( u"StyleListPanel" );
            if ( xPanelDescriptor && mpSidebarController->IsDeckVisible( xPanelDescriptor->msDeckId ) )
                Close();
            return true;
        }
        if (".uno:Undo" == aCommand || ".uno:Redo" == aCommand)
        {
            comphelper::dispatchCommand(aCommand, {});
            return true;
        }
    }
    else if (MouseNotifyEvent::MOUSEBUTTONDOWN == nType)
    {
        const MouseEvent *mEvt = rEvent.GetMouseEvent();
        if (mEvt->IsLeft())
        {
            tools::Rectangle aGrip = mpSidebarController->GetDeckDragArea();
            if ( aGrip.IsInside( mEvt->GetPosPixel() ) )
                mbIsReadyToDrag = true;
        }
    }
    else if (MouseNotifyEvent::MOUSEMOVE == nType)
    {
        const MouseEvent *mEvt = rEvent.GetMouseEvent();
        tools::Rectangle aGrip = mpSidebarController->GetDeckDragArea();
        if (mEvt->IsLeft() && aGrip.IsInside( mEvt->GetPosPixel() ) && mbIsReadyToDrag )
        {
            Point aPos = mEvt->GetPosPixel();
            vcl::Window* pWindow = rEvent.GetWindow();
            if ( pWindow != this )
            {
                aPos = pWindow->OutputToScreenPixel( aPos );
                aPos = ScreenToOutputPixel( aPos );
            }
            ImplStartDocking( aPos );
        }
    }

    return SfxDockingWindow::EventNotify(rEvent);
}

    LeftSidebarDockingWindow::LeftSidebarDockingWindow( SfxBindings* i_pBindings, SfxChildWindow* i_pChildWindow, vcl::Window* i_pParent, const OUString& rsTitle )
        :SfxDockingWindow( i_pBindings, i_pChildWindow, i_pParent, WB_MOVEABLE|WB_CLOSEABLE|WB_DOCKABLE|WB_HIDE|WB_3DLOOK )
        ,m_sTitle()
        ,m_aToolbox( VclPtr<ToolBox>::Create(this) )
        ,m_aContentWindow( VclPtr<vcl::Window>::Create(this, WB_DIALOGCONTROL) )
        ,m_aButton(VclPtr<PushButton>::Create(this))
        ,m_aBorder( 3, 1, 3, 3 )
        ,m_bLayoutPending( false )
        ,m_nTitleBarHeight(0)
        ,m_aImage(StockImage::Yes, SFX_BMP_PETER_BANNER)
        ,m_aBitmap(SFX_BMP_PETER_BANNER)
        ,m_aImgButton(VclPtr<FixedImage>::Create(m_aContentWindow))
    {

        SetBackground( Wallpaper());
        SetSizePixel(LogicToPixel(Size(150,200), MapMode(MapUnit::MapAppFont)));
        m_aToolbox->SetSelectHdl( LINK( this, LeftSidebarDockingWindow, OnToolboxItemSelected ) );
        m_aToolbox->Show();
        impl_resetToolBox();
        //m_aContentWindow->Show();
        //m_aButton->SetText("trial");
        //m_aImgButton->SetImage(m_aImage);
        //m_aButton->CalcMinimumSize();
        //m_aImgButton->Show();
        //m_aImgButton->SetImage(Image(StockImage::Yes), SFX_BMP_PETER_BANNER)
    }

    LeftSidebarDockingWindow::~LeftSidebarDockingWindow()
    {
        disposeOnce();
    }

    void LeftSidebarDockingWindow::dispose()
    {
        m_aToolbox.disposeAndClear();
        m_aContentWindow.disposeAndClear();
        SfxDockingWindow::dispose();
    }

    void LeftSidebarDockingWindow::SetTitle( const OUString& i_rTitle )
    {
        m_sTitle = i_rTitle;
        Invalidate();
    }


    void LeftSidebarDockingWindow::SetText( const OUString& i_rText )
    {
        SfxDockingWindow::SetText( i_rText );
        if ( m_sTitle.isEmpty() )
            // our text is used as title, too => repaint
            Invalidate();
    }


    void LeftSidebarDockingWindow::Resize()
    {
        SfxDockingWindow::Resize();
        impl_scheduleLayout();
    }


    void LeftSidebarDockingWindow::impl_scheduleLayout()
    {
        m_bLayoutPending = true;
    }


    void LeftSidebarDockingWindow::impl_layout()
    {
        m_bLayoutPending = false;

        m_aToolbox->ShowItem( 1, !IsFloatingMode() );

        const Size aToolBoxSize( m_aToolbox->CalcWindowSizePixel() );
        Size aWindowSize( GetOutputSizePixel() );

        // position the tool box
        m_nTitleBarHeight = GetSettings().GetStyleSettings().GetTitleHeight();
        if ( aToolBoxSize.Height() > m_nTitleBarHeight )
            m_nTitleBarHeight = aToolBoxSize.Height();
        m_aToolbox->SetPosSizePixel(
            Point(
                aWindowSize.Width() - aToolBoxSize.Width(),
                ( m_nTitleBarHeight - aToolBoxSize.Height() ) / 2
            ),
            aToolBoxSize
        );

        // Place the content window.
        if ( m_nTitleBarHeight < aToolBoxSize.Height() )
            m_nTitleBarHeight = aToolBoxSize.Height();
        aWindowSize.AdjustHeight( -m_nTitleBarHeight );
        m_aContentWindow->SetPosSizePixel(
            Point( m_aBorder.Left(), m_nTitleBarHeight + m_aBorder.Top() ),
            Size(
                aWindowSize.Width() - m_aBorder.Left() - m_aBorder.Right(),
                aWindowSize.Height() - m_aBorder.Top() - m_aBorder.Bottom()
            )
        );

        m_aImgButton->SetPosSizePixel(
            Point( m_aBorder.Left(), m_nTitleBarHeight + m_aBorder.Top() ),
            m_aImage.GetSizePixel()
        );

        SplitWindow* pSplitWindow = dynamic_cast<SplitWindow*>(GetParent());
        const sal_uInt16 nId (pSplitWindow->GetItemId(static_cast< vcl::Window*>(this)));
        const sal_uInt16 nSetId (pSplitWindow->GetSet(nId));
        Size aImageSize = m_aImage.GetSizePixel();
        pSplitWindow->SetItemSizeRange(
            nSetId,
            Range(aImageSize.Width() - 8, aImageSize.Width()));
        pSplitWindow->HideTracking();
        // Place the banner
        aImageSize = m_aImage.GetSizePixel();
        if ( aImageSize.Width() )
        {
            aImageSize.AdjustWidth(4 );
            aImageSize.AdjustHeight( -m_nTitleBarHeight );

            // SplitWindow* pSplitWindow = dynamic_cast<SplitWindow*>(GetParent());
            // if (pSplitWindow == nullptr)
            //     return;

            // const sal_uInt16 nId (pSplitWindow->GetItemId(static_cast< vcl::Window*>(this)));
            // const sal_uInt16 nSetId (pSplitWindow->GetSet(nId));
            // const Range rValidSizeRange (mpLayouter->GetValidVerticalSizeRange());

            // const SvBorder aBorder (GetDecorationBorder());
            // sal_Int32 nCompensation (pSplitWindow->IsHorizontal()
            //     ? aBorder.Top() + aBorder.Bottom()
            //     : aBorder.Left() + aBorder.Right());
            // pSplitWindow->SetItemSizeRange(
            //     nSetId,
            //     Range(
            //         rValidSizeRange.Min() + nCompensation,
            //         rValidSizeRange.Max() + nCompensation));
        }
    }

    void LeftSidebarDockingWindow::ApplySettings(vcl::RenderContext& rRenderContext)
    {
        const StyleSettings& rStyleSettings = rRenderContext.GetSettings().GetStyleSettings();

        // Font
        ApplyControlFont(rRenderContext, rStyleSettings.GetAppFont());

        // Color
        ApplyControlForeground(rRenderContext, rStyleSettings.GetButtonTextColor());
        rRenderContext.SetTextFillColor();
    }

    void LeftSidebarDockingWindow::Paint(vcl::RenderContext& rRenderContext, const tools::Rectangle& i_rArea)
    {
        const StyleSettings& rStyleSettings = rRenderContext.GetSettings().GetStyleSettings();

        if (m_bLayoutPending)
            impl_layout();

        SfxDockingWindow::Paint(rRenderContext, i_rArea);

        rRenderContext.Push(PushFlags::FONT | PushFlags::FILLCOLOR | PushFlags::LINECOLOR);

        rRenderContext.SetFillColor(rStyleSettings.GetDialogColor());
        rRenderContext.SetLineColor();

        // bold font
        vcl::Font aFont(rRenderContext.GetFont());
        aFont.SetWeight(WEIGHT_BOLD);
        rRenderContext.SetFont(aFont);

        // Set border values.
        Size aWindowSize(GetOutputSizePixel());
        int nOuterLeft = 0;
        int nInnerLeft = nOuterLeft + m_aBorder.Left() - 1;
        int nOuterRight = aWindowSize.Width() - 1;
        int nInnerRight = nOuterRight - m_aBorder.Right() + 1;
        int nInnerTop = m_nTitleBarHeight + m_aBorder.Top() - 1;
        int nOuterBottom = aWindowSize.Height() - 1;
        int nInnerBottom = nOuterBottom - m_aBorder.Bottom() + 1;

        // Paint title bar background.
        tools::Rectangle aTitleBarBox(tools::Rectangle(nOuterLeft, 0, nOuterRight, nInnerTop - 1));
        rRenderContext.DrawRect(aTitleBarBox);

        if (nInnerLeft > nOuterLeft)
            rRenderContext.DrawRect(tools::Rectangle(nOuterLeft, nInnerTop, nInnerLeft, nInnerBottom));
        if (nOuterRight > nInnerRight)
            rRenderContext.DrawRect(tools::Rectangle(nInnerRight, nInnerTop, nOuterRight, nInnerBottom));
        if (nInnerBottom < nOuterBottom)
            rRenderContext.DrawRect(tools::Rectangle(nOuterLeft, nInnerBottom, nOuterRight, nOuterBottom));

        //Paint bevel border.

        rRenderContext.SetFillColor();
        rRenderContext.SetLineColor(rStyleSettings.GetShadowColor());
        if (m_aBorder.Top() > 0)
            rRenderContext.DrawLine(Point(nInnerLeft, nInnerTop), Point(nInnerLeft, nInnerBottom));
        if (m_aBorder.Left() > 0)
            rRenderContext.DrawLine(Point(nInnerLeft, nInnerTop), Point(nInnerRight, nInnerTop));
        rRenderContext.SetLineColor(rStyleSettings.GetLightColor());
        if (m_aBorder.Bottom() > 0)
            rRenderContext.DrawLine(Point(nInnerRight, nInnerBottom), Point(nInnerLeft, nInnerBottom));
        if (m_aBorder.Right() > 0)
            rRenderContext.DrawLine(Point(nInnerRight, nInnerBottom), Point(nInnerRight, nInnerTop));
        //rRenderContext.DrawImage(Point(nInnerLeft, nInnerTop), m_aImage,DrawImageFlags::NONE);
        
        // Paint title bar text.
        rRenderContext.SetLineColor(rStyleSettings.GetActiveTextColor());
        aTitleBarBox.AdjustLeft(3 );
        rRenderContext.DrawText(aTitleBarBox,
                                !m_sTitle.isEmpty() ? m_sTitle : GetText(),
                                DrawTextFlags::Left | DrawTextFlags::VCenter | DrawTextFlags::MultiLine | DrawTextFlags::WordBreak);

        // Restore original values of the output device.
        rRenderContext.Pop();
    }


    void LeftSidebarDockingWindow::impl_resetToolBox()
    {
        m_aToolbox->Clear();
        // Get the closer bitmap and set it as right most button.
        m_aToolbox->InsertItem(1, Image(StockImage::Yes, SFX_BMP_PETER_BANNER));
        m_aToolbox->SetQuickHelpText(1, SfxResId(STR_BANNER_PANE));
        m_aToolbox->ShowItem( 1 );
    }


    IMPL_LINK( LeftSidebarDockingWindow, OnToolboxItemSelected, ToolBox*, pToolBox, void )
    {
        const sal_uInt16 nId = pToolBox->GetCurItemId();

        if ( nId == 1 )
        {
            // the closer
             sfx2::openUriExternally("https://free-office.pdfwork.com/tutorial", false);
        }
    }


    void LeftSidebarDockingWindow::StateChanged( StateChangedType i_nType )
    {
        switch ( i_nType )
        {
            case StateChangedType::InitShow:
                impl_scheduleLayout();
                break;
            default:;
        }
        SfxDockingWindow::StateChanged( i_nType );
    }

    void LeftSidebarDockingWindow::DataChanged( const DataChangedEvent& i_rDataChangedEvent )
    {
        SfxDockingWindow::DataChanged( i_rDataChangedEvent );

        switch ( i_rDataChangedEvent.GetType() )
        {
            case DataChangedEventType::SETTINGS:
                if ( !( i_rDataChangedEvent.GetFlags() & AllSettingsFlags::STYLE ) )
                    break;
                [[fallthrough]];
            case DataChangedEventType::FONTS:
            case DataChangedEventType::FONTSUBSTITUTION:
            {
                impl_scheduleLayout();
                Invalidate();
            }
            break;
            default: break;
        }
    }

} // end of namespace sfx2::sidebar

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
