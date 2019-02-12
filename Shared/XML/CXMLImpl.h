/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        xml/CXMLImpl.h
 *  PURPOSE:     XML handler class
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#pragma once

#include <xml/CXML.h>

class CXMLImpl : public CXML
{
public:
    CXMLImpl();
    virtual ~CXMLImpl();

    CXMLFile* CreateXML(const char* szFilename, bool bUseIDs, bool bReadOnly);
    void      DeleteXML(CXMLFile* pFile);

    CXMLNode* CreateDummyNode();

    CXMLAttribute* GetAttrFromID(unsigned long ulID);
    CXMLFile*      GetFileFromID(unsigned long ulID);
    CXMLNode*      GetNodeFromID(unsigned long ulID);
};
