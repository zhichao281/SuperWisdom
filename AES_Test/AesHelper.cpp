#include "AesHelper.h"
#include "aes.h"
#include "zbase64.h"

CAesHelper::CAesHelper(void)
{
}


CAesHelper::~CAesHelper(void)
{
}

void CAesHelper::StringToHex( const char* pSrc, unsigned char* pDest )
{
	int nSrcLen = 0;
	if( pSrc != 0 )
	{
		nSrcLen = strlen(pSrc);
		memcpy(pDest, pSrc, nSrcLen);
	}
	Padding( pDest, nSrcLen );
}

void CAesHelper::Padding( unsigned char* pSrc, int nSrcLen )
{
	if( nSrcLen < KEYCODELENGTH )
	{
		unsigned char ucPad = KEYCODELENGTH - nSrcLen;
		for( int nID = KEYCODELENGTH; nID > nSrcLen; --nID )
		{
			pSrc[nID - 1] = ucPad;
		}
	}
}

std::string CAesHelper::Encrypt( std::string strSrc, std::string strKey )
{
	ZBase64 tool;
	aes_context ctx;
	const char* pSrc = 0;
	const char* pTmpSrc = 0;
	unsigned char* pDest = 0;
	unsigned char* pTmpDest = 0;
	int nSrcLen = 0;
	int nDestLen = 0;
	unsigned char buf[KEYCODELENGTH];
	unsigned char key[KEYCODELENGTH];
	StringToHex( strKey.c_str(), key );
    aes_set_key( &ctx, key, 128);
	pSrc = strSrc.c_str();
	nSrcLen = strSrc.size();
	nDestLen = (nSrcLen / KEYCODELENGTH) * KEYCODELENGTH + KEYCODELENGTH;
	pDest = new unsigned char[nDestLen];
	memset( pDest, 0, nDestLen );
	pTmpSrc = pSrc;
	pTmpDest = pDest;
	while( (pTmpSrc - pSrc) < nSrcLen )
	{
		StringToHex(pTmpSrc, buf);
		aes_encrypt( &ctx, buf, buf );
		memcpy( pTmpDest, buf, KEYCODELENGTH );
		pTmpSrc += KEYCODELENGTH;
		pTmpDest += KEYCODELENGTH;
	}
	if( (pTmpDest - pDest) < nDestLen )	// if the source size % KEYCODELENGTH == 0 then need to add an extra buffer 
	{
		StringToHex(0, buf);
		aes_encrypt( &ctx, buf, buf );
		memcpy( pTmpDest, buf, KEYCODELENGTH );
	}
    
	std::string strRet = tool.Encode(pDest, nDestLen);
	delete [] pDest;
	return strRet;
}

std::string CAesHelper::Decrypt( std::string strSrc, std::string strKey )
{
	ZBase64 tool;
	aes_context ctx;
	const char* pSrc = 0;
	const char* pTmpSrc = 0;
	unsigned char* pDest = 0;
	unsigned char* pTmpDest = 0;
	int nSrcLen = 0;
	int nDestLen = 0;
	unsigned char buf[KEYCODELENGTH];
	unsigned char key[KEYCODELENGTH];
	StringToHex( strKey.c_str(), key );
    aes_set_key( &ctx, key, 128);
	std::string strSrcHex = tool.Decode(strSrc.c_str(), strSrc.size(), nSrcLen);
	pSrc = strSrcHex.data();
	nDestLen = nSrcLen;
	pDest = new unsigned char[nDestLen];
	memset( pDest, 0, nDestLen );
	pTmpSrc = pSrc;
	pTmpDest = pDest;
	while( (pTmpSrc - pSrc) < nSrcLen )
	{
		memcpy(buf, pTmpSrc, KEYCODELENGTH);
		aes_decrypt( &ctx, buf, buf );
		memcpy(pTmpDest, buf, KEYCODELENGTH);
		pTmpSrc += KEYCODELENGTH;
		pTmpDest += KEYCODELENGTH;
	}
	unsigned char ucTest = 0;
	while(ucTest = *(pTmpDest - 1))
	{
		if( ucTest > 0 && ucTest <= 0x10 )
			*(pTmpDest-1) = 0;
		else
			break;
		pTmpDest--;
	}
	std::string strRet = (char*)pDest;
	delete [] pDest;
	return strRet;
}