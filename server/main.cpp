#include <iostream>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main()
{
	WSADATA wsaData;
	int initResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (initResult != 0)
	{
		cerr << "Can't initialize winsock!! Quitting" << endl;
		return -1;
	}

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
	{
		cerr << "Can't create a socket!! Quitting" << endl;
		WSACleanup();
		return -1;
	}

	sockaddr_in hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.sin_family = AF_INET;
	hints.sin_port = htons(54000);
	hints.sin_addr.S_un.S_addr = INADDR_ANY;

	int bindResult = bind(listenSocket, (sockaddr*)&hints, sizeof(hints));
	if (bindResult == SOCKET_ERROR)
	{
		cerr << "Can't bind a socket!! Quitting" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return -1;
	}

	int listenResult = listen(listenSocket, SOMAXCONN);
	if (listenResult == SOCKET_ERROR)
	{
		cerr << "Can't lsiten a socket!! Quitting" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return -1;
	}

	sockaddr_in clientSockInfo;
	int clientSockInfoSize = sizeof(clientSockInfo);
	ZeroMemory(&clientSockInfo, sizeof(clientSockInfo));

	SOCKET clientSocket = accept(listenSocket, (sockaddr*)&clientSockInfo, &clientSockInfoSize);

	cout << "Client connected" << endl;

	if (clientSocket == INVALID_SOCKET)
	{
		cerr << "Can't accept a socket!! Quitting" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return -1;
	}

	closesocket(listenSocket);

	const int BUF_SIZE = 4096;
	char buf[BUF_SIZE];

	while (true)
	{
		ZeroMemory(buf, BUF_SIZE);

		int bytesReceived = recv(clientSocket, buf, BUF_SIZE, 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			cerr << "Error in recv()!! Quitting" << endl;
			break;
		}

		if (bytesReceived == 0)
		{
			cout << "Client disconnected!!" << endl;
			break;
		}

		cout << buf << endl;
		send(clientSocket, buf, bytesReceived + 1, 0);
	}

	closesocket(clientSocket);
	WSACleanup();

	return 0;
}