#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"
#include "Beryll/Core/Log.h"

namespace Beryll
{
    class File
    {
    public:
        File() = delete;

        // This fn() call new[] !!! You must call delete[] for free buffer.
        // return buffer + '\0'. size will contains number bytes without '\0'
        static char* readToBuffer(const char* filepath, uint32_t* size = nullptr)
        {
            SDL_RWops *rw = SDL_RWFromFile(filepath, "rb"); // read binary
            BR_ASSERT((rw != nullptr), "File loading error:{0}", filepath);

            uint32_t resSize = SDL_RWsize(rw);
            BR_ASSERT((resSize != 0), "File loading size = 0:{0}", filepath);
            char* res = new char[resSize + 1];

            uint32_t  readTotal = 0, read = 1;
            char* buf = res; // because we will move buf pointer and keep res to point first element
            while (readTotal < resSize && read != 0)
            {
                read = SDL_RWread(rw, buf, 1, (resSize - readTotal));
                readTotal += read;
                buf += read;
            }

            SDL_RWclose(rw);

            if (readTotal != resSize)
            {
                delete[] res;
                BR_ASSERT(false, "readTotal != resSize after loading {0}", filepath);
            }

            res[resSize] = '\0';
            if(size) { *size = resSize; }

            return res;
        }
    };
}