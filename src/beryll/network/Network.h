#pragma once

#include "LibsHeaders.h"
#include "CppHeaders.h"

namespace Beryll
{
    class Network
    {
    public:
        Network() = delete;
        ~Network() = delete;

        static void create();

        static void sendTextToIPAndPort(const std::string IP, const unsigned short port, const std::string text);

    private:
        static std::thread m_threadToSendText;
    };
}
