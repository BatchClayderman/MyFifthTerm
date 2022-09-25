#include <iostream>
#include <iomanip>
#ifndef _VM_H
#define _VM_H
#define MaxSequenceLen 20//定义页面访问序列长度
#define MaxNumOfPages 10//定义最大页面数
#define NumAllocatedPages 3//定义 OS 分配给进程的最大页数
#define PageOffSet 1//页面数的最小值
#endif//_VM_H
using namespace std;
int Sequence[MaxSequenceLen];//保存页面访问序列的一维数组
int PageAccessTime[MaxNumOfPages];//记录页面访问时间的一维数组
int PageArray[NumAllocatedPages];//系统分配给进程的页面一维数组


bool isInside(int page)//检查指定页面是否在内存中
{
	for (int i = 0; i < NumAllocatedPages; ++i)
		if (page == PageArray[i])
			return true;//遍历内存，若存在查找的页面则返回 true
	return false;
}

int getLRUPage()//定义函数获取内存中的最近最少使用页面
{	
	int LRUPage = 0;//定义变量记录 LRU 页面
	int LRUTime = 65535;//定义变量记录 LRU 页面的最近使用时间
	for (int i = 0; i < NumAllocatedPages; ++i)//遍历内存并找到最近使用时间最小的页面
		if (PageAccessTime[PageArray[i]] < LRUTime)
		{
			LRUPage = i;
			LRUTime = PageAccessTime[PageArray[i]];
		}
	return LRUPage;
}

int getSpareSpace()//定义函数遍历内存以查找空余位置
{
	for (int i = 0; i < NumAllocatedPages; ++i)//查找成功则返回空闲位置，否则返回 EOF
		if (0 == PageArray[i])
			return i;
	return EOF;
}



int main()
{
	/* 初始化 */
	srand((unsigned int)time(NULL));//初始化随机数种子
	int missTime = 0;//记录未命中次数
	int index = EOF;//记录系统分配页面位置
	bool hit = false;//标记是否被命中
	for (int i = 0; i < MaxSequenceLen; ++i)//随机生成页面访问序列
		Sequence[i] = rand() % MaxNumOfPages + PageOffSet;

	/* 输出生成的页面访问序列 */
	cout << "Page Sequence: ";
	for (int i = 0; i < MaxSequenceLen; ++i)
		cout << Sequence[i] << " ";
	cout << endl << endl;
	
	/* 主过程 */
	cout << setw(8) << "SeqID" << setw(18) << "Working Set" << setw(11) << "Page" << endl;
	for (int i = 0; i < MaxSequenceLen; ++i)//遍历页面访问序列以实现对序列中的所有页面的访问
	{
		if (!isInside(Sequence[i]))//若要访问页面不在内存中则执行调入操作
		{
			hit = false;//修改命中标记为未命中
			++missTime;//未命中次数加一
			index = getSpareSpace();//查看内存中是否还有空闲位置
			if (index >= 0)//若索引结果大于等于 0 则表示仍有空余位置，直接调入页面
				PageArray[index] = Sequence[i];
			else//否则表示没有空余位置，需要获取 LRU 页面并完成替换
			{
				index = getLRUPage();
				PageArray[index] = Sequence[i];
			}
		}
		
		cout << setw(8) << (i + 1) << setw(8) << " ";//输出访问页面后内存的分配页面情况（(i + 1) 表示下一页）
		for (int i = 0; i < NumAllocatedPages; ++i)
			cout << setw(3) << PageArray[i];
		
		cout << setw(12) << Sequence[i];//输出本次访问的页面
		
		if (hit)//若访问页面在内存中则命中标记仍为命中，此时输出命中标识
			cout << "\t\t*hit*";
		cout << endl;
		
		PageAccessTime[Sequence[i] - PageOffSet] = i + 1;//将访问页面对应的访问时间更新为当前时间（下标对应闭区间减最小分页计数）
		hit = true;//重置命中标记为命中状态
	}
	cout << endl << endl;
	
	/* 输出命中、未命中次数以及缺页率（未命中率） */
	cout << "Hit = " << MaxSequenceLen - missTime << ", Miss = " << missTime << endl;
	cout << "Page Fault Rate = " << missTime << " / " << MaxSequenceLen << " = " << missTime / (double)MaxSequenceLen << endl;
	return EXIT_SUCCESS;
}