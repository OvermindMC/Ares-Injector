#pragma once

#include "Utils.h"

class SocketClient {
public:
public:
    WSADATA wsaData;
    SOCKET clientSocket;
    sockaddr_in serverAddr;
public:
    SocketClient(std::string, int);
public:
    auto tryConnect(void) -> bool;
    auto isConnected(void) -> bool;
public:
    auto sendData(json&) -> json;
    auto getFromRes(void) -> json;
public:
    auto disconnect(void) -> void;
public:
    auto getUser(void) -> json;
};