#include <iostream>
#include <Windows.h>
#include <vector>
#include <iomanip>
#ifndef _MST_H
#define _MST_H 2/* 1 = Prim   2 = Kruskal */
#define INF 0xFFFFFF
#define NodeUB 16
#define NodeLB 2
#define DistanceUB 512
#define DistanceLB 1
#define PromptLeft  "/****************************** "
#define PromptRight " ******************************/"
#define PromptWidth 6
#define DATA "data.txt"
#define SEPRATE ' '
#endif//_MST_H
using namespace std;
typedef int DistanceType;
DistanceType setPK[NodeUB];


struct edge
{
	DistanceType x, y;
	DistanceType w;
};


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

vector<DistanceType> split(string strArray, char seprate)//按行转 vector
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

	vector<DistanceType> ret;
	size_t seprate_len = 1, start = 0, index = 0;
	while ((index = s.find(seprate, start)) != -1)
	{
		if (s.substr(start, index - start) == "-1" || s.substr(start, index - start) == "INF")//此路不通
			ret.push_back(INF);
		else
			ret.push_back((DistanceType)atof(s.substr(start, index - start).c_str()));
		start = index + seprate_len;
	}
	if (start < s.length())
	{
		if (s.substr(start, s.length() - start) == "-1" || s.substr(start, s.length() - start) == "INF")
			ret.push_back(INF);
		else
			ret.push_back((DistanceType)atof(s.substr(start, s.length() - start).c_str()));
	}
	return ret;
}

bool getInput(vector<DistanceType>& G, int& n)
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
				{
					vector<DistanceType> tmp = split(tmpString, SEPRATE);//将现有的 tmpString 一并放入图中
					for (size_t i = 0; i < tmp.size(); ++i)
						G.push_back(tmp[i]);
				}
				break;
			}
			else if ('\n' == c)//换行
			{
				vector<DistanceType> tmp = split(tmpString, SEPRATE);
				for (size_t i = 0; i < tmp.size(); ++i)
					G.push_back(tmp[i]);
				tmpString = "";//记得清行
			}
			else
				tmpString += c;
		}
		if (sqrt(G.size()) == (int)sqrt(G.size()))
		{
			n = (int)sqrt(G.size());
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

#if (1 == _MST_H)
/* Prim */
void Prim(vector<DistanceType>& A, int n)
{
	vector<DistanceType> prev, cost;
	for (int i = 0; i < n; ++i)
	{
		prev.push_back(NULL);
		cost.push_back(INF);
		setPK[i] = 0;
	}
	int ori_min = INF, new_min = NULL, count = 0;
	cost[0] = 0;// set 0 as root
	prev[0] = -1;

	while (count < n - 1)
	{
		for (int i = 0; i < n; ++i)
			if (setPK[i] == 0 && cost[i] < ori_min)
			{
				ori_min = cost[i];
				new_min = i;
			}
		
		setPK[new_min] = 1;// set min1 as next node 
		for (int i = 0; i < n; ++i)
		{
			if (A[new_min * n + i] > 0 && cost[i] > A[new_min * n + i] && setPK[i] == 0)
			{
				cost[i] = A[new_min * n + i];
				prev[i] = new_min;
			}
		}
		ori_min = INF;
		++count;
	}

	cout << PromptLeft << "Prim Minimum Spanning Tree" << PromptRight << endl;
	for (int i = 1; i < n; ++i)
		cout << "\t\tEdge: " << setw(2) << prev[i] << " - " << setw(2) << i << "\t\t\tWeight: " << A[prev[i] * n + i] << endl;
	return;
}

#elif (2 == _MST_H)
/* Kruskal */
void sort(vector<edge>& a, int n)// simply use bubble
{
	for (int i = 0; i < n - 1; ++i)
		for (int j = i + 1; j < n; ++j)
			if (a[i].w > a[j].w)
				swap(a[i], a[j]);
	return;
}

int findK(int x)
{
	int root = x, node = x, parent;
	while (root != setPK[root])
		root = setPK[root];
	while (node != root)
	{
		parent = setPK[node];
		setPK[node] = root;
		node = parent;
	}
	return root;
}

void unionK(vector<DistanceType>& height, int repx, int repy)// remember to use &
{
	if (height[repx] == height[repy])
	{
		++height[repx];
		setPK[repy] = repx;// y's tree points to x's tree
	}
	else if (height[repx] > height[repy])
		setPK[repy] = repx;// y's tree points to x's tree
	else
		setPK[repx] = repy;// x's tree points to y's tree
}

void Kruskal(vector<DistanceType> &A, int n)
{
	vector<edge> E, T;
	vector<DistanceType> height;
	edge tmp = { 0, 0, 0 };
	for (int i = 0; i < n; ++i)
	{
		height.push_back(NULL);
		for (int j = 0; j < n; ++j)
		{
			E.push_back({ A[i * n + j], i, j });
			T.push_back(tmp);
		}
	}
	
	sort(E, n * n);// Sort the edge firstly

	for (int i = 0; i < n; ++i)// Make a set
	{
		setPK[i] = i;
		height[i] = 0;
	}

	int index = 0;
	for (int i = 0; i < n * n; ++i)
		if (E[i].w != 0)
		{
			int ucomp = findK(E[i].x), vcomp = findK(E[i].y);
			if (vcomp != ucomp)
			{
				T[index] = E[i];
				unionK(height, ucomp, vcomp);
				if (++index == n - 1)
					break;
			}
		}
	
	cout << PromptLeft << "Kruskal Minimum Spanning Tree" << PromptRight << endl;
	for (int i = 0; i < n - 1; ++i)
		cout << "\t\tEdge: " << setw(2) << T[i].x << " - " << setw(2) << T[i].y << "\t\t\tWeight: " << T[i].w << endl;
	return;
}
#endif



int main()
{
	/* preparation */
	vector<DistanceType> A;
	srand((unsigned int)time(NULL));
	int n = (rand() % (NodeUB - NodeLB + 1)) + NodeLB;// rang in [NodeLB, NodeUB]
	
	/* initial */
	if (getInput(A, n))
		cout << "Initial: " << n << " vertices read from " << DATA << ". " << endl << endl;
	else
	{
		cout << "Initial: " << n << " vertices rang in [" << NodeLB << ", " << NodeUB << "]. " << endl << endl;
		for (int i = 0; i < n; ++i)
			for (int j = 0; j < n; ++j)
				if (i < j)
					A.push_back(rand() % (DistanceUB - DistanceLB + 1) + DistanceLB);
				else if (i == j)
					A.push_back(NULL);
				else
					A.push_back(A[j * n + i]);
	}

	/* output */
	cout << PromptLeft << "Two-Dimensional Matrix A (" << n << " * " << n << ")" << PromptRight << endl;
	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < n; ++j)
			cout << setw(PromptWidth) << A[i * n + j];
		cout << endl;
	}
	cout << endl;

#if (1 == _MST_H)
	Prim(A, n);
#elif (2 == _MST_H)
	Kruskal(A, n);
#endif
	cout << endl << endl;
	return EXIT_SUCCESS;
}