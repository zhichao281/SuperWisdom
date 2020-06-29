#include "sigslot.h"  
#include <string>  
#include <stdio.h>  
#include <iostream>  
#include <windows.h>  

using namespace sigslot;

using namespace std;

class CSender
{
public:
	sigslot::signal2<string, int>  sig_Danger;

	void Panic()
	{
		static int nVal = 0;
		char szVal[20] = { 0 };
		sprintf_s(szVal, 20, "help--%d", nVal);
		sig_Danger(szVal, nVal++);
	}
};

class CReceiver :public sigslot::has_slots<>
{
public:
	void slot_Danger(string strMsg, int nVal)
	{
		//printf("%s ==> %d", strMsg.c_str(), nVal);  
		cout << strMsg.c_str() << " ==> " << nVal << endl;
		Sleep(3000);
	}
};

int main()
{
	CSender sender;
	CReceiver recever;

	cout << "create object ok..." << endl;

	sender.sig_Danger.connect(&recever, &CReceiver::slot_Danger);

	cout << "connect succ!" << endl;

	while (1)
	{
		cout << "in while..." << endl;

		sender.Panic();
		Sleep(2000);

		cout << "end of sleep" << endl;
	}


	return 0;
}