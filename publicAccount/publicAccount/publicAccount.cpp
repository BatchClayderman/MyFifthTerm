#include <iostream>
#include <Windows.h>
#include <vector>
#include <algorithm>
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
#define N 14
#endif
#ifndef LOOP
#define LOOP(x) for (unsigned short int x = 0; x < N; ++x)
#endif
using namespace std;

/* 声明信号量等全局变量 */
HANDLE global_semo[N] = { 0 }, main_thread = NULL, debt_lock = NULL;
CRITICAL_SECTION cs;
int transaction = 10;
unsigned short int id = 0, debt_id = 4;


/* 结构体 */
enum Color
{
	Black,
	Blue,
	Green,
	Aqua,
	Red,
	Purple,
	Yellow,
	White,
	Gray,
	Light_Blue,
	Light_Green,
	Light_Aqua,
	Light_Red,
	Light_Purple,
	Light_Yellow,
	Bright_White
};

struct Account
{
	string name;
	int finance_op;
	HANDLE sema;
};


/* 输出显示与函数优化 */
void echo(string str, short int x)//输出流
{
	if (x >= Color::Black && x <= Color::Bright_White)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), x);
	cout << str << endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), Color::Light_Yellow);//归还默认色
	return;
}

void press_any_key_to_exit()
{
	rewind(stdin);
	fflush(stdin);
	char ch = _getch();
	cout << endl << endl;
	return;
}

string fill_zero(unsigned short int _id, string mark)
{
	size_t length = to_string(N).length() - to_string(_id).length();
	string sRet = "#";
	for (size_t i = 0; i < length; ++i)
		sRet += "0";
	sRet += to_string(_id) + mark;
	return sRet;
}

vector<unsigned short int> random_permut(unsigned short int n)
{
	vector<unsigned short int> temp;
	for (unsigned short int i = 0; i < n; ++i)
		temp.push_back(i);
	random_shuffle(temp.begin(), temp.end());
	return temp;
}


/* 子函数 */
DWORD WINAPI Deposit(LPVOID lpp)//存款
{
	Account* alpp = (Account*)lpp;
	WaitForSingleObject(alpp->sema, INFINITE);//线程创建，等待用户去银行这一事件发生
	EnterCriticalSection(&cs);//用户提交银行
	transaction += alpp->finance_op;
	echo(fill_zero(++id, " [+] ") + alpp->name + " deposits " + to_string(abs(alpp->finance_op)) + ", and current account balance is " + to_string(transaction) + ". ", Color::Light_Green);
	if (debt_id == 0 && id >= N)
		ReleaseSemaphore(main_thread, 1, NULL);//可返回主线程
	LeaveCriticalSection(&cs);//银行处理完毕
	ReleaseSemaphore(debt_lock, 1, NULL);//释放信号以供取款
	return EXIT_SUCCESS;
}

DWORD WINAPI Withdraw(LPVOID lpp)//取款
{
	Account* alpp = (Account*)lpp;
	WaitForSingleObject(alpp->sema, INFINITE);//线程创建，等待用户去银行这一事件发生
	EnterCriticalSection(&cs);//用户提交银行
	unsigned short int cur_id = ++id;
	if (transaction + alpp->finance_op < 0)//钱不够
	{
		echo(fill_zero(cur_id, " [!] ") + alpp->name + " fails to withdraw " + to_string(abs(alpp->finance_op)) + ", waiting. Current account balance is " + to_string(transaction) + ". ", Color::Yellow);
		do
		{
			LeaveCriticalSection(&cs);//暂不处理
			WaitForSingleObject(debt_lock, INFINITE);//等待有钱
			EnterCriticalSection(&cs);//用户提交银行
		} while (transaction + alpp->finance_op < 0);//钱仍然不够
	}
	transaction += alpp->finance_op;
	echo(fill_zero(cur_id, " [-] ") + alpp->name + " withdraws " + to_string(abs(alpp->finance_op)) + ", and current account balance is " + to_string(transaction) + ". ", Color::Light_Blue);
	if (--debt_id == 0 && id >= N)//处理正在进行中的交易（注意 --debt_id 要在 && 前面以免未执行）
		ReleaseSemaphore(main_thread, 1, NULL);//可返回主线程
	LeaveCriticalSection(&cs);//银行处理完毕
	ReleaseSemaphore(debt_lock, 1, NULL);//释放信号以防该交易发生在最后导致主线程提前关闭线程句柄
	return EXIT_SUCCESS;
}



/* main 函数 */
int main()
{
	/* 初始化随机数种子和临界区 */
	srand((unsigned)time(NULL));
	InitializeCriticalSection(&cs);

	/* 初始化信号量变量 */
	main_thread = CreateSemaphore(NULL, 0, 1, NULL);
	debt_lock = CreateSemaphore(NULL, 0, 1, NULL);
	BOOLEAN semo_state = (main_thread != NULL && debt_lock != NULL);
	LOOP(i)
	{
		global_semo[i] = CreateSemaphore(NULL, 0, 1, NULL);
		semo_state = semo_state && global_semo[i];
	}
	if (!semo_state)
	{
		echo("初始化信号量变量失败，请按任意键退出！", Color::Light_Red);
		press_any_key_to_exit();
		return EXIT_FAILURE;
	}
	
	/* 初始化参数 */
	Account accounts[N] = {
		{ "Father", 10, global_semo[0] },
		{ "Mother", 20, global_semo[1] },
		{ "Grandmother", 30, global_semo[2] },
		{ "Grandfather", 40, global_semo[3] },
		{ "Uncle", 50, global_semo[4] },
		{ "Mary", -50, global_semo[5] },
		{ "Sally", -100, global_semo[6] },
		{ "Father", 10, global_semo[7] },
		{ "Mother", 20, global_semo[8] },
		{ "Grandmother", 30, global_semo[9] },
		{ "Grandfather", 40, global_semo[10] },
		{ "Uncle", 50, global_semo[11] },
		{ "Mary", -50, global_semo[12] },
		{ "Sally", -100, global_semo[13] }
	};

	/* 创建多线程 */
	BOOLEAN thread_state = TRUE;
	HANDLE threads[N] = { NULL };
	LOOP(i)
	{
		/* 三目表达式判断存取款函数 */
		threads[i] = CreateThread(NULL, 0, accounts[i].finance_op >= 0 ? Deposit : Withdraw, &accounts[i], 0, 0);
		thread_state = thread_state && threads[i];
	}
	if (!thread_state)
	{
		echo("初始化多线程失败，请按任意键退出！", Color::Light_Red);
		press_any_key_to_exit();
		return EXIT_FAILURE;
	}

	/* 执行任务 */
	echo("初始化存款值为 " + to_string(transaction) + "，仿真开始。\n", Color::Light_Yellow);
	vector<unsigned short int> randoms = random_permut(N);
	LOOP(i)
		ReleaseSemaphore(global_semo[randoms[i]], 1, NULL);//用户去银行办理手续

	/* 关闭线程句柄 */
	WaitForSingleObject(main_thread, INFINITE);//等待所有线程完成
	WaitForSingleObject(debt_lock, INFINITE);//等待所有交易完成
	EnterCriticalSection(&cs);//等待线程完成
	LOOP(i)
		CloseHandle(threads[i]);
	LeaveCriticalSection(&cs);
	echo("\n\n操作成功结束，账户最终余额为 " + to_string(transaction) + "，请按任意键退出。", Color::Light_Yellow);
	press_any_key_to_exit();

	return EXIT_SUCCESS;
}