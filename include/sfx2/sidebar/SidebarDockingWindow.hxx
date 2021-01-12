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
#pragma once

#include <sfx2/dockwin.hxx>
#include <sfx2/dllapi.h>

#include <vcl/toolbox.hxx>
#include <vcl/vclptr.hxx>
#include <tools/svborder.hxx>
#include <vcl/button.hxx>
#include <vcl/fixed.hxx>

#include <rtl/ref.hxx>

class SfxViewShell;

namespace svt { class AcceleratorExecute; }

namespace sfx2::sidebar {

class SidebarChildWindow;

class LeftSidebarChildWindow;

class SidebarController;

class SidebarNotifyIdle;

class SFX2_DLLPUBLIC SidebarDockingWindow final : public SfxDockingWindow
{
public:
    SidebarDockingWindow(SfxBindings* pBindings, SidebarChildWindow& rChildWindow,
                         vcl::Window* pParent, WinBits nBits);
    virtual ~SidebarDockingWindow() override;
    virtual void dispose() override;
    virtual bool EventNotify(NotifyEvent& rEvent) override;
    virtual bool Close() override;

    /// Force generation of all panels by completion.
    void SyncUpdate();

    void NotifyResize();
    auto& GetSidebarController() const { return mpSidebarController; }
    using SfxDockingWindow::Close;

private:
    // Window overridables
    virtual void GetFocus() override;
    virtual void Resize() override;

    virtual SfxChildAlignment CheckAlignment (
        SfxChildAlignment eCurrentAlignment,
        SfxChildAlignment eRequestedAlignment) override;

    /// Notify LOKit that we closed and release the LOKNotifier.
    void LOKClose();

    ::rtl::Reference<sfx2::sidebar::SidebarController> mpSidebarController;
    bool mbIsReadyToDrag;
    std::unique_ptr<svt::AcceleratorExecute> mpAccel;

    std::unique_ptr<SidebarNotifyIdle> mpIdleNotify;
};

class SFX2_DLLPUBLIC LeftSidebarDockingWindow final : public SfxDockingWindow
{
    public:
        LeftSidebarDockingWindow(
            SfxBindings* i_pBindings, SfxChildWindow* i_pChildWindow,
            vcl::Window* i_pParent, const OUString& rsTitle);

        virtual ~LeftSidebarDockingWindow() override;
        virtual void dispose() override;

        /** sets a title to be displayed in the docking window
        */
        void        SetTitle( const OUString& i_rTitle );

        /** returns the content window, which is to be used as parent window for any content to be displayed
            in the docking window.
        */
        vcl::Window&       GetContentWindow()          { return *m_aContentWindow; }
        const vcl::Window& GetContentWindow() const    { return *m_aContentWindow; }

        /** Return the border that is painted around the inner window as
            decoration.
        */
        const SvBorder&  GetDecorationBorder() const  { return m_aBorder; }

    protected:
        // Window overridables
        virtual void Paint(vcl::RenderContext& rRenderContext, const tools::Rectangle& i_rArea) override;
        virtual void Resize() override;
        virtual void StateChanged( StateChangedType i_nType ) override;
        virtual void DataChanged( const DataChangedEvent& i_rDataChangedEvent ) override;
        virtual void SetText( const OUString& i_rText ) override;

        virtual void ApplySettings(vcl::RenderContext& rRenderContext) override;
    protected:
        /** internal version of ResetToolBox
        */
        void    impl_resetToolBox();

    private:
        DECL_LINK( OnToolboxItemSelected, ToolBox*, void );

        void    impl_layout();
        void    impl_scheduleLayout();

    private:
        OUString            m_sTitle;
        VclPtr<ToolBox>     m_aToolbox;
        VclPtr<Window>      m_aContentWindow;
        VclPtr<PushButton>      m_aButton;
        VclPtr<FixedImage> m_aImgButton;
        Image m_aImage;
        BitmapEx m_aBitmap;

        /** The border that is painted around the inner window.  The bevel
            shadow lines are part of the border, so where the border is 0 no
            such line is painted.
        */
        SvBorder            m_aBorder;

        /** Remember that a layout is pending, i.e. Resize() has been called
            since the last Paint().
        */
        bool                m_bLayoutPending;

        /** Height of the title bar.  Calculated in impl_layout().
        */
        int                 m_nTitleBarHeight;

    };
} // end of namespace sfx2::sidebar

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
