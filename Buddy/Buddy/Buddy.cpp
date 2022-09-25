#include <iostream>
#include <iomanip>
#include <conio.h>
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif
#ifndef EOF
#define EOF (-1)
#endif
#ifndef _BUDDY_H
#define _BUDDY_H 2//定义问题
#define totalSizeK 10
#define MAX_PROCESS_NUM 11
#if (_BUDDY_H == 1)
#define programNum 3
#define SIZEK { 7, 4, 8 }
#else
#define programNum 8
#define SizeUB 8
#define SizeLB 3
#endif
#define DELIMS "/************************************************************/"
#define MAX_WIDTH 20
#endif//_BUDDY_H
using namespace std;


struct Block
{
	int id;
	int sizeK;//分区的大小，以 2 的幂形式 2^sizeK，可写成 1 << sizeK
	int startAddr;
	int status;
	int pid;
	struct Block* prev;//前一个结点
	struct Block* next;//后一个结点
};

struct PCB//定义 PCB 结构体
{
	int pid;
	int neededSizeK;//需要的大小是 2 的幂形式
	int status;
	int blockId;
	struct PCB* next;//下一个结点
};
struct Block* blockList[MAX_PROCESS_NUM];// MAX_PROCESS_NUM 个进程


void errorMalloc()
{
	cout << endl << "Error allocating memory, please press any key to exit. " << endl;
	rewind(stdin);
	fflush(stdin);
	char ch = _getch();
	return;
}

void printState()//定义内存划分情况输出函数
{
	struct Block* temp;//定义 Block 指针指向分区
	for (int index = 0; index < MAX_PROCESS_NUM; ++index)//从头开始遍历分区列表
	{
		temp = blockList[index];
		while (temp)
		{
			if (0 == temp->status)//若分区存在且状态为空闲则输出
				cout << "free block id: " << temp->id << ", size: 2^" << temp->sizeK << " = " << (1 << temp->sizeK) << ", startAddr: " << temp->startAddr << endl;
			temp = temp->next;
		}
	}
	
	for (int index = 0; index < MAX_PROCESS_NUM; ++index)//从头开始遍历分区列表
	{
		temp = blockList[index];
		while (temp)
		{
			if (1 == temp->status)//若分区存在且状态为占用则输出
				cout << "used block id: " << temp->id << ", size: 2^" << temp->sizeK << " = " << (1 << temp->sizeK) << ", startAddr: " << temp->startAddr << ", pid: " << temp->pid << endl;
			temp = temp->next;
		}
	}

	return;
}



int main()
{
#if (_BUDDY_H == 2)
	srand((unsigned int)time(NULL));//初始化随机数种子
#endif
	struct PCB* p = (struct PCB*)malloc(sizeof(struct PCB));//定义指针并分配空间以生成 PCB 链表头部
	if (!p)
	{
		errorMalloc();
		return EXIT_FAILURE;
	}
	struct PCB* tmpPCB;//定义指针 tmpPCB 暂存 PCB 块
	struct PCB* temp = p;//定义指针 temp 指向 PCB 链表队尾
	cout << DELIMS << endl;
	cout << setw(MAX_WIDTH) << "ProcessID" << setw(MAX_WIDTH) << "NeededSizeK" << endl;
	
#if (_BUDDY_H == 1)
	int sizeK[] = SIZEK;//存放进程需要的大小（已使用 lb 取对数）
#endif
	for (int i = 0; i < programNum; ++i)//依次生成 programNum 个 PCB 块
	{
		tmpPCB = (struct PCB*)malloc(sizeof(struct PCB));//分配 PCB 块空间
		if (!tmpPCB)
		{
			errorMalloc();
			return EXIT_FAILURE;
		}
		tmpPCB->pid = i;//设置进程的 id
#if (_BUDDY_H == 1)
		tmpPCB->neededSizeK = sizeK[i];//生成进程的所需大小
#else
		tmpPCB->neededSizeK = rand() % (SizeUB - SizeLB + 1) + SizeLB;//随机生成进程的所需大小
#endif
		tmpPCB->status = 0;//初始化进程状态为未执行状态
		tmpPCB->blockId = EOF;//初始化进程被分配至的区块 id 为 EOF
		tmpPCB->next = NULL;//初始化 PCB 的 next 域为空
		temp->next = tmpPCB;//将 PCB 块插入 PCB 链表尾部
		temp = temp->next;//指针移向 PCB 链表尾部
		cout << setw(MAX_WIDTH) << tmpPCB->pid << setw(MAX_WIDTH) << tmpPCB->neededSizeK << endl;
	}
	
	p = p->next;//使指针 p 指向 PCB 链表第一个 PCB 块
	struct PCB* pcbStart = p;//定义指针 pcbStart 记录 PCB 链表首部
	struct Block* b = (struct Block*)malloc(sizeof(struct Block));//定义指针 b 并分配空间以生成区块链表首部
	if (!b)
	{
		errorMalloc();
		return EXIT_FAILURE;
	}
	struct Block* blockTailed = b;//定义指针 blockTailed 记录区块链表尾部
	struct Block* pBlock;//定义指针 pBlock 用以指向区块
	struct Block* tmpBlock = (struct Block*)malloc(sizeof(struct Block));//定义指针 tmpBlock 用以指向区块
	if (!tmpBlock)
	{
		errorMalloc();
		return EXIT_FAILURE;
	}
	int i = 0;//定义变量 i 记录区块 id
	
	/* 生成第一个区块 */
	tmpBlock->id = i;
	tmpBlock->sizeK = totalSizeK;
	tmpBlock->startAddr = 0;
	tmpBlock->status = 0;
	tmpBlock->pid = EOF;
	tmpBlock->prev = NULL;
	tmpBlock->next = NULL;
	b->next = tmpBlock;//将区块插入区块链表尾部
	blockTailed = blockTailed->next;//指针指向区块链表尾部
	b = b->next;//将指针b指向区块链表第一个区块
	blockList[10] = b;//将建立的第一个区块放在K为10的队列中
	int index;//定义索引指向队列序号
	int flag;//定义flag标记是否分配成功
	
	/* 分配 */
	while (p)//遍历 PCB 链表
	{
		flag = 0;//初始化 flag 为 0
		index = p->neededSizeK;//获取程序需求大小的分区队列索引
		while (flag == 0)//当未分配成功时循环
		{
			while (!blockList[index] && index++ < 10);//向上查找直至找到非空队列（当查找到 10 时仍为空则查找失败）
			tmpBlock = blockList[index];//获取索引对应队列的队首
			while (tmpBlock)//当队首存在时寻找第一个空闲区块
			{
				if (0 == tmpBlock->status)
					break;
				tmpBlock = tmpBlock->next;
			}
			
			if (!tmpBlock)//若空闲区块不存在则寻找更大的区块
			{
				if (index >= 10)//若索引为 10 则分配失败
					break;
				else//使用 else 会快些
					++index;
			}
			else//否则分配成功
				flag = 1;
		}
		
		if (flag)//若能够分配成功
		{
			cout << endl << DELIMS << endl;
			cout << "\tAllocation -> PID: " << p->pid << " -> Memory: 2^" << p->neededSizeK << " = " << (1 << p->neededSizeK) << endl;
			cout << endl << DELIMS << endl;
			
			while (index > p->neededSizeK)//当空闲区块的大小大于需求的大小时执行循环
			{
				if (tmpBlock->prev)//若空闲区块存在前驱结点则重设前驱结点的后驱
					tmpBlock->prev->next = tmpBlock->next;
				else//否则设置队首为其后驱结点
					blockList[index] = tmpBlock->next;
				if (tmpBlock->next)//若空闲区块存在后驱结点则重设后驱结点的前驱
					tmpBlock->next->prev = tmpBlock->prev;
				tmpBlock->prev = NULL;//空闲区块指针重置
				--tmpBlock->sizeK;//区块大小减半
				++i;//区块索引加一
				pBlock = (struct Block*)malloc(sizeof(struct Block));//分配空间生成新区块
				
				/* 初始化区块 */
				pBlock->id = i;
				pBlock->pid = EOF;
				pBlock->sizeK = tmpBlock->sizeK;
				pBlock->status = 0;
				pBlock->startAddr = tmpBlock->startAddr + (1 << pBlock->sizeK);
				
				/* 链接两个空闲区块 */
				tmpBlock->next = pBlock;
				pBlock->prev = tmpBlock;
				pBlock->next = NULL;
				cout << "Devide Block " << tmpBlock->id << " into Block " << tmpBlock->id << " and " << pBlock->id << " of size 2^" << tmpBlock->sizeK << "..." << endl;
				--index;//索引减一以便移至下一队列
				pBlock = blockList[index];//获取下一队列的队首
				if (pBlock)//若队列不为空则将空闲区块插入队尾
				{
					while (pBlock->next)
						pBlock = pBlock->next;
					pBlock->next = tmpBlock;
					tmpBlock->prev = pBlock;
				}
				else//否则设置为队首
					blockList[index] = tmpBlock;
			}
			
			/* 当获取到大小匹配的空闲区块时设置区块及进程信息 */
			tmpBlock->pid = p->pid;
			tmpBlock->status = 1;
			p->blockId = tmpBlock->id;
			p->status = 1;
			printState();
		}
		else//若区块分配失败则输出分配错误并设置进程状态
		{
			p->status = EOF;
			cout << endl << DELIMS << endl;
			cout << "\tFailed to Allocate -> PID: " << p->pid << " -> Memory: 2^" << p->neededSizeK << " = " << (1 << p->neededSizeK) << endl;
			cout << endl << DELIMS << endl;
			printState();
		}
		p = p->next;//寻找下一进程
	}
	
	/* 恢复 */
	p = pcbStart;//使指针p重新指向PCB队列首部
	int buddyAddr;//定义变量暂存兄弟区块起始地址
	
	while (p)//遍历所有 PCB
	{
		if (1 == p->status)//若进程状态为正在执行则释放
		{
			cout << endl << DELIMS << endl;
			cout << "\tRecycle -> PID: " << p->pid << " -> Memory: 2^" << p->neededSizeK << " = " << (1 << p->neededSizeK) << endl;
			cout << endl << DELIMS << endl;
			tmpBlock = blockList[p->neededSizeK];//获取当前进程所对应的队列首部
			while (tmpBlock)//遍历该队列
			{
				if (tmpBlock->id == p->blockId)//根据 ID 查找进程所占用的区块
				{
					if (tmpBlock->startAddr % (1 << (tmpBlock->sizeK + 1)) == 0)//根据区块起始地址计算其兄弟区块起始地址
						buddyAddr = tmpBlock->startAddr + (1 << tmpBlock->sizeK);
					else
						buddyAddr = tmpBlock->startAddr - (1 << tmpBlock->sizeK);
					pBlock = blockList[p->neededSizeK];//再次获取当前进程所对应的队列首部
					while (pBlock)//遍历队列以查找区块的兄弟区块
					{
						if (pBlock->startAddr == buddyAddr)
						{
							while (0 == pBlock->status)//当兄弟区块为空闲区块时合并区块
							{
								if (tmpBlock->prev)//若区块存在前驱结点则重设前驱结点的后驱
									tmpBlock->prev->next = tmpBlock->next;
								else//否则设置队首为其后驱结点
									blockList[tmpBlock->sizeK] = tmpBlock->next;
								if (tmpBlock->next)//若区块存在后驱结点则重设后驱结点的前驱
									tmpBlock->next->prev = tmpBlock->prev;
								
								/* 重置区块指针 */
								tmpBlock->prev = NULL;
								tmpBlock->next = NULL;
								if (pBlock->prev)//若兄弟区块存在前驱结点则重设前驱结点的后驱
									pBlock->prev->next = pBlock->next;
								else//否则设置队首为其后驱结点
									blockList[pBlock->sizeK] = pBlock->next;
								
								if (pBlock->next)//若兄弟区块存在后驱结点则重设后驱结点的前驱
									pBlock->next->prev = pBlock->prev;
								pBlock->prev = NULL;//重置兄弟区块指针
								pBlock->next = NULL;
								cout << "Combine Block " << tmpBlock->id << " and " << pBlock->id << " of size 2^" << tmpBlock->sizeK << "..." << endl;
								
								++tmpBlock->sizeK;//区块大小翻倍
								if (tmpBlock->startAddr > pBlock->startAddr)//选取两区块的起始地址较小者作为合并后区块的起始地址
									tmpBlock->startAddr = pBlock->startAddr;
								
								/* 获取区块所对应的队列首部 */
								pBlock = blockList[tmpBlock->sizeK];
								if (pBlock)//若队列不为空则将空闲区块插入队尾
								{
									while (pBlock->next)
										pBlock = pBlock->next;
									pBlock->next = tmpBlock;
									tmpBlock->prev = pBlock;
								}
								else//否则设置为队首
									blockList[tmpBlock->sizeK] = tmpBlock;
								
								/* 重新设置区块信息 */
								tmpBlock->status = 0;
								tmpBlock->pid = EOF;
								buddyAddr = tmpBlock->startAddr + ((tmpBlock->startAddr % (1 << (tmpBlock->sizeK + 1)) == 0) ? (1 << tmpBlock->sizeK) : -(1 << tmpBlock->sizeK));//重新计算合并后区块的兄弟区块起始地址
								
								/* 查找该兄弟区块 */
								pBlock = blockList[tmpBlock->sizeK];
								while (pBlock)
								{
									if (pBlock->startAddr == buddyAddr)
										break;
									pBlock = pBlock->next;
								}
								
								if (!pBlock)//若查找失败则退出循环，否则继续尝试合并
									break;
							}
							/* 用作未发生合并时设置区块信息 */
							tmpBlock->status = 0;
							tmpBlock->pid = EOF;
							break;
						}
						pBlock = pBlock->next;
					}
					tmpBlock->status = 0;
					tmpBlock->pid = EOF;
					break;
				}
				tmpBlock = tmpBlock->next;
			}
			
			/* 修改进程状态及 blockID */
			p->status = 0;
			p->blockId = EOF;
			printState();//输出区块链表分配情况
		}
		p = p->next;//查看下一进程
	}
	return EXIT_SUCCESS;
}