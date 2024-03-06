// PortScanner.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <vector>


#define NumOfPorts 10000

#pragma comment(lib, "Ws2_32.lib")

bool isPortOpen(const std::string& host, int port) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed." << std::endl;
        return false;
    }

    SOCKET socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//AF_INET - basic address family, SOCK_STREAM - for TCP connection
    if (socket_fd == 0) {
        std::cerr << "Error creating socket." << std::endl;
        WSACleanup();
        return false;
    }

    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &server_address.sin_addr);

    if (connect(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) == 0) {
        closesocket(socket_fd);
        WSACleanup();
        return true;
    }
    else {
        closesocket(socket_fd);
        WSACleanup();
        return false;
    }
}

void portScanner(const std::string& host, int port) {
    if (isPortOpen(host, port)) {
        std::cout << "Port " << port << " on " << host << " is open." << std::endl;
    }
}

int main() {
    std::string host = "127.0.0.1";
    std::vector<std::thread> threads;


    //unlocking port 1234 with the sequence:
    threads.emplace_back(portScanner, host, 1111);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    threads.emplace_back(portScanner, host, 3333);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    threads.emplace_back(portScanner, host, 2222);

    //regular scanning
    for (int i = 0; i < NumOfPorts; i++) {
        threads.emplace_back(portScanner, host, i);
    }

    // Join all threads to wait for them to finish
    for (auto& thread : threads) {
        thread.join();
    }

    return 0;
}
