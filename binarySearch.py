from random import sample
N = 20
k = 10
target = 8
array = sorted(sample(range(1, N), k))#从 1 到 N 中随机生成 k 个元素有序系列

def binary_search(array, target):#非递归
	left = 0
	right = len(array) - 1
	while right >= left:
		mid = (left + right) >> 1#取中间值
		if target == array[mid]:#找到
			return mid
		if target > array[mid]:#继续细分
			left = mid + 1
		else:
			right = mid - 1
		#print(array[mid], left, right, sep = "\t")
	return -1

def binarySearch(array, target, start = 0, end = None):#递归
	end = len(array) - 1 if end is None else end
	mid_index = (start + end) >> 1#取中间值
	if end >= start:
		if target > array[mid_index]:#没找到则继续细分
			#print(array[mid_index + 1:end + 1])
			return binarySearch(array, target, start = mid_index + 1, end = end)
		elif target < array[mid_index]:
			#print(array[start:mid_index])
			return binarySearch(array, target, start = start, end = mid_index - 1)
		elif target == array[mid_index]:#找到
			return mid_index
		else:
			return -1
	else:
		return -1

def main():
	print(array)
	print(binary_search(array, target))
	print(binarySearch(array, target))



if __name__ == "__main__":
	main()