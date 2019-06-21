#pragma once


typedef unsigned       int uint4;
typedef unsigned short int uint2;
typedef unsigned      char uchar;

/************************************************************************/
/* 函数名: MD5String                                                    */
/* 参数  : szString 要获取MD5值的字符串									*/
/* 参数  : pDestMd5 存放MD5值的缓冲区									*/
/* 参数  : iBufSz   缓冲区pDestMd5的大小								*/
/* 返回值: 存放MD5值的缓冲区指针  pDestMd5								*/
/************************************************************************/
TCHAR* MD5String(TCHAR* szString, TCHAR* pDestMd5, int iBufSz);

/************************************************************************/
/* 函数名: MD5File	                                                    */
/* 参数  : szFilename 要获取MD5值的文件路径								*/
/* 参数  : pDestMd5   存放MD5值的缓冲区									*/
/* 参数  : iBufSz     缓冲区pDestMd5的大小								*/
/* 返回值: 存放MD5值的缓冲区指针  pDestMd5								*/
/************************************************************************/
TCHAR* MD5File(TCHAR* szFilename, TCHAR* pDestMd5, int iBufSz);

/************************************************************************/
/* 函数名: PrintMD5	 由MD5String  MD5File调用                           */
/* 参数  : md5Digest 存放以计算出的MD5值的缓冲区						*/
/* 参数  : pDestMd5  存放MD5值的缓冲区									*/
/* 参数  : iBufSz    缓冲区pDestMd5的大小								*/
/* 返回值: 存放MD5值的缓冲区指针  pDestMd5								*/
/************************************************************************/
TCHAR* PrintMD5(uchar md5Digest[16], TCHAR* pDestMd5, int iBufSz);


class CMd5
{
// Methods
public:
	CMd5() { Init(); }
	void	Init();
	void	Update(uchar* chInput, uint4 nInputLen);
	void	Finalize();
	uchar*	Digest() { return m_Digest; }

private:
	void	Transform(uchar* block);
	void	Encode(uchar* dest, uint4* src, uint4 nLength);
	void	Decode(uint4* dest, uchar* src, uint4 nLength);

	inline	uint4	rotate_left(uint4 x, uint4 n)
	                 { return ((x << n) | (x >> (32-n))); }

	inline	uint4	F(uint4 x, uint4 y, uint4 z)
	                 { return ((x & y) | (~x & z)); }

	inline  uint4	G(uint4 x, uint4 y, uint4 z)
	                 { return ((x & z) | (y & ~z)); }

	inline  uint4	H(uint4 x, uint4 y, uint4 z)
	                 { return (x ^ y ^ z); }

	inline  uint4	I(uint4 x, uint4 y, uint4 z)
	                 { return (y ^ (x | ~z)); }

	inline	void	FF(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac)
	                 { a += F(b, c, d) + x + ac; a = rotate_left(a, s); a += b; }

	inline	void	GG(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac)
                     { a += G(b, c, d) + x + ac; a = rotate_left(a, s); a += b; }

	inline	void	HH(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac)
                     { a += H(b, c, d) + x + ac; a = rotate_left(a, s); a += b; }

	inline	void	II(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac)
                     { a += I(b, c, d) + x + ac; a = rotate_left(a, s); a += b; }

// Data
private:
	uint4		m_State[4];
	uint4		m_Count[2];
	uchar		m_Buffer[64];
	uchar		m_Digest[16];
	uchar		m_Finalized;
};