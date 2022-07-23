#include <iostream>
#include <string>
#include <queue>
using namespace std;
const int max_num = 64;//最大背包数量的常量


struct object//物品结构体
{
	double w, v, d;//重量、价值、价值重量比
	int id;//物品序号
} a[max_num];
int cnt, best_vector_x[max_num];//物品种数和最优解向量
double weights, bestValue;//背包容量和最优价值

struct node//结点结构体
{
	double cv, lv, lw;//当前价值、价值上界和剩余容量
	int id, x[max_num];//物品排序后的序号和解向量
	node()
	{
		memset(x, 0, sizeof(x));
		lv = 0;
	}
	node(double _cv, double _lv, double _lw, int _id)//构造函数
		:cv(_cv), lv(_lv), lw(_lw), id(_id)
	{
		memset(x, 0, sizeof(x));
	}
};

struct NodeCompare//结点优先级，价值上界高的优先级高
{
	bool operator() (node& n1, node& n2)
	{
		return n1.lv < n2.lv;
	}
};


bool compareBetween(object b1, object b2)//以单位价值高的优先
{
	return b1.d > b2.d;
}

void init()//初始化
{
	memset(best_vector_x, 0, sizeof(best_vector_x));
	bestValue = 0;
	sort(a + 1, a + cnt + 1, compareBetween);//以 compareBetween 的比较方法排序
	return;
}

double calcInf(node t)//计算价值上界
{
	int num = t.id;
	double maxvalue = t.cv, left = t.lw;
	while (num <= cnt && left >= a[num].w)
	{
		left -= a[num].w;
		maxvalue += a[num].v;
		++num;
	}
	if (num <= cnt && left > 0)//记得防止越界
		maxvalue += 1.0 * a[num].v / a[num].w * left;// 1.0 乘可以转 double
	return maxvalue;
}

void bfs()
{
	priority_queue<node, vector<node>, NodeCompare> q;
	double sumv = 0;
	for (int i = 1; i <= cnt; ++i)
		sumv += a[i].v;
	q.push(node(0, sumv, weights, 1));//放入根结点

	while (!q.empty())
	{
		node live, lchild, rchild;
		live = q.top();
		q.pop();
		int t = live.id;//当前处理物品的序号
		if (t > cnt || live.lw == 0)//到达叶子结点或者没有容量了
		{
			if (live.cv >= bestValue)//更新最优解（第一次计算得到的值也要更新）
			{
				for (int i = 1; i <= cnt; ++i)
					best_vector_x[i] = live.x[i];
				bestValue = live.cv;
			}
			continue;
		}

		if (live.lv < bestValue)//不满足限界条件
			continue;

		if (live.lw >= a[t].w)//满足约束条件，可以生成左孩子
		{
			lchild.cv = live.cv + a[t].v;
			lchild.lw = live.lw - a[t].w;
			lchild.id = t + 1;
			lchild.lv = calcInf(lchild);
			for (int i = 1; i <= cnt; ++i)
				lchild.x[i] = live.x[i];
			lchild.x[t] = 1;
			if (lchild.cv > bestValue)//注意要更新最优值
				bestValue = lchild.cv;
			q.push(lchild);
		}

		rchild.cv = live.cv;
		rchild.lw = live.lw;
		rchild.id = t + 1;
		rchild.lv = calcInf(rchild);
		if (rchild.lv >= bestValue)//满足限界条件，可以生成右孩子
		{
			for (int i = 1; i <= cnt; ++i)
				rchild.x[i] = live.x[i];
			rchild.x[t] = 0;
			q.push(rchild);
		}
	}
	return;
}

void rfstdin()
{
	rewind(stdin);
	fflush(stdin);
	return;
}

void input(string input_str, double& w, double& v)
{
	size_t index = 0;
	for (index = 0; index < input_str.size(); ++index)//查找空格所在的地方
		if (' ' == input_str[index] || '\t' == input_str[index])
			break;
	if (index <= 0 || index >= input_str.size() - 1)//没有找到合适的分隔符号
	{
		w = NULL;
		v = NULL;
	}
	else
	{
		w = atof(input_str.substr(0, index).c_str());
		v = atof(input_str.substr(index + 1).c_str());
	}
	return;
}

void output()
{
	cout << endl << "可装载物品的最大价值为：" << bestValue << endl;
	if (bestValue > 0)
	{
		cout << "装入的物品 ID 为：" << endl;
		cout << "{" << a[1].id;
		for (int i = 2; i <= cnt; ++i)
			if (best_vector_x[i])
				cout << ", " << a[i].id;
		cout << "}" << endl;
	}
	return;
}



int main()
{
	cout << "请输入物品个数：";
	rfstdin();
	cin >> cnt;
	cout << "请输入背包容量：";
	rfstdin();
	cin >> weights;
	if (cnt < 1 || weights < 1 || cnt > max_num)
	{
		cout << endl << "\a您的输入不合理或超出问题处理规模限制，程序已退出。" << endl;
		return EXIT_FAILURE;
	}

	for (int i = 1; i <= cnt; ++i)
	{
		cout << "请输入物品 " << i << " 的重量和价值（使用一个空格隔开）：";
		rfstdin();
		string input_str;
		getline(cin, input_str);
		input(input_str, a[i].w, a[i].v);
		if (a[i].w < 1 || a[i].v < 1)
		{
			--i;//重新输入
			cout << "\a上述输入有误，请重新输入。" << endl;
			continue;
		}
		a[i].d = a[i].v / a[i].w;
		a[i].id = i;//标记 id
	}

	init();
	bfs();
	output();
	return EXIT_SUCCESS;
}