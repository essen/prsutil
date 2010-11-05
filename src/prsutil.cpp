#include <windows.h>
#include <stdio.h>
#include "prs.cpp"

int __stdcall WinMain(HINSTANCE,HINSTANCE,char* srcname,int)
{
    char destname[MAX_PATH];
    unsigned long size,size2,bytesread;
    bool compress = false,quest = false;
    if (srcname[0] == '*')
    {
        compress = true;
        srcname++;
    }
    strcpy(destname,srcname);
    if (compress) strcat(destname,".prs");
    else strcat(destname,".bin");

    printf("> fuzziqer software prs [de]compressor\n> source file: %s\n> dest file: %s\n\n",srcname,destname);
    HANDLE src = CreateFile(srcname,GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL);
    if (src == INVALID_HANDLE_VALUE)
    {
        printf("> could not open source file\n");
        return 0;
    }
    size = GetFileSize(src,NULL);
    void* srcdata = malloc(size);
    if (!srcdata)
    {
        CloseHandle(src);
        printf("> not enough free memory to load source data\n");
        return 0;
    }
    printf("> loading %d bytes\n",size);
    ReadFile(src,srcdata,size,&bytesread,NULL);
    CloseHandle(src);
    if (bytesread != size)
    {
        free(srcdata);
        printf("> could not read source data\n");
        return 0;
    }

    void* destdata = malloc(compress ? size : prs_decompress_size(srcdata));
    if (!destdata)
    {
        free(srcdata);
        printf("> not enough free memory to process data\n");
        return 0;
    }

    unsigned long start,end;
    start = GetTickCount();
    if (compress) size2 = prs_compress(srcdata,destdata,size);
    else size2 = prs_decompress(srcdata,destdata);
    end = GetTickCount();
    start = end - start;
    end = (start / 1000);
    printf("> total time %d.%03d seconds\n",end,start - (end * 1000));

    HANDLE dest = CreateFile(destname,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);
    if (dest == INVALID_HANDLE_VALUE)
    {
        CloseHandle(src);
        printf("> could not open dest file\n");
        return 0;
    }
    printf("> saving %d bytes\n",size2);
    WriteFile(dest,destdata,size2,&bytesread,NULL);
    CloseHandle(dest);
    if (bytesread != size2)
    {
        free(srcdata);
        free(destdata);
        printf("> could not write dest data\n");
        return 0;
    }
    printf("> done.\n",size,size2);
    free(srcdata);
    free(destdata);
    return 0;
}

