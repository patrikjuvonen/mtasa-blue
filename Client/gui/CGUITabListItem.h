/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUITabListItem.h
 *  PURPOSE:     Tab-able Element Interface
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/
#pragma once

#include <gui/CGUI.h>

class CGUITabListItem : public CGUIElement
{
public:
    virtual bool ActivateOnTab() = 0;
};
