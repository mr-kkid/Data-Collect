#pragma once
#pragma once
#pragma once
#pragma once
//防止二次编译
#include <stdint.h>  // 定义了int16_t 、 uint32_t 、 int64_t 
#include <io.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "stdafx.h"
#include <WS2tcpip.h>    //TCP协议相关
#include <iostream>
using namespace std;

class DC_C_Collector_Filter//数据过滤，输出文件信息
{
public:
	DC_C_Collector_Filter(uint64_t size) :m_targetSize(size)
	{}
	~DC_C_Collector_Filter(void) {}

	bool isMatched(const char* filePath);//判断是否匹配

private:
	uint64_t m_targetSize;
};

typedef struct
{
	char name[256];
	uint64_t size;
}File_Info;

class DC_C_Collector
{
public:
	DC_C_Collector(const char* path);
	~DC_C_Collector(void);

	File_Info* getResultList(void);//得到结果链表
	uint32_t getResultNum(void);//得到结果数目
	uint64_t* getResultSize(void);//得到结果大小

private:
	void getDirFileList(const char* dir, uint32_t* num);
	void getFileInfoList(char fileList[1000][1000], uint32_t num, uint32_t* rNum);

public:
	const char* m_rootPath;//源路径，只读不可被修改
	bool m_rflag;
	char m_files[1000][1000];  //文件目录
	File_Info m_fileList[1000];//收集到的所有符合条件的文件信息
	uint64_t size[1000];   //文件大小
	uint32_t m_listNum;
};

bool DC_C_Collector_Filter::isMatched(const char* filePath)
{
	//cout << "筛选函数" << endl;
	FILE* pFile;
	//puts(filePath);
	char last[1000];
	int len = strlen(filePath);
	int num = 0;
	//printf("len=%d\n", len);

	//读取文件的尾部
	for (int i = len - 1; i >= 0; i--) {
		if (filePath[i] == '.' || filePath[i] == '\\') {
			break;
		}
		else {
			last[num++] = filePath[i];
		}
	}last[num] = '\0';
	//cout << last << endl;
	if (strcmp(last, "txt") != 0) {
		return false;
	}
	//puts(filePath);
	fopen_s(&pFile, filePath, "r");//打开文件
	fseek(pFile, 0, SEEK_END);
	uint64_t fileSize = ftell(pFile);//得到文件大小
									 //printf("fileSize=%d\n", fileSize);
	fclose(pFile);
	cout << filePath << " " << fileSize <<"KB"<< endl;
	return (fileSize > m_targetSize); //如果文件大小大于指定文件则传true
}

DC_C_Collector::DC_C_Collector(const char* path)
{
	m_rflag = true;
	m_rootPath = _strdup(path);//赋值源路径
							   //puts("进入函数");
	getDirFileList(m_rootPath, &m_listNum);     
	getFileInfoList(m_files, m_listNum, &m_listNum);
}

DC_C_Collector::~DC_C_Collector(void)
{
	//???
}

File_Info* DC_C_Collector::getResultList(void)
{
	return m_fileList;
}

uint64_t* DC_C_Collector::getResultSize(void)
{
	return size;
}

uint32_t DC_C_Collector::getResultNum(void)
{
	return m_listNum;
}

void DC_C_Collector::getDirFileList(const char* dir, uint32_t* num)
{
	uint32_t rNum = 0;
	struct _finddata_t fileInfo;
	//printf("查找C盘文件\n");
	//puts(dir);
	long handle = _findfirst(dir, &fileInfo);
	int done = 0;
	char temp_dir[1000];
	strcpy_s(temp_dir, dir);
	temp_dir[strlen(dir) - 1] = '\0';
	if (-1 != handle)
	{
		do {
			if (strcmp(fileInfo.name, ".") == 0 || strcmp(fileInfo.name, "..") == 0) {
				continue;
			}
			if (fileInfo.attrib&_A_SUBDIR)//递归搜索文件夹
			{
				if (m_rflag)//是否允许查找子文件夹
				{
					char path[256];
					strcpy_s(path, temp_dir);
					strcat_s(path, fileInfo.name);
					strcat_s(path, "\\*");
					getDirFileList(path, &rNum);
				}
			}
			else
			{
				char path[256];
				strcpy_s(path, temp_dir);
				strcat_s(path, fileInfo.name);
				strcpy_s(m_files[rNum], path);
				//puts(m_files[rNum]);
				rNum++;
			}

		} while (!(done = _findnext(handle, &fileInfo)));
	}
	*num = rNum;
}

void DC_C_Collector::getFileInfoList(char fileList[1000][1000], uint32_t num, uint32_t* rNum)
{
	uint32_t counter = 0;
	cout << "要选择的文件大小（以上）：1000" << endl;;
	uint64_t  minimalSize=1000;
	//cin >> minimalSize;
	DC_C_Collector_Filter filter(minimalSize);

	for (uint32_t idx = 0; idx < num; ++idx)
	{

		if (filter.isMatched(fileList[idx]))
		{
			FILE* pFile;
			//printf("读取文件的地址:");
			//puts(fileList[idx]);
			fopen_s(&pFile, fileList[idx], "r");
			fseek(pFile, 0, SEEK_END);
			uint64_t fileSize = ftell(pFile);
			fclose(pFile);
			//puts(fileList[idx]);
			strcpy_s(m_fileList[counter].name, fileList[idx]);
			size[counter] = fileSize;
			counter++;
		}
	}

	*rNum = counter;
}