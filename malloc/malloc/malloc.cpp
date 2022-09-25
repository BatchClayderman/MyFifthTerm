#include <iostream>
#include <conio.h>
#include <iomanip>
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif
#ifndef EOF
#define EOF (-1)
#endif
#ifndef NULL
#define NULL 0
#endif
#ifndef _MALLOC_H
#define _MALLOC_H
#define MaxMem 1024
#define NumOfProcs 20
#define MemUB 200
#define MemLB 100
#define SUCCESS 1
#define FAIL (-1)
#define SEP_LINE "********************************************************************************"
#define MAX_WIDTH 16
#endif//_MALLOC_H
using namespace std;
int BlkID = NULL;


struct Block
{
	int id;				//分区的序号
	int size;			//分区的大小
	int startAddr;		//分区的开始位置
	bool status;		//分区的状态: true 为空闲，false 为被占用
	int pid;			//如果该分区被占用, 则存放占用进程的 ID，否则为 -1
	struct Block* prev;	//指向前面一块内存分区
	struct Block* next;	//指向后面一块内存分区
};
Block* freeBlk = NULL, * usedBlk = NULL;

struct PCB
{
	int pid;			//进程的序号
	int neededMem;		//需要的内存分区大小
	int status;			// 1：内存分配成功，-1：分配失败，0：未分配
	int blockID;		//如果分配成功, 保存占用分区的 id, 否则为 -1
	struct PCB* next;	//指向下一个 PCB
};
PCB process[NumOfProcs];


short int Allocate(PCB* procs) // 申请
{
	Block* ptrFree = freeBlk;

	while (ptrFree != NULL)
	{
		if (ptrFree->size < procs->neededMem)
			ptrFree = ptrFree->next;
		else
			break;
	}
	if (ptrFree == NULL) // 如果没有大于或等于当前进程大小的自由块
	{
		procs->status = FAIL;
		return FAIL; // 申请内存失败
	}

	if (ptrFree->size == procs->neededMem)
	{
		ptrFree->pid = procs->pid;
		ptrFree->status = false;
		if (NULL == ptrFree->prev)
		{
			freeBlk = ptrFree->next; // FT
			freeBlk->prev = NULL; // FT
			ptrFree->prev = NULL;
		}
		else
		{
			ptrFree->prev->next = ptrFree->next;
			ptrFree->next->prev = ptrFree->prev;
			ptrFree->next = NULL;
			ptrFree->prev = NULL;
		}
	}
	else
	{
		int splitLB = 0, splitUB = ptrFree->size - procs->neededMem, position = rand() % (splitUB - splitLB) + 1;
		if (ptrFree->size - position == procs->neededMem) // 情形一：自由区块被划分为两个
		{
			ptrFree->size = position;

			Block* tmpUsedBlk = new Block;
			tmpUsedBlk->id = BlkID++; // 先赋值再自加，下同
			tmpUsedBlk->size = procs->neededMem;
			tmpUsedBlk->startAddr = ptrFree->startAddr + position;
			tmpUsedBlk->pid = procs->pid;
			tmpUsedBlk->status = false;
			tmpUsedBlk->next = NULL;
			tmpUsedBlk->prev = NULL;

			ptrFree = tmpUsedBlk;
		}
		else if (ptrFree->size - position > procs->neededMem) // 情形二：自由区块被分为三部分
		{
			Block* tmpFreeBlk = new Block;
			tmpFreeBlk->id = BlkID++;
			tmpFreeBlk->size = ptrFree->size - position - procs->neededMem;
			tmpFreeBlk->startAddr = ptrFree->startAddr + position + procs->neededMem;
			tmpFreeBlk->pid = EOF;
			tmpFreeBlk->status = true;
			tmpFreeBlk->next = ptrFree->next;
			tmpFreeBlk->prev = ptrFree;
			if (ptrFree->next != NULL)
				ptrFree->next->prev = tmpFreeBlk;
			ptrFree->next = tmpFreeBlk;

			Block* tmpUsedBlk = new Block;
			tmpUsedBlk->id = BlkID++;
			tmpUsedBlk->size = procs->neededMem;
			tmpUsedBlk->startAddr = ptrFree->startAddr + position;
			tmpUsedBlk->pid = procs->pid;
			tmpUsedBlk->status = false;
			tmpUsedBlk->next = NULL;
			tmpUsedBlk->prev = NULL;

			ptrFree->size = position;
			ptrFree = tmpUsedBlk;
		}
	}

	/* 将已分配的区块插入到已使用区块的链接列表的尾部 */
	if (NULL == usedBlk)
		usedBlk = ptrFree;
	else
	{
		Block* tmpUsedPtr = usedBlk;
		while (tmpUsedPtr->next != NULL)
			tmpUsedPtr = tmpUsedPtr->next;

		tmpUsedPtr->next = ptrFree;
		ptrFree->prev = tmpUsedPtr;
	}

	return SUCCESS;
}

void Recycle(Block* usedBlkPtr) // 回收
{
	Block* freeBlkPtr;

	freeBlkPtr = freeBlk;
	while (freeBlkPtr != NULL)
	{
		if (freeBlkPtr->startAddr + freeBlkPtr->size == usedBlkPtr->startAddr)
		{
			freeBlkPtr->size += usedBlkPtr->size;
			usedBlk = usedBlkPtr->next;
			if (usedBlkPtr->next != NULL)
				usedBlkPtr->next->prev = NULL;
			delete usedBlkPtr;
			break;
		}
		else if (
			freeBlkPtr->startAddr + freeBlkPtr->size < usedBlkPtr->startAddr
			&& freeBlkPtr->next->startAddr + freeBlkPtr->next->size > usedBlkPtr->startAddr
		)
		{
			usedBlk = usedBlkPtr->next;
			if (usedBlkPtr->next != NULL)
				usedBlkPtr->next->prev = NULL;
			usedBlkPtr->next = freeBlkPtr->next;
			usedBlkPtr->prev = freeBlkPtr;
			freeBlkPtr->next = usedBlkPtr;
			if (freeBlkPtr->next != NULL)
				freeBlkPtr->next->prev = usedBlkPtr;
			break;
		}
		freeBlkPtr = freeBlkPtr->next;
	}

	return;
}

void printMemory()
{
	Block* usedBlkPtr, * freeBlkPtr;
	usedBlkPtr = usedBlk;
	freeBlkPtr = freeBlk;

	if (freeBlkPtr != NULL)
		cout << setw(MAX_WIDTH) << "FreeBlkID" << setw(MAX_WIDTH) << "StartAddr" << setw(MAX_WIDTH) << "Size" << endl;
	while (freeBlkPtr != NULL)
	{
		cout << setw(MAX_WIDTH) << freeBlkPtr->id << setw(MAX_WIDTH) << freeBlkPtr->startAddr << setw(MAX_WIDTH) << freeBlkPtr->size << endl;
		freeBlkPtr = freeBlkPtr->next;
	}
	if (usedBlkPtr != NULL)
		cout << setw(MAX_WIDTH) << "UsedBlkID" << setw(MAX_WIDTH) << "StartAddr" << setw(MAX_WIDTH) << "Size" << setw(MAX_WIDTH) << "ProcID" << endl;
	while (usedBlkPtr != NULL)
	{
		cout << setw(MAX_WIDTH) << usedBlkPtr->id << setw(MAX_WIDTH) << usedBlkPtr->startAddr << setw(MAX_WIDTH) << usedBlkPtr->size << setw(MAX_WIDTH) << usedBlkPtr->pid << endl;
		usedBlkPtr = usedBlkPtr->next;
	}

	return;
}



int main()
{
	srand((unsigned int)time(NULL));//初始化随机数种子

	/* 初始化进程 */
	cout << setw(MAX_WIDTH) << "ProcessID" << setw(MAX_WIDTH) << "NeededMem" << endl;
	for (int i = 0; i < NumOfProcs; ++i)
	{
		process[i].pid = i;
		process[i].neededMem = rand() % (MemUB - MemLB) + MemLB;
		process[i].blockID = EOF;
		process[i].status = 0;
		process[i].next = NULL;
		cout << setw(MAX_WIDTH) << process[i].pid << setw(MAX_WIDTH) << process[i].neededMem << endl;
	}

	/* 初始化内存块 */
	freeBlk = new Block;
	freeBlk->id = BlkID++;
	freeBlk->pid = EOF;
	freeBlk->size = MaxMem;
	freeBlk->startAddr = 0;
	freeBlk->status = true;
	freeBlk->prev = NULL;
	freeBlk->next = NULL;
	cout << SEP_LINE << endl;
	printMemory();

	/* 分配内存 */
	for (int i = 0; i < NumOfProcs; ++i)
	{
		cout << SEP_LINE << endl;
		cout << "Allocating memory for process " << process[i].pid << ", memory requirement = " << process[i].neededMem << ". " << endl;
		int res = Allocate(&process[i]);
		if (res != SUCCESS)
			cout << "Memory allocation failed for process " << process[i].pid << "! " << endl;
		printMemory();
	}

	/* 回收内存 */
	Block* tmpUsedBlkPtr = usedBlk;
	while (tmpUsedBlkPtr != NULL)
	{
		cout << SEP_LINE << endl;
		cout << "Recycling used memory block for process " << tmpUsedBlkPtr->pid << ". " << endl;
		Recycle(tmpUsedBlkPtr);
		printMemory();
		tmpUsedBlkPtr = usedBlk;
	}
	Block* tmpFreeBlkPtr = freeBlk->next;
	while (freeBlk->next != NULL)
	{
		freeBlk->size += tmpFreeBlkPtr->size;
		freeBlk->next = tmpFreeBlkPtr->next;
		if (tmpFreeBlkPtr->next != NULL)
			tmpFreeBlkPtr->next->prev = freeBlk;
		delete tmpFreeBlkPtr;
		cout << SEP_LINE << endl;
		tmpFreeBlkPtr = freeBlk->next;
		printMemory();
	}

	cout << endl << endl << "Recycling finished, press any key to exit. " << endl;
	rewind(stdin);
	fflush(stdin);
	char ch = _getch();
	return EXIT_SUCCESS;
}