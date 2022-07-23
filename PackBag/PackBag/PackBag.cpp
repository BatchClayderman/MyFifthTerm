#include <iostream>  
#include <vector>  
using namespace std;


class PackBag
{
protected:
	int         m_n;				//物品的件数
	int         m_c;				//背包容量
	vector<int> m_w;				// N 个物品的重量
	vector<int> m_p;				// N 个物品的价格
	int         maxValue;			//背包最大价值
	int         currentValue;		//当前背包中物品的价值
	int         currentWeight;		//当前背包中物品的重量

private:
	void BackTrack(int depth)//回溯法
	{
		if (depth >= m_n)//达到最大深度
		{
			if (maxValue < currentValue)//保存最优解
				maxValue = currentValue;
			return;
		}

		if (currentWeight + m_w[depth] <= m_c)//如果满足约束条件
		{
			/* 临时更改背包的容量和价值 */
			currentWeight += m_w[depth];
			currentValue += m_p[depth];

			/* 选取了第 i 件物品 */
			BackTrack(depth + 1);//递归求解下一个结点（回溯法）

			/* 恢复背包的容量和价值 */
			currentWeight -= m_w[depth];
			currentValue -= m_p[depth];
		}

		/* 不取第 i 件物品 */
		BackTrack(depth + 1);//递归求解下一个结点（回溯法）
		return;
	}

public:
	PackBag(int n, int c, vector<int>& w, vector<int>& p)//构造函数
		:m_n(n), m_c(c), m_p(w), m_w(p)
	{
		maxValue = 0;//初始化各值
		currentValue = 0;
		currentWeight = 0;
	}

	bool isValid()//检测各值是否合法
	{
		if (this->m_w.size() != this->m_n || this->m_p.size() != this->m_n)//序列数和物品件数不一致
			return false;
		for (int i = 0; i < this->m_n; ++i)
			if (m_w[i] < 1 || m_p[i] < 1)//现实生活中不存在非正数重量或价格的背包
				return false;
		return true;
	}

	int GetBestValue()//获取背包内物品的最大值
	{
		BackTrack(0);//从 0 开始
		return maxValue;//返回最优解（最大值）
	}
};

void rfstdin()//清空输入缓冲区
{
	rewind(stdin);
	fflush(stdin);
	return;
}



int main()
{
	int n = 1, c = 1;
	cout << "请输入物品的件数：";
	rfstdin();
	cin >> n;
	cout << "请输入背包的容量：";
	rfstdin();
	cin >> c;
	if (n < 1 || c < 1)//非法输入
	{
		cout << "\a输入非法，程序已退出。" << endl;
		return EXIT_FAILURE;
	}

	vector<int> w(n), p(n);
	cout << "请依次输入 " << n << " 个物品的重量（请用空格隔开）：" << endl;
	rfstdin();
	for (int i = 0; i < n; ++i)
		cin >> w[i];
	cout << "请依次输入 " << n << " 个物品的价格（请用空格隔开）：" << endl;
	rfstdin();
	for (int j = 0; j < n; ++j)
		cin >> p[j];

	PackBag pack(n, c, p, w);//构造类
	if (pack.isValid())
	{
		cout << endl << "背包内的物品的最大价值为：" << pack.GetBestValue() << endl;
		return EXIT_SUCCESS;
	}
	else
	{
		cout << "\a输入序列含有非法值，程序已退出。" << endl;
		return EXIT_FAILURE;
	}
}