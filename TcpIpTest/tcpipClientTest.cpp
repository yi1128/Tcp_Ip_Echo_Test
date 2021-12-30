#include <stdio.h>
#include <stdlib.h>
//#include <windows.h>
#include <winsock2.h>
#include <iostream>

// time check
#include <time.h>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment(lib, "ws2_32");

/**********************************/
/*           TCP define           */
/**********************************/
#define TCP_NON_TEST_MODE	0x11
#define TCP_COMM_TEST_MODE	0xFF

FILE* fp = fopen("write.txt", "w");


/**********************************/
/*           TCP State            */
/**********************************/
enum TCP_STATE
{
	NET_STATE_READY = 0,
	NET_STATE_INIT_COMPLETE,
	NET_STATE_IP_CONNECT,
	NET_STATE_START,
	NET_STATE_STOP
};

/**********************************/
/*         TCP Error Code         */
/**********************************/
enum NET_ERROR_CODE
{
	NET_ERROR_NONE = 0,
	NET_ERROR_INIT,
	NET_ERROR_RECV,
	NET_ERROR_SEND,
	NET_ERROR_TIMEOUT,
	NET_ERROR_CONNECT_FAIL
};

typedef struct
{
	uint8_t ip1;
	uint8_t ip2;
	uint8_t ip3;
	uint8_t ip4;
}IP_ADDRESS;

/**********************************/
/*      TCP Client Structure      */
/**********************************/
#pragma pack(push, 1)
typedef struct
{
	uint8_t head;	// 0xAE
	uint8_t type;	// 0:REQ, 1:RESP
	int time_stamp;
	double send_time_stamp;
	double recv_time_stamp;
	IP_ADDRESS ipAddress;
	NET_ERROR_CODE error_code;
	uint32_t count;
	int port;
	uint8_t tail;	// 0xEA
}TCP_COMM_TEST_SET;
#pragma pack(pop)

void ErrorHandling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

int main()
{
	clock_t start_time, send_end_time, recv_end_time;
	start_time = clock();

	int time_stamp = 0;
	uint32_t count = 0;
	TCP_COMM_TEST_SET sendData;
	TCP_COMM_TEST_SET recvData;


	std::string ipAddress = "192.168.0.117"; //"127.0.0.1";
	int port = 7;
	WSADATA wsaData;
	SOCKET hSocket;
	SOCKADDR_IN servAddr;

	char message[30];
	int strLen;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		std::cerr << "WSASTartup() error!" << std::endl;

	std::cerr << "WSASTartup()!" << std::endl;
	hSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
	{
		std::cerr << "socket() error" << std::endl;
		return -1;
	}
	std::cerr << "socket()!" << std::endl;

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(ipAddress.c_str());
	servAddr.sin_port = htons(port);

	if (connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR)
	{
		std::cerr << "connect() error!" << std::endl;
		return -1;
	}
	std::cerr << "Connect Complete!!" << std::endl;
	
	int sendErrCount = 0;
	int recvErrCount = 0;
	
	// Initial
	sendData.send_time_stamp = (double)(0.0);
	sendData.recv_time_stamp = (double)(0.0);

	for (;;)
	{
		sendData.head = 0xAE;
		sendData.type = 0;
		sendData.time_stamp = time_stamp;
		sendData.ipAddress.ip1 = 192;
		sendData.ipAddress.ip2 = 168;
		sendData.ipAddress.ip3 = 0;
		sendData.ipAddress.ip4 = 1;
		sendData.error_code = NET_ERROR_NONE;
		
		sendData.count = count;
		sendData.port = port;
		sendData.tail = 0xEA;
		time_stamp++;
		count++;
		if (count > 1000)
			count = count % 1000;

		int sendResult = send(hSocket, (char *)&sendData, sizeof(TCP_COMM_TEST_SET), 0);
		if (sendResult == -1)
		{
			sendErrCount++;
		}
		send_end_time = clock();
		sendData.send_time_stamp = (double)(send_end_time - start_time);
		strLen = recv(hSocket, (char*)&recvData, sizeof(TCP_COMM_TEST_SET), 0);
		
		recv_end_time = clock();
		sendData.recv_time_stamp = (double)(recv_end_time - start_time);
		if (strLen == -1)
		{
			recvErrCount++;
			/*
			std::cerr << "read() Error" << std::endl;

			std::cout << "---------------------------------" << std::endl;
			std::cout << "TCP/IP COMMUNICATION SEND DATA" << std::endl;
			std::cout << "---------------------------------" << std::endl;
			std::cout << "head : " << (int)sendData.head << std::endl;
			std::cout << "type : " << (int)sendData.type << std::endl;
			std::cout << "time stamp : " << sendData.time_stamp << std::endl;
			std::cout << "ipAddress : " << (int)sendData.ipAddress.ip1 << "." << (int)sendData.ipAddress.ip2 << "."
				<< (int)sendData.ipAddress.ip3 << "." << (int)sendData.ipAddress.ip4 << std::endl;
			std::cout << "error code : " << (int)sendData.error_code << std::endl;
			std::cout << "count : " << sendData.count << std::endl;
			std::cout << "port : " << (int)sendData.port << std::endl;
			std::cout << "tail : " << (int)sendData.tail << std::endl;
			std::cout << "---------------------------------" << std::endl;


			std::cout << "---------------------------------" << std::endl;
			std::cout << "TCP/IP COMMUNICATION RECEIVE DATA" << std::endl;
			std::cout << "---------------------------------" << std::endl;
			std::cout << "head : " << (int)recvData.head << std::endl;
			std::cout << "type : " << (int)recvData.type << std::endl;
			std::cout << "time stamp : " << recvData.time_stamp << std::endl;
			std::cout << "ipAddress : " << (int)recvData.ipAddress.ip1 << "." << (int)recvData.ipAddress.ip2 << "."
				<< (int)recvData.ipAddress.ip3 << "." << (int)recvData.ipAddress.ip4 << std::endl;
			std::cout << "error code : " << (int)recvData.error_code << std::endl;
			std::cout << "count : " << recvData.count << std::endl;
			std::cout << "port : " << (int)recvData.port << std::endl;
			std::cout << "tail : " << (int)recvData.tail << std::endl;
			std::cout << "---------------------------------" << std::endl;
			*/
			std::cout << "count : " << recvData.time_stamp << std::endl;
			break;
		}
		fprintf(fp, "%d\t%d\t\t\t%d\t%d\t\t\t%f\t%f\t%f\n", sendErrCount, recvErrCount, sendData.time_stamp, recvData.time_stamp,
			sendData.send_time_stamp, sendData.recv_time_stamp, sendData.recv_time_stamp - sendData.send_time_stamp);
		//printf("----------------------\n");
		//printf("sendErrCount : %d\n", sendErrCount);
		//printf("recvErrCount : %d\n", recvErrCount);
		//printf("----------------------\n\n\n");
	}
	closesocket(hSocket);
	WSACleanup();
	fclose(fp);
	return 0;
}
