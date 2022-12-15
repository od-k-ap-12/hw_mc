#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <vector>
#include <string>
#include <iostream>
using namespace std;

int main()
{
    int HamburgerTime = 5000;
    int SpriteTime = 1000;
    int FriesTime = 3000;
    int CookingTime = 0;
    static int Clients=0;
    int MaxClients=30;
    vector<sockaddr_in> ClientsAddr;
    vector<int> Orders;

    WSADATA wsadata;

    int res = WSAStartup(MAKEWORD(2, 2), &wsadata);
    if (res != NO_ERROR)
    {
        cout << "WSAStartup failked with error " << res << endl;
        return 1;
    }

    SOCKET udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udpSocket == INVALID_SOCKET)
    {
        cout << "socket failed with error " << WSAGetLastError() << endl;
        return 2;
    }

    while (Clients <= MaxClients) {
        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(23000);
        inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);

        if (bind(udpSocket, (SOCKADDR*)&addr, sizeof(addr)) != NO_ERROR)
        {
            cout << "bind failed with error " << WSAGetLastError() << endl;
            return 3;
        }

        const size_t receiveBufSize = 1024;
        char receiveBuf[receiveBufSize];

        sockaddr_in senderAddr;
        int senderAddrSize = sizeof(senderAddr);

        ClientsAddr.push_back(senderAddr);
        Clients++;


        cout << "Waiting for orders...." << endl;
        int bytesReceived = recvfrom(udpSocket, receiveBuf, receiveBufSize, 0, (SOCKADDR*)&senderAddr, &senderAddrSize);

        if (bytesReceived == SOCKET_ERROR)
        {
            cout << "recvfrom failed with error " << WSAGetLastError() << endl;
            return 4;
        }

        receiveBuf[bytesReceived] = '\0';
        cout << "Received from " << senderAddr.sin_addr.s_host << endl;
        cout << "Data: " << receiveBuf << endl;

        string Order = receiveBuf;
        size_t Found = Order.find("hamburger");
        if (Found != string::npos) {
            CookingTime += HamburgerTime;
        }
        Found = Order.find("sprite");
        if (Found != string::npos) {
            CookingTime += SpriteTime;
        }
        Found = Order.find("fries");
        if (Found != string::npos) {
            CookingTime += FriesTime;
        }
        Orders.push_back(CookingTime);

        const size_t sendBufSize = 2024;
        char sendBuf[sendBufSize] = "Cooking in progress...";

        int sendResult = sendto(udpSocket, sendBuf, strlen(sendBuf), 0, (SOCKADDR*)&senderAddr, senderAddrSize);
        if (sendResult == SOCKET_ERROR)
        {
            cout << "sendto failed with error " << WSAGetLastError() << endl;
            return 4;
        }

        Sleep(CookingTime);

        char sendBuf1[sendBufSize] = "Your order is ready!";

        vector<int>::iterator position = find(Orders.begin(), Orders.end(), CookingTime);
        if (position != Orders.end())
            Orders.erase(position);
        Clients--;
        sendResult = sendto(udpSocket, sendBuf1, strlen(sendBuf1), 0, (SOCKADDR*)&senderAddr, senderAddrSize);

    }
    if (Clients == 0) {
        closesocket(udpSocket);
        WSACleanup();
    }
}