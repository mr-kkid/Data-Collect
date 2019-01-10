#pragma once
#pragma once
#pragma once
#pragma once
//��ֹ���α���
#include <stdint.h>  // ������int16_t �� uint32_t �� int64_t 
#include <io.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "stdafx.h"
#include <WS2tcpip.h>    //TCPЭ�����
#include <iostream>
using namespace std;

class DC_C_Collector_Filter//���ݹ��ˣ�����ļ���Ϣ
{
public:
	DC_C_Collector_Filter(uint64_t size) :m_targetSize(size)
	{}
	~DC_C_Collector_Filter(void) {}

	bool isMatched(const char* filePath);//�ж��Ƿ�ƥ��

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

	File_Info* getResultList(void);//�õ��������
	uint32_t getResultNum(void);//�õ������Ŀ
	uint64_t* getResultSize(void);//�õ������С

private:
	void getDirFileList(const char* dir, uint32_t* num);
	void getFileInfoList(char fileList[1000][1000], uint32_t num, uint32_t* rNum);

public:
	const char* m_rootPath;//Դ·����ֻ�����ɱ��޸�
	bool m_rflag;
	char m_files[1000][1000];  //�ļ�Ŀ¼
	File_Info m_fileList[1000];//�ռ��������з����������ļ���Ϣ
	uint64_t size[1000];   //�ļ���С
	uint32_t m_listNum;
};

bool DC_C_Collector_Filter::isMatched(const char* filePath)
{
	//cout << "ɸѡ����" << endl;
	FILE* pFile;
	//puts(filePath);
	char last[1000];
	int len = strlen(filePath);
	int num = 0;
	//printf("len=%d\n", len);

	//��ȡ�ļ���β��
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
	fopen_s(&pFile, filePath, "r");//���ļ�
	fseek(pFile, 0, SEEK_END);
	uint64_t fileSize = ftell(pFile);//�õ��ļ���С
									 //printf("fileSize=%d\n", fileSize);
	fclose(pFile);
	cout << filePath << " " << fileSize <<"KB"<< endl;
	return (fileSize > m_targetSize); //����ļ���С����ָ���ļ���true
}

DC_C_Collector::DC_C_Collector(const char* path)
{
	m_rflag = true;
	m_rootPath = _strdup(path);//��ֵԴ·��
							   //puts("���뺯��");
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
	//printf("����C���ļ�\n");
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
			if (fileInfo.attrib&_A_SUBDIR)//�ݹ������ļ���
			{
				if (m_rflag)//�Ƿ�����������ļ���
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
	cout << "Ҫѡ����ļ���С�����ϣ���1000" << endl;;
	uint64_t  minimalSize=1000;
	//cin >> minimalSize;
	DC_C_Collector_Filter filter(minimalSize);

	for (uint32_t idx = 0; idx < num; ++idx)
	{

		if (filter.isMatched(fileList[idx]))
		{
			FILE* pFile;
			//printf("��ȡ�ļ��ĵ�ַ:");
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