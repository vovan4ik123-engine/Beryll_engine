#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

#include "beryll/core/Log.h"

namespace BeryllUtils
{
    class File
    {
    public:
        File() = delete;
        ~File() = delete;

        // This fn() call new[] !!! You must call delete[] for free buffer.
        // Returns buffer + '\0'. size will contains number of bytes without '\0'.
        static char* readToBuffer(const char* filepath, uint32_t* size = nullptr)
        {
            SDL_RWops *rw = SDL_RWFromFile(filepath, "rb"); // Read binary.
            BR_ASSERT((rw != nullptr), "File reading error: %s", filepath);

            uint32_t resSize = SDL_RWsize(rw);
            BR_ASSERT((resSize != 0), "File size = 0: %s", filepath);
            char* res = new char[resSize + 1];

            uint32_t  readTotal = 0, read = 1;
            char* buf = res; // Because we will move buf pointer and keep res to point first element.
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
                BR_ASSERT(false, "readTotal != resSize after loading: %s", filepath);
            }

            res[resSize] = '\0';
            if(size) { *size = resSize; }

            return res;
        }
    };
}
