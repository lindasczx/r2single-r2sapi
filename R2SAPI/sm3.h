// GM/T 0004-2012: SM3密码杂凑算法. 国家密码管理局, 2010年12月

/**
* \file sm3.h
* thanks to Xyssl
* SM3 standards:http://www.oscca.gov.cn/News/201012/News_1199.htm
* author:goldboar
* email:goldboar@163.com
* 2011-10-26
*/
#ifndef XYSSL_SM3_H
#define XYSSL_SM3_H


/**
* \brief SM3 context structure
*/
typedef struct
{
    unsigned int total[2]; /*!< number of bytes processed */
    unsigned int state[8]; /*!< intermediate digest state */
    unsigned char buffer[64]; /*!< data block being processed */

//    unsigned char ipad[64]; /*!< HMAC: inner padding */
//    unsigned char opad[64]; /*!< HMAC: outer padding */

}
sm3_context, SM3_CTX;

#ifdef __cplusplus
extern "C" {
#endif

/**
* \brief SM3 context setup
*
* \param ctx context to be initialized
*/
void sm3_starts( sm3_context *ctx );

/**
* \brief SM3 process buffer
*
* \param ctx SM3 context
* \param input buffer holding the data
* \param ilen length of the input data
*/
void sm3_update( sm3_context *ctx, const unsigned char *input, int ilen );

/**
* \brief SM3 final digest
*
* \param ctx SM3 context
*/
void sm3_finish( sm3_context *ctx, unsigned char output[32] );

/**
* \brief Output = SM3( input buffer )
*
* \param input buffer holding the data
* \param ilen length of the input data
* \param output SM3 checksum result
*/
void sm3( const unsigned char *input, int ilen,
           unsigned char output[32]);

//////// r2sapi接口
#include <stddef.h>
int __stdcall SM3_Init(SM3_CTX *c);
int __stdcall SM3_Update(SM3_CTX *c, const void *data, size_t len);
int __stdcall SM3_Final(unsigned char *md, SM3_CTX *c);
unsigned char * __stdcall SM3(const unsigned char *d, size_t n, unsigned char *md);


#ifdef __cplusplus
}
#endif

#endif /* sm3.h */
