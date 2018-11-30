#include <iostream>
#include <WS2tcpip.h>
#include <string>
#pragma comment (lib,"ws2_32.lib")

using namespace std;

void main()
{

	//Initialize win sock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &wsData);
	if (wsResult != 0)
	{
		cerr << "Can't initialize win sock ! Quitting"<< wsResult << endl;
		return;
	}

    //Create a socket
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		cerr << "Can't create a socket ! Quitting" << endl;
		return;
	}

	//Bind the ip address and port to a socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;//Could alos use inet_pton
	bind(listening, (sockaddr*)&hint, sizeof(hint));

	//Tell win sock the socket is for listening 
	listen(listening, SOMAXCONN);

	//Wait for a connection
	sockaddr_in client;
	int clientSize = sizeof(client);
	SOCKET clientSocket = accept(listening, (sockaddr*)&client, &clientSize);
	char host[NI_MAXHOST]; //Client's remote name
	char service[NI_MAXHOST]; //Service(ie. port) the client is connect on
	ZeroMemory(host, NI_MAXHOST); //same as memset(host,0,NI_MAXHOST);
	ZeroMemory(service, NI_MAXHOST);
	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXHOST, 0) == 0)
	{
		cout << host<<" connected on port "<< service << endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << "connected on port" << ntohs(client.sin_port) << endl;
	}

	//Close listening socket
	closesocket(listening);

	//While loop: accept and echo message back to client
	char buf[4096];
		while (true)
		{
			ZeroMemory(buf, 4096);
			// Wait for client to send data
	byte byteReceived = recv(clientSocket, buf, 4096, 0);
			if (byteReceived == SOCKET_ERROR)
			{
				cerr << "Error in recv().Quitting" << endl;
				break;
			}
			if (byteReceived==0)
			{
				cout << "Client disconnected" << endl;
				break;
			}
			//Echo message back to client
			cout<<string(buf, 0, byteReceived) << endl;
			send(clientSocket, " Message Received ", 17, 0);
		}

	//Close the socket
	closesocket(clientSocket);

	//cleanup win sock
	WSACleanup();
}