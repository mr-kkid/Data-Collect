
#include "stdafx.h"
#include <stdio.h>
#include <winsock2.h>
#include <iostream>
#include <algorithm>
#include <iostream>
#include <thread>
#include <direct.h>
#include <string>
#include <map>
#include <sstream>
#include <vector>
#include<fstream>
#pragma comment(lib,"ws2_32.lib")



int link_num = 0;
std::map<std::string, int> Map;
std::vector<std::string> vec[1025];
bool Flag = false;


std::vector<std::string> splitEx(const std::string& src, std::string separate_character)
{
	std::vector<std::string> strs;

	int separate_characterLen = separate_character.size();//�ָ��ַ����ĳ���,�����Ϳ���֧���硰,,�����ַ����ķָ���
	int lastPosition = 0, index = -1;
	while (-1 != (index = src.find(separate_character, lastPosition)))
	{
		strs.push_back(src.substr(lastPosition, index - lastPosition));
		lastPosition = index + separate_characterLen;
	}
	std::string lastString = src.substr(lastPosition);//��ȡ���һ���ָ����������
	if (!lastString.empty())
		strs.push_back(lastString);//������һ���ָ����������ݾ����
	return strs;
}

void Save(SOCKET sClient, sockaddr_in remoteAddr) {
	//��������
	//puts("�����߳�");
	while (1) {
		char revData[255];
		int ret = recv(sClient, revData, 255, 0);
		//recv����ʵ��copy���ֽ���
		if (ret > 0)
		{
			revData[ret] = 0x00;
			if (revData[0] == '0') {
				//send(sClient, "1", strlen("1"), 0);
				std::cout << "�ͻ���" << inet_ntoa(remoteAddr.sin_addr) << "��������" << std::endl;
				closesocket(sClient);
				return;
			}//����������˳�

			if (revData[0] == '1') {
				std::string str = revData;
				std::ofstream ofile_temp;               //��������ļ�
				std::string add_port = inet_ntoa(remoteAddr.sin_addr);
				std::string a;
				printf(revData);
				a = "D:\\����ҵ\\�����ռ�\\Server\\temp_";
				a=a + add_port;
				std::cout << a << std::endl;
				ofile_temp.open(a.c_str());     //��Ϊ����ļ���
				ofile_temp << str;
				ofile_temp.close();
				//printf(a.c_str());
				//FILE* pFile= fopen(a.c_str(), "r");
				std::ifstream fin;
				fin.open(a.c_str());
				char line[1024];				
				//std::string add_port = inet_ntoa(remoteAddr.sin_addr);
				std::cout << add_port << std::endl;
				if (Map[add_port] == 0) {
					link_num++;
					Map[add_port] = link_num;
						
				}
				std::string all="";
				int num = 0;
				std::string x = "";
				std::string y = "";
				while (fin.getline(line, sizeof(line)))
				{
					num++;
					std::cout << num << std::endl;
					std::stringstream word(line);
					word >> x;
					word >> y;
					std::cout << "x: " << x << std::endl;
					std::cout << "y: " << y << std::endl;
					std::vector<std::string> vctt = splitEx(x, "\\");
					all += vctt[vctt.size() - 1]+" "+y + '\n';
					vctt = splitEx(vctt[vctt.size() - 1], " ");
					std::string file_name = vctt[0];
					vec[link_num].push_back(file_name);
					
				}
				//fclose(pFile);
	
				std::cout << all;
				std::ofstream ofile;               //��������ļ�
				ofile.open(add_port.c_str());     //��Ϊ����ļ���
				ofile << all << std::endl;   //����д���ļ�
				if (!Flag) {
					std::string data;
					std::cout << "�Ƿ񴥷��ͻ��˶�ʱ������ 1��ʾ�� 2��ʾ��" << std::endl;
					std::cin >> data;
					if (data == "1") {
						Flag = true;
						send(sClient, "true", strlen("true"), 0);
					}
					else {
						send(sClient, "false", strlen("true"), 0);
					}
					
				}
				else {
					send(sClient, "", 0, 0);
				}
				
			}
			if (revData[0] == '2') {
				std::string add_port = inet_ntoa(remoteAddr.sin_addr);
				int op = Map[add_port];
				std::string data;
				for (int i = 0; i < vec[op].size(); i++) {
					data += vec[op][i];
				}
				send(sClient, data.c_str(), strlen(data.c_str()), 0);
			}
		}
	}
}

int main() {
	WORD sockVersion = MAKEWORD(2, 2);//�汾��
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return 0;
	}

	//�����׽���
	SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//TCP IPV4 �ֽ���
	if (slisten == INVALID_SOCKET)
	{
		printf("socket error !");
		return 0;
	}

	//��IP�Ͷ˿�
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(8888);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	//
	if (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf("bind error !");
	}

	//��ʼ����
	if (listen(slisten, 5) == SOCKET_ERROR)
	{
		printf("listen error !");
		return 0;
	}

	//ѭ����������
	SOCKET sClient;
	sockaddr_in remoteAddr;
	int nAddrlen = sizeof(remoteAddr);
	printf("�ȴ�����...\n");
	while (true)
	{
		sClient = accept(slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);
		if (sClient == INVALID_SOCKET)
		{
			printf("accept error !");
			continue;
		}
		printf("���ܵ�һ�����ӣ�%s : %d\r\n", inet_ntoa(remoteAddr.sin_addr), (remoteAddr.sin_port));
		//send(sClient, "", 0, 0);
		std::thread t(Save, sClient, remoteAddr);
		t.detach();

	}
	closesocket(slisten);
	WSACleanup();

	return 0;
}
