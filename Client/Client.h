#pragma once
// my_Client.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include<stdio.h>
#include<stdlib.h>
#pragma comment(lib,"ws2_32.lib")  //网络相关API的支持库
#include <WS2tcpip.h>     //TCP协议
#include<iostream>
#include<string>
#include "Status.h"
#include "dc_c_collector.h"


class Client :public Status{
private:

	WORD sockVersion;  //2byte的无符号整数    
	SOCKET sclient;    //嵌套字描述
	sockaddr_in serAddr;  // 保护地址族，16位TCP/UDP端口号，32位IP地址
	char recData[255];
	const char * sendData;

public:
	Client() {

	}
	Client(const char* addr) {
		sockVersion = MAKEWORD(2, 2);//将两个byte合并成一个word型，左边高8位，右边高8位
		WSADATA data;//这个结构用来存储被WSAStartup函数调用后返回的Windows Sockets数据。包含Winsock.dll执行的数据
		WSAStartup(sockVersion, &data);//指明Sockets API的版本号，获得特定的Windows Sockets实现的细节
		sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//函数返回嵌套字描述符
		/*
		AF_INET :IPV4协议
		SOCK_STREAM：字节流嵌套字
		IPPROTO_TCP TCP传输协议
		*/
		serAddr.sin_family = AF_INET;//	IPv4协议
		serAddr.sin_port = htons(8888);  //16位端口号
		serAddr.sin_addr.S_un.S_addr = inet_addr(addr);//32位IP地址
		
	}
	//更新IP地址
	void init(const char* addr) {
		sockVersion = MAKEWORD(2, 2);//指定socket的版本
		WSADATA data;////这个结构用来存储被WSAStartup函数调用后返回的Windows Sockets数据。包含Winsock.dll执行的数据
		WSAStartup(sockVersion, &data);
		sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		serAddr.sin_family = AF_INET;
		serAddr.sin_port = htons(8888);
		serAddr.sin_addr.S_un.S_addr = inet_addr(addr);
	}

	char * Search() {
		//puts("调用查找文件函数");
		//DC_C_Collector* File_Collect = new DC_C_Collector("D:\\光庭实训\\大作业——数据收集\\测试数据\\*");
		//cout << "目标目录";
		string a;
		//cin >> a;
		//a.replace("\\", "\\\\");
		//DC_C_Collector* File_Collect = new DC_C_Collector("F:\\课设\\光庭实训\\大作业——数据收集\\测试数据\\*");
		//DC_C_Collector* File_Collect = new DC_C_Collector("C:\\Users\\黄怡\\Desktop\\大作业——数据收集\\测试数据\\*");
		DC_C_Collector* File_Collect = new DC_C_Collector("C:\\Users\\75173\\Desktop\\test\\*");

		//DC_C_Collector* File_Collect = new DC_C_Collector("C:\\*");
		File_Info* m_fileList = File_Collect->getResultList();
		uint64_t* Size = File_Collect->getResultSize();
		int all_size = File_Collect->getResultNum();//收集信息
		static char imfo[1024];
		strcpy_s(imfo, "");
		for (int i = 0; i < all_size; i++) {
			strcat_s(imfo, m_fileList[i].name);//strcat将两个char类型连接
			strcat_s(imfo, " ");
			//文件名中间留空格
			char size[20];
			int num = 0;
			uint64_t temp = Size[i];
			while (temp) {
				size[num++] = temp % 10 + '0';
				temp /= 10;
			}size[num] = '\0';
			for (int i = 0; i <num / 2; i++) {
				swap(size[i], size[num - 1 - i]);
			}

			strcat_s(imfo, size);
			strcat_s(imfo, "\n");
		}
		//puts(imfo);
		return imfo;
	}

	virtual void SetStatus(bool status) {
		this->flag = status;
	}

	virtual bool GetStatus() {
		return this->flag;
	}

	bool run(int iii) {
		if (sclient == INVALID_SOCKET)
		{
			printf("invalid socket!");
			return 0;
		}
		if (connect(sclient, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
		{  //连接失败 
			printf("connect error !");
			return 0;
		}printf("1");
		char revData[255];
		strcpy(revData, "\0");
		while (true) {
			cout << "线程" << iii << endl;
			cout << "0:EXIT 1:发送数据 2：显示传送到服务器的数据" << endl;
			SetStatus(true);
			int op;
			cin >> op;
			if (op == 0&&GetStatus()) {
				char data[1005] = "0";
				send(sclient, data, strlen(data), 0);
				closesocket(sclient);
				SetStatus(false);
				return 0;
			}
			if (op == 1 && GetStatus()) {
				int op_1=0;
				//puts(revData);
				if (strcmp(revData, "true")==0) {
					op_1 = 1;
				}
				if (op_1 == 1) {
					cout << "开始定时收集数据（5S）" << endl;
					while (1)
					{
						char data[1005] = "1";
						char *sendData = Search();
						strcat(data, sendData);
						send(sclient, data, strlen(data), 0);
						Sleep(5000);
					}
				}
				else {
					char data[1005] = "1";
					char *sendData = Search();
					strcat(data, sendData);
					send(sclient, data, strlen(data), 0);
				}
				int ret = recv(sclient, revData, 255, 0);
				revData[ret] = 0x00;
			}
			if (op == 2 && GetStatus()) {
				char data[1005] = "2";
				send(sclient, data, strlen(data), 0);
				int ret = recv(sclient, recData, 255, 0);
				if (ret>0) {
					recData[ret] = 0x00;
					puts(recData);
				}
			}

		}
		system("pause");
		WSACleanup();//中止Winsock 2 DLL 的使用  
	}
};

