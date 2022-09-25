#include <iostream>
#include <vector>
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
#ifndef MIN_SERVICE
#define MIN_SERVICE 10
#endif
#ifndef MAX_SERVICE
#define MAX_SERVICE 50
#endif
#ifndef MAX_WIDTH
#define MAX_WIDTH 12
#endif
#ifndef MAX_ITEM
#define MAX_ITEM 3
#endif
using namespace std;

int getRandom(int a, int b)
{
	return (rand() % (b - a + 1)) + a;
}

void initial(vector<int>& ArrivalTime, vector<double>& ServiceTime)
{
	LOOP(i)
	{
		ArrivalTime.push_back(getRandom(MIN_ARRIVE, MAX_ARRIVE));
		ServiceTime.push_back(getRandom(MIN_SERVICE, MAX_SERVICE));
	}
	cout << "/*************** HRRN Algorithm ***************/" << endl;
	cout << setw(MAX_WIDTH) << "PID" << setw(MAX_WIDTH) << "ARRIVE" << setw(MAX_WIDTH) << "SERVICE" << endl;
	LOOP(i)
		cout << setw(MAX_WIDTH) << (i + 1) << setw(MAX_WIDTH) << ArrivalTime[i] << setw(MAX_WIDTH) << ServiceTime[i] << endl;
	return;
}

void HRRN(vector<int> T, vector<double> S, vector<int>& P, vector<int>& FT, vector<int>& WT, vector<double>& WWT)// HRRN 高相应比优先级调度算法
{
	int CurTime = 0, temp = 0, length = (int)T.size();
	vector<bool> Finished(length, true);
	vector<double> Rp(length, 0);//优先级
	for (int i = 0; i < length; ++i)
	{
		for (int i = 0; i < length; ++i)//迭代计算未进行进程的全部优先级
			if (Finished[i])
			{
				Rp[i] = (abs(T[i] - CurTime) + S[i]) / S[i];
				if (Rp[i] > Rp[temp])
					temp = i;//寻找最高优先级
			}
		while (CurTime < T[temp])//输出当前时间进程状态
		{
			cout << "Time " << setw(MAX_ITEM) << CurTime << ": No Program is Running. " << endl;
			++CurTime;
		}
		for (int t = 1; t <= S[temp]; ++t)
		{
			++CurTime;
			cout << "Time " << setw(MAX_ITEM) << CurTime << ": Program " << temp << " is Running. " << endl;
		}
		P.push_back(temp);
		cout << "Time " << setw(MAX_ITEM) << (CurTime + 1) << ": Program " << temp << " is already done. " << endl;
		FT[temp] = CurTime;
		WT[temp] = CurTime - T[temp];
		WWT[temp] = WT[temp] / S[temp];
		Finished[temp] = 0;
		Rp[temp] = 0;//完成该进程后使其初始化
	}
	return;
}

void calc(vector<int> T, vector<double> S, vector<int> P, vector<int> FT, vector<int> WT, vector<double> WWT)//计算相关时间
{
	cout << "/**************************************** Results of HRRN Algorithm ****************************************/" << endl;
	cout << setw(MAX_WIDTH) << "ORDER" << setw(MAX_WIDTH) << "PID" << setw(MAX_WIDTH) << "ARRIVE" << setw(MAX_WIDTH) << "SERVICE" << setw(MAX_WIDTH) << "START" << setw(MAX_WIDTH) << "FINISH" << setw(MAX_WIDTH) << "TURNAROUND" << setw(MAX_WIDTH) << "WEIGHTED" << endl;
	double aver_turnaroundTime = 0, aver_weightedTime = 0;
	LOOP(i)
	{
		cout << setw(MAX_WIDTH) << (i + 1) << setw(MAX_WIDTH) << P[i] << setw(MAX_WIDTH) << T[P[i]] << setw(MAX_WIDTH) << S[P[i]] << setw(MAX_WIDTH) << (FT[P[i]] - S[P[i]]) << setw(MAX_WIDTH) << FT[P[i]] << setw(MAX_WIDTH) << WT[P[i]] << setw(MAX_WIDTH) << WWT[P[i]] << endl;
		aver_turnaroundTime += WT[P[i]];
		aver_weightedTime += WWT[P[i]];
	}
	aver_turnaroundTime /= N;
	cout << endl << "The arithmetic average of turnaround time is " << aver_turnaroundTime << ", and the weighted one of that is " << aver_weightedTime << ". " << endl;
	return;
}

int main()
{
	vector<int> ArrivalTime;
	vector<double> ServiceTime;
	vector<int> PServiceTime;
	vector<int> FinishTime(N, 0);
	vector<int> WholeTime(N, 0);
	vector<double> WeightWholeTime(N, 0);

	srand((unsigned int)(time(NULL)));
	initial(ArrivalTime, ServiceTime); //初始化函数
	cout << endl;
	HRRN(ArrivalTime, ServiceTime, PServiceTime, FinishTime, WholeTime, WeightWholeTime);
	cout << endl;
	calc(ArrivalTime, ServiceTime, PServiceTime, FinishTime, WholeTime, WeightWholeTime);
	cout << "The HRRN algorithm performance finished. Press any key to exit. " << endl;
	char ch = _getch();

	return EXIT_SUCCESS;
}