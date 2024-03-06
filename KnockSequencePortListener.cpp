#include <iostream>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <atomic>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 1234
#define FIRSTKNOCK 1111
#define SECONDKNOCK 3333
#define THIRDKNOCK 2222

void listenOn(int port);
void knock(int port);
void handleClient(SOCKET clientSocket);

std::atomic<int> key(1); // key value is for each knock
SOCKET firstKnockSocket;
SOCKET secondKnockSocket;
SOCKET thirdKnockSocket;

void knock(int port) {
    if ((key.load() == 1 && port == FIRSTKNOCK) ||
        (key.load() == 2 && port == SECONDKNOCK) ||
        (key.load() == 3 && port == THIRDKNOCK)) {
        key++;
    }
    else {
        key.store(1); // Reset the key if the knock sequence is incorrect
    }

    if (key.load() == 4) {
        std::cout << "Server is revealed. Knock on ports 1234 to connect it." << std::endl;
        listenOn(PORT);
        closesocket(firstKnockSocket);
        closesocket(secondKnockSocket);
        closesocket(thirdKnockSocket);
    }
}

void handleClient(SOCKET clientSocket) {
    std::cout << "Connection established with client." << std::endl;
    closesocket(clientSocket);
}

void listenOn(int port) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Error creating socket." << std::endl;
        WSACleanup();
        return;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Bind failed." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "listenOn failed." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    while (true) {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Accept failed." << std::endl;
            closesocket(serverSocket);
            WSACleanup();
            return;
        }

        if (port != PORT) {
            std::thread(knock, port).detach();
        }
        else {
            std::thread(handleClient, clientSocket).detach();
        }
    }

    closesocket(serverSocket);
    WSACleanup();
}

int main() {
    firstKnockSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    secondKnockSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    thirdKnockSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    std::thread(listenOn, FIRSTKNOCK).detach();
    std::thread(listenOn, SECONDKNOCK).detach();
    std::thread(listenOn, THIRDKNOCK).detach();

    std::cout << "Server is hidden. Knock on ports 1111, 2222, 3333 to reveal it." << std::endl;

    std::this_thread::sleep_for(std::chrono::hours(1));

    return 0;
}
