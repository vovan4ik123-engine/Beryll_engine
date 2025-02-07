#include "Network.h"

#include "beryll/core/Log.h"

namespace Beryll
{
    std::thread Network::m_threadToSendText;

    void Network::create()
    {
        if(!SDLNet_Init())
        {
            BR_ASSERT(false, "SDLNet_Init() error: %s", SDL_GetError());
        }

        //SDLNet_Quit();
    }

    void Network::sendTextToIPAndPort(const std::string IP, const unsigned short port, const std::string text)
    {
        BR_ASSERT(false, "%s", "Need new implementation after migrate to SDL3.")

//        if(IP.empty() || text.empty()) { return; }
//
//        m_threadToSendText = std::thread([IP, port, text](){
//
//            IPaddress desiredIP;
//
//            SDLNet_ResolveHost(&desiredIP, IP.c_str(), port);
//
//            if(desiredIP.host == INADDR_NONE)
//            {
//                BR_ERROR("Can not resolve IP. error: %s", SDLNet_GetError());
//            }
//            else
//            {
//                BR_INFO("%s", "IP resolved.");
//
//                TCPsocket tcpSocket = SDLNet_TCP_Open(&desiredIP);
//
//                if(!tcpSocket)
//                {
//                    BR_ERROR("Can not connect. error: %s", SDLNet_GetError());
//                }
//                else
//                {
//                    BR_INFO("%s", "Socket connected.");
//                    // We connected. Send data.
//                    int countBytesToSend = text.size();
//                    int countBytesActuallySent = SDLNet_TCP_Send(tcpSocket, text.c_str(), countBytesToSend);
//
//                    if(countBytesActuallySent < countBytesToSend)
//                    {
//                        // Some error happened and we sent less than should.
//                        // Try one more time.
//                        countBytesActuallySent = SDLNet_TCP_Send(tcpSocket, text.c_str(), countBytesToSend);
//                    }
//
//                    BR_INFO("sendTextToIPAndPort() sent %d bytes.", countBytesActuallySent);
//                    // Two attempts should be enough.
//                    SDLNet_TCP_Close(tcpSocket);
//                }
//            }
//
//            BR_INFO("%s", "m_threadToSendText finished.");
//        });
//
//        m_threadToSendText.detach();
//
//        BR_INFO("%s", "sendTextToIPAndPort return.");
    }
}
