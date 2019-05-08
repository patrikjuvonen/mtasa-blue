/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUIStaticImage_Impl.cpp
 *  PURPOSE:     Static image widget class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

#define CGUISTATICIMAGE_NAME "CGUI/StaticImage"

CGUIStaticImage_Impl::CGUIStaticImage_Impl(CGUI_Impl* pGUI, CGUIElement* pParent)
{
    // Initialize
    m_pImageManager = pGUI->GetImageManager();
    m_pImage = nullptr;
    m_pGUI = pGUI;
    m_pManager = pGUI;
    m_pTexture = nullptr;
    m_bCreatedTexture = false;

    // Get an unique identifier for CEGUI
    char szUnique[CGUI_CHAR_SIZE];
    pGUI->GetUniqueName(szUnique);

    // Create the control and set default properties
    m_pWindow = pGUI->GetWindowManager()->createWindow(CGUISTATICIMAGE_NAME, szUnique);
    m_pWindow->setDestroyedByParent(false);
    m_pWindow->setProperty("BackgroundEnabled", "false");
    m_pWindow->setArea(CEGUI::URect(cegui_reldim(0.0f), cegui_reldim(0.0f), cegui_reldim(1.0f), cegui_reldim(1.0f)));

    // Store the pointer to this CGUI element in the CEGUI element
    m_pWindow->setUserData(reinterpret_cast<void*>(this));

    AddEvents();

    // If a parent is specified, add it to it's children list, if not, add it as a child to the pManager
    if (pParent)
        SetParent(pParent);
    else
    {
        pGUI->AddChild(this);
        SetParent(nullptr);
    }
}

CGUIStaticImage_Impl::~CGUIStaticImage_Impl()
{
    // Clear the image
    Clear();

    DestroyElement();
}

bool CGUIStaticImage_Impl::LoadFromFile(const char* szFilename)
{
    // Load texture
    if (!m_pTexture)
    {
        m_pTexture = new CGUITexture_Impl(m_pGUI);
        m_bCreatedTexture = true;
    }

    if (!m_pTexture->LoadFromFile(szFilename))
        return false;

    // Load image
    return LoadFromTexture(m_pTexture);
}

bool CGUIStaticImage_Impl::LoadFromTexture(CGUITexture* pTexture)
{
    if (m_pTexture && pTexture != m_pTexture)
    {
        if (m_bCreatedTexture)
        {
            delete m_pTexture;
            m_pTexture = nullptr;
            m_bCreatedTexture = false;
        }
    }

    m_pTexture = (CGUITexture_Impl*)pTexture;

    // Get CEGUI texture
    CEGUI::Texture* pCEGUITexture = m_pTexture->GetTexture();

    // Get an unique identifier for CEGUI for the image
    char szUnique[CGUI_CHAR_SIZE];
    m_pGUI->GetUniqueName(szUnique);
    
    // Set the image just loaded as the image to be drawn for the widget
    CEGUI::Sizef size = pCEGUITexture->getSize();
    reinterpret_cast<CEGUI::BasicImage*>(m_pWindow)->setArea(CEGUI::Rectf(0, 0, size.d_width, size.d_height));
    reinterpret_cast<CEGUI::BasicImage*>(m_pWindow)->setTexture(pCEGUITexture);

    // Success
    return true;
}

void CGUIStaticImage_Impl::Clear()
{
    // Stop the control from using it
    reinterpret_cast<CEGUI::BasicImage*>(m_pWindow)->setTexture(nullptr);

    // Kill the images
    if (m_bCreatedTexture)
    {
        delete m_pTexture;
        m_pTexture = nullptr;
        m_bCreatedTexture = false;
    }
    m_pImage = nullptr;
}

bool CGUIStaticImage_Impl::GetNativeSize(CVector2D& vecSize)
{
    if (m_pTexture)
    {
        if (m_pTexture->GetTexture())
        {
            CEGUI::Sizef size = m_pTexture->GetTexture()->getSize();
            vecSize.fX = size.d_width;
            vecSize.fY = size.d_height;
            return true;
        }
    }
    return false;
}

void CGUIStaticImage_Impl::SetFrameEnabled(bool bFrameEnabled)
{
    m_pWindow->setProperty("FrameEnabled", bFrameEnabled ? "true" : "false");
}

bool CGUIStaticImage_Impl::IsFrameEnabled()
{
    return m_pWindow->getProperty("FrameEnabled") == "true";
}

CEGUI::Image* CGUIStaticImage_Impl::GetDirectImage()
{
    // return const_cast<CEGUI::Image*>(reinterpret_cast<CEGUI::StaticImage*>(m_pWindow)->getImage());
    return nullptr;
}

void CGUIStaticImage_Impl::Render()
{
    m_pWindow->render();
}
