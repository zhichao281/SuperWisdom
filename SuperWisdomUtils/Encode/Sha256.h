#ifndef _SHA256_H
#define _SHA256_H

#include <Windows.h>


#define SHA256_DIGEST_SIZE  32
#define SHA256_BLOCK_SIZE	64
#define SHA256_MASK			(SHA256_BLOCK_SIZE - 1)

typedef	unsigned int	sha2_32t;
typedef unsigned char	uint8_t;

/************************************************************************/
/* 函数名: Sha256String                                                 */
/* 参数  : szString    (in)需散列的字符串								*/
/* 参数  : pDestSha    (in/out)存放SHA256 字符串 的缓冲区				*/
/* 参数  : iBufSz      (in) pDestSha缓冲区的大小>=64					*/
/* 返回  : 指向缓冲区 pDestSha1 的指针									*/
/************************************************************************/
TCHAR *Sha256String(LPCTSTR szString, LPTSTR pDestSha, int iBufSz=64);

/************************************************************************/
/* 函数名: Sha256File		                                            */
/* 参数  : szFilePath  (in)需散列的文件路径								*/
/* 参数  : pDestSha    (in/out)存放SHA256 字符串 的缓冲区				*/
/* 参数  : iBufSz      (in) pDestSha缓冲区的大小>=64					*/
/* 返回  : 指向缓冲区 pDestSha1 的指针									*/
/************************************************************************/
TCHAR *Sha256File(LPCTSTR szFilePath, LPTSTR pDestSha, int iBufSz=64);

/************************************************************************/
/* 函数名: Sha256DataString                                             */
/* 参数  : szString    (in)需散列的字符串								*/
/* 参数  : Digest      (in/out)存放SHA1值的缓冲区						*/
/* 返回  : 指向缓冲区 Digest 的指针										*/
/************************************************************************/
uint8_t *Sha256DataString(LPCTSTR szString, uint8_t Digest[SHA256_DIGEST_SIZE]);

/************************************************************************/
/* 函数名: Sha256DataFile                                               */
/* 参数  : szString    (in)需散列的字符串								*/
/* 参数  : Digest      (in/out)存放SHA1值的缓冲区						*/
/* 返回  : 指向缓冲区 Digest 的指针										*/
/************************************************************************/
uint8_t *Sha256DataFile(LPCTSTR szFilePath, uint8_t Digest[SHA256_DIGEST_SIZE]);



/* type to hold the SHA256 context				*/
typedef struct
{
	sha2_32t count[2];
	sha2_32t hash[8];
	sha2_32t wbuf[16];
} sha256_ctx;

class CSha256
{
public:
	void sha256_begin(sha256_ctx ctx[1]);
	void sha256_hash(const unsigned char data[], unsigned long len, sha256_ctx ctx[1]);
	void sha256_end(unsigned char hval[], sha256_ctx ctx[1]);
	void sha256(unsigned char hval[32], const unsigned char data[], unsigned long len); 

private:
	void sha256_compile(sha256_ctx ctx[1]);
};

#endif
