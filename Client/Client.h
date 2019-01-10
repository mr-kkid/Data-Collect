#pragma once
// my_Client.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include "stdafx.h"
#include<stdio.h>
#include<stdlib.h>
#pragma comment(lib,"ws2_32.lib")  //�������API��֧�ֿ�
#include <WS2tcpip.h>     //TCPЭ��
#include<iostream>
#include<string>
#include "Status.h"
#include "dc_c_collector.h"


class Client :public Status{
private:

	WORD sockVersion;  //2byte���޷�������    
	SOCKET sclient;    //Ƕ��������
	sockaddr_in serAddr;  // ������ַ�壬16λTCP/UDP�˿ںţ�32λIP��ַ
	char recData[255];
	const char * sendData;

public:
	Client() {

	}
	Client(const char* addr) {
		sockVersion = MAKEWORD(2, 2);//������byte�ϲ���һ��word�ͣ���߸�8λ���ұ߸�8λ
		WSADATA data;//����ṹ�����洢��WSAStartup�������ú󷵻ص�Windows Sockets���ݡ�����Winsock.dllִ�е�����
		WSAStartup(sockVersion, &data);//ָ��Sockets API�İ汾�ţ�����ض���Windows Socketsʵ�ֵ�ϸ��
		sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//��������Ƕ����������
		/*
		AF_INET :IPV4Э��
		SOCK_STREAM���ֽ���Ƕ����
		IPPROTO_TCP TCP����Э��
		*/
		serAddr.sin_family = AF_INET;//	IPv4Э��
		serAddr.sin_port = htons(8888);  //16λ�˿ں�
		serAddr.sin_addr.S_un.S_addr = inet_addr(addr);//32λIP��ַ
		
	}
	//����IP��ַ
	void init(const char* addr) {
		sockVersion = MAKEWORD(2, 2);//ָ��socket�İ汾
		WSADATA data;////����ṹ�����洢��WSAStartup�������ú󷵻ص�Windows Sockets���ݡ�����Winsock.dllִ�е�����
		WSAStartup(sockVersion, &data);
		sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		serAddr.sin_family = AF_INET;
		serAddr.sin_port = htons(8888);
		serAddr.sin_addr.S_un.S_addr = inet_addr(addr);
	}

	char * Search() {
		//puts("���ò����ļ�����");
		//DC_C_Collector* File_Collect = new DC_C_Collector("D:\\��ͥʵѵ\\����ҵ���������ռ�\\��������\\*");
		//cout << "Ŀ��Ŀ¼";
		string a;
		//cin >> a;
		//a.replace("\\", "\\\\");
		//DC_C_Collector* File_Collect = new DC_C_Collector("F:\\����\\��ͥʵѵ\\����ҵ���������ռ�\\��������\\*");
		//DC_C_Collector* File_Collect = new DC_C_Collector("C:\\Users\\����\\Desktop\\����ҵ���������ռ�\\��������\\*");
		DC_C_Collector* File_Collect = new DC_C_Collector("C:\\Users\\75173\\Desktop\\test\\*");

		//DC_C_Collector* File_Collect = new DC_C_Collector("C:\\*");
		File_Info* m_fileList = File_Collect->getResultList();
		uint64_t* Size = File_Collect->getResultSize();
		int all_size = File_Collect->getResultNum();//�ռ���Ϣ
		static char imfo[1024];
		strcpy_s(imfo, "");
		for (int i = 0; i < all_size; i++) {
			strcat_s(imfo, m_fileList[i].name);//strcat������char��������
			strcat_s(imfo, " ");
			//�ļ����м����ո�
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
		{  //����ʧ�� 
			printf("connect error !");
			return 0;
		}printf("1");
		char revData[255];
		strcpy(revData, "\0");
		while (true) {
			cout << "�߳�" << iii << endl;
			cout << "0:EXIT 1:�������� 2����ʾ���͵�������������" << endl;
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
					cout << "��ʼ��ʱ�ռ����ݣ�5S��" << endl;
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
		WSACleanup();//��ֹWinsock 2 DLL ��ʹ��  
	}
};

