
#include "stdafx.h"
#include "Client.h"
#include <stdio.h>  
#include <winsock2.h>  
#include <string>
#include <iostream>
#include <thread>
#include "dc_c_collector.h"
#pragma comment(lib,"ws2_32.lib") 


Client* client;
void run(string ip,int i) {
	client = new Client(ip.c_str());
	client->run(i);
	
}

int main()
{
	cout << "11111" << endl;
	for (int i = 1; i <= 2; i++) {
		thread t(run, "127.0.0.1",i);
		t.join();
	}
	return 0;
}
