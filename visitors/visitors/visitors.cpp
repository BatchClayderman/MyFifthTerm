#include <iostream>
#include <Windows.h>
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
#ifndef _VISITOR_H
#define _VISITOR_H
#define MAX_VISITOR 10
#define ALL_VISITOR 20
#define LOOP(x) for (DWORD x = 0; x < ALL_VISITOR; ++x)
#define MAX_STAY 60
#define MIN_STAY 30
#define MAX_WALK 2
#define MIN_WALK 1
#define MAX_PATIENCE 600
#define MIN_PATIENCE 120
#define Light_Red 0xC
#define Light_Yellow 0xE
#define Light_Green 0xA
#endif//_VISITOR_H
#pragma warning(disable:6001)
using namespace std;
HANDLE semaphore, sem_exhibition;
CRITICAL_SECTION cs, cs_exhibition, cs_print;
DWORD global_n = ALL_VISITOR;


enum class State
{
	waiting_for_entering,
	entering,
	entered,
	exhibiting,
	waiting_for_exiting,
	exiting,
	finished,
	abundant
};


void echo(DWORD id, bool bSuccess)
{
	EnterCriticalSection(&cs_print);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), bSuccess ? Light_Green : Light_Red);
	if (bSuccess)
		cout << "Note: Visitor " << id << " has finished his or her journey happily. " << endl;
	else
		cout << "Warning: Visitor " << id << " wouldn't come again due to the narrow gangway. " << endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), Light_Yellow);// Restore
	LeaveCriticalSection(&cs_print);
	return;
}

void press_any_key_to_exit()
{
	rewind(stdin);
	fflush(stdin);
	char ch = _getch();
	return;
}


class Visitor
{
private:
	State state = State::waiting_for_entering;
	DWORD walk = rand() % (MAX_WALK - MIN_WALK + 1) + MIN_WALK;
	DWORD stay = rand() % (MAX_STAY - MIN_STAY + 1) + MIN_STAY;
	DWORD patience = rand() % (MAX_PATIENCE - MIN_PATIENCE + 1) + MIN_PATIENCE;
	bool Enter()
	{
		if (WaitForSingleObject(semaphore, patience))// Wait for the exhibition
		{
			this->state = State::abundant;// Wait too long time
			return false;
		}
		EnterCriticalSection(&cs);
		this->state = State::entering;
		Sleep(this->walk);
		this->state = State::entered;
		LeaveCriticalSection(&cs);
		return true;
	}
	void Visit()
	{
		this->state = State::exhibiting;
		Sleep(this->stay);
		return;
	}
	void Exit()
	{
		this->state = State::waiting_for_exiting;
		EnterCriticalSection(&cs);
		this->state = State::exiting;
		Sleep(this->walk);
		this->state = State::finished;
		LeaveCriticalSection(&cs);
		ReleaseSemaphore(semaphore, 1, NULL);// Leave
		return;
	}
public:
	DWORD id = 0;
	void Go()
	{
		if (this->Enter())
		{
			this->Visit();
			this->Exit();
			echo(this->id, true);
		}
		else
			echo(this->id, false);
		return;
	}
};

DWORD WINAPI go(LPVOID lpp)
{
	Visitor visitor;
	visitor.id = *(DWORD*)lpp;
	visitor.Go();
	EnterCriticalSection(&cs_exhibition);
	if (!--global_n)
		ReleaseSemaphore(sem_exhibition, 1, NULL);
	LeaveCriticalSection(&cs_exhibition);
	return EXIT_SUCCESS;
}



int main()
{
	srand((unsigned)time(NULL));
	InitializeCriticalSection(&cs);
	InitializeCriticalSection(&cs_exhibition);
	InitializeCriticalSection(&cs_print);
	semaphore = CreateSemaphore(NULL, MAX_VISITOR, MAX_VISITOR, NULL);
	sem_exhibition = CreateSemaphore(NULL, 0, 1, NULL);
	if (semaphore && sem_exhibition)
		cout << "Performance begins with ratio at " << MAX_VISITOR << " / " << ALL_VISITOR << ". " << endl << endl << endl;
	else
	{
		cout << "Error initialing semaphore, please press any key to exit. " << endl << endl;
		press_any_key_to_exit();
		return EXIT_FAILURE;
	}

	HANDLE hThread[ALL_VISITOR] = { NULL };
	DWORD pid[ALL_VISITOR] = { NULL };
	LOOP(i)
	{
		pid[i] = i;
		hThread[i] = CreateThread(NULL, NULL, go, &pid[i], NULL, NULL);
		if (!hThread[i])
		{
			cout << "Error initialing threads, please press any key to exit. " << endl << endl;
			press_any_key_to_exit();
			return EXIT_FAILURE;
		}
	}
	
	WaitForSingleObject(sem_exhibition, INFINITE);// all people's actions were disposed
	EnterCriticalSection(&cs_exhibition);
	LOOP(i)
		CloseHandle(hThread[i]); 
	LeaveCriticalSection(&cs_exhibition);
	CloseHandle(semaphore);
	CloseHandle(sem_exhibition);
	cout << endl << endl << endl << "Performance finished, please press any key to exit. " << endl;
	press_any_key_to_exit();
	return EXIT_SUCCESS;
}