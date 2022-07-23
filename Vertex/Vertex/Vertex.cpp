#include <iostream>
#include <Windows.h>
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
#ifndef INF
#define INF 0x0FFFFFFF
#endif
#ifndef K
#define K 5
#endif
#ifndef DATA
#define DATA "data.txt"
#endif
#ifndef SEPRATE
#define SEPRATE ' '
#endif
using namespace std;
typedef unsigned int NodeType;
typedef int IDType;
typedef short int CountType;
vector<vector<NodeType>> G;//图
vector<IDType> p;//结果


/* 从文件读入数据 */
#ifdef WIN32
string GF_GetEXEPath()//获取程序所在路径
{
	char FilePath[MAX_PATH];
	GetModuleFileNameA(NULL, FilePath, MAX_PATH);
	(strrchr(FilePath, '\\'))[1] = 0;
	return string(FilePath);
}
#else
string GF_GetEXEPath()
{
	int rval;
	char link_target[4096];
	char* last_slash;
	size_t result_Length;
	char* result;
	string strExeDir;
	rval = readlink("/proc/self/exe", link_target, 4096);
	if (rval < 0 || rval >= 1024)
		return "";
	link_target[rval] = 0;
	last_slash = strrchr(link_target, '/');
	if (last_slash == 0 || last_slash == link_target)
		return "";
	result_Length = last_slash - link_target;
	result = (char*)malloc(result_Length + 1);
	strncpy(result, link_target, result_Length);
	result[result_Length] = 0;
	strExeDir.append(result);
	strExeDir.append("/");
	free(result);
	return strExeDir;
}
#endif

vector<NodeType> split(string strArray, char seprate)//按行转 vector
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
		if (s.substr(start, index - start) == "-1" || s.substr(start, index - start) == "INF")//此路不通
			ret.push_back(INF);
		else
			ret.push_back((NodeType)atof(s.substr(start, index - start).c_str()));
		start = index + seprate_len;
	}
	if (start < s.length())
	{
		if (s.substr(start, s.length() - start) == "-1" || s.substr(start, s.length() - start) == "INF")
			ret.push_back(INF);
		else
			ret.push_back((NodeType)atof(s.substr(start, s.length() - start).c_str()));
	}
	return ret;
}

bool getInput()
{
	FILE* fp;
	fopen_s(&fp, (GF_GetEXEPath() + DATA).c_str(), "r");//打开目标文件
	if (nullptr != fp)
	{
		string tmpString = "";//存放每行
		for (;;)//无条件死循环
		{
			char c = fgetc(fp);//逐字符遍历
			if (EOF == c)//如果遍历完成
			{
				if (tmpString.length() > 0)
					G.push_back(split(tmpString, SEPRATE));//将现有的 tmpString 一并放入图中
				break;
			}
			else if ('\n' == c)//换行
			{
				G.push_back(split(tmpString, SEPRATE));
				tmpString = "";//记得清行
			}
			else
				tmpString += c;
		}
		return true;
	}
	else
		return false;
}



/* 多段图规划函数
 *	G 图的邻接矩阵
 *	p 最小成本路径返回数组
 *	k 图的段数（k 段图）
 *	n 结点个数
 */
void MultiSatgeGraph(CountType k, CountType n)
{
	cout << "/**************************************** printing logs ****************************************/" << endl;

	/* 初始化最短距离 */
	vector<NodeType> cost;//存储当前结点到终点的最短距离
	for (CountType i = 0; i < n; ++i)
		cost.push_back(NULL);
	cost[n - 1] = 0;
	
	/* 初始化点 */
	vector<IDType> d;//保存当前阶段各个结点到下一阶段点的最短距离的点
	for (CountType i = 0; i < n; ++i)
		d.push_back(NULL);
	
	/* 算法核心 */
	for (CountType i = n - 2; i >= 0; --i)//从倒数第二个结点开始依次寻找
	{
		cost[i] = INF;//初始化结点到终点距离
		for (CountType r = i + 1; r < n; ++r)//扫描当前阶段到终点之间的结点，找到当前阶段到终点的最小距离
		{
			cout << "G[" << i << "][" << r << "] = " << (INF == G[i][r] ? "INF" : to_string(G[i][r])) << "\t\t" << "cost[" << r << "] = " << (INF == cost[r] ? "INF" : to_string(cost[r])) << "\t\t" << "cost[" << i << "] = " << (INF == cost[i] ? "INF" : to_string(cost[i])) << "\t\t";
			if (G[i][r] + cost[r] < cost[i])
			{
				cost[i] = G[i][r] + cost[r];
				d[i] = r;//记录形成最小距离的结点
				cout << "d[" << i << "] = " << r;
			}
			cout << endl;
		}
	}

	p[0] = 0;
	p[k - 1] = n - 1;
	for (CountType i = 1; i < k; ++i)//从头收集，形成最短路径
		p[i] = d[p[i - 1]];
	
	return;
}

void display()//打印函数
{
	string map = "\n\n图\t";//打印图的邻接矩阵及结果
	for (size_t i = 0; i < G.size(); ++i) //生成表头
		map += (to_string(i + 1) + "\t");
	map += "\n";
	for (size_t i = 0; i < G.size(); ++i)//生成每一行数据字符串
	{
		map += (to_string(i + 1) + "\t");
		for (size_t j = 0; j < G[0].size(); ++j)
		{
			if (i == j)
				map += "0\t";
			else if (INF == G[i][j])
				map += "INF\t";
			else
				map += to_string(G[i][j]) + "\t";
		}
		map += "\n";
	}
	cout << map << endl << endl;//打印邻接矩阵

	/* 输出最终结果 */
	string result = "路径：" + to_string(p[0] + 1);
	for (size_t i = 1; i < p.size(); ++i)
		result += "->" + to_string(p[i] + 1);
	cout << result << endl;
	return;
}



int main()
{
	if (!getInput())//初始化读入数据
	{
		cout << "获取输入失败，请确保您已将数据保存程序所在目录下的 " << DATA << " 中，并确保本程序对其可读。" << endl;
		return EXIT_FAILURE;
	}

	CountType k = K, n = (CountType)G.size();
	if (k <= 1 || n <= 2 || n <= k || G.size() != G[0].size())//粗略检查数据
	{
		cout << "数据不合规，请检查您的数据！" << endl;
		cout << EXIT_FAILURE;
	}

	for (CountType i = 0; i < k; ++i)//初始化 p
		p.push_back(NULL);
	MultiSatgeGraph(k, n);
	display();

	return EXIT_SUCCESS;
}

/*
0, 3, 9, 2, 3, -1, -1, -1, -1, -1, -1, -1
-1, 0, -1, -1, -1, 6, 8, 1, -1, -1, -1, -1
-1, -1, 0, -1, -1, 7, 5, -1, -1, -1, -1, -1
-1, -1, -1, 0, -1, -1, -1, 11, -1, -1, -1, -1
-1, -1, -1, -1, 0, -1, 12, 8, -1, -1, -1, -1
-1, -1, -1, -1, -1, 0, -1, -1, 5, 7, -1, -1
-1, -1, -1, -1, -1, -1, 0, -1, 6, 8, -1, -1
-1, -1, -1, -1, -1, -1, -1, 0, -1, 5, 6, -1
-1, -1, -1, -1, -1, -1, -1, -1, 0, -1, -1, 3
-1, -1, -1, -1, -1, -1, -1, -1, -1, 0, -1, 5
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0, -1
-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0
*/