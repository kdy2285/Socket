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

	fd_set set;
	FD_ZERO(&set);
	FD_SET(listenSocket, &set);
	sockaddr_in clntAdr;
	int clntSize;

	while (true)
	{
		fd_set copy = set;

		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);
		clntSize = sizeof(clntAdr);
		for (int i = 0; i < socketCount; i++)
		{
			SOCKET socket = copy.fd_array[i];

			if (socket == listenSocket)
			{
				SOCKET client = accept(listenSocket, (sockaddr*)&clntAdr, &clntSize);
				FD_SET(client, &set);
				string msg = "Welcome!!\r\n";
				send(client, msg.c_str(), msg.size() + 1, 0);
			}
			else
			{
				char buf[4096];
				ZeroMemory(buf, 4096);

				int recvBytes = recv(socket, buf, 4096, 0);
				if (recvBytes <= 0)
				{
					closesocket(socket);
					FD_CLR(socket, &set);
				}
				else
				{
					for (int i = 0; i < set.fd_count; i++)
					{
						SOCKET outSocket = set.fd_array[i];
						if (outSocket != listenSocket && outSocket != socket)
						{
							send(outSocket, buf, recvBytes, 0);
						}
					}
				}
			}
		}
	}

	WSACleanup();

	return 0;
}