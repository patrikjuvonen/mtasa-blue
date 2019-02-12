/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        core/CTrayIcon.cpp
 *  PURPOSE:     Header file for the tray icon class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include <core/CTrayIconInterface.h>
#include <ShellAPI.h>

class CTrayIcon : public CTrayIconInterface
{
public:
    CTrayIcon();
    ~CTrayIcon();
    bool DoesTrayIconExist() { return m_bTrayIconExists; };
    bool CreateTrayIcon();
    void DestroyTrayIcon();
    bool CreateTrayBallon(SString strText, eTrayIconType trayIconType, bool useSound);

private:
    NOTIFYICONDATAW* m_pNID;
    bool             m_bTrayIconExists;
    long long        m_llLastBalloonTime;
};
