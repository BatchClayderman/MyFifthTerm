import platform
import os
from sys import exit
from sys import argv
from msvcrt import getch
try:
	import numpy as np
	import cv2
	from matplotlib import pyplot as plt
	from math import log
	from PIL import Image
	from tqdm import tqdm
except:
	print("缺乏必要的库，请确保您的计算机含有 numpy、opencv-python、matplotlib、tqdm库。")
	getch()
	exit(-1)

os.chdir(os.path.abspath(os.path.dirname(__file__)))#解析进入当前目录


# 清屏函数
def clearScreen(fakeClear = 120):
	if sys.stdin.isatty():#在终端
		if platform.system().lower() == "windows":
			os.system("cls")
		elif platform.system().lower() == "linux":
			os.system("clear")
		else:
			print("\n" * int(fakeClear))
	else:
		print("\n" * int(fakeClear))

# 计算图像块信息熵
def infoEntropy(img, m, n, p, q):
	tmp = [0 for i in range(256)]
	k = 0
	res = 0
	for i in range(m, m + p):
		for j in range(n, n + q):
			val = img[i][j]
			tmp[val] = float(tmp[val] + 1)
			k = float(k + 1)
	for i in range(len(tmp)):
		tmp[i] = float(tmp[i] / k)
	for i in range(len(tmp)):
		if tmp[i] != 0:
			res = float(res - tmp[i] * (log(tmp[i]) / log(2.0)))
	return res


# 主函数
def img_info_entropy(path1, path2, path3 = os.getcwd()):
	image1, image2 = cv2.imread(path1, 0), cv2.imread(path2, 0)
	if image1 is None or image2 is None:
		print("图片读入错误，请确保您的路径不含有中文等非法字符，并确保图片存在！")
		return 1
	
	# 计算初始图片的长宽、水印图片的长宽
	length1, width1 = len(image1), len(image1[0])
	length2, width2 = len(image2), len(image2[0])

	if length1 < length2 or width1 < width2:
		print("读取图片成功，但两张图片不符合规范！")
		return 2
	
	img = np.array(image1)
	m, n, ie = 0, 0, 10000
	for i in tqdm(range(length1 - length2), ncols = 80):
		for j in range(0, width1 - width2):
			t = infoEntropy(img, i, j, length2, width2)
			#print(i, j, t)
			if t < ie:
				m, n, ie = i, j, t
	
	print("信息熵最小的图像块在第 {0} 行第 {1} 列，其大小为 {2}。".format(m, n, ie))
	
	for i in range(width2):
		for j in range(length2):
			if image2[i][j] < 100:
				image1[i + m][j + n] = image2[i][j]
	
	if platform.system().lower() == "windows":
		if not (path3 == "." or path3.lower() == os.getcwd().lower()) and not (os.path.exists(path3) and os.path.isdir(path3)):
			os.makedirs(path3)
	else:
		if not (path3 == "." or path3 == os.getcwd()) and not (os.path.exists(path3) and os.path.isdir(path3)):
			os.makedirs(path3)
	
	cv2.imwrite(os.path.join(path3, "res.jpg"), image1)
	return 0



if __name__ == "__main__":
	if len(argv) == 4:
		path1 = argv[1]
		path2 = argv[2]
		path3 = argv[3]
		print("正在处理生成 res.jpg，请稍候。")
	elif os.path.exists("Lena.jpg") and os.path.exists("Logo.jpg"):
		print("已捕获到 Lena.jpg 和 Logo.jpg，正在处理生成 res.jpg，请稍候。")
		path1 = "Lena.jpg"
		path2 = "Logo.jpg"
		path3 = "."#os.getcwd()
	else:
		clearScreen()
		path1 = input("请输入图片路径：")
		path2 = input("请输入水印路径：")
		path3 = input("请输入保存的目录路径：")
		clearScreen()
		print("\n正在处理生成 res.jpg，请稍候。")
	bRet = img_info_entropy(path1, path2, path3 = path3)
	if len(argv) == 4:
		if bRet == 0:
			print("\n操作成功结束，请按任意键退出。")
		else:
			print("\n操作没有完全成功，请按任意键退出。")
		getch()
		clearScreen()
	else:
		if bRet == 0:
			print("\n操作成功结束！")
		else:
			print("\n操作没有完全成功！")
	exit(bRet)