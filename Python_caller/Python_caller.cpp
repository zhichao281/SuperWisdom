// Python_caller.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Python.h>

int main()
{
	//python常用几种值与C的映射 python - >
	// s ->char*
	// i -> int
	// u -> wchar_t* //本人未确认
	// b/B -> unsigned char
	// h -> short int
	// H -> unsigned short int
	// l -> long
	// k -> unsigned long
	// c -> char
	// f -> float
	// d -> double
	// o -> python object
	// w -> char* 可读可写


	Py_Initialize();
	PyRun_SimpleString("import sys");//普通运行python
	PyRun_SimpleString("sys.path.append('./')");//定位到目录

	PyObject *pFile, *pRes, *pFunc;
	pFile = PyImport_ImportModule("hihiPython");//读python文件, 记住别加 .py
	pFunc = PyObject_GetAttrString(pFile, "func");//取函数对象

	PyObject *pArg1, *pArg2, *pArg3;
	pArg1 = Py_BuildValue("i", 99);
	pArg2 = Py_BuildValue("i", 100);
	pArg3 = Py_BuildValue("s", "HiHiChina");
	PyObject* pParamTuple = PyTuple_New(3);//参数以tuple形式打包
	PyTuple_SetItem(pParamTuple, 0, pArg1);
	PyTuple_SetItem(pParamTuple, 1, pArg2);
	PyTuple_SetItem(pParamTuple, 2, pArg3);
	pRes = PyEval_CallObject(pFunc, pParamTuple);//调用函数
	//这边pRes是一个 python_tuple, [99, "Hello"];
	int nRes = 0;
	char* pHello = 0;
	PyArg_ParseTuple(pRes, "is", &nRes, &pHello);//取值, 这边的 is 根据楼上的映射关系

	//这是另一种调用方式
	pRes = PyObject_CallMethod(pFile, "func", "(ssi)", "99", "100", 300);//这边的  ssi根据楼上的映射关系

	pRes = PyObject_CallMethod(pFile, "GetDict", "");
	//这边的pRes是一个map, {1:'A', 2:'B', 3:'C'}
	if (PyDict_Check(pRes) == true) {
		printf("Is Dict");
		int nSize = PyDict_Size(pRes);//size
		PyObject* pKeys = PyDict_Items(pRes);//获取keys, 是个 python list [1, 2, 3]
		int nListSize = PyList_Size(pKeys);//list size
		for (int iList = 0; iList < nListSize; iList++) {
			int nNumber;//key
			PyObject* pTuple = PyList_GetItem(pKeys, iList);
			char* psz;//value
			PyArg_ParseTuple(pTuple, "is", &nNumber, &psz);//获取 key, value
		}
	}

	Py_Finalize();
	system("pause");
    return 0;
}

