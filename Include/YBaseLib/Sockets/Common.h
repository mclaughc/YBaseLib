#pragma once
#include "YBaseLib/Common.h"

#ifdef Y_PLATFORM_WINDOWS
    #include "YBaseLib/Windows/WindowsHeaders.h"
    #include <WinSock2.h>
    #include <WS2tcpip.h>
#else
    #include <sys/socket.h>
    #include <sys/types.h>
#endif

// Implementation
#define Y_SOCKET_IMPLEMENTATION_GENERIC