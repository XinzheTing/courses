// project3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <windows.h>
#include <math.h>
#include "afxwin.h"


CWinThread *thread1, *thread2;
DWORD ID1, ID2,Bus = 0;
UINT aThread(LPVOID pParam);
UINT bThread(LPVOID pParam);
using namespace std;

int _tmain(int argc, TCHAR* argv[],TCHAR* envp[])
{
	int nRetCode = 0;
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		cerr << _T("fatal error:MFC intialization failed") << endl;
		nRetCode = 1;
	}
	else
	{//启动线程a和线程b
		thread1 =::AfxBeginThread(aThread, NULL);
		ID1 = thread1->m_nThreadID;
		thread2 = ::AfxBeginThread(bThread, NULL);
		ID2 = thread2->m_nThreadID;
		getchar();
	}
	return nRetCode;
}

//线程a代表主机a
UINT aThread(LPVOID pParam)
{
	//初始化
	int i = 0;
	int collosionCounter = 16;
	double collosionWindow = 0.00512;
	int randNumn = rand() % 3;
LOOP:
	if (Bus == 0)
	{
		Bus = Bus | ID1;
		Sleep(12);
		if (Bus == ID1)
		{
			printf("%d send success\n", ID1);
			collosionCounter = 16;
			Sleep(rand() % 10);
			i++;
			Bus = 0;
			printf("HOST a send successfully=%d\n", i);
			if (i < 5)
				goto LOOP;
		}
		else
		{
			printf("%d send collision\n", ID1);
			Bus = 0;
			collosionCounter--;
			if (collosionCounter > 0)
			{
				Sleep(randNumn*(int)pow(2, (collosionCounter > 10) ? 10 : collosionCounter)*collosionWindow);
				goto LOOP;
			}
			else
				printf("%d send failure\n", ID1);
		}
	}
	else
	{
		goto LOOP;
	}
	return 0;
}

//线程b代表主机b
UINT bThread(LPVOID pParam)
{
	//初始化
	int i = 0;
	int collosionCounter = 16;
	double collosionWindow = 0.00512;
	int randNumn = rand() % 3;
LOOP:
	if (Bus == 0)
	{
		Bus = Bus | ID2;
		Sleep(12);
		if (Bus == ID2)
		{
			printf("%d send success\n", ID2);
			collosionCounter = 16;
			Sleep(rand() % 10);
			i++;
			Bus = 0;
			printf("HOST b send successfully=%d\n", i);
			if (i < 5)
				goto LOOP;
		}
		else
		{
			printf("%d send collision\n", ID2);
			Bus = 0;
			collosionCounter--;
			if (collosionCounter > 0)
			{
				Sleep(randNumn*(int)pow(2, (collosionCounter > 10) ? 10 : collosionCounter)*collosionWindow);
				goto LOOP;
			}
			else
				printf("%d send failure\n", ID2);
		}
	}
	else
	{
		goto LOOP;
	}
	return 0;
}
