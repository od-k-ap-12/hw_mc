#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <conio.h>
#include <string>
#include <iostream>
using namespace std;

int main()
{
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

    sockaddr_in addrTo;
    addrTo.sin_family = AF_INET;
    addrTo.sin_port = htons(23000);
    inet_pton(AF_INET, "127.0.0.1", &addrTo.sin_addr.s_addr);
    cout << "1.Order\n2.Exit\n";
    int Choose = _getch();
    char Order[200];
    switch (Choose) {
    case '1':
        cout << "Hamburger-5sec\nSprite-1sec\nFries-3sec\n";
        cout << "Enter your order: \n";
        cin.getline(Order,200);
        break;
    case '2':
        closesocket(udpSocket);
        WSACleanup();
        break;
    default:
        break;
    }

   cout << "Sending data...." << endl;
    int sendResult = sendto(udpSocket, Order, strlen(Order), 0, (SOCKADDR*)&addrTo, sizeof(addrTo));
    if (sendResult == SOCKET_ERROR)
    {
        cout << "sendto failed with error " << WSAGetLastError() << endl;
        return 4;
    }

    const size_t receiveBufSize = 1024;
    char receiveBuf[receiveBufSize];

    sockaddr_in addrFrom;
    int addrFromSize = sizeof(addrFrom);

    cout << "Receiving data...." << endl;
    int bytesReceived = recvfrom(udpSocket, receiveBuf, receiveBufSize, 0, (SOCKADDR*)&addrFrom, &addrFromSize);

    if (bytesReceived == SOCKET_ERROR)
    {
        cout << "recvfrom failed with error " << WSAGetLastError() << endl;
        return 4;
    }

    receiveBuf[bytesReceived] = '\0';
    cout << "Received from " << addrFrom.sin_addr.s_host << endl;
    cout << "Order status: " << receiveBuf << endl;

    cout << "Waiting for your order to be ready...." << endl;
    bytesReceived = recvfrom(udpSocket, receiveBuf, receiveBufSize, 0, (SOCKADDR*)&addrFrom, &addrFromSize);

    receiveBuf[bytesReceived] = '\0';
    cout << "Received from " << addrFrom.sin_addr.s_host << endl;
    cout << "Order status: " << receiveBuf << endl;

    closesocket(udpSocket);
    WSACleanup();
}
