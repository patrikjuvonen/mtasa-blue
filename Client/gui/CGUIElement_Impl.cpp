/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUIElement_Impl.cpp
 *  PURPOSE:     Element (widget) base class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

// Define no-drawing zones, a.k.a. the inside borders in the FrameWindow of BlueLook in pixels
// If something is drawn inside of these areas, the theme border is drawn on top of it
#define CGUI_NODRAW_LEFT 9.0f
#define CGUI_NODRAW_RIGHT 9.0f
#define CGUI_NODRAW_TOP 9.0f
#define CGUI_NODRAW_BOTTOM 9.0f

CGUIElement_Impl::CGUIElement_Impl()
{
    m_pData = nullptr;
    m_pWindow = nullptr;
    m_pParent = nullptr;
    m_pManager = nullptr;
}

void CGUIElement_Impl::DestroyElement()
{
    m_pManager->RemoveFromRedrawQueue(reinterpret_cast<CGUIElement*>((m_pWindow)->getUserData()));

    // Clear pointer back to this
    m_pWindow->setUserData(nullptr);

    // Destroy the control
    m_pManager->GetWindowManager()->destroyWindow(m_pWindow);

    // Destroy the properties list
    EmptyProperties();
}

void CGUIElement_Impl::SetVisible(bool bVisible)
{
    m_pWindow->setVisible(bVisible);
}

bool CGUIElement_Impl::IsVisible()
{
    return m_pWindow->isVisible();
}

void CGUIElement_Impl::SetEnabled(bool bEnabled)
{
    m_pWindow->setEnabled(bEnabled);
    // m_pWindow->setZOrderingEnabled ( bEnabled );
}

bool CGUIElement_Impl::IsEnabled()
{
    return !m_pWindow->isDisabled();
}

void CGUIElement_Impl::SetZOrderingEnabled(bool bZOrderingEnabled)
{
    m_pWindow->setZOrderingEnabled(bZOrderingEnabled);
}

bool CGUIElement_Impl::IsZOrderingEnabled()
{
    return m_pWindow->isZOrderingEnabled();
}

void CGUIElement_Impl::BringToFront()
{
    m_pWindow->moveToFront();
}

void CGUIElement_Impl::MoveToBack()
{
    m_pWindow->moveToBack();
}

void CGUIElement_Impl::SetPosition(const CVector2D& Position, bool bRelative)
{
    if (bRelative)
        m_pWindow->setPosition(CEGUI::Vector2<CEGUI::UDim>(cegui_reldim(Position.fX), cegui_reldim(Position.fY)));
    else
        m_pWindow->setPosition(CEGUI::Vector2<CEGUI::UDim>(cegui_absdim(Position.fX), cegui_absdim(Position.fY)));

    CorrectEdges();
}

CVector2D CGUIElement_Impl::GetPosition(bool bRelative)
{
    if (bRelative)
    {
        CEGUI::UVector2 temp = m_pWindow->getPosition();
        return CVector2D(temp.d_x.d_scale, temp.d_y.d_scale);
    }
    else
    {
        CEGUI::Vector2f temp = m_pWindow->getPixelPosition();
        return CVector2D(temp.d_x, temp.d_y);
    }
}

void CGUIElement_Impl::GetPosition(CVector2D& vecPosition, bool bRelative)
{
    if (bRelative)
    {
        CEGUI::UVector2 temp = m_pWindow->getPosition();

        vecPosition.fX = temp.d_x.d_offset;
        vecPosition.fY = temp.d_y.d_offset;
    }
    else
    {
        CEGUI::Vector2f temp = m_pWindow->getPixelPosition();

        vecPosition.fX = temp.d_x;
        vecPosition.fY = temp.d_y;
    }
}

void CGUIElement_Impl::SetWidth(float fX, bool bRelative)
{
    if (bRelative)
        m_pWindow->setWidth(cegui_reldim(fX));
    else
        m_pWindow->setWidth(cegui_absdim(fX));
}

void CGUIElement_Impl::SetHeight(float fY, bool bRelative)
{
    if (bRelative)
        m_pWindow->setHeight(cegui_reldim(fY));
    else
        m_pWindow->setHeight(cegui_absdim(fY));
}

void CGUIElement_Impl::SetSize(const CVector2D& vecSize, bool bRelative)
{
    CEGUI::USize size;

    if (bRelative)
        size = CEGUI::USize(cegui_reldim(vecSize.fX), cegui_reldim(vecSize.fY));
    else
        size = CEGUI::USize(cegui_absdim(vecSize.fX), cegui_absdim(vecSize.fY));

    m_pWindow->setSize(size);
    CorrectEdges();
}

CVector2D CGUIElement_Impl::GetSize(bool bRelative)
{
    if (bRelative)
    {
        CEGUI::USize size = m_pWindow->getSize();
        return CVector2D(size.d_width.d_scale, size.d_height.d_scale);
    }
    else
    {
        CEGUI::Sizef size = m_pWindow->getPixelSize();
        return CVector2D(size.d_width, size.d_height);
    }
}

void CGUIElement_Impl::GetSize(CVector2D& vecSize, bool bRelative)
{
    CEGUI::USize size = m_pWindow->getSize();

    if (bRelative)
    {
        CEGUI::USize size = m_pWindow->getSize();
        vecSize.fX = size.d_width.d_scale;
        vecSize.fY = size.d_height.d_scale;
    }
    else
    {
        CEGUI::Sizef size = m_pWindow->getPixelSize();
        vecSize.fX = size.d_width;
        vecSize.fY = size.d_height;
    }
}

void CGUIElement_Impl::AutoSize(const char* Text, float fPaddingX, float fPaddingY)
{
    const CEGUI::Font* pFont = m_pWindow->getFont();
    m_pWindow->setSize(
        CEGUI::USize(cegui_absdim(pFont->getTextExtent(CGUI_Impl::GetUTFString(Text ? Text : GetText())) + fPaddingX),
                     cegui_absdim(pFont->getFontHeight() + fPaddingY)));            // Add hack factor to height to allow for long characters such as 'g' or 'j'
}

void CGUIElement_Impl::SetMinimumSize(const CVector2D& vecSize)
{
    m_pWindow->setMinSize(CEGUI::USize(cegui_absdim(vecSize.fX), cegui_absdim(vecSize.fY)));
}

CVector2D CGUIElement_Impl::GetMinimumSize()
{
    const CEGUI::USize& minSize = m_pWindow->getMinSize();
    return CVector2D(minSize.d_width.d_offset, minSize.d_height.d_offset);
}

void CGUIElement_Impl::GetMinimumSize(CVector2D& vecSize)
{
    const CEGUI::USize& minSize = m_pWindow->getMinSize();
    vecSize.fX = minSize.d_width.d_offset;
    vecSize.fY = minSize.d_height.d_offset;
}

void CGUIElement_Impl::SetMaximumSize(const CVector2D& vecSize)
{
    m_pWindow->setMaxSize(CEGUI::USize(cegui_absdim(vecSize.fX), cegui_absdim(vecSize.fY)));
}

CVector2D CGUIElement_Impl::GetMaximumSize()
{
    const CEGUI::USize& maxSize = m_pWindow->getMaxSize();
    return CVector2D(maxSize.d_width.d_offset, maxSize.d_height.d_offset);
}

void CGUIElement_Impl::GetMaximumSize(CVector2D& vecSize)
{
    const CEGUI::USize& maxSize = m_pWindow->getMaxSize();
    vecSize.fX = maxSize.d_width.d_offset;
    vecSize.fY = maxSize.d_height.d_offset;
}

void CGUIElement_Impl::SetText(const char* szText)
{
    m_pWindow->setText(CGUI_Impl::GetUTFString(szText));
}

std::string CGUIElement_Impl::GetText()
{
    return CGUI_Impl::GetUTFString(m_pWindow->getText().c_str()).c_str();
}

void CGUIElement_Impl::SetAlpha(float fAlpha)
{
    m_pWindow->setAlpha(fAlpha);
}

float CGUIElement_Impl::GetAlpha()
{
    return m_pWindow->getAlpha();
}

float CGUIElement_Impl::GetEffectiveAlpha()
{
    return m_pWindow->getEffectiveAlpha();
}

void CGUIElement_Impl::SetInheritsAlpha(bool bInheritsAlpha)
{
    m_pWindow->setInheritsAlpha(bInheritsAlpha);
}

bool CGUIElement_Impl::GetInheritsAlpha()
{
    return m_pWindow->inheritsAlpha();
}

void CGUIElement_Impl::Activate()
{
    m_pWindow->activate();
}

void CGUIElement_Impl::Deactivate()
{
    m_pWindow->deactivate();
}

bool CGUIElement_Impl::IsActive()
{
    return m_pWindow->isActive();
}

void CGUIElement_Impl::SetAlwaysOnTop(bool bAlwaysOnTop)
{
    m_pWindow->setAlwaysOnTop(bAlwaysOnTop);
}

bool CGUIElement_Impl::IsAlwaysOnTop()
{
    return m_pWindow->isAlwaysOnTop();
}

CRect2D CGUIElement_Impl::AbsoluteToRelative(const CRect2D& Rect)
{
    CEGUI::URect AbsRect(cegui_absdim(Rect.fX1), cegui_absdim(Rect.fY1), cegui_absdim(Rect.fX2), cegui_absdim(Rect.fY2));
    CEGUI::Rectf RelRect = CEGUI::CoordConverter::asRelative(AbsRect, m_pWindow->getPixelSize());
    return CRect2D(RelRect.d_min.d_x, RelRect.d_min.d_y, RelRect.d_max.d_x, RelRect.d_max.d_y);
}

CVector2D CGUIElement_Impl::AbsoluteToRelative(const CVector2D& Vector)
{
    CEGUI::USize AbsVec(cegui_absdim(Vector.fX), cegui_absdim(Vector.fY));
    CEGUI::Sizef RelVec = CEGUI::CoordConverter::asRelative(AbsVec, m_pWindow->getPixelSize());
    return CVector2D(RelVec.d_width, RelVec.d_height);
}

CRect2D CGUIElement_Impl::RelativeToAbsolute(const CRect2D& Rect)
{
    CEGUI::URect AbsRect(cegui_absdim(Rect.fX1), cegui_absdim(Rect.fY1), cegui_absdim(Rect.fX2), cegui_absdim(Rect.fY2));
    CEGUI::Rectf RelRect = CEGUI::CoordConverter::asAbsolute(AbsRect, m_pWindow->getPixelSize());
    return CRect2D(RelRect.d_min.d_x, RelRect.d_min.d_y, RelRect.d_max.d_x, RelRect.d_max.d_y);
}

CVector2D CGUIElement_Impl::RelativeToAbsolute(const CVector2D& Vector)
{
    CEGUI::USize AbsVec(cegui_absdim(Vector.fX), cegui_absdim(Vector.fY));
    CEGUI::Sizef RelVec = CEGUI::CoordConverter::asAbsolute(AbsVec, m_pWindow->getPixelSize());
    return CVector2D(RelVec.d_width, RelVec.d_height);
}

void CGUIElement_Impl::SetParent(CGUIElement* pParent)
{
    // Disable z-sorting if the label has a parent
    if (GetType() == CGUI_LABEL)
        m_pWindow->setZOrderingEnabled(pParent == NULL);

    if (pParent)
    {
        CGUIElement_Impl* pElement = dynamic_cast<CGUIElement_Impl*>(pParent);
        if (pElement)
            pElement->m_pWindow->addChild(m_pWindow);
    }
    m_pParent = pParent;
}

CGUIElement* CGUIElement_Impl::GetParent()
{
    // Validate
    if (m_pParent && m_pWindow && !m_pWindow->getParent())
        return NULL;

    return m_pParent;
}

CEGUI::Window* CGUIElement_Impl::GetWindow()
{
    return m_pWindow;
}

void CGUIElement_Impl::CorrectEdges()
{
    CEGUI::UVector2 currentPosition = m_pWindow->getPosition();
    CEGUI::USize    currentSize = m_pWindow->getSize();
    // Label turns out to be buggy
    if (m_pWindow->getType() == "CGUI/StaticText")
        return;

    if (m_pWindow->getParent()->getType() == "CGUI/FrameWindow")
    {
        CEGUI::USize parentSize = m_pWindow->getParent()->getSize();
        if (currentPosition.d_x.d_offset < CGUI_NODRAW_LEFT)
            currentPosition.d_x.d_offset += CGUI_NODRAW_LEFT - currentPosition.d_x.d_offset;
        if (currentPosition.d_y.d_offset < CGUI_NODRAW_TOP)
            currentPosition.d_y.d_offset += CGUI_NODRAW_TOP - currentPosition.d_x.d_offset;
        if ((currentSize.d_height.d_offset + currentPosition.d_y.d_offset) > (parentSize.d_height.d_offset - CGUI_NODRAW_BOTTOM))
            currentSize.d_height.d_offset -=
                (currentSize.d_height.d_offset + currentPosition.d_y.d_offset) - (parentSize.d_height.d_offset - CGUI_NODRAW_BOTTOM);
        if ((currentSize.d_width.d_offset + currentPosition.d_x.d_offset) > (parentSize.d_width.d_offset - CGUI_NODRAW_RIGHT))
            currentSize.d_width.d_offset -= (currentSize.d_width.d_offset + currentPosition.d_x.d_offset) - (parentSize.d_width.d_offset - CGUI_NODRAW_RIGHT);
        m_pWindow->setPosition(currentPosition);
        m_pWindow->setSize(currentSize);
    }
}

bool CGUIElement_Impl::SetFont(const char* szFontName)
{
    try
    {
        m_pWindow->setFont(CEGUI::String(szFontName));
        return true;
    }
    catch (CEGUI::Exception e)
    {
        return false;
    }
}

std::string CGUIElement_Impl::GetFont()
{
    try
    {
        const CEGUI::Font* pFont = m_pWindow->getFont();
        if (pFont)
        {
            // Return the contname. std::string will copy it.
            CEGUI::String strFontName = pFont->getName();
            return strFontName.c_str();
        }
    }
    catch (CEGUI::Exception e)
    {
    }

    return "";
}

void CGUIElement_Impl::SetProperty(const char* szProperty, const char* szValue)
{
    try
    {
        m_pWindow->setProperty(CGUI_Impl::GetUTFString(szProperty), CGUI_Impl::GetUTFString(szValue));
    }
    catch (CEGUI::Exception e)
    {
    }
}

std::string CGUIElement_Impl::GetProperty(const char* szProperty)
{
    CEGUI::String strValue;
    try
    {
        // Return the string. std::string will copy it
        strValue = CGUI_Impl::GetUTFString(m_pWindow->getProperty(CGUI_Impl::GetUTFString(szProperty)).c_str());
    }
    catch (CEGUI::Exception e)
    {
    }

    return strValue.c_str();
}

void CGUIElement_Impl::FillProperties()
{
    CEGUI::Window::PropertyIterator itPropertySet = ((CEGUI::PropertySet*)m_pWindow)->getPropertyIterator();
    while (!itPropertySet.isAtEnd())
    {
        CEGUI::String strKey = itPropertySet.getCurrentKey();
        CEGUI::String strValue = m_pWindow->getProperty(strKey);

        CGUIProperty* pProperty = new CGUIProperty;
        pProperty->strKey = strKey.c_str();
        pProperty->strValue = strValue.c_str();

        m_Properties.push_back(pProperty);
        itPropertySet++;
    }
}

void CGUIElement_Impl::EmptyProperties()
{
    if (!m_Properties.empty())
    {
        CGUIPropertyIter iter = m_Properties.begin();
        CGUIPropertyIter iterEnd = m_Properties.end();
        for (; iter != iterEnd; iter++)
        {
            if (*iter)
            {
                delete (*iter);
            }
        }
    }
}

CGUIPropertyIter CGUIElement_Impl::GetPropertiesBegin()
{
    try
    {
        // Fill the properties list, if it's still empty (on first call)
        if (m_Properties.empty())
            FillProperties();

        // Return the list begin iterator
        return m_Properties.begin();
    }
    catch (CEGUI::Exception e)
    {
        return *(CGUIPropertyIter*)NULL;
    }
}

CGUIPropertyIter CGUIElement_Impl::GetPropertiesEnd()
{
    try
    {
        // Fill the properties list, if it's still empty (on first call)
        if (m_Properties.empty())
            FillProperties();

        // Return the list begin iterator
        return m_Properties.end();
    }
    catch (CEGUI::Exception e)
    {
        return *(CGUIPropertyIter*)NULL;
    }
}

void CGUIElement_Impl::SetMovedHandler(GUI_CALLBACK Callback)
{
    m_OnMoved = Callback;
}

void CGUIElement_Impl::SetSizedHandler(GUI_CALLBACK Callback)
{
    m_OnSized = Callback;
}

void CGUIElement_Impl::SetClickHandler(GUI_CALLBACK Callback)
{
    m_OnClick = Callback;
}

void CGUIElement_Impl::SetDoubleClickHandler(GUI_CALLBACK Callback)
{
    m_OnDoubleClick = Callback;
}

void CGUIElement_Impl::SetMouseEnterHandler(GUI_CALLBACK Callback)
{
    m_OnMouseEnter = Callback;
}

void CGUIElement_Impl::SetMouseLeaveHandler(GUI_CALLBACK Callback)
{
    m_OnMouseLeave = Callback;
}

void CGUIElement_Impl::SetMouseButtonDownHandler(GUI_CALLBACK Callback)
{
    m_OnMouseDown = Callback;
}

void CGUIElement_Impl::SetActivateHandler(GUI_CALLBACK Callback)
{
    m_OnActivate = Callback;
}

void CGUIElement_Impl::SetDeactivateHandler(GUI_CALLBACK Callback)
{
    m_OnDeactivate = Callback;
}

void CGUIElement_Impl::SetKeyDownHandler(GUI_CALLBACK Callback)
{
    m_OnKeyDown = Callback;
}

void CGUIElement_Impl::SetEnterKeyHandler(GUI_CALLBACK Callback)
{
    m_OnEnter = Callback;
}

void CGUIElement_Impl::SetKeyDownHandler(const GUI_CALLBACK_KEY& Callback)
{
    m_OnKeyDownWithArgs = Callback;
}

void CGUIElement_Impl::AddEvents()
{
    // Note: Mouse Click, Double Click, Enter, Leave and ButtonDown are handled by global events in CGUI_Impl
    // Register our events
    m_pWindow->subscribeEvent(CEGUI::Window::EventMoved, CEGUI::Event::Subscriber(&CGUIElement_Impl::Event_OnMoved, this));
    m_pWindow->subscribeEvent(CEGUI::Window::EventSized, CEGUI::Event::Subscriber(&CGUIElement_Impl::Event_OnSized, this));
    m_pWindow->subscribeEvent(CEGUI::Window::EventActivated, CEGUI::Event::Subscriber(&CGUIElement_Impl::Event_OnActivated, this));
    m_pWindow->subscribeEvent(CEGUI::Window::EventDeactivated, CEGUI::Event::Subscriber(&CGUIElement_Impl::Event_OnDeactivated, this));
    m_pWindow->subscribeEvent(CEGUI::Window::EventKeyDown, CEGUI::Event::Subscriber(&CGUIElement_Impl::Event_OnKeyDown, this));
}

bool CGUIElement_Impl::Event_OnMoved(const CEGUI::EventArgs& e)
{
    if (m_OnMoved)
        m_OnMoved(reinterpret_cast<CGUIElement*>(this));
    return true;
}

bool CGUIElement_Impl::Event_OnSized(const CEGUI::EventArgs& e)
{
    if (m_OnSized)
        m_OnSized(reinterpret_cast<CGUIElement*>(this));
    return true;
}

bool CGUIElement_Impl::Event_OnClick()
{
    if (m_OnClick)
        m_OnClick(reinterpret_cast<CGUIElement*>(this));
    return true;
}

bool CGUIElement_Impl::Event_OnDoubleClick()
{
    if (m_OnDoubleClick)
        m_OnDoubleClick(reinterpret_cast<CGUIElement*>(this));
    return true;
}

bool CGUIElement_Impl::Event_OnMouseEnter()
{
    if (m_OnMouseEnter)
        m_OnMouseEnter(reinterpret_cast<CGUIElement*>(this));
    return true;
}

bool CGUIElement_Impl::Event_OnMouseLeave()
{
    if (m_OnMouseLeave)
        m_OnMouseLeave(reinterpret_cast<CGUIElement*>(this));
    return true;
}

bool CGUIElement_Impl::Event_OnMouseButtonDown()
{
    if (m_OnMouseDown)
        m_OnMouseDown(reinterpret_cast<CGUIElement*>(this));
    return true;
}

bool CGUIElement_Impl::Event_OnActivated(const CEGUI::EventArgs& e)
{
    if (m_OnActivate)
        m_OnActivate(reinterpret_cast<CGUIElement*>(this));
    return true;
}

bool CGUIElement_Impl::Event_OnDeactivated(const CEGUI::EventArgs& e)
{
    if (m_OnDeactivate)
        m_OnDeactivate(reinterpret_cast<CGUIElement*>(this));
    return true;
}

bool CGUIElement_Impl::Event_OnKeyDown(const CEGUI::EventArgs& e)
{
    const CEGUI::KeyEventArgs& Args = reinterpret_cast<const CEGUI::KeyEventArgs&>(e);
    CGUIElement*               pCGUIElement = reinterpret_cast<CGUIElement*>(this);

    if (m_OnKeyDown)
    {
        m_OnKeyDown(pCGUIElement);
    }

    if (m_OnKeyDownWithArgs)
    {
        CGUIKeyEventArgs NewArgs;

        // copy the variables
        NewArgs.codepoint = Args.codepoint;
        NewArgs.scancode = (CGUIKeys::Scan)Args.scancode;
        NewArgs.sysKeys = Args.sysKeys;

        // get the CGUIElement
        CGUIElement* pElement = reinterpret_cast<CGUIElement*>((Args.window)->getUserData());
        NewArgs.pWindow = pElement;

        m_OnKeyDownWithArgs(NewArgs);
    }

    if (m_OnEnter)
    {
        switch (Args.scancode)
        {
            // Return key
            case CEGUI::Key::NumpadEnter:
            case CEGUI::Key::Return:
            {
                // Fire the event
                m_OnEnter(pCGUIElement);
                break;
            }
        }
    }

    return true;
}

inline void CGUIElement_Impl::ForceRedraw()
{
    m_pWindow->getGUIContext().draw();
}
