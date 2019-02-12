/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CKeyBinds.h
 *  PURPOSE:     Header file for core keybind manager class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

class CKeyBinds;

#pragma once

#include <windows.h>
#include <string.h>
#include <list>

#include <core/CCoreInterface.h>
#include <core/CCommandsInterface.h>

struct SDefaultCommandBind
{
    char szKey[20];
    bool bState;
    char szCommand[20];
    char szArguments[20];
};

class CKeyBinds : public CKeyBindsInterface
{
public:
    CKeyBinds(class CCore* pCore);
    ~CKeyBinds();

    bool ProcessMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    bool ProcessCharacter(WPARAM wChar);
    bool ProcessKeyStroke(const SBindableKey* pKey, bool bState);

public:
    // Basic funcs
    void Add(CKeyBind* pKeyBind);
    void Remove(CKeyBind* pKeyBind);
    void Clear();
    void RemoveDeletedBinds();
    void ClearCommandsAndControls();
    bool Call(CKeyBind* pKeyBind);

    std::list<CKeyBind*>::const_iterator IterBegin() { return m_pList->begin(); }
    std::list<CKeyBind*>::const_iterator IterEnd() { return m_pList->end(); }

    // Command-bind funcs
    bool AddCommand(const char* szKey, const char* szCommand, const char* szArguments, bool bState, const char* szResource = NULL, bool bScriptCreated = false,
                    const char* szOriginalScriptKey = NULL);
    bool AddCommand(const SBindableKey* pKey, const char* szCommand, const char* szArguments = NULL, bool bState = true);
    bool RemoveCommand(const char* szKey, const char* szCommand, bool bCheckState = false, bool bState = true);
    bool RemoveAllCommands(const char* szKey, bool bCheckState = false, bool bState = true);
    bool RemoveAllCommands();
    bool CommandExists(const char* szKey, const char* szCommand, bool bCheckState = false, bool bState = true, const char* szArguments = NULL,
                       const char* szResource = NULL, bool bCheckScriptCreated = false, bool bScriptCreated = false);
    bool SetCommandActive(const char* szKey, const char* szCommand, bool bState, const char* szArguments, const char* szResource, bool bActive,
                          bool checkHitState, bool bConsiderDefaultKey = false);
    void SetAllCommandsActive(const char* szResource, bool bActive, const char* szCommand = NULL, bool bState = true, const char* szArguments = NULL,
                              bool checkHitState = false, const char* szOnlyWithDefaultKey = nullptr);
    CCommandBind* GetBindFromCommand(const char* szCommand, const char* szArguments = NULL, bool bMatchCase = true, const char* szKey = NULL,
                                     bool bCheckHitState = false, bool bState = NULL);
    bool          GetBoundCommands(const char* szCommand, std::list<CCommandBind*>& commandsList);
    void          UserChangeCommandBoundKey(CCommandBind* pBind, const SBindableKey* pNewBoundKey);
    void          UserRemoveCommandBoundKey(CCommandBind* pBind);
    CCommandBind* FindMatchingUpBind(CCommandBind* pBind);
    CCommandBind* FindCommandMatch(const char* szKey, const char* szCommand, const char* szArguments, const char* szResource, const char* szOriginalScriptKey,
                                   bool bCheckState, bool bState, bool bCheckScriptCreated, bool bScriptCreated);
    void          SortCommandBinds();

    // Control-bind funcs
    bool AddGTAControl(const char* szKey, const char* szControl);
    bool AddGTAControl(const SBindableKey* pKey, SBindableGTAControl* pControl);
    bool RemoveGTAControl(const char* szKey, const char* szControl);
    bool RemoveAllGTAControls(const char* szKey);
    bool RemoveAllGTAControls();
    bool GTAControlExists(const char* szKey, const char* szControl);
    bool GTAControlExists(const SBindableKey* pKey, SBindableGTAControl* pControl);
    void CallGTAControlBind(CGTAControlBind* pBind, bool bState);
    void CallAllGTAControlBinds(eControlType controlType, bool bState);
    bool GetBoundControls(SBindableGTAControl* pControl, std::list<CGTAControlBind*>& controlsList);

    // Control-bind extra funcs
    bool GetMultiGTAControlState(CGTAControlBind* pBind);
    bool IsControlEnabled(const char* szControl);
    bool SetControlEnabled(const char* szControl, bool bEnabled);
    void SetAllControlsEnabled(bool bGameControls, bool bMTAControls, bool bEnabled);
    void ResetGTAControlState(SBindableGTAControl* pControl);
    void ResetAllGTAControlStates();

    // Function-bind funcs
    bool AddFunction(const char* szKey, KeyFunctionBindHandler Handler, bool bState = true, bool bIgnoreGUI = false);
    bool AddFunction(const SBindableKey* pKey, KeyFunctionBindHandler Handler, bool bState = true, bool bIgnoreGUI = false);
    bool RemoveFunction(const char* szKey, KeyFunctionBindHandler Handler, bool bCheckState = false, bool bState = true);
    bool RemoveFunction(const SBindableKey* pKey, KeyFunctionBindHandler Handler, bool bCheckState = false, bool bState = true);
    bool RemoveAllFunctions(KeyFunctionBindHandler Handler);
    bool RemoveAllFunctions();
    bool FunctionExists(const char* szKey, KeyFunctionBindHandler Handler, bool bCheckState = false, bool bState = true);
    bool FunctionExists(const SBindableKey* pKey, KeyFunctionBindHandler Handler, bool bCheckState = false, bool bState = true);

    // Function-control-bind funcs
    bool AddControlFunction(const char* szControl, ControlFunctionBindHandler Handler, bool bState = true);
    bool AddControlFunction(SBindableGTAControl* pControl, ControlFunctionBindHandler Handler, bool bState = true);
    bool RemoveControlFunction(const char* szControl, ControlFunctionBindHandler Handler, bool bCheckState = false, bool bState = true);
    bool RemoveControlFunction(SBindableGTAControl* pControl, ControlFunctionBindHandler Handler, bool bCheckState = false, bool bState = true);
    bool RemoveAllControlFunctions(ControlFunctionBindHandler Handler);
    bool RemoveAllControlFunctions();
    bool ControlFunctionExists(const char* szControl, ControlFunctionBindHandler Handler, bool bCheckState = false, bool bState = true);
    bool ControlFunctionExists(SBindableGTAControl* pControl, ControlFunctionBindHandler Handler, bool bCheckState = false, bool bState = true);

    // Key/code funcs
    const SBindableKey* GetBindableFromKey(const char* szKey);
    const SBindableKey* GetBindableFromGTARelative(int iGTAKey);
    bool                IsKey(const char* szKey);
    const SBindableKey* GetBindableFromMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bState);
    void                SetKeyStrokeHandler(KeyStrokeHandler Handler) { m_KeyStrokeHandler = Handler; }
    void                SetCharacterKeyHandler(CharacterKeyHandler Handler) { m_CharacterKeyHandler = Handler; }

    // Control/action funcs
    SBindableGTAControl* GetBindableFromControl(const char* szControl);
    SBindableGTAControl* GetBindableFromAction(eControllerAction action);
    bool                 IsControl(const char* szControl);
    void                 SetAllControls(bool bState);
    void                 SetAllFootControls(bool bState);
    void                 SetAllVehicleControls(bool bState);

    void SetAllBindStates(bool bState, eKeyBindType onlyType = KEY_BIND_UNDEFINED);

    unsigned int Count(eKeyBindType bindType = KEY_BIND_UNDEFINED);

    void DoPreFramePulse();
    void DoPostFramePulse();

    bool LoadFromXML(CXMLNode* pMainNode);
    bool SaveToXML(CXMLNode* pMainNode);
    void LoadDefaultBinds();
    void LoadDefaultCommands(bool bForce);
    void LoadControlsFromGTA();

    void BindCommand(const char* szCmdLine);
    void UnbindCommand(const char* szCmdLine);
    void PrintBindsCommand(const char* szCmdLine);

    static bool IsFakeCtrl_L(UINT message, WPARAM wParam, LPARAM lParam);

    bool TriggerKeyStrokeHandler(const SString& strKey, bool bState, bool bIsConsoleInputKey);

private:
    CCore* m_pCore;

    std::list<CKeyBind*>* m_pList;
    bool                  m_bMouseWheel;
    bool                  m_bInVehicle;
    CCommandBind*         m_pChatBoxBind;
    bool                  m_bProcessingKeyStroke;
    KeyStrokeHandler      m_KeyStrokeHandler;
    CharacterKeyHandler   m_CharacterKeyHandler;
    bool                  m_bWaitingToLoadDefaults;
};
