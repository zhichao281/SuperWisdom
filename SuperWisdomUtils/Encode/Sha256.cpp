#include "Sha256.h"
#include <memory.h>
#include <tchar.h>
#include <stdio.h>


TCHAR* Sha256File(LPCTSTR szFilePath, LPTSTR pDestSha, int iBufSz/* =64 */)
{
	uint8_t Digest[SHA256_DIGEST_SIZE]	= {0};
	TCHAR	chEach[8]					= {0};

	Sha256DataFile(szFilePath, Digest);
	*pDestSha = 0;

	for (int nCount = 0; nCount < SHA256_DIGEST_SIZE; nCount++)
	{
		_stprintf_s(chEach, _T("%02x"), Digest[nCount]);
		_tcsncat_s(pDestSha, iBufSz, chEach, 8);
	}

	return pDestSha;
}

TCHAR* Sha256String(LPCTSTR szString, LPTSTR pDestSha, int iBufSz/* =64 */)
{
	uint8_t Digest[SHA256_DIGEST_SIZE]	= {0};
	TCHAR	chEach[8]					= {0};

	Sha256DataString(szString, Digest);
	*pDestSha = 0;

	for (int nCount = 0; nCount < SHA256_DIGEST_SIZE; nCount++)
	{
		_stprintf_s(chEach, _T("%02x"), Digest[nCount]);
		_tcsncat_s(pDestSha, iBufSz, chEach, 8);
	}

	return pDestSha;
}

uint8_t* Sha256DataFile(LPCTSTR szFilePath, uint8_t Digest[SHA256_DIGEST_SIZE])
{
	FILE*			file;
	int				nLen;
	unsigned char	chBuffer[1024];
	CSha256			ShaObj;
	sha256_ctx		ShaContext;

	try
	{
		memset(chBuffer, 0, 1024);
		_tfopen_s(&file, szFilePath, _T("rb"));

		if (file != NULL)
		{
			ShaObj.sha256_begin(&ShaContext);

			while (nLen = fread (chBuffer, 1, 1024, file))
				ShaObj.sha256_hash(chBuffer, nLen, &ShaContext);

			ShaObj.sha256_end(Digest, &ShaContext);

			fclose (file);
		}
	}
	catch(...)
	{

	}

	return Digest;
}

uint8_t* Sha256DataString(LPCTSTR szString, uint8_t Digest[SHA256_DIGEST_SIZE])
{
	int		iLen	= _tcslen(szString);
	char*	pTemp	= new char[iLen + 1];
	CSha256	shaObj;

#ifdef UNICODE
	WideCharToMultiByte(CP_ACP, 0, szString, -1, pTemp, iLen+1, NULL, NULL);
#else
	strcpy_s(pTemp, iLen+1, szString);
#endif

	shaObj.sha256(Digest, (uint8_t*)pTemp, iLen);

	delete []pTemp;
	return Digest;
}





//////////////////////////////////////////////////////////////////////////
//
//  CSha256
//
//////////////////////////////////////////////////////////////////////////

#define	s_u32				u
#define sfx_lo(x,y)			x##y
#define sfx_hi(x,y)			sfx_lo(x,y)
#define n_u32(p)			sfx_hi(0x##p,s_u32)


#if !defined(bswap_32)
#define bswap_32(x) (rotr32((x), 24) & 0x00ff00ff | rotr32((x), 8) & 0xff00ff00)
#endif

#define ch(x,y,z)	(((x) & (y)) ^ (~(x) & (z)))
#define maj(x,y,z)	(((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define rotr32(x,n)	(((x) >> n) | ((x) << (32 - n)))
#define	bsw_32(p,n)	{ int _i = (n);	while(_i--) p[_i] = bswap_32(p[_i]); }
#define s256_0(x)	(rotr32((x),  2) ^ rotr32((x), 13) ^ rotr32((x), 22)) 
#define s256_1(x)	(rotr32((x),  6) ^ rotr32((x), 11) ^ rotr32((x), 25)) 
#define g256_0(x)	(rotr32((x),  7) ^ rotr32((x), 18) ^ ((x) >>  3)) 
#define g256_1(x)	(rotr32((x), 17) ^ rotr32((x), 19) ^ ((x) >> 10))

#define h2(i)		ctx->wbuf[i & 15] += \
	g256_1(ctx->wbuf[(i + 14) & 15]) + ctx->wbuf[(i + 9) & 15] + g256_0(ctx->wbuf[(i + 1) & 15])

#define h2_cycle(i,j)  \
	v[(7 - i) & 7] += (j ? h2(i) : ctx->wbuf[i & 15]) + k256[i + j] \
	+ s256_1(v[(4 - i) & 7]) + ch(v[(4 - i) & 7], v[(5 - i) & 7], v[(6 - i) & 7]); \
	v[(3 - i) & 7] += v[(7 - i) & 7]; \
	v[(7 - i) & 7] += s256_0(v[(0 - i) & 7]) + maj(v[(0 - i) & 7], v[(1 - i) & 7], v[(2 - i) & 7])

const sha2_32t k256[64] = {
	n_u32(428a2f98), n_u32(71374491), n_u32(b5c0fbcf), n_u32(e9b5dba5), 
	n_u32(3956c25b), n_u32(59f111f1), n_u32(923f82a4), n_u32(ab1c5ed5), 
	n_u32(d807aa98), n_u32(12835b01), n_u32(243185be), n_u32(550c7dc3), 
	n_u32(72be5d74), n_u32(80deb1fe), n_u32(9bdc06a7), n_u32(c19bf174), 
	n_u32(e49b69c1), n_u32(efbe4786), n_u32(0fc19dc6), n_u32(240ca1cc), 
	n_u32(2de92c6f), n_u32(4a7484aa), n_u32(5cb0a9dc), n_u32(76f988da), 
	n_u32(983e5152), n_u32(a831c66d), n_u32(b00327c8), n_u32(bf597fc7), 
	n_u32(c6e00bf3), n_u32(d5a79147), n_u32(06ca6351), n_u32(14292967), 
	n_u32(27b70a85), n_u32(2e1b2138), n_u32(4d2c6dfc), n_u32(53380d13), 
	n_u32(650a7354), n_u32(766a0abb), n_u32(81c2c92e), n_u32(92722c85),
	n_u32(a2bfe8a1), n_u32(a81a664b), n_u32(c24b8b70), n_u32(c76c51a3), 
	n_u32(d192e819), n_u32(d6990624), n_u32(f40e3585), n_u32(106aa070), 
	n_u32(19a4c116), n_u32(1e376c08), n_u32(2748774c), n_u32(34b0bcb5), 
	n_u32(391c0cb3), n_u32(4ed8aa4a), n_u32(5b9cca4f), n_u32(682e6ff3), 
	n_u32(748f82ee), n_u32(78a5636f), n_u32(84c87814), n_u32(8cc70208), 
	n_u32(90befffa), n_u32(a4506ceb), n_u32(bef9a3f7), n_u32(c67178f2),
};

const sha2_32t i256[8] = {
	n_u32(6a09e667), n_u32(bb67ae85), n_u32(3c6ef372), n_u32(a54ff53a),
	n_u32(510e527f), n_u32(9b05688c), n_u32(1f83d9ab), n_u32(5be0cd19)
};

static sha2_32t  m1[4] = {
	n_u32(00000000), n_u32(ff000000), n_u32(ffff0000), n_u32(ffffff00)
};

static sha2_32t  b1[4] = {
	n_u32(80000000), n_u32(00800000), n_u32(00008000), n_u32(00000080)
};

void CSha256::sha256_compile(sha256_ctx ctx[1])
{
	sha2_32t	v[8], j;

	memcpy(v, ctx->hash, 8 * sizeof(sha2_32t));

	for(j = 0; j < 64; j += 16)
	{
		h2_cycle( 0, j); h2_cycle( 1, j); h2_cycle( 2, j); h2_cycle( 3, j);
		h2_cycle( 4, j); h2_cycle( 5, j); h2_cycle( 6, j); h2_cycle( 7, j);
		h2_cycle( 8, j); h2_cycle( 9, j); h2_cycle(10, j); h2_cycle(11, j);
		h2_cycle(12, j); h2_cycle(13, j); h2_cycle(14, j); h2_cycle(15, j);
	}

	ctx->hash[0] += v[0]; ctx->hash[1] += v[1]; ctx->hash[2] += v[2]; ctx->hash[3] += v[3];
	ctx->hash[4] += v[4]; ctx->hash[5] += v[5]; ctx->hash[6] += v[6]; ctx->hash[7] += v[7];
}

void CSha256::sha256_begin(sha256_ctx ctx[1])
{
	ctx->count[0] = ctx->count[1] = 0;
	memcpy(ctx->hash, i256, 8 * sizeof(sha2_32t));
}

void CSha256::sha256_hash(const unsigned char data[], unsigned long len, sha256_ctx ctx[1])
{   
	sha2_32t pos = (sha2_32t)(ctx->count[0] & SHA256_MASK), space = SHA256_BLOCK_SIZE - pos;
	const unsigned char *sp = data;

	if((ctx->count[0] += len) < len)
		++(ctx->count[1]);

	while(len >= space)
	{
		memcpy(((unsigned char*)ctx->wbuf) + pos, sp, space);
		sp += space; len -= space; space = SHA256_BLOCK_SIZE; pos = 0; 
		bsw_32(ctx->wbuf, SHA256_BLOCK_SIZE >> 2)
			sha256_compile(ctx);
	}

	memcpy(((unsigned char*)ctx->wbuf) + pos, sp, len);
}

void CSha256::sha256_end(unsigned char hval[], sha256_ctx ctx[1])
{
	sha2_32t i = (sha2_32t)(ctx->count[0] & SHA256_MASK);

	bsw_32(ctx->wbuf, (i + 3) >> 2)
		ctx->wbuf[i >> 2] = (ctx->wbuf[i >> 2] & m1[i & 3]) | b1[i & 3];

	if(i > SHA256_BLOCK_SIZE - 9)
	{
		if(i < 60) ctx->wbuf[15] = 0;
		sha256_compile(ctx);
		i = 0;
	}
	else
	{
		i = (i >> 2) + 1;
	}

	while(i < 14)
		ctx->wbuf[i++] = 0;

	ctx->wbuf[14] = (ctx->count[1] << 3) | (ctx->count[0] >> 29);
	ctx->wbuf[15] = ctx->count[0] << 3;

	sha256_compile(ctx);

	for(i = 0; i < SHA256_DIGEST_SIZE; ++i)
		hval[i] = (unsigned char)(ctx->hash[i >> 2] >> 8 * (~i & 3));
}

void CSha256::sha256(unsigned char hval[32], const unsigned char data[], unsigned long len) 
{
	sha256_ctx  cx[1];

	sha256_begin(cx);
	sha256_hash(data, len, cx);
	sha256_end(hval, cx);
}
