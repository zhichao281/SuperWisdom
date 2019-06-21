///////////////////////////////
// http://mingcn.cnblogs.com //
//  xelz CopyRight (c) 2010  //
///////////////////////////////


#if !defined(AFX_AES_H__6BDD3760_BDE8_4C42_85EE_6F7A434B81C4__INCLUDED_)
#define AFX_AES_H__6BDD3760_BDE8_4C42_85EE_6F7A434B81C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Note: ����ģʽΪebc-128-zeroPadding
// ����key������16�ֽ�

class AES  
{
public:
	AES(unsigned char* key);	// key �� ��Կ��������16�ֽ�(128bit)
	virtual ~AES();

	char* CipherToBase64(char* input, int nLen, char* outBuf, int nBufSz);
	char* InvCipherFromBase64(char* input, char* outBuf, int nBufSz);

	void* Cipher(void* input, int length=0);
	void* InvCipher(void* input, int length);

protected:
	unsigned char* Cipher(unsigned char* input);
	unsigned char* InvCipher(unsigned char* input);

private:
	unsigned char Sbox[256];
	unsigned char InvSbox[256];
	unsigned char w[11][4][4];

	void KeyExpansion(unsigned char* key, unsigned char w[][4][4]);
	unsigned char FFmul(unsigned char a, unsigned char b);

	void SubBytes(unsigned char state[][4]);
	void ShiftRows(unsigned char state[][4]);
	void MixColumns(unsigned char state[][4]);
	void AddRoundKey(unsigned char state[][4], unsigned char k[][4]);

	void InvSubBytes(unsigned char state[][4]);
	void InvShiftRows(unsigned char state[][4]);
	void InvMixColumns(unsigned char state[][4]);
};

#endif // !defined(AFX_AES_H__6BDD3760_BDE8_4C42_85EE_6F7A434B81C4__INCLUDED_)