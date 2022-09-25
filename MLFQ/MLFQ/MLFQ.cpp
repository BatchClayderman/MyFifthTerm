#include <iostream>
#include <Windows.h>
#include <string>
#include <conio.h>
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif
#ifndef NULL
#define NULL 0
#endif
#ifndef MAX_PATH
#define MAX_PATH 260
#endif
#ifndef _MLFQ_H
#define _MLFQ_H
#define numOfProcs 20				// Total number of processes
#define numOfQueues 5				// Total number of run queues
#define waitTime 50					// Fixed waitTime
#define minTimeSlice 10				// The time slice of the first queue is 10 ms
#define neededTimeLB 2				// lower bound of neededTime
#define neededTimeUB 200			// upper bound of neededTime
#define colorGenerator Color::Light_Blue
#define colorScheduler Color::Light_Red
#define colorExecutor Color::Light_Green
#endif//_MLFQ_H
using namespace std;
int numOfGenProcs = 0;				// Number of generated processes so far;
int nextArrivalTime = 0;			// Next time point a new process arrives
int numOfFinishedProcs = 0;			// Number of finished processes
int sumOfWaitTime = 0;				// The accumulated total wait time of all the processes
HANDLE SemProc;						// SemProc: semaphore to indicate whether there are processes ready to run
HANDLE SemQueue;					// SemQueue: semaphore to indicate whether the run queues are being used by other thread
HANDLE SemPrint;					// SemPrint: semaphore to optimize the UI


/* structure */
struct PCB
{
	DWORD pid;						// PID
	char state;						// { 'r': "running", 'w': "waiting", 'e': "end", 'b': "block" }
	unsigned int priority;			// Priorities
	unsigned int arrivalTime;
	unsigned int neededTime;
	unsigned int usedTime;
	unsigned int totalWaitTime;
	struct PCB* next;
};

struct Queue
{
	unsigned int priority;			// priority of the queue
	unsigned int timeSlice;			// time slice of current queue;  time slice of the i-th queue is twice of that of the (i+1)-th queue 
	struct PCB* list;				// point to the begining of the list of processes
};
struct Queue queue[numOfQueues];	// define the run queues


/* sub-sub-functions */
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

void echo(string str, short int x)//输出流
{
	WaitForSingleObject(SemPrint, INFINITE);
	if (x >= Color::Black && x <= Color::Bright_White)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), x);
	cout << str << endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), Color::Light_Yellow);//归还默认色
	ReleaseSemaphore(SemPrint, 1, NULL);
	return;
}

void press_any_key_to_exit()
{
	rewind(stdin);
	fflush(stdin);
	char ch = _getch();
	return;
}


/* sub-functions */
DWORD WINAPI Generator(LPVOID)
{
	for (int i = 0; i < numOfProcs; ++i)
	{
		/* Generate a new process */
		struct PCB* tmpPCB = new struct PCB;
		tmpPCB->pid = i;
		tmpPCB->state = 'w';
		tmpPCB->priority = 1;// Initially, the priority of a new process is set to be the same as the first queue, which is 1. 
		tmpPCB->neededTime = rand() % (neededTimeUB - neededTimeLB + 1) + neededTimeLB;// random neededTime between range [neededTimeLB, neededTimeUB]
		tmpPCB->totalWaitTime = 0;
		tmpPCB->usedTime = 0;
		tmpPCB->arrivalTime = nextArrivalTime;
		tmpPCB->next = NULL;
		echo("Generator: Process " + to_string(tmpPCB->pid) + " is generated. Its neededTime is " + to_string(tmpPCB->neededTime) + ", and its arrivalTime is " + to_string(tmpPCB->arrivalTime) + ". ", colorGenerator);

		/* Insert the generated process to the first run queue, using semaphore/mutex mechanism while inserting. */
		WaitForSingleObject(SemQueue, INFINITE);
		if (NULL == queue[0].list)
			queue[0].list = tmpPCB;
		else
		{
			struct PCB* ptr = queue[0].list;
			while (ptr->next != NULL)
				ptr = ptr->next;
			ptr->next = tmpPCB;
		}
		ReleaseSemaphore(SemQueue, 1, NULL);
		ReleaseSemaphore(SemProc, 1, NULL);

		++numOfGenProcs;
		echo("Generator: Sleep for " + to_string(waitTime) + " ms before generating next new process... ", colorGenerator);
		Sleep(waitTime);// Sleep for arrivalInterval ms, before generating the next new process. 
		nextArrivalTime += waitTime;// Here is to get the arrival time for the next new process. 
	}
	return EXIT_SUCCESS;
}

struct PCB* Executor(int queueID)
{
	struct PCB* tmpPCB;

	if (0 == queueID)// Current process is in the 1st queue, so semaphore/mutex is required, because newly generated process will be put into the same queue. 
	{
		WaitForSingleObject(SemQueue, INFINITE);
		tmpPCB = queue[queueID].list;
		queue[queueID].list = tmpPCB->next;
		ReleaseSemaphore(SemQueue, 1, NULL);
	}
	else
	{
		tmpPCB = queue[queueID].list;
		queue[queueID].list = tmpPCB->next;
	}
	tmpPCB->next = NULL;
	tmpPCB->usedTime += queue[queueID].timeSlice;
	tmpPCB->state = 'r';
	
	echo("Executor: Process " + to_string(tmpPCB->pid) + " in queue " + to_string(queueID) + " consumes " + to_string(queue[queueID].timeSlice) + " ms. ", colorExecutor);
	Sleep(queue[queueID].timeSlice);

	struct PCB* pcb;
	for (int i = 0; i < numOfQueues; ++i)
	{
		if (NULL == queue[i].list)
			continue;

		pcb = queue[i].list;
		if (0 == i)
		{
			WaitForSingleObject(SemQueue, INFINITE);
			while (pcb != NULL)
			{
				pcb->totalWaitTime += queue[queueID].timeSlice;
				pcb = pcb->next;
			}
			ReleaseSemaphore(SemQueue, 1, NULL);
		}
		else
			while (pcb != NULL)
			{
				pcb->totalWaitTime += queue[queueID].timeSlice;
				pcb = pcb->next;
			}
	}
	return tmpPCB;
}

void Scheduler()
{
	for (;;) // instead of while(true)
	{
		if (numOfProcs == numOfFinishedProcs)
			break;

		if (0 == numOfGenProcs - numOfFinishedProcs)
			WaitForSingleObject(SemProc, INFINITE);

		for (int i = 0; i < numOfQueues; ++i)
		{
			if (NULL == queue[i].list)
				continue;

			struct PCB* currentProc = Executor(i);
			if (currentProc->usedTime >= currentProc->neededTime)// Current process finished, so remove it from the queue
			{
				currentProc->state = 'e';
				echo("Scheduler: Process " + to_string(currentProc->pid) + " finished. Its total wait time is " + to_string(currentProc->totalWaitTime) + ". ", colorScheduler);
				sumOfWaitTime += currentProc->totalWaitTime;
				++numOfFinishedProcs;
				delete currentProc;
			}
			else// Current process needs more time to run, so we remove it to the next queue
			{
				struct PCB* ptr = NULL;
				int j = (numOfQueues - 1 == i ? i : i + 1);
				
				if (NULL == queue[j].list)
					queue[j].list = currentProc;
				else
				{
					ptr = queue[j].list;
					while (ptr->next != NULL)
						ptr = ptr->next;
					ptr->next = currentProc;
				}
				if (currentProc)
				{
					currentProc->state = 'b';
					currentProc->priority = queue[j].priority;
					echo("Scheduler: Process " + to_string(currentProc->pid) + " is moved to" + (numOfQueues - 1 == i ? " the tail of " : " ") + "queue " + to_string(j) + ". Its priority is " + to_string(currentProc->priority) + ". ", colorScheduler);
				}
			}
			break;
		}
	}
	return;
}



/* main entry */
int main()
{
	HANDLE handle;
	DWORD ThreadID;

	srand((unsigned int)time(NULL));

	for (int i = 0; i < numOfQueues; ++i)
	{
		queue[i].priority = i + 1;
		queue[i].timeSlice = minTimeSlice << i;
		queue[i].list = NULL;
	}
	SemProc = CreateSemaphore(NULL, 0, 1, NULL);
	SemQueue = CreateSemaphore(NULL, 1, 1, NULL);
	SemPrint = CreateSemaphore(NULL, 1, 1, NULL);
	handle = CreateThread(NULL, 0, Generator, NULL, 0, &ThreadID);
	if (!handle)
	{
		echo("\n\nError creating thread(s)! Please press any key to exit. ", Color::Light_Yellow);
		press_any_key_to_exit();
		return EXIT_FAILURE;
	}

	echo("Starting performing, please enjoy it! \n\n", Color::Light_Yellow);
	Scheduler();

	WaitForSingleObject(handle, INFINITE);
	CloseHandle(SemProc);
	CloseHandle(SemQueue);
	CloseHandle(SemPrint);
	CloseHandle(handle);

	char result[MAX_PATH] = { 0 };
	sprintf_s(result, "Average waiting time is %.2lf, and press any key to exit. ", double(sumOfWaitTime) / numOfProcs);
	echo("\n\n\nAll prcoesses are completed! ", Color::Light_Yellow);
	echo(result, Color::Light_Yellow);
	press_any_key_to_exit();
	return EXIT_SUCCESS;
}