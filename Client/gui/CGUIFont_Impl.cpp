/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUIFont_Impl.cpp
 *  PURPOSE:     Font type class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

CGUIFont_Impl::CGUIFont_Impl(CGUI_Impl* pGUI, const char* szFontName, const char* szFontFile, unsigned int uSize, unsigned int uFlags, bool bAutoScale)
{
    // Store the fontmanager and create a font with the given attributes
    m_pFontManager = pGUI->GetFontManager();
    m_pFont = nullptr;
    while (!m_pFont)
    {
        try
        {
            m_pFont = &m_pFontManager->createFreeTypeFont(szFontName, uSize, true, szFontFile, "", CEGUI::ASM_Both, CEGUI::Sizef(1024.0f, 768.0f));
        }
        catch (CEGUI::RendererException)
        {
            // Reduce size until it can fit into a texture
            if (--uSize == 1)
                throw;
        }
    }

    // Define our glyphs
    // m_pFont->setInitialFontGlyphs(" !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");

    // Set default attributes
    SetNativeResolution(1024, 768);
    SetAutoScalingEnabled(bAutoScale);

    m_pGeometryBuffer = &CEGUI::System::getSingleton().getRenderer()->createGeometryBuffer();
}

CGUIFont_Impl::~CGUIFont_Impl()
{
    CEGUI::System::getSingleton().getRenderer()->destroyGeometryBuffer(*m_pGeometryBuffer);
    m_pFontManager->destroy(*m_pFont);
}

void CGUIFont_Impl::SetAntiAliasingEnabled(bool bAntialiased)
{
    // m_pFont->setAntiAliased(bAntialiased);
}

void CGUIFont_Impl::DrawTextString(const char* szText, CRect2D DrawArea, float fZ, CRect2D ClipRect, unsigned long ulFormat, unsigned long ulColor,
                                   float fScaleX, float fScaleY)
{
    CEGUI::HorizontalTextFormatting fmt;

    if (ulFormat == DT_CENTER)
        fmt = CEGUI::HorizontalTextFormatting::HTF_CENTRE_ALIGNED;
    else if (ulFormat == DT_RIGHT)
        fmt = CEGUI::HorizontalTextFormatting::HTF_RIGHT_ALIGNED;
    else
        fmt = CEGUI::HorizontalTextFormatting::HTF_LEFT_ALIGNED;

    // CEGUI::Rectf clipRect(ClipRect.fX1, ClipRect.fY1, ClipRect.fX2, ClipRect.fY2);

    m_pFont->drawText(*m_pGeometryBuffer, szText ? CGUI_Impl::GetUTFString(szText) : CEGUI::String(), CEGUI::Vector2f(DrawArea.fX1, DrawArea.fY1), 0,
                      CEGUI::ColourRect(CEGUI::Colour((CEGUI::argb_t)ulColor)), 0, fScaleX, fScaleY);
}

bool CGUIFont_Impl::IsAntiAliasingEnabled()
{
    // return m_pFont->isAntiAliased();
    return true;
}

void CGUIFont_Impl::SetAutoScalingEnabled(bool bAutoScaled)
{
    m_pFont->setAutoScaled(bAutoScaled ? CEGUI::ASM_Both : CEGUI::ASM_Disabled);
}

bool CGUIFont_Impl::IsAutoScalingEnabled()
{
    return m_pFont->getAutoScaled() != CEGUI::ASM_Disabled;
}

void CGUIFont_Impl::SetNativeResolution(int iX, int iY)
{
    m_pFont->setNativeResolution(CEGUI::Size<float>(static_cast<float>(iX), static_cast<float>(iY)));
}

float CGUIFont_Impl::GetCharacterWidth(int iChar, float fScale)
{
    char szBuf[2];
    szBuf[0] = iChar;
    szBuf[1] = 0;

    return m_pFont->getTextExtent(szBuf, fScale);
}

float CGUIFont_Impl::GetFontHeight(float fScale)
{
    float fHeight = m_pFont->getFontHeight(fScale);            // average height.. not the maximum height for long characters such as 'g' or 'j'
    fHeight += 2.0f;                                           // so hack it

    return fHeight;
}

float CGUIFont_Impl::GetTextExtent(const char* szText, float fScale)
{
    return m_pFont->getTextExtent(CGUI_Impl::GetUTFString(szText), fScale);
}

CEGUI::Font* CGUIFont_Impl::GetFont()
{
    return m_pFont;
}
