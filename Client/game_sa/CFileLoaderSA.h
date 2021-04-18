#pragma once

struct SRelatedModelInfo
{
    RpClump* pClump;
    bool bDeleteOldRwObject;
};

struct SFileObjectInstance
{
    CVector     position;
    CVector4D   rotation;
    int         modelID;
    int         interiorID;
    int         lod;
};

class CFileLoaderSA
{
public:
    CFileLoaderSA();
    ~CFileLoaderSA();

    static void StaticSetHooks();
};

bool CFileLoader_LoadAtomicFile(RwStream *stream, unsigned int modelId);
RpAtomic* CFileLoader_SetRelatedModelInfoCB(RpAtomic* atomic, SRelatedModelInfo* pRelatedModelInfo);
