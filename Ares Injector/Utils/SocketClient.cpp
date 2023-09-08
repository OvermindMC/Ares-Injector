#include "SocketClient.h"

SocketClient::SocketClient(std::string ip, int port) {

    this->serverAddr.sin_family = AF_INET;
    this->serverAddr.sin_port = htons(port);
    this->serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());

    this->tryConnect();

};

auto SocketClient::tryConnect(void) -> bool {

    if (WSAStartup(MAKEWORD(2, 2), &this->wsaData) == 0) {

        this->clientSocket = socket(AF_INET, SOCK_STREAM, 0);

        if (this->clientSocket != INVALID_SOCKET) {

            if (connect(this->clientSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) != SOCKET_ERROR) {

                return true;

            }
            else {

                closesocket(this->clientSocket);
                this->clientSocket = INVALID_SOCKET;
                WSACleanup();
                return false;

            };

        }
        else {

            WSACleanup();
            return false;

        };
    };

    return false;

};

auto SocketClient::isConnected(void) -> bool {

    if (this->clientSocket == INVALID_SOCKET)
        return false;

    int error = 0;
    int errorSize = sizeof(error);
    if (getsockopt(clientSocket, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&error), &errorSize) == 0) {
        return error == 0;
    }
    else {
        return false;
    };

};

auto SocketClient::sendData(json& jsonObject) -> json {

    json result;
    result["success"] = false;

    if (!this->isConnected()) {
        return result;
    };

    std::string message = jsonObject.dump();
    int bytesSent = send(clientSocket, message.c_str(), static_cast<int>(message.length()), 0);

    if (bytesSent == SOCKET_ERROR)
        std::cout << "Failed to send message via socket" << std::endl;
    else
        result = this->getFromRes();

    return result;

};

auto SocketClient::getFromRes(void) -> json {

    json result;
    result["success"] = false;

    if (!this->isConnected())
        return result;

    char buffer[1024];
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

    if (bytesReceived > 0) {

        try {
            std::string receivedData(buffer, bytesReceived);
            result = result.parse(receivedData);
        }
        catch (const std::exception& e) {};

    };

    return result;

};

auto SocketClient::disconnect(void) -> void {

    if (!this->isConnected())
        return;

    closesocket(this->clientSocket);
    this->clientSocket = INVALID_SOCKET;
    WSACleanup();

};

auto SocketClient::getUser(void) -> json {

    json userObj;
    auto token = Utils::getToken();
    
    json request = {
        {"token", token},
        {"type", "version"}
    };

    auto result = this->sendData(request);
    
    if (result["success"]) {

        auto version = (
            result["data"].is_object() && result["data"]["version"].is_string() ?
            result["data"]["version"] : ""
        );

        request = {
            {"token", token},
            {"type", "login"},
            {"version", version}
        };

        result = this->sendData(request);
        
        if (result["success"])
            userObj = result["data"];

    };

    return userObj;

};