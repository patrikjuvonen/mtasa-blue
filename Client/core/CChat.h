/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CChat.h
 *  PURPOSE:     Header file for the chatbox class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include "CGUI.h"
#include <core/CCoreInterface.h>

#define CHAT_WIDTH 320                                            // Chatbox default width
#define CHAT_TEXT_COLOR SColorRGBA(235, 221, 178, 255)            // Chatbox default text color
#define CHAT_MAX_LINES 100                                        // Chatbox maximum chat lines
#define CHAT_MAX_CHAT_LENGTH 96                                   // Chatbox maximum chat message length
#define CHAT_BUFFER 1024                                          // Chatbox buffer size

class CChatLineSection
{
public:
    friend class CChatLine;

    CChatLineSection();
    CChatLineSection(const CChatLineSection& other);

    CChatLineSection& operator=(const CChatLineSection& other);

    void        Draw(const CVector2D& vecPosition, unsigned char ucAlpha, bool bShadow, bool bOutline, const CRect2D& RenderBounds);
    float       GetWidth();
    const char* GetText() { return m_strText.c_str(); }
    void        SetText(const char* szText) { m_strText = szText; }
    void        GetColor(SColorRGBA& color) { color = m_Color; }
    void        SetColor(const SColorRGBA& color) { m_Color = color; }

protected:
    std::string  m_strText;
    SColorRGBA   m_Color = SColorRGBA(255, 255, 255, 255);
    float        m_fCachedWidth;
    unsigned int m_uiCachedLength;
};

class CChatLine
{
public:
    CChatLine();

    virtual const char* Format(const char* szText, float fWidth, SColorRGBA& color, bool bColorCoded);
    virtual void        Draw(const CVector2D& vecPosition, unsigned char ucAlpha, bool bShadow, bool bOutline, const CRect2D& RenderBounds);
    virtual float       GetWidth();
    bool                IsActive() { return m_bActive; }
    void                SetActive(bool bActive) { m_bActive = bActive; }

    unsigned long GetCreationTime() { return m_ulCreationTime; }
    void          UpdateCreationTime();

protected:
    bool                          m_bActive;
    std::vector<CChatLineSection> m_Sections;
    unsigned long                 m_ulCreationTime;
};

class CChatInputLine : public CChatLine
{
public:
    void Draw(CVector2D& vecPosition, unsigned char ucAlpha, bool bShadow, bool bOutline);
    void Clear();

    CChatLineSection       m_Prefix;
    std::vector<CChatLine> m_ExtraLines;
};

//
// SDrawListLineItem
//
struct SDrawListLineItem
{
    uint      uiLine;
    CVector2D vecPosition;
    uchar     ucAlpha;

    bool operator!=(const SDrawListLineItem& other) const { return !operator==(other); }
    bool operator==(const SDrawListLineItem& other) const { return uiLine == other.uiLine && vecPosition == other.vecPosition && ucAlpha == other.ucAlpha; }
};

//
// SDrawList - Used to store a snapshot of what the chatbox is currently rendering
//
struct SDrawList
{
    CRect2D                        renderBounds;
    bool                           bShadow;
    bool                           bOutline;
    std::vector<SDrawListLineItem> lineItemList;

    bool operator!=(const SDrawList& other) const { return !operator==(other); }
    bool operator==(const SDrawList& other) const
    {
        if (lineItemList.size() != other.lineItemList.size() || bShadow != other.bShadow || bOutline != other.bOutline || renderBounds != other.renderBounds)
            return false;

        for (uint i = 0; i < lineItemList.size(); i++)
            if (lineItemList[i] != other.lineItemList[i])
                return false;

        return true;
    }
};

class CChat
{
    friend class CChatLine;
    friend class CChatInputLine;
    friend class CChatLineSection;

public:
    CChat(){};
    CChat(CGUI* pManager, const CVector2D& vecPosition);
    virtual ~CChat();

    virtual void Draw(bool bUseCacheTexture, bool bAllowOutline);
    virtual void Output(const char* szText, bool bColorCoded = true);
    void         Clear();
    void         ClearInput();
    bool         CharacterKeyHandler(CGUIKeyEventArgs KeyboardArgs);
    void         SetDxFont(LPD3DXFONT pDXFont);

    bool IsVisible() { return m_bVisible; }
    void SetVisible(bool bVisible);
    bool IsInputVisible() { return m_bVisible && m_bInputVisible; }
    void SetInputVisible(bool bVisible);

    const char* GetInputPrefix();
    void        SetInputPrefix(const char* szPrefix);
    const char* GetInputText() { return m_strInputText.c_str(); }
    void        SetInputText(const char* szText);
    const char* GetCommand() { return m_strCommand.c_str(); }
    void        SetCommand(const char* szCommand);
    CVector2D   CalcInputSize();

    static float GetFontHeight(float fScale = 1.0f);
    static float GetTextExtent(const char* szText, float fScale = 1.0f);
    static void  DrawTextString(const char* szText, CRect2D DrawArea, float fZ, CRect2D ClipRect, unsigned long ulFormat, unsigned long ulColor, float fScaleX,
                                float fScaleY, bool bOutline, const CRect2D& RenderBounds);

    void SetColor(const SColorRGBA& Color);
    void SetInputColor(const SColorRGBA& Color);
    void SetTextColor(const SColorRGBA& Color) { m_TextColor = Color; };
    void SetNumLines(unsigned int uiNumLines);

    void Scroll(int iState) { m_iScrollState = iState; };
    void ScrollUp();
    void ScrollDown();

    void SetChatFont(eChatFont Font);
    void OnModLoad();

private:
    void LoadCVars();

protected:
    virtual void UpdatePosition();

    void UpdateGUI();
    void UpdateSmoothScroll(float* pfPixelScroll, int* piLineScroll);
    void DrawDrawList(const SDrawList& drawList, const CVector2D& topLeftOffset = CVector2D(0, 0));
    void GetDrawList(SDrawList& outDrawList, bool bUsingOutline);
    void DrawInputLine(bool bUsingOutline);

    CChatLine      m_Lines[CHAT_MAX_LINES];            // Circular buffer
    int            m_iScrollState;                     // 1 up, 0 stop, -1 down
    unsigned int   m_uiMostRecentLine;
    unsigned int   m_uiScrollOffset;
    float          m_fSmoothScroll;
    float          m_fSmoothLastTimeSeconds;
    float          m_fSmoothAllowAfter;
    float          m_fSmoothScrollResetTime;
    float          m_fSmoothRepeatTimer;
    CChatInputLine m_InputLine;
    SString        m_strLastPlayerNamePart;
    SString        m_strLastPlayerName;

    float                   m_fPositionOffsetX;
    float                   m_fPositionOffsetY;
    eChatPositionHorizontal m_ePositionHorizontal;
    eChatPositionVertical   m_ePositionVertical;
    eChatTextAlign          m_eTextAlign;

    CGUI*      m_pManager;
    CGUIFont*  m_pFont;
    LPD3DXFONT m_pDXFont;

    CVector2D m_vecBackgroundPosition;
    CVector2D m_vecBackgroundSize;
    CVector2D m_vecInputPosition;
    CVector2D m_vecInputSize;

    CGUITexture*     m_pBackgroundTexture;
    CGUITexture*     m_pInputTexture;
    CGUIStaticImage* m_pBackground;
    CGUIStaticImage* m_pInput;

    std::string m_strInputText;
    std::string m_strCommand;

    bool  m_bVisible;
    bool  m_bInputVisible;
    int   m_iScrollingBack;                    // Non zero if currently scrolling back
    float m_fCssStyleOverrideAlpha;            // For fading out 'CssStyle' effect. (When entering text or scrolling back)
    float m_fBackgroundAlpha;
    float m_fInputBackgroundAlpha;

    unsigned int  m_uiNumLines;
    SColorRGBA    m_Color;
    SColorRGBA    m_TextColor;
    SColorRGBA    m_InputColor;
    SColorRGBA    m_InputTextColor;
    bool          m_bCssStyleText;
    bool          m_bCssStyleBackground;
    bool          m_bTextBlackOutline;
    unsigned long m_ulChatLineLife;
    unsigned long m_ulChatLineFadeOut;
    bool          m_bUseCEGUI;
    CVector2D     m_vecScale;
    float         m_fNativeWidth;
    float         m_fRcpUsingDxFontScale;

    bool m_bCanChangeWidth;
    int  m_iCVarsRevision;

    SDrawList          m_PrevDrawList;
    CRenderTargetItem* m_pCacheTexture;
    int                m_iCacheTextureRevision;

    CVector2D  m_RenderTargetChatSize;
    int        m_iReportCount;
    CTickCount m_lastRenderTargetCreationFail;

    bool m_bNickCompletion;
};
