#include <iostream>
#include <vector>
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
#ifndef EOF
#define EOF (-1)
#endif
#ifndef _QuickSort_H
#define _QuickSort_H
#define SEPRATE ' '
#endif//_QuickSort_H
using namespace std;
typedef double NodeType;


/* 输入字符串并排序 */
void rfstdin()//清空缓冲区
{
	rewind(stdin);
	fflush(stdin);
	return;
}

vector<NodeType> split(string strArray, char seprate)
{
	char lastChar = seprate;
	string s = "";
	for (size_t i = 0; i < strArray.size(); ++i)
		if ((strArray[i] >= '0' && strArray[i] <= '9') || '.' == strArray[i] || '-' == strArray[i])//合法的值（连续小数点由 atof 处理）
		{
			s += strArray[i];
			lastChar = strArray[i];
		}
		else if (lastChar != seprate)//任意非法字符作为分隔符并去重
		{
			s += seprate;
			lastChar = seprate;
		}
	if (s.length() <= 0)//空输入置零
		s = "0";

	vector<NodeType> ret;
	size_t seprate_len = 1, start = 0, index = 0;
	while ((index = s.find(seprate, start)) != -1) 
	{
		ret.push_back((NodeType)atof(s.substr(start, index - start).c_str()));
		start = index + seprate_len;
	}
	if (start < s.length())
		ret.push_back((NodeType)atof(s.substr(start, s.length() - start).c_str()));
	return ret;
}

vector<NodeType> getInput()//从控制台获得输入
{
	string input_str = "";
	cout << "请输入若干个数，并使用“" << SEPRATE << "”隔开：" << endl;
	rfstdin();//清空输入缓冲区
	getline(cin, input_str);
	rfstdin();//清空输入缓冲区
	return split(input_str, SEPRATE);
}

void quickSort(vector<NodeType>& vec, int low, int high)//快速排序算法
{
	if (low < high)//传参检查
	{
		int l = low, h = high;
		NodeType key = vec[l];// 记录 key 值

		while (l < h)
		{
			while (l < h && key <= vec[h])//从右往左遍历找到第一个小于 key 的元素
				--h;
			vec[l] = vec[h];
			while (l < h && key >= vec[l])//从左往右遍历找到第一个大于 key 值的元素
				++l;
			vec[h] = vec[l];
		}
		vec[l] = key;//此时 l = h

		quickSort(vec, low, h - 1);//递归实现
		quickSort(vec, l + 1, high);
	}
	return;
}

string getArray(vector<NodeType> array_lists)// vector 转成 python 的列表形式
{
	string strArray = "[" + to_string(array_lists[0]);
	for (size_t i = 1; i < array_lists.size(); ++i)
		strArray += ", " + to_string(array_lists[i]);
	strArray += "]";
	return strArray;
}


/* 二分查找 */
NodeType getNode()
{
	NodeType node;
	rfstdin();
	cin >> node;
	return node;
}

int BinarySearch(vector<NodeType> array_lists, int nBegin, int nEnd, NodeType target, unsigned int& compareCount)
{
	if (nBegin > nEnd)
		return EOF;//未能找到目标
	int nMid = (nBegin + nEnd) >> 1;//使用位运算加速
	++compareCount;
	if (array_lists[nMid] == target)//找到目标
		return nMid;
	else if (array_lists[nMid] > target)//分而治之
		return BinarySearch(array_lists, nBegin, nMid - 1, target, compareCount);
	else
		return BinarySearch(array_lists, nMid + 1, nEnd, target, compareCount);
	return EOF;//未能找到目标
}


/* 改良二分查找 */
void AdvancedBinarySearch(vector<NodeType> array_lists, int nBegin, int nEnd, NodeType target, unsigned int& compareCount, int& tBegin, int& tEnd)
{
	if (nBegin > nEnd)
	{
		tBegin = nEnd;//传引用实现返回值
		tEnd = nBegin;
		return;
	}
	int nMid = (nBegin + nEnd) >> 1;
	++compareCount;
	if (array_lists[nMid] == target)
	{
		tBegin = nBegin;//传引用实现返回值
		tEnd = nEnd;
		return;
	}
	else if (array_lists[nMid] > target)//分而治之
		AdvancedBinarySearch(array_lists, nBegin, nMid - 1, target, compareCount, tBegin, tEnd);
	else
		AdvancedBinarySearch(array_lists, nMid + 1, nEnd, target, compareCount, tBegin, tEnd);
	return;
}



/* main 函数 */
int main()
{
	/* 排序 */
	vector<NodeType> array_lists = getInput();//获得输入
	cout << endl << "原始列表：" << endl << getArray(array_lists) << endl;//打印字符串
	quickSort(array_lists, 0, array_lists.size() - 1);//完全排序
	cout << endl << "排序后列表：" << endl << getArray(array_lists) << endl << endl;

	/* 二分查找 */
	cout << endl << "请输入要查找的值：";
	NodeType target = getNode();
	unsigned int compareCount = 0;
	int index = BinarySearch(array_lists, 0, array_lists.size() - 1, target, compareCount);
	if (EOF == index)
		cout << "二分查找：查找失败！共比较了 " << compareCount << " 次。" << endl;
	else
		cout << "二分查找：目标值位于索引 " << index << " 处！共比较了 " << compareCount << " 次。" << endl;

	/* 改良二分查找 */
	compareCount = 0;
	int tBegin = 0, tEnd = 0;
	AdvancedBinarySearch(array_lists, 0, array_lists.size() - 1, target, compareCount, tBegin, tEnd);
	if (tBegin == tEnd)
		cout << "改良二分查找：查找成功，目标值位于索引 " << tBegin << " 处！共比较了 " << compareCount << " 次。" << endl;
	else if (tBegin < 0)//说明在前面
		cout << "改良二分查找：目标值位于索引 " << tEnd << " 前！共比较了 " << compareCount << " 次。" << endl;
	else if (tEnd >= (int)array_lists.size())//说明在后面
		cout << "改良二分查找：目标值位于索引 " << tBegin << " 后！共比较了 " << compareCount << " 次。" << endl;
	else if (tBegin < tEnd)
		cout << "改良二分查找：目标值位于索引 " << tBegin << " 和 " << tEnd << " 之间！共比较了 " << compareCount << " 次。" << endl;

	return EXIT_SUCCESS;
}