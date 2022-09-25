#include <iostream>
#include <iomanip>
#include <conio.h>
#ifndef NULL
#define NULL 0
#endif
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif
#ifndef N
#define N 10
#endif
#ifndef LOOP
#define LOOP(x) for (int x = 0; x < N; ++x)
#endif
#ifndef MIN_ARRIVE
#define MIN_ARRIVE 1
#endif
#ifndef MAX_ARRIVE
#define MAX_ARRIVE 20
#endif
#ifndef MIN_RUNNING
#define MIN_RUNNING 10
#endif
#ifndef MAX_RUNNING
#define MAX_RUNNING 50
#endif
#ifndef MAX_WIDTH
#define MAX_WIDTH 12
#endif
using namespace std;
typedef unsigned short int SJFType;
typedef void AlgType;


struct SJF//定义进程的结构体
{
	SJFType pid;//进程序号
	SJFType arriveTime;//到达时间
	SJFType runningTime;//运行时间
	SJFType beginTime;//开始时间
	SJFType endTime;//完成时间
	SJFType turnaroundTime;//周转时间
	double weightedTime;//带权周转时间
};
struct SJF sjf[N];


/* 初始化 */
SJFType getRandom(SJFType a, SJFType b)
{
	return (rand() % (b - a + 1)) + a;
}

AlgType initial()//初始化函数
{
	cout << "/*************** SJF Algorithm ***************/" << endl;
	cout << setw(MAX_WIDTH) << "PID" << setw(MAX_WIDTH) << "ARRIVE" << setw(MAX_WIDTH) << "RUNNING" << endl;
	LOOP(i)
	{
		sjf[i].pid = i + 1;
		sjf[i].arriveTime = getRandom(MIN_ARRIVE, MAX_ARRIVE);
		sjf[i].runningTime = getRandom(MIN_RUNNING, MAX_RUNNING);
		sjf[i].beginTime = 0;
		sjf[i].endTime = 0;
	}

	for (int i = 0; i < N - 1; ++i)
		for (int j = 0; j < N - i - 1; ++j)
			if (sjf[j].arriveTime > sjf[j + 1].arriveTime)//不使用临时变量的交换
			{
				sjf[j].arriveTime += sjf[j + 1].arriveTime;
				sjf[j + 1].arriveTime = sjf[j].arriveTime - sjf[j + 1].arriveTime;
				sjf[j].arriveTime -= sjf[j + 1].arriveTime;
				sjf[j].runningTime += sjf[j + 1].runningTime;
				sjf[j + 1].runningTime = sjf[j].runningTime - sjf[j + 1].runningTime;
				sjf[j].runningTime -= sjf[j + 1].runningTime;
			}

	LOOP(i)
		cout << setw(MAX_WIDTH) << sjf[i].pid << setw(MAX_WIDTH) << sjf[i].arriveTime << setw(MAX_WIDTH) << sjf[i].runningTime << endl;

	return;
}


/* 主算法 */
AlgType sortSJF()//按短作业优先算法排序
{
	SJFType nowtime = 0;
	float avgturnaroundtime = 0;
	float aqtt = 0;
	sjf[0].endTime = sjf[0].arriveTime + sjf[0].runningTime;
	sjf[0].turnaroundTime = sjf[0].endTime - sjf[0].arriveTime;
	nowtime = sjf[0].endTime;
	for (int i = 1; i < N; ++i)
	{
		for (int j = i + 1; j < N; ++j)
			if (sjf[i].arriveTime <= nowtime && sjf[j].arriveTime <= nowtime)//当多个进程在这一时刻都已经到达，选取其中所需时间短的
				if (sjf[j].runningTime < sjf[i].runningTime)//将时间短的排到前面
				{
					SJF tmp = sjf[j];
					sjf[j] = sjf[i];
					sjf[i] = tmp;
				}
		sjf[i].endTime = nowtime + sjf[i].runningTime;
		nowtime += sjf[i].runningTime;
	}
	return;
}

AlgType dealSJF(SJFType arriveTime, SJFType runningTime, SJFType beginTime, SJFType endTime, SJFType turnaroundTime, SJFType weightedTime)//计算运行结果
{
	sjf[0].beginTime = sjf[0].arriveTime;
	sjf[0].endTime = sjf[0].arriveTime + sjf[0].runningTime;
	for (int k = 1; k < N; ++k)
	{
		sjf[k].beginTime = sjf[k - 1].endTime;// current beginTime = previous endTime
		sjf[k].endTime = sjf[k - 1].endTime + sjf[k].runningTime;// current endTime = previous endTime + current runningTime
	}
	LOOP(k)
	{
		sjf[k].turnaroundTime = sjf[k].endTime - sjf[k].arriveTime;// turnaroundTime = endTime - arriveTime
		if (sjf[k].runningTime)
			sjf[k].weightedTime = (double)sjf[k].turnaroundTime / sjf[k].runningTime;// weightedTime = turnaroundTime / runningTime
	}
	return;
}

AlgType PrintSJF()
{
	cout << "/**************************************** Results of SJF Algorithm ****************************************/" << endl;
	cout << setw(MAX_WIDTH) << "ORDER" << setw(MAX_WIDTH) << "PID" << setw(MAX_WIDTH) << "ARRIVE" << setw(MAX_WIDTH) << "RUNNING" << setw(MAX_WIDTH) << "BEGIN" << setw(MAX_WIDTH) << "END" << setw(MAX_WIDTH) << "TURNAROUND" << setw(MAX_WIDTH) << "WEIGHTED" << endl;
	double aver_turnaroundTime = 0, aver_weightedTime = 0;
	LOOP(k)
	{
		cout << setw(MAX_WIDTH) << (k + 1) << setw(MAX_WIDTH) << sjf[k].pid << setw(MAX_WIDTH) << sjf[k].arriveTime << setw(MAX_WIDTH) << sjf[k].runningTime << setw(MAX_WIDTH) << sjf[k].beginTime << setw(MAX_WIDTH) << sjf[k].endTime << setw(MAX_WIDTH) << sjf[k].turnaroundTime << setw(MAX_WIDTH) << sjf[k].weightedTime << endl;
		aver_turnaroundTime += sjf[k].turnaroundTime;
		aver_weightedTime += sjf[k].weightedTime;
	}
	aver_turnaroundTime /= N;
	cout << endl << "The arithmetic average of turnaround time is " << aver_turnaroundTime << ", and the weighted one of that is " << aver_weightedTime << ". " << endl;
	return;
}

AlgType runSJF()
{
	SJFType arriveTime = 0, runningTime = 0, beginTime = 0, endTime = 0, turnaroundTime = 0, weightedTime = 0;
	sortSJF();
	dealSJF(arriveTime, runningTime, beginTime, endTime, turnaroundTime, weightedTime);
	PrintSJF();
	return;
}



int main()
{
	srand((unsigned int)(time(NULL)));
	initial();
	cout << endl << endl;
	runSJF();
	cout << "The SJF algorithm performance finished. Press any key to exit. " << endl;
	char ch = _getch();
	return EXIT_SUCCESS;
}