#pragma once

class CBase64
{
public:
	// input:			  编码原文
	// outBuf:			  编码结果, 缓存空间必须足够大, 否则返回空串
	// nBufSz:			  outBuf的大小(单位: Byte)
	// trimTrailingZeros: 结果是否以'\0'结尾
	// 返回值:			  编码后的长度，若结果不为0，单编码结果为空串，说明缓存大小不够
	unsigned int Decode(char* input, char* outBuf, unsigned int nBufSz, bool trimTrailingZeros = true);

	// orig:		原文
	// origLength:	原文长度
	// outBuf:		编码
	// nBufSz:		缓存大小(单位: Byte)
	// 返回值:		outBuf指针
	char* Encode(char const* orig, unsigned origLength, char* outBuf, int nBufSz);

public:
	CBase64(void)	{ haveInitedBase64DecodeTable = false; }
	~CBase64(void)	{}
protected:
	void initDecodeTable();

private:
	char base64DecodeTable[256];
	bool haveInitedBase64DecodeTable;
};
