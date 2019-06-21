#include "Base64.h"
#include <string.h>

static const char base64Char[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";  

void CBase64::initDecodeTable()
{  
	int i;  
	for (i = 0; i < 256; ++i) base64DecodeTable[i] = (char)0x80;  

	for (i = 'A'; i <= 'Z'; ++i) base64DecodeTable[i] = 0 + (i - 'A');  
	for (i = 'a'; i <= 'z'; ++i) base64DecodeTable[i] = 26 + (i - 'a');  
	for (i = '0'; i <= '9'; ++i) base64DecodeTable[i] = 52 + (i - '0');  
	base64DecodeTable[(unsigned char)'+'] = 62;  
	base64DecodeTable[(unsigned char)'/'] = 63;  
	base64DecodeTable[(unsigned char)'='] = 0;  
}  

unsigned int CBase64::Decode(char* input, char* outBuf, unsigned int nBufSz, bool trimTrailingZeros)   
{
	if (NULL == input || 0 == *input)
	{
		*outBuf = 0;
		return 0;
	}

	unsigned int resultSize = 0;

	if (!haveInitedBase64DecodeTable)  
	{  
		initDecodeTable();  
		haveInitedBase64DecodeTable = true;  
	}  

	unsigned char* out = new unsigned char[strlen(input) + 1]; // ensures we have enough space  
	memset(out, 0, strlen(input) + 1);

	int k = 0;  
	int const jMax = strlen(input) - 3;  
	// in case "in" is not a multiple of 4 bytes (although it should be)  
	for (int j = 0; j < jMax; j += 4)   
	{  
		char inTmp[4], outTmp[4];  
		for (int i = 0; i < 4; ++i)   
		{  
			inTmp[i] = input[i+j];  
			outTmp[i] = base64DecodeTable[(unsigned char)inTmp[i]];  
			if ((outTmp[i]&0x80) != 0) outTmp[i] = 0; // pretend the input was 'A'  
		}  

		out[k++] = (outTmp[0]<<2) | (outTmp[1]>>4);  
		out[k++] = (outTmp[1]<<4) | (outTmp[2]>>2);  
		out[k++] = (outTmp[2]<<6) | outTmp[3];  
	}  

	if (trimTrailingZeros)   
	{  
		while (k > 0 && out[k-1] == '\0') --k;  
	}  
	resultSize = k; 

	if (nBufSz < resultSize + 1)
	{
		delete[] out;  
		return resultSize;
	}

	memmove(outBuf, out, resultSize);
	outBuf[resultSize] = 0;
	delete[] out;  

	return resultSize;
}  

char* CBase64::Encode(char const* origSigned, unsigned origLength, char* outBuf, int nBufSz)   
{  
	unsigned char const* orig = (unsigned char const*)origSigned; // in case any input bytes have the MSB set  
	if (orig == NULL) return NULL;  

	unsigned const numOrig24BitValues = origLength/3;  
	bool havePadding = origLength > numOrig24BitValues*3;  
	bool havePadding2 = origLength == numOrig24BitValues*3 + 2;  
	unsigned const numResultBytes = 4*(numOrig24BitValues + havePadding);  
	char* result = new char[numResultBytes+1]; // allow for trailing '\0'  

	memset(result, 0, numResultBytes+1);

	// Map each full group of 3 input bytes into 4 output base-64 characters:  
	unsigned i;  
	for (i = 0; i < numOrig24BitValues; ++i)   
	{  
		result[4*i+0] = base64Char[(orig[3*i]>>2)&0x3F];  
		result[4*i+1] = base64Char[(((orig[3*i]&0x3)<<4) | (orig[3*i+1]>>4))&0x3F];  
		result[4*i+2] = base64Char[((orig[3*i+1]<<2) | (orig[3*i+2]>>6))&0x3F];  
		result[4*i+3] = base64Char[orig[3*i+2]&0x3F];  
	}  

	// Now, take padding into account.  (Note: i == numOrig24BitValues)  
	if (havePadding)   
	{  
		result[4*i+0] = base64Char[(orig[3*i]>>2)&0x3F];  
		if (havePadding2)  
		{  
			result[4*i+1] = base64Char[(((orig[3*i]&0x3)<<4) | (orig[3*i+1]>>4))&0x3F];  
			result[4*i+2] = base64Char[(orig[3*i+1]<<2)&0x3F];  
		}   
		else   
		{  
			result[4*i+1] = base64Char[((orig[3*i]&0x3)<<4)&0x3F];  
			result[4*i+2] = '=';  
		}  
		result[4*i+3] = '=';  
	}  

	result[numResultBytes] = '\0';
	strcpy_s(outBuf, nBufSz, result);
	delete []result;

	return outBuf;  
}  