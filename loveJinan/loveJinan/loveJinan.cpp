#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <conio.h>
#include <string>
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif
#ifndef NULL
#define NULL 0
#endif
#ifndef N
#define N 4
#endif
#ifndef LOOP
#define LOOP(x) for (short int x = 0; x < N; ++x)
#endif
using namespace std;
HANDLE g_hSemaphore[N + 1] = { NULL };//声明信号量变量
DWORD global_N = N;


struct SLpp
{
	string str;
	unsigned short int sema;
};

void rfstdin()
{
	rewind(stdin);
	fflush(stdin);
	return;
}

DWORD getThreadCount()
{
	HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hProcessSnap)
		return NULL;
	
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);
	BOOL bMore = ::Process32First(hProcessSnap, &pe32);
	DWORD PID = GetCurrentProcessId();

	while (bMore)
		if (pe32.th32ProcessID == PID)
			return pe32.cntThreads;
		else
			bMore = Process32Next(hProcessSnap, &pe32);

	return NULL;
}

BOOLEAN isContinue(DWORD right_dw)
{
	DWORD dw = getThreadCount();
	if (0 == dw || right_dw == dw)//可忽略检测线程数错误
		return TRUE;
	string text = "检测到有 " + to_string(dw - right_dw) + " 条线程注入到了本程序中，这可能会影响运行结果，是否继续运行？";
	return (BOOLEAN)(MessageBoxA(NULL, text.c_str(), "存在线程注入", MB_YESNO | MB_ICONQUESTION) == IDYES);
}

DWORD WINAPI print(LPVOID lpp)
{
	SLpp* slpp = (SLpp*)lpp;
	while (global_N > 0)//控制循环次数
	{
		WaitForSingleObject(g_hSemaphore[slpp->sema], INFINITE);//前驱申请
		cout << slpp->str.c_str() << " ";
		if (0 == slpp->sema)
			--global_N;
		else if (N - 1 == slpp->sema)
			cout << endl;//换行
		ReleaseSemaphore(g_hSemaphore[(slpp->sema + 1) % N], 1, NULL);//释放后继
	}
	if (N - 1 == slpp->sema)
		ReleaseSemaphore(g_hSemaphore[N], 1, NULL);//释放信号给主线程
	return EXIT_SUCCESS;
}



int main()
{
	/* 创建信号量对象 */
	if (!isContinue(1))
		return EXIT_FAILURE;

	cout << "程序开始运行，即将创建信号量对象。" << endl;
	for (short int i = 0; i < N + 1; ++i)
	{
		g_hSemaphore[i] = CreateSemaphore(NULL,     //信号量的安全特性
			0,                                      //设置信号量的初始计数（可设置零到最大值之间的一个值）
			1,                                      //设置信号量的最大计数
			NULL                                    //指定信号量对象的名称
		);
		if (NULL == g_hSemaphore[i])
		{
			cout << "创建 信号量" << i << " 对象失败，返回错误代码为：" << GetLastError() << "。" << endl;
			return EXIT_FAILURE;
		}
	}

	/* 创建多线程 */
	cout << "创建信号量对象完毕，即将创建并启动子线程。" << endl;
	SLpp sLpp[N] = { SLpp{"I", 0}, SLpp{"Love", 1}, SLpp{"Jinan", 2}, SLpp{"University!", 3} };
	HANDLE t[N] = {
		CreateThread(NULL, 0, print, &sLpp[0], 0, 0),
		CreateThread(NULL, 0, print, &sLpp[1], 0, 0),
		CreateThread(NULL, 0, print, &sLpp[2], 0, 0),
		CreateThread(NULL, 0, print, &sLpp[3], 0, 0)
	};

	/* 判断线程是否创建成功 */
	BOOLEAN bSuccess[N + 1] = { TRUE, TRUE, TRUE, TRUE, TRUE };
	LOOP(i)
		if (NULL == t[i])
		{
			cout << "创建线程 t" << i << " 失败！" << endl;
			bSuccess[i] = FALSE;
			bSuccess[N] = FALSE;
		}

	/* 如果线程存在创建失败的情况则退出进程 */
	if (bSuccess[N])
	{
		if (!isContinue(1 + N))
			return EXIT_FAILURE;

		cout << "创建并启动子线程完毕，即将执行打印。请输入打印数量：";
		rfstdin();
		if (scanf_s("%ul", &global_N) != 1 || 0 == global_N)
		{
			cout << "输入有误，打印数量被重置为 " << N << "。" << endl << endl << endl;
			global_N = N;
		}
		else
			cout << endl << endl << endl;
	}
	else
	{
		LOOP(i)
			if (NULL != t[i])
				CloseHandle(t[i]);
		cout << endl << endl;
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0xC);
		cout << "部分线程未能成功创建，请确保您的系统资源充足，并确认您未使用 PC Hunter Standard 等工具禁止创建线程！" << endl;
		cout << "请按任意键退出，并尝试稍后重试！" << endl;
		rfstdin(); 
		char ch = _getch();
		return EXIT_FAILURE;
	}

	/* 执行打印 */
	ReleaseSemaphore(g_hSemaphore[0], 1, NULL);//释放
	WaitForSingleObject(g_hSemaphore[N], INFINITE);//等待子线程打印完成
	cout << endl;

	/* 关闭线程句柄 */
	LOOP(i)
		CloseHandle(t[i]);
	cout << endl << endl << endl << "操作成功结束，请按任意键退出。" << endl;
	rfstdin();
	char ch = _getch();

	return EXIT_SUCCESS;
}