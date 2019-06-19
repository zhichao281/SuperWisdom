
#include "Crypto.h"

unsigned char PADDING[64] =
{
	0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// Encodes binary data to Base64 code
// Returns size of encoded data.
int CryptoBase64::Encode(const unsigned char* inData,
	int dataLength,
	wstring& outCode)
{

	wstring result;

	// output buffer which holds code during conversation
	int len = GetCodeLength(dataLength);
	wchar_t* out = new wchar_t[len];

	// charachers used by Base64
	static const wchar_t alph[] =
	{
		'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
		'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
		'0','1','2','3','4','5','6','7','8','9',CHAR_63,CHAR_64
	};

	// mask - first six bits
	const int mask = 0x3F;

	// used as temp 24-bits buffer
	union
	{
		unsigned char bytes[4];
		unsigned int block;
	} buffer;

	// coversation is done by taking three bytes at time of input data int temp
	// then four six-bits values are extracted, converted to base64 characters
	// and at the end they are written to output buffer
	for (int i = 0, j = 0, left = dataLength; i < dataLength; i += 3, j += 4, left -= 3)
	{
		//------------------------
		// filling temp buffer

		// get first byte and puts it at MSB position in temp buffer
		buffer.bytes[2] = inData[i];

		// more data left?
		if (left > 1)
		{
			// get second byte and puts it at middle position in temp buffer
			buffer.bytes[1] = inData[i + 1];
			// more data left?
			if (left > 2)
				// get third byte and puts it at LSB position in temp buffer
				buffer.bytes[0] = inData[i + 2];
			else
				// zero-padding of input data (last bytes)
				buffer.bytes[0] = 0;
		}
		else
		{
			// zero-padding of input data (last two bytes)
			buffer.bytes[1] = 0;
			buffer.bytes[0] = 0;
		}

		//------------------------
		// constructing code from temp buffer
		// and putting it in output buffer

		// extract first and second six-bit value from temp buffer
		// and convert is to base64 character
		out[j] = alph[(buffer.block >> 18) & mask];
		out[j + 1] = alph[(buffer.block >> 12) & mask];
		// more data left?
		if (left > 1)
		{
			// extract third six-bit value from temp buffer
			// and convert it to base64 character
			out[j + 2] = alph[(buffer.block >> 6) & mask];
			// more data left?
			if (left > 2)
				// extract forth six-bit value from temp buffer
				// and convert it to base64 character
				out[j + 3] = alph[buffer.block & mask];
			else
				// pad output code
				out[j + 3] = CHAR_PAD;
		}
		else
		{
			// pad output code
			out[j + 2] = CHAR_PAD;
			out[j + 3] = CHAR_PAD;
		}
	}

	outCode.clear();
	outCode.append(out, len);
	delete[] out;
	return len;


}

// Decodes Base64 code to binary data
// Returns size of decoded data.
int CryptoBase64::Decode(const wstring& inCode,
	int codeLength,
	unsigned char* outData)
{

	// used as temp 24-bits buffer
	union
	{
		unsigned char bytes[4];
		unsigned int block;
	} buffer;
	buffer.block = 0;

	// number of decoded bytes
	int j = 0;

	for (int i = 0; i < codeLength; i++)
	{
		// position in temp buffer
		int m = i % 4;

		wchar_t x = inCode[i];
		int val = 0;

		// converts base64 character to six-bit value
		if (x >= 'A' && x <= 'Z')
			val = x - 'A';
		else if (x >= 'a' && x <= 'z')
			val = x - 'a' + 'Z' - 'A' + 1;
		else if (x >= '0' && x <= '9')
			val = x - '0' + ('Z' - 'A' + 1) * 2;
		else if (x == CHAR_63)
			val = 62;
		else if (x == CHAR_64)
			val = 63;

		// padding chars are not decoded and written to output buffer
		if (x != CHAR_PAD)
			buffer.block |= val << (3 - m) * 6;
		else
			m--;

		// temp buffer is full or end of code is reached
		// flushing temp buffer
		if (m == 3 || x == CHAR_PAD)
		{
			// writes byte from temp buffer (combined from two six-bit values) to output buffer
			outData[j++] = buffer.bytes[2];
			// more data left?
			if (x != CHAR_PAD || m > 1)
			{
				// writes byte from temp buffer (combined from two six-bit values) to output buffer
				outData[j++] = buffer.bytes[1];
				// more data left?
				if (x != CHAR_PAD || m > 2)
					// writes byte from temp buffer (combined from two six-bit values) to output buffer
					outData[j++] = buffer.bytes[0];
			}

			// restarts temp buffer
			buffer.block = 0;
		}

		// when padding char is reached it is the end of code
		if (x == CHAR_PAD)
			break;
	}

	return j;

}

// Returns maximum size of decoded data based on size of Base64 code.
int CryptoBase64::GetDataLength(int codeLength)
{
	return codeLength - codeLength / 4;
}

// Returns maximum length of Base64 code based on size of uncoded data.
int CryptoBase64::GetCodeLength(int dataLength)
{
	int len = dataLength + dataLength / 3 + (int)(dataLength % 3 != 0);

	// output code size must be multiple of 4 bytes
	if (len % 4)
		len += 4 - len % 4;

	return len;
}



// <summary> 
//     使用TEA算法加密bit数据，即个字节。
// </summary> 
// <param name="lpData">
//     64bit(8个字节)的需要加密数据首地址。
// </param>
// <param name="lpKey">
//     128bit(即16个字节)的密钥首地址。
// </param>
void CryptoTEA::EncipherQword(void *lpData, const void *lpKey)
{
	const unsigned long cnDelta = 0x9E3779B9;
	register unsigned long y = ((unsigned long *)lpData)[0], z = ((unsigned long *)lpData)[1];
	register unsigned long sum = 0;
	unsigned long a = ((unsigned long *)lpKey)[0], b = ((unsigned long *)lpKey)[1];
	unsigned long c = ((unsigned long *)lpKey)[2], d = ((unsigned long *)lpKey)[3];
	int n = 32;
	while (n-- > 0)
	{
		sum += cnDelta;
		y += (z << 4) + a ^ z + sum ^ (z >> 5) + b;
		z += (y << 4) + c ^ y + sum ^ (y >> 5) + d;
	}
	((unsigned long *)lpData)[0] = y;
	((unsigned long *)lpData)[1] = z;
}


// <summary> 
//     使用TEA算法解密bit数据，即个字节。
// </summary> 
// <param name="lpData">
//     64bit(8个字节)的需要解密数据首地址。
// </param>
// <param name="lpKey">
//     128bit(即个字节)的密钥首地址。
// </param>
void CryptoTEA::DecipherQword(void *lpData, const void *lpKey)
{
	const unsigned long cnDelta = 0x9E3779B9;
	register unsigned long y = ((unsigned long *)lpData)[0], z = ((unsigned long *)lpData)[1];
	register unsigned long sum = 0xC6EF3720;
	unsigned long a = ((unsigned long *)lpKey)[0], b = ((unsigned long *)lpKey)[1];
	unsigned long c = ((unsigned long *)lpKey)[2], d = ((unsigned long *)lpKey)[3];
	int n = 32;

	// sum = delta << 5, in general sum = delta * n
	while (n-- > 0)
	{
		z -= (y << 4) + c ^ y + sum ^ (y >> 5) + d;
		y -= (z << 4) + a ^ z + sum ^ (z >> 5) + b;
		sum -= cnDelta;
	}
	((unsigned long *)lpData)[0] = y;
	((unsigned long *)lpData)[1] = z;
}

// <summary> 
//     加密内存区域。
// </summary> 
// <param name="lpData">
//     [IN/OUT]加密数据缓冲区首地址。
// </param>
// <param name="nBufLen">
//     加密数据缓冲区长度，必须圆整为的倍数。
// </param>
// <param name="lpKey">
//     128位密钥长度，必须由调用方保证至少字节长。
// </param>
// <returns>
//     加密数据的实际长度，<0 则表示失败。
// </returns>
int CryptoTEA::EncipherMemory(void* lpData, int nBufLen, const void *lpKey)
{
	if (nBufLen <= 0) return 0;
	if (lpData == NULL || lpKey == NULL || (nBufLen % 8 != 0))
		return (-9); // 无效参数

	// 加密数据
	unsigned char* pDest = (unsigned char*)lpData;
	int n = nBufLen >> 3;
	while (n--)
	{
		EncipherQword(pDest, lpKey);
		pDest += 8;
	}

	return nBufLen;
}

// <summary> 
//     解密内存区。
// </summary> 
// <param name="lpData">
//     [IN/OUT]解密数据缓冲区首地址。
// </param>
// <param name="nBufLen">
//     解密数据缓冲区长度，必须是的倍数。
// </param>
// <param name="lpKey">
//     128bit密钥。
// </param>
// <returns>
//     解密后数据的长度，<0 则表示失败。
// </returns>
int CryptoTEA::DecipherMemory(void* lpData, int nBufLen, const void *lpKey)
{
	if (nBufLen <= 0) return 0;
	if (lpData == NULL || lpKey == NULL || (nBufLen % 8 != 0))
		return (-9); // 无效参数

	// 解密
	unsigned char* pDest = (unsigned char*)lpData;
	int n = nBufLen >> 3;
	while (n--)
	{
		DecipherQword(pDest, lpKey);
		pDest += 8;
	}
	return nBufLen;
}


// <summary> 
//     加密字符串。
// </summary> 
// <param name="Source">
//     源字符串。
// </param>
// <param name="Result">
//     结果字符串。
// </param>
// <param name="Key">
//     128位密钥。
// </param>
// <returns>
//     >=0 表示成功，<0表示失败。
// </returns>
int CryptoTEA::EncipherString(const std::wstring& Source, std::wstring& Result, const void* Key)
{
	Result.clear();
	if (Key == NULL) return (-9); // 无效参数
	if (Source.empty()) return 0;

	// 确定缓冲区长度
	DWORD SourceLength = (DWORD)Source.length();
	DWORD EncodeNumber = ENCODE_SINGATURE;
	int BufferLen = (int)SourceLength * sizeof(wchar_t);
	BufferLen = ((BufferLen + 7) >> 3) << 3;  // 圆整为的倍数
	BufferLen += 8;
	unsigned char* Buffer = new unsigned char[BufferLen];
	if (Buffer == NULL) return (-3); // out of memory
	memset(Buffer, 0, BufferLen);
	memcpy(Buffer + 0, &EncodeNumber, sizeof(DWORD));
	memcpy(Buffer + 4, &SourceLength, sizeof(DWORD));
	memcpy(Buffer + 8, Source.c_str(), SourceLength * sizeof(wchar_t));

	// 进行加密
	int rc = EncipherMemory(Buffer, BufferLen, Key);
	if (rc < 0)
	{
		delete[] Buffer;
		return rc;
	}

	// 构造结果
	Result.resize(BufferLen * 2);
	for (int Index = 0; Index < BufferLen; Index++)
	{
		Result[2 * Index + 0] = HEX_DIGIT_TO_ASCII((Buffer[Index] >> 4) & 0x0F);
		Result[2 * Index + 1] = HEX_DIGIT_TO_ASCII(Buffer[Index] & 0x0F);
	}

	// 释放内存并返回
	delete[] Buffer;
	return (int)Result.length();
}

// <summary> 
//     ASCII字符转换为进制数。
// </summary> 
// <param name="chr">
//     16进制字符ASCII表示。
// </param>
// <returns>
//     16进制数值。
// </returns>
unsigned char CryptoTEA::Asc2Hex(wchar_t chr)
{
	if (chr >= '0' && chr <= '9')
		return (chr - '0');

	if (chr >= 'a' && chr <= 'f')
		return (chr - 'a' + 10);

	if (chr >= 'A' && chr <= 'F')
		return (chr - 'A' + 10);

	// 无效字符
	return (-1);
}



// <summary> 
//     解密字符串。
// </summary> 
// <param name="Source">
//     源字符串。
// </param>
// <param name="Result">
//     结果字符串。
// </param>
// <param name="Key">
//     128位密钥。
// </param>
// <returns>
//     >=0 表示成功，<0表示失败。
// </returns>
int CryptoTEA::DecipherString(const std::wstring& Source, std::wstring& Result, const void* Key)
{
	Result.clear();

	//==============================================================================
	// 检查输入
	//==============================================================================

	if (Key == NULL) return (-9); // 无效参数
	if (Source.empty()) return 0;

	// 检查输入是否有效
	if (Source.length() % 16 != 0)
		return (-9);  // 输入必须是的倍数

	// 输入必须是进制数0-9 A-F
	int Index = 0;
	for (Index = 0; Index < (int)Source.length(); Index++)
	{
		// 检查是否为数字
		if (Source[Index] >= '0' && Source[Index] <= '9')
			continue;

		// 检查是否为A-F
		if (Source[Index] >= 'a' && Source[Index] <= 'f')
			continue;

		// 检查是否为A-F
		if (Source[Index] >= 'A' && Source[Index] <= 'F')
			continue;

		// 无效输入
		return (-9);
	}

	//==============================================================================
	// 准备缓冲区
	//==============================================================================
	unsigned char Chr = 0;
	int BufferLen = (int)Source.length() / 2;
	unsigned char* Buffer = new unsigned char[BufferLen];
	if (Buffer == NULL) return (-3); // Out of memory
	for (Index = 0; Index < BufferLen; Index++)
	{
		Chr = Asc2Hex(Source[2 * Index + 0]);
		Chr <<= 4;
		Chr |= Asc2Hex(Source[2 * Index + 1]);
		Buffer[Index] = Chr;
	}

	//==============================================================================
	// 进行解密
	//==============================================================================
	int rc = DecipherMemory(Buffer, BufferLen, Key);
	if (rc < 0)
	{
		delete[] Buffer;
		return rc;
	}

	// 前面字节为长度信息
	DWORD SourceLength = 0;
	DWORD EncodeNumber = 0;
	memcpy(&EncodeNumber, Buffer + 0, sizeof(DWORD));
	memcpy(&SourceLength, Buffer + 4, sizeof(DWORD));
	if ((EncodeNumber != ENCODE_SINGATURE) || (SourceLength > (BufferLen - 8) / sizeof(wchar_t)))
	{
		delete[] Buffer;
		return (-15); // 无效数据
	}

	// 准备结果
	Result.assign((const wchar_t*)(Buffer + 8), (size_t)SourceLength);

	// 返回
	delete[] Buffer;
	return (int)Result.length();
}


// <summary> 
//     随机产生加密的密钥。
// </summary> 
// <param name="EncKey">
//     用于保存加密密钥。
// </param>
void CryptoTEA::GenerateEncKey(DWORD* EncKey)
{
	DWORD RandNum = 0;
	srand((unsigned int)GetTickCount());
	for (int index = 0; index < 4; index++)
	{
		RandNum = (DWORD)rand();
		RandNum <<= 16;
		RandNum |= (DWORD)rand();
		EncKey[index] = RandNum;
	}
}



/* Note: Replace "for loop" with standard memset if possible. */
void CryptoMD5::MD5Memset(unsigned char* output, int value, unsigned int len)
{
	unsigned int i;

	for (i = 0; i < len; i++)
		((char *)output)[i] = (char)value;
}

/* Note: Replace "for loop" with standard memcpy if possible. */
void CryptoMD5::MD5Memcpy(unsigned char* output, unsigned char* input, unsigned int len)
{
	unsigned int i;

	for (i = 0; i < len; i++)
		output[i] = input[i];
}

/*
Encodes input (unsigned long) into output (unsigned char). Assumes len is
a multiple of 4.
*/
void CryptoMD5::Encode(unsigned char *output, unsigned long *input, unsigned int len)
{
	unsigned int i, j;

	for (i = 0, j = 0; j < len; i++, j += 4) {
		output[j] = (unsigned char)(input[i] & 0xff);
		output[j + 1] = (unsigned char)((input[i] >> 8) & 0xff);
		output[j + 2] = (unsigned char)((input[i] >> 16) & 0xff);
		output[j + 3] = (unsigned char)((input[i] >> 24) & 0xff);
	}
}

/*
Decodes input (unsigned char) into output (unsigned long). Assumes len is
a multiple of 4.
*/
void CryptoMD5::Decode(unsigned long *output, unsigned char *input, unsigned int len)
{
	unsigned int i, j;

	for (i = 0, j = 0; j < len; i++, j += 4)
		output[i] = ((unsigned long)input[j]) | (((unsigned long)input[j + 1]) << 8)
		| (((unsigned long)input[j + 2]) << 16) | (((unsigned long)input[j + 3]) << 24);
}

/* MD5 basic transformation. Transforms state based on block. */
void CryptoMD5::MD5Transform(unsigned long state[4], unsigned char block[64])
{
	unsigned long a = state[0], b = state[1], c = state[2], d = state[3], x[16];

	Decode(x, block, 64);

	/* Round 1 */
	FF(a, b, c, d, x[0], S11, 0xd76aa478); /* 1 */
	FF(d, a, b, c, x[1], S12, 0xe8c7b756); /* 2 */
	FF(c, d, a, b, x[2], S13, 0x242070db); /* 3 */
	FF(b, c, d, a, x[3], S14, 0xc1bdceee); /* 4 */
	FF(a, b, c, d, x[4], S11, 0xf57c0faf); /* 5 */
	FF(d, a, b, c, x[5], S12, 0x4787c62a); /* 6 */
	FF(c, d, a, b, x[6], S13, 0xa8304613); /* 7 */
	FF(b, c, d, a, x[7], S14, 0xfd469501); /* 8 */
	FF(a, b, c, d, x[8], S11, 0x698098d8); /* 9 */
	FF(d, a, b, c, x[9], S12, 0x8b44f7af); /* 10 */
	FF(c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
	FF(b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
	FF(a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
	FF(d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
	FF(c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
	FF(b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

	/* Round 2 */
	GG(a, b, c, d, x[1], S21, 0xf61e2562); /* 17 */
	GG(d, a, b, c, x[6], S22, 0xc040b340); /* 18 */
	GG(c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
	GG(b, c, d, a, x[0], S24, 0xe9b6c7aa); /* 20 */
	GG(a, b, c, d, x[5], S21, 0xd62f105d); /* 21 */
	GG(d, a, b, c, x[10], S22, 0x2441453); /* 22 */
	GG(c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
	GG(b, c, d, a, x[4], S24, 0xe7d3fbc8); /* 24 */
	GG(a, b, c, d, x[9], S21, 0x21e1cde6); /* 25 */
	GG(d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
	GG(c, d, a, b, x[3], S23, 0xf4d50d87); /* 27 */
	GG(b, c, d, a, x[8], S24, 0x455a14ed); /* 28 */
	GG(a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
	GG(d, a, b, c, x[2], S22, 0xfcefa3f8); /* 30 */
	GG(c, d, a, b, x[7], S23, 0x676f02d9); /* 31 */
	GG(b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

	/* Round 3 */
	HH(a, b, c, d, x[5], S31, 0xfffa3942); /* 33 */
	HH(d, a, b, c, x[8], S32, 0x8771f681); /* 34 */
	HH(c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
	HH(b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
	HH(a, b, c, d, x[1], S31, 0xa4beea44); /* 37 */
	HH(d, a, b, c, x[4], S32, 0x4bdecfa9); /* 38 */
	HH(c, d, a, b, x[7], S33, 0xf6bb4b60); /* 39 */
	HH(b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
	HH(a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
	HH(d, a, b, c, x[0], S32, 0xeaa127fa); /* 42 */
	HH(c, d, a, b, x[3], S33, 0xd4ef3085); /* 43 */
	HH(b, c, d, a, x[6], S34, 0x4881d05); /* 44 */
	HH(a, b, c, d, x[9], S31, 0xd9d4d039); /* 45 */
	HH(d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
	HH(c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
	HH(b, c, d, a, x[2], S34, 0xc4ac5665); /* 48 */

	/* Round 4 */
	II(a, b, c, d, x[0], S41, 0xf4292244); /* 49 */
	II(d, a, b, c, x[7], S42, 0x432aff97); /* 50 */
	II(c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
	II(b, c, d, a, x[5], S44, 0xfc93a039); /* 52 */
	II(a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
	II(d, a, b, c, x[3], S42, 0x8f0ccc92); /* 54 */
	II(c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
	II(b, c, d, a, x[1], S44, 0x85845dd1); /* 56 */
	II(a, b, c, d, x[8], S41, 0x6fa87e4f); /* 57 */
	II(d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
	II(c, d, a, b, x[6], S43, 0xa3014314); /* 59 */
	II(b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
	II(a, b, c, d, x[4], S41, 0xf7537e82); /* 61 */
	II(d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
	II(c, d, a, b, x[2], S43, 0x2ad7d2bb); /* 63 */
	II(b, c, d, a, x[9], S44, 0xeb86d391); /* 64 */

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;

	/* Zeroize sensitive information. */
	MD5Memset((unsigned char*)x, 0, sizeof(x));
}

/* MD5 initialization. Begins an MD5 operation, writing a new context. */
void CryptoMD5::MD5Init(MD5_CONTEXT *context)
{
	context->count[0] = context->count[1] = 0;
	/* Load magic initialization constants. */
	context->state[0] = 0x67452301;
	context->state[1] = 0xEFCDAB89;
	context->state[2] = 0x98BADCFE;
	context->state[3] = 0x10325476;
}

/*
MD5 block update operation. Continues an MD5 message-digest
operation, processing another message block, and updating the
context.
*/
void CryptoMD5::MD5Update(MD5_CONTEXT *context, unsigned char *input, unsigned int inputLen)
{
	unsigned int i, index, partLen;

	/* Compute number of bytes mod 64 */
	index = (unsigned int)((context->count[0] >> 3) & 0x3F);

	/* Update number of bits */
	if ((context->count[0] += ((unsigned long)inputLen << 3))
		< ((unsigned long)inputLen << 3))
		context->count[1]++;
	context->count[1] += ((unsigned long)inputLen >> 29);

	partLen = 64 - index;

	/* Transform as many times as possible. */
	if (inputLen >= partLen) {
		MD5Memcpy((unsigned char*)&context->buffer[index], (unsigned char*)input, partLen);
		MD5Transform(context->state, context->buffer);

		for (i = partLen; i + 63 < inputLen; i += 64)
			MD5Transform(context->state, &input[i]);

		index = 0;
	}
	else
		i = 0;

	/* Buffer remaining input */
	MD5Memcpy((unsigned char*)&context->buffer[index], (unsigned char*)&input[i], inputLen - i);
}

/*
MD5 finalization. Ends an MD5 message-digest operation, writing the
the message digest and zeroizing the context.
*/
void CryptoMD5::MD5Final(unsigned char digest[16], MD5_CONTEXT *context)
{
	unsigned char bits[8];
	unsigned int index, padLen;

	/* Save number of bits */
	Encode(bits, context->count, 8);

	/* Pad out to 56 mod 64. */
	index = (unsigned int)((context->count[0] >> 3) & 0x3f);
	padLen = (index < 56) ? (56 - index) : (120 - index);
	MD5Update(context, PADDING, padLen);

	/* Append length (before padding) */
	MD5Update(context, bits, 8);

	/* Store state in digest */
	Encode(digest, context->state, 16);

	/* Zeroize sensitive information. */
	MD5Memset((unsigned char*)context, 0, sizeof(*context));
}
std::wstring CryptoMD5::MD5String(std::wstring  Source)
{
	int size = Source.size();

	BYTE*   pSource = new BYTE[size * 2];
	ZeroMemory(pSource, size * 2);

	WideCharToMultiByte(CP_ACP, 0, Source.c_str(), size, (LPSTR)pSource, size * 2, NULL, NULL);

	MD5_CONTEXT context;
	BYTE  digest[16] = { 0 };
	MD5Init(&context);
	MD5Update(&context, pSource, Source.size() * 2);
	MD5Final(digest, &context);






	printf("加密前:%s\n加密后16位:", pSource);
	for (int i = 4; i < 12; i++) {
		printf("%02x", digest[i]);
	}


	printf("\n加密前:%s\n加密后32位:", pSource);
	for (int i = 0; i < 16; i++) {
		printf("%02x", digest[i]);
	}

	delete[]pSource;
	wchar_t  CheckCode[33] = { 0 };     // 保存校验码，个数+null-terminal
	for (int Index = 0; Index < 16; ++Index)
	{
		CheckCode[2 * Index] = HEX_DIGIT_TO_ASCII((digest[Index] >> 4) & 0x0F);
		CheckCode[2 * Index + 1] = HEX_DIGIT_TO_ASCII(digest[Index] & 0x0F);
	}

	std::wstring str(CheckCode);
	return  str;
}