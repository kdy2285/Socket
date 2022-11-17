#include <iostream>
#include <WS2tcpip.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main()
{
	const char* serverIp = "127.0.0.1";

	WSADATA wsaData;
	int initResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (initResult != 0)
	{
		cerr << "Can't initialize winsock!! Quitting" << endl;
		return -1;
	}

	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSocket == INVALID_SOCKET)
	{
		cerr << "Can't create socket!! Quitting" << endl;
		WSACleanup();
		return -1;
	}

	sockaddr_in hints;
	ZeroMemory(&hints, sizeof(hints));
	hints.sin_family = AF_INET;
	hints.sin_port = htons(54000);
	int convResult = inet_pton(AF_INET, serverIp, &hints.sin_addr);
	if (convResult != 1)
	{
		cerr << "Can't convert Ip address!! Quitting" << endl;
		WSACleanup();
		return -1;
	}

	int connectResult = connect(clientSocket, (sockaddr*)&hints, sizeof(hints));
	if (connectResult == SOCKET_ERROR)
	{
		cerr << "Can't connect to server!! Quitting" << endl;
		closesocket(clientSocket);
		WSACleanup();
		return -1;
	}

	const int BUF_SIZE = 4096;
	char recvBuf[BUF_SIZE];
	
	ZeroMemory(recvBuf, BUF_SIZE);
	string userMsg;
	
	do
	{
		cout << "input : ";
		getline(cin, userMsg);

		if (userMsg.size() > 0)
		{
			int sendResult = send(clientSocket, userMsg.c_str(), userMsg.size() + 1, 0);
			if (sendResult != SOCKET_ERROR)
			{
				ZeroMemory(recvBuf, BUF_SIZE);
				int bytesReceived = recv(clientSocket, recvBuf, BUF_SIZE, 0);
				if (bytesReceived > 0)
				{
					cout << "Server : " << recvBuf << endl;
				}
			}
		}
	} while (userMsg.size() > 0);
	

	closesocket(clientSocket);
	WSACleanup();


	return 0;
}