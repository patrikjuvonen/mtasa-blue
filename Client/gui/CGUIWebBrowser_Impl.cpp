/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUIWebBrowser_Impl.cpp
 *  PURPOSE:     WebBrowser widget class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#include "StdInc.h"
#include <core/CWebViewInterface.h>

CGUIWebBrowser_Impl::CGUIWebBrowser_Impl(CGUI_Impl* pGUI, CGUIElement* pParent)
{
    // Initialize
    m_pImageManager = pGUI->GetImageManager();
    m_pImage = nullptr;
    m_pGUI = pGUI;
    m_pManager = pGUI;
    m_pWebView = nullptr;

    // Get an unique identifier for CEGUI
    char szUnique[CGUI_CHAR_SIZE];
    pGUI->GetUniqueName(szUnique);

    // Create the control and set default properties
    m_pWindow = pGUI->GetWindowManager()->createWindow(CGUIWEBBROWSER_NAME, szUnique);
    m_pWindow->setDestroyedByParent(false);
    m_pWindow->setArea(CEGUI::UDim::zero(), CEGUI::UDim::zero(), cegui_reldim(1.0f), cegui_reldim(1.0f));
    m_pWindow->setProperty("BackgroundEnabled", "false");

    // Store the pointer to this CGUI element in the CEGUI element
    m_pWindow->setUserData(reinterpret_cast<void*>(this));

    AddEvents();

    // Apply browser events
    m_pWindow->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::Event::Subscriber(&CGUIWebBrowser_Impl::Event_MouseButtonDown, this));
    m_pWindow->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::Event::Subscriber(&CGUIWebBrowser_Impl::Event_MouseButtonUp, this));
    m_pWindow->subscribeEvent(CEGUI::Window::EventMouseMove, CEGUI::Event::Subscriber(&CGUIWebBrowser_Impl::Event_MouseMove, this));
    m_pWindow->subscribeEvent(CEGUI::Window::EventMouseWheel, CEGUI::Event::Subscriber(&CGUIWebBrowser_Impl::Event_MouseWheel, this));
    m_pWindow->subscribeEvent(CEGUI::Window::EventActivated, CEGUI::Event::Subscriber(&CGUIWebBrowser_Impl::Event_Activated, this));
    m_pWindow->subscribeEvent(CEGUI::Window::EventDeactivated, CEGUI::Event::Subscriber(&CGUIWebBrowser_Impl::Event_Deactivated, this));

    // If a parent is specified, add it to it's children list, if not, add it as a child to the pManager
    if (pParent)
        SetParent(pParent);
    else
    {
        pGUI->AddChild(this);
        SetParent(nullptr);
    }
}

CGUIWebBrowser_Impl::~CGUIWebBrowser_Impl()
{
    Clear();

    DestroyElement();
}

void CGUIWebBrowser_Impl::Clear()
{
    // Stop the control from using it
    reinterpret_cast<CEGUI::BasicImage*>(m_pWindow)->setTexture(nullptr);

    // Kill the images
    if (m_pImage)
    {
        delete m_pImage;
        m_pImage = nullptr;
    }
}

void CGUIWebBrowser_Impl::LoadFromWebView(CWebViewInterface* pWebView)
{
    m_pWebView = pWebView;

    CGUIWebBrowserTexture* pCEGUITexture = new CGUIWebBrowserTexture(m_pGUI->GetRenderer(), m_pWebView);

    // Get an unique identifier for CEGUI for the image
    char szUnique[CGUI_CHAR_SIZE];
    m_pGUI->GetUniqueName(szUnique);

    // Set the image just loaded as the image to be drawn for the widget
    CEGUI::Sizef size = pCEGUITexture->getSize();
    reinterpret_cast<CEGUI::BasicImage*>(m_pWindow)->setArea(CEGUI::Rectf(0, 0, size.d_width, size.d_height));
    reinterpret_cast<CEGUI::BasicImage*>(m_pWindow)->setTexture(pCEGUITexture);
}

void CGUIWebBrowser_Impl::SetFrameEnabled(bool bFrameEnabled)
{
    m_pWindow->setProperty("FrameEnabled", bFrameEnabled ? "true" : "false");
}

bool CGUIWebBrowser_Impl::IsFrameEnabled()
{
    return m_pWindow->getProperty("FrameEnabled") == "true";
}

CEGUI::Image* CGUIWebBrowser_Impl::GetDirectImage()
{
    // return const_cast<CEGUI::Image*>(reinterpret_cast<CEGUI::StaticImage*>(m_pWindow)->getImage());
    return nullptr;
}

void CGUIWebBrowser_Impl::Render()
{
    // return reinterpret_cast<CEGUI::StaticImage*>(m_pWindow)->render();
    m_pWindow->render();
}

bool CGUIWebBrowser_Impl::HasInputFocus()
{
    return m_pWebView->HasInputFocus();
}

void CGUIWebBrowser_Impl::SetSize(const CVector2D& vecSize, bool bRelative)
{
    // Call base class function
    CGUIElement_Impl::SetSize(vecSize, bRelative);
    auto absSize = CGUIElement_Impl::GetSize(false);

    // Update image area
    if (m_pImage)
        m_pImage->notifyDisplaySizeChanged(CEGUI::Sizef(absSize.fX, absSize.fY));

    // Resize underlying web view as well
    if (m_pWebView)
        m_pWebView->Resize(absSize);
}

bool CGUIWebBrowser_Impl::Event_MouseButtonDown(const CEGUI::EventArgs& e)
{
    const CEGUI::MouseEventArgs& args = reinterpret_cast<const CEGUI::MouseEventArgs&>(e);

    if (args.button == CEGUI::MouseButton::LeftButton)
        m_pWebView->InjectMouseDown(eWebBrowserMouseButton::BROWSER_MOUSEBUTTON_LEFT);
    else if (args.button == CEGUI::MouseButton::MiddleButton)
        m_pWebView->InjectMouseDown(eWebBrowserMouseButton::BROWSER_MOUSEBUTTON_MIDDLE);
    else if (args.button == CEGUI::MouseButton::RightButton)
        m_pWebView->InjectMouseDown(eWebBrowserMouseButton::BROWSER_MOUSEBUTTON_RIGHT);

    return true;
}

bool CGUIWebBrowser_Impl::Event_MouseButtonUp(const CEGUI::EventArgs& e)
{
    const CEGUI::MouseEventArgs& args = reinterpret_cast<const CEGUI::MouseEventArgs&>(e);

    if (args.button == CEGUI::MouseButton::LeftButton)
        m_pWebView->InjectMouseUp(eWebBrowserMouseButton::BROWSER_MOUSEBUTTON_LEFT);
    else if (args.button == CEGUI::MouseButton::MiddleButton)
        m_pWebView->InjectMouseUp(eWebBrowserMouseButton::BROWSER_MOUSEBUTTON_MIDDLE);
    else if (args.button == CEGUI::MouseButton::RightButton)
        m_pWebView->InjectMouseUp(eWebBrowserMouseButton::BROWSER_MOUSEBUTTON_RIGHT);

    return true;
}

bool CGUIWebBrowser_Impl::Event_MouseMove(const CEGUI::EventArgs& e)
{
    const CEGUI::MouseEventArgs& args = reinterpret_cast<const CEGUI::MouseEventArgs&>(e);
    const CEGUI::Vector2f winPos = m_pWindow->getPixelPosition();
    m_pWebView->InjectMouseMove((int)(args.position.d_x - winPos.d_x), (int)(args.position.d_y - winPos.d_y));
    return true;
}

bool CGUIWebBrowser_Impl::Event_MouseWheel(const CEGUI::EventArgs& e)
{
    const CEGUI::MouseEventArgs& args = reinterpret_cast<const CEGUI::MouseEventArgs&>(e);

    m_pWebView->InjectMouseWheel((int)(args.wheelChange * 40), 0);
    return true;
}

bool CGUIWebBrowser_Impl::Event_Activated(const CEGUI::EventArgs& e)
{
    m_pWebView->Focus(true);
    return true;
}

bool CGUIWebBrowser_Impl::Event_Deactivated(const CEGUI::EventArgs& e)
{
    m_pWebView->Focus(false);
    return true;
}

CGUIWebBrowserTexture::CGUIWebBrowserTexture(CEGUI::Renderer* pOwner, CWebViewInterface* pWebView)
    : CEGUI::Direct3D9Texture(reinterpret_cast<CEGUI::Direct3D9Renderer&>(pOwner), ""), m_pWebView(pWebView)
{
}

const CEGUI::Sizef& CGUIWebBrowserTexture::getSize() const
{
    return CEGUI::Sizef(m_pWebView->GetSize().fX, m_pWebView->GetSize().fY);
}

LPDIRECT3DTEXTURE9 CGUIWebBrowserTexture::getDirect3D9Texture() const
{
    return m_pWebView->GetTexture();
}
