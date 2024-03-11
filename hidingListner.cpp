#include <iostream>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>


#pragma comment(lib, "ws2_32.lib")

#define IP "192.168"
#define PORT 1234

bool startsWith(const std::string& str, const std::string& prefix) {
    return str.substr(0, prefix.length()) == prefix;
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Failed to initialize Winsock\n";
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Error creating socket: " << WSAGetLastError() << "\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY; 
    serverAddress.sin_port = htons(PORT);       

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
        std::cerr << "Error binding socket: " << WSAGetLastError() << "\n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server bound to port "<< PORT <<"\n";


    char serverIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(serverAddress.sin_addr), serverIP, INET_ADDRSTRLEN);

    while (true) {
        if (startsWith(serverIP, IP) != 0) {// Black listing the computer ip
            if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
                std::cerr << "Error listening for connections: " << WSAGetLastError() << "\n";
                closesocket(serverSocket);
                WSACleanup();
                return 1;
            }

            while (true) {
                SOCKET clientSocket = accept(serverSocket, NULL, NULL);
                if (clientSocket == INVALID_SOCKET) {
                    std::cerr << "Error accepting connection: " << WSAGetLastError() << "\n";
                    closesocket(serverSocket);
                    WSACleanup();
                    return 1;
                }
                std::cerr << "Connection established" << "\n";
                closesocket(clientSocket);
            }
        }
    }
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
