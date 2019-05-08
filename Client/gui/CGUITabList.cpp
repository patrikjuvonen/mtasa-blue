/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        gui/CGUITabList.cpp
 *  PURPOSE:     Tab-able elements list class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"

void CGUITabList::SelectNext(CGUITabListItem* pBase)
{
    // Loop through all elements which can be activated
    bool bFound = false;
    for (auto iter : m_Items)
    {
        if (iter == pBase)
            bFound = true;
        else if (bFound && iter->IsEnabled())
        {
            // we found an element that wants to get selected
            iter->ActivateOnTab();
            return;
        }
    }

    // Contine to search an element from the beginning
    for (auto iter : m_Items)
    {
        if (iter == pBase)
            // just where we started, so we don't have to do anything
            return;
        else if (iter->IsEnabled())
        {
            // finally found something different than the current element
            iter->ActivateOnTab();
            return;
        }
    }
}
