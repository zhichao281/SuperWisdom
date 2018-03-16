from ctypes import *
def func(a, b, i):
	print(a+b);
	print(i);
	windll.kernel32.OutputDebugStringA("Output in vs'Output!");
	return 99,"Hello";
	
def GetDict():
	print("Dict");
	return {1:'A', 2:'B', 3:'C'};

def GetList():
	return [1, 3, 5, 7];

def GetTuple():
	return [1, 3, "HiHiChina"];