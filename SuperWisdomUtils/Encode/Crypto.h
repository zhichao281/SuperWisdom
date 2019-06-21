#pragma once

#include <string>
#include <windows.h>
using namespace std;


// BASE64 
// 十六进制数到ASCII表示
#define HEX_DIGIT_TO_ASCII(x)   ((((x) >= 0) && ( (x) <= 9))? (x)+'0': (x)-10+'A')
// Encoding and decoding Base64 code
class CryptoBase64
{

public:

	// 63rd char used for Base64 code
	static const wchar_t CHAR_63 = '+';

	// 64th char used for Base64 code
	static const wchar_t CHAR_64 = '/';

	// Char used for padding
	static const wchar_t CHAR_PAD = '=';

public:

	// Encodes binary data to Base64 code
	// Returns size of encoded data.
	static int Encode(const unsigned char* inData,
		int dataLength,
		wstring& outCode);

	// Decodes Base64 code to binary data
	// Returns size of decoded data.
	static int Decode(const wstring& inCode,
		int codeLength,
		unsigned char* outData);

	// Returns maximum size of decoded data based on size of Base64 code.
	static int GetDataLength(int codeLength);

	// Returns maximum length of Base64 code based on size of uncoded data.
	static int GetCodeLength(int dataLength);

};





/////////////////////////TEA加密//////////////////////////////////
#define ENCODE_SINGATURE        (DWORD)0x4B434E45

class CryptoTEA
{


public:

	// <summary> 
	//     使用TEA算法加密64bit数据，即8个字节。
	// </summary> 
	// <param name="lpData">
	//     64bit(8个字节)的需要加密数据首地址。
	// </param>
	// <param name="lpKey">
	//     128bit(即16个字节)的密钥首地址。
	// </param>
	static void EncipherQword(void *lpData, const void *lpKey);

	// <summary> 
	//     使用TEA算法解密bit数据，即个字节。
	// </summary> 
	// <param name="lpData">
	//     64bit(8个字节)的需要解密数据首地址。
	// </param>
	// <param name="lpKey">
	//     128bit(即个字节)的密钥首地址。
	// </param>
static void DecipherQword(void *lpData, const void *lpKey);

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
static int EncipherMemory(void* lpData, int nBufLen, const void *lpKey);

// <summary> 
//     解密内存区。
// </summary> 
// <param name="lpData">
//     [IN/OUT]解密数据缓冲区首地址。
// </param>
// <param name="nBufLen">
//     解密数据缓冲区长度，必须是8的倍数。
// </param>
// <param name="lpKey">
//     128bit密钥。
// </param>
// <returns>
//     解密后数据的长度，<0 则表示失败。
// </returns>
static int DecipherMemory(void* lpData, int nBufLen, const void *lpKey);


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
static int EncipherString(const std::wstring& Source, std::wstring& Result, const void* Key);

// <summary> 
//     ASCII字符转换为进制数。
// </summary> 
// <param name="chr">
//     16进制字符ASCII表示。
// </param>
// <returns>
//     16进制数值。
// </returns>
static unsigned char Asc2Hex(wchar_t chr);

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
static int DecipherString(const std::wstring& Source, std::wstring& Result, const void* Key);

// <summary> 
//     随机产生加密的密钥。
// </summary> 
// <param name="EncKey">
//     用于保存加密密钥。
// </param>
static void GenerateEncKey(DWORD* EncKey);

};




/////////////////////////// MD5/////////////////////////////


/* MD5 context. */
typedef struct
{
	unsigned long state[4];         /* state (ABCD) */
	unsigned long count[2];         /* number of bits, modulo 2^64 (lsb first) */
	unsigned char buffer[64];       /* input buffer */
} MD5_CONTEXT;


#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21



/* F, G, H and I are basic MD5 functions. */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits. */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/*
FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
Rotation is separate from addition to prevent recomputation.
*/
#define FF(a, b, c, d, x, s, ac) { \
    (a) += F ((b), (c), (d)) + (x) + (unsigned long)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
    }
#define GG(a, b, c, d, x, s, ac) { \
    (a) += G ((b), (c), (d)) + (x) + (unsigned long)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
    }
#define HH(a, b, c, d, x, s, ac) { \
    (a) += H ((b), (c), (d)) + (x) + (unsigned long)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
    }
#define II(a, b, c, d, x, s, ac) { \
    (a) += I ((b), (c), (d)) + (x) + (unsigned long)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
    }

//1）md5存取的数据长度仅为64位，位于数据的最前端，大于令其自然溢出。

//（2）update函数和final函数处理得很繁琐，需要仔细分析。

//（3）16位md5码取32位md5码的中间16位。
class CryptoMD5
{

public:
	/* Note: Replace "for loop" with standard memset if possible. */
	static void MD5Memset(unsigned char* output, int value, unsigned int len);

	/* Note: Replace "for loop" with standard memcpy if possible. */
	static void MD5Memcpy(unsigned char* output, unsigned char* input, unsigned int len);


	/*
	Encodes input (unsigned long) into output (unsigned char). Assumes len is
	a multiple of 4.
	*/
	static void Encode(unsigned char *output, unsigned long *input, unsigned int len);


	/*
	Decodes input (unsigned char) into output (unsigned long). Assumes len is
	a multiple of 4.
	*/
	static void Decode(unsigned long *output, unsigned char *input, unsigned int len);


	/* MD5 basic transformation. Transforms state based on block. */
	static void MD5Transform(unsigned long state[4], unsigned char block[64]);


	/* MD5 initialization. Begins an MD5 operation, writing a new context. */
	static void MD5Init(MD5_CONTEXT *context);


	/*
	MD5 block update operation. Continues an MD5 message-digest
	operation, processing another message block, and updating the
	context.
	*/
	static void MD5Update(MD5_CONTEXT *context, unsigned char *input, unsigned int inputLen);


	/*
	MD5 finalization. Ends an MD5 message-digest operation, writing the
	the message digest and zeroizing the context.
	*/
	static void MD5Final(unsigned char digest[16], MD5_CONTEXT *context);

	static std::wstring MD5String32(std::wstring  Source);

	static std::wstring MD5String16(std::wstring  Source);
};
