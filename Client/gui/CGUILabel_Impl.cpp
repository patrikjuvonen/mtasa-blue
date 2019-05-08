/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUILabel_Impl.cpp
 *  PURPOSE:     Label widget class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

#define CGUILABEL_NAME "CGUI/StaticText"

CGUILabel_Impl::CGUILabel_Impl(CGUI_Impl* pGUI, CGUIElement* pParent, const char* szText)
{
    m_pManager = pGUI;

    // Get an unique identifier for CEGUI (gah, there's gotta be an another way)
    char szUnique[CGUI_CHAR_SIZE];
    pGUI->GetUniqueName(szUnique);

    // Create the window and set default settings
    m_pWindow = pGUI->GetWindowManager()->createWindow(CGUILABEL_NAME, szUnique);
    m_pWindow->setDestroyedByParent(false);

    // Store the pointer to this CGUI element in the CEGUI element
    m_pWindow->setUserData(reinterpret_cast<void*>(this));

    AddEvents();

    SetHorizontalAlign((CGUIHorizontalAlign)CEGUI::HTF_LEFT_ALIGNED);
    SetVerticalAlign((CGUIVerticalAlign)CEGUI::VTF_TOP_ALIGNED);
    SetText(szText);
    m_pWindow->setProperty("BackgroundEnabled", "false");

    // If a parent is specified, add it to it's children list, if not, add it as a child to the pManager
    if (pParent)
        SetParent(pParent);
    else
    {
        pGUI->AddChild(this);
        SetParent(nullptr);
    }
}

CGUILabel_Impl::~CGUILabel_Impl()
{
    DestroyElement();
}

void CGUILabel_Impl::SetText(const char* Text)
{
    // Set the new text and size the text field after it
    m_pWindow->setText(CGUI_Impl::GetUTFString(Text));
}

void CGUILabel_Impl::SetVerticalAlign(CGUIVerticalAlign eAlign)
{
    reinterpret_cast<CEGUI::TextComponent*>(m_pWindow)->setVerticalFormatting((CEGUI::VerticalTextFormatting)eAlign);
}

CGUIVerticalAlign CGUILabel_Impl::GetVerticalAlign()
{
    return (CGUIVerticalAlign)reinterpret_cast<CEGUI::TextComponent*>(m_pWindow)->getVerticalFormattingFromComponent();
}

void CGUILabel_Impl::SetHorizontalAlign(CGUIHorizontalAlign eAlign)
{
    reinterpret_cast<CEGUI::TextComponent*>(m_pWindow)->setHorizontalFormatting((CEGUI::HorizontalTextFormatting)eAlign);
}

CGUIHorizontalAlign CGUILabel_Impl::GetHorizontalAlign()
{
    return (CGUIHorizontalAlign)reinterpret_cast<CEGUI::TextComponent*>(m_pWindow)->getHorizontalFormattingFromComponent();
}

void CGUILabel_Impl::SetTextColor(CGUIColor Color)
{
    reinterpret_cast<CEGUI::TextComponent*>(m_pWindow)->setColours(CEGUI::Colour(1.0f / 255.0f * Color.R, 1.0f / 255.0f * Color.G, 1.0f / 255.0f * Color.B));
}

void CGUILabel_Impl::SetTextColor(unsigned char ucRed, unsigned char ucGreen, unsigned char ucBlue)
{
    reinterpret_cast<CEGUI::TextComponent*>(m_pWindow)->setColours(CEGUI::Colour(1.0f / 255.0f * ucRed, 1.0f / 255.0f * ucGreen, 1.0f / 255.0f * ucBlue));
}

CGUIColor CGUILabel_Impl::GetTextColor()
{
    CGUIColor temp;
    GetTextColor(temp.R, temp.G, temp.B);
    return temp;
}

void CGUILabel_Impl::GetTextColor(unsigned char& ucRed, unsigned char& ucGreen, unsigned char& ucBlue)
{
    CEGUI::Colour r = (reinterpret_cast<CEGUI::TextComponent*>(m_pWindow)->getColours()).getColourAtPoint(0, 0);

    ucRed = (unsigned char)(r.getRed() * 255);
    ucGreen = (unsigned char)(r.getGreen() * 255);
    ucBlue = (unsigned char)(r.getBlue() * 255);
}

void CGUILabel_Impl::SetFrameEnabled(bool bFrameEnabled)
{
    m_pWindow->setProperty("FrameEnabled", "false");
}

bool CGUILabel_Impl::IsFrameEnabled()
{
    return m_pWindow->getProperty("FrameEnabled") == "true";
}

float CGUILabel_Impl::GetCharacterWidth(int iCharIndex)
{
    if (true)
        return true;
}

float CGUILabel_Impl::GetFontHeight()
{
    const CEGUI::Font* pFont = m_pWindow->getFont();
    if (pFont)
        return pFont->getFontHeight();
    return 14.0f;
}

float CGUILabel_Impl::GetTextExtent()
{
    const CEGUI::Font* pFont = m_pWindow->getFont();
    if (pFont)
    {
        try
        {
            // Retrieve the longest line's extent
            std::stringstream ssText(m_pWindow->getText().c_str());
            std::string       sLineText;
            float             fMax = 0.0f, fLineExtent = 0.0f;

            while (std::getline(ssText, sLineText))
            {
                fLineExtent = pFont->getTextExtent(CGUI_Impl::GetUTFString(sLineText));
                if (fLineExtent > fMax)
                    fMax = fLineExtent;
            }
            return fMax;
        }
        catch (CEGUI::Exception e)
        {
        }
    }

    return 0.0f;
}
