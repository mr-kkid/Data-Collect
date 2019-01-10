
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

	int separate_characterLen = separate_character.size();//分割字符串的长度,这样就可以支持如“,,”多字符串的分隔符
	int lastPosition = 0, index = -1;
	while (-1 != (index = src.find(separate_character, lastPosition)))
	{
		strs.push_back(src.substr(lastPosition, index - lastPosition));
		lastPosition = index + separate_characterLen;
	}
	std::string lastString = src.substr(lastPosition);//截取最后一个分隔符后的内容
	if (!lastString.empty())
		strs.push_back(lastString);//如果最后一个分隔符后还有内容就入队
	return strs;
}

void Save(SOCKET sClient, sockaddr_in remoteAddr) {
	//接收数据
	//puts("进入线程");
	while (1) {
		char revData[255];
		int ret = recv(sClient, revData, 255, 0);
		//recv返回实际copy的字节数
		if (ret > 0)
		{
			revData[ret] = 0x00;
			if (revData[0] == '0') {
				//send(sClient, "1", strlen("1"), 0);
				std::cout << "客户端" << inet_ntoa(remoteAddr.sin_addr) << "结束连接" << std::endl;
				closesocket(sClient);
				return;
			}//如果是零则退出

			if (revData[0] == '1') {
				std::string str = revData;
				std::ofstream ofile_temp;               //定义输出文件
				std::string add_port = inet_ntoa(remoteAddr.sin_addr);
				std::string a;
				printf(revData);
				a = "D:\\大作业\\数据收集\\Server\\temp_";
				a=a + add_port;
				std::cout << a << std::endl;
				ofile_temp.open(a.c_str());     //作为输出文件打开
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
				std::ofstream ofile;               //定义输出文件
				ofile.open(add_port.c_str());     //作为输出文件打开
				ofile << all << std::endl;   //标题写入文件
				if (!Flag) {
					std::string data;
					std::cout << "是否触发客户端定时器发送 1表示是 2表示否" << std::endl;
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
	WORD sockVersion = MAKEWORD(2, 2);//版本号
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return 0;
	}

	//创建套接字
	SOCKET slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//TCP IPV4 字节流
	if (slisten == INVALID_SOCKET)
	{
		printf("socket error !");
		return 0;
	}

	//绑定IP和端口
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(8888);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	//
	if (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
	{
		printf("bind error !");
	}

	//开始监听
	if (listen(slisten, 5) == SOCKET_ERROR)
	{
		printf("listen error !");
		return 0;
	}

	//循环接收数据
	SOCKET sClient;
	sockaddr_in remoteAddr;
	int nAddrlen = sizeof(remoteAddr);
	printf("等待连接...\n");
	while (true)
	{
		sClient = accept(slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);
		if (sClient == INVALID_SOCKET)
		{
			printf("accept error !");
			continue;
		}
		printf("接受到一个连接：%s : %d\r\n", inet_ntoa(remoteAddr.sin_addr), (remoteAddr.sin_port));
		//send(sClient, "", 0, 0);
		std::thread t(Save, sClient, remoteAddr);
		t.detach();

	}
	closesocket(slisten);
	WSACleanup();

	return 0;
}
