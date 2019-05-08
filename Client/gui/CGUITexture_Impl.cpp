/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUITexture_Impl.cpp
 *  PURPOSE:     Texture handling class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CGUITexture_Impl::CGUITexture_Impl(CGUI_Impl* pGUI) : m_pGUI(pGUI)
{
    // Save the renderer
    m_pRenderer = pGUI->GetRenderer();

    // Create the texture
    char szUnique[CGUI_CHAR_SIZE];
    pGUI->GetUniqueName(szUnique);
    m_pTexture = &m_pRenderer->createTexture(szUnique);
}

CGUITexture_Impl::~CGUITexture_Impl()
{
    if (m_pTexture)
        m_pRenderer->destroyTexture(*m_pTexture);
}

bool CGUITexture_Impl::LoadFromFile(const char* szFilename)
{
    // Try to load it
    try
    {
        m_pTexture->loadFromFile(szFilename, "");
    }
    catch (CEGUI::Exception)
    {
        return false;
    }
    return true;
}

void CGUITexture_Impl::LoadFromMemory(const void* pBuffer, unsigned int uiWidth, unsigned int uiHeight)
{
    m_pTexture->loadFromMemory(pBuffer, CEGUI::Sizef(uiWidth, uiHeight), CEGUI::Texture::PF_RGBA);
}

void CGUITexture_Impl::Clear()
{
    // Destroy the previous texture and recreate it (empty)
    m_pRenderer->destroyTexture(*m_pTexture);

    // Create the texture
    char szUnique[CGUI_CHAR_SIZE];
    m_pGUI->GetUniqueName(szUnique);
    m_pTexture = &m_pRenderer->createTexture(szUnique);
}

CEGUI::Texture* CGUITexture_Impl::GetTexture()
{
    return m_pTexture;
}

void CGUITexture_Impl::SetTexture(CEGUI::Texture* pTexture)
{
    m_pTexture = pTexture;
}

LPDIRECT3DTEXTURE9 CGUITexture_Impl::GetD3DTexture()
{
    return reinterpret_cast<CEGUI::Direct3D9Texture*>(m_pTexture)->getDirect3D9Texture();
}

void CGUITexture_Impl::CreateTexture(unsigned int width, unsigned int height)
{
    return reinterpret_cast<CEGUI::Direct3D9Texture*>(m_pTexture)->setOriginalDataSize(CEGUI::Sizef(width, height));
}
