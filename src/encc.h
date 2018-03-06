#ifndef __ALGO_H_
#define __ALGO_H_


#ifdef __cplusplus 
extern "C" {
#endif

/* 3des ecb */
/* algo_3des_ecb_init
 * 
 * 3des ecb 加密初始化
 * 
 * @param key[in] 加密密匙, 32字节或者48字节长度, 例如密码是
 * "111111111111111111111111", 24个1,这里应该填写
 * "31313131313131313131313131313131313131313131313131313131", 就是1的hex string形式
 * 
 * @return none
 */
void algo_3des_ecb_init(char *key);


/* algo_3des_ecb_encrypt
 * 
 * 3des ecb 加密
 * 
 * @param str[in] 需要加密的字符串, 原始字符串
 * @param enstr[out] 加密输出的字符串,经过base64 编码
 *
 * @return 返回加密后字符串长度
 */
int algo_3des_ecb_encrypt(char *str, char *enstr);

/* algo_3des_ecb_decrypt
 * 
 * 3des ecb 解密
 * 
 * @param str[in] 需要解密的字符串, 经过base64的
 * @param destr[out] 解密后的字符串,
 *
 * @return 返回　解密字符串长度 
 */
int algo_3des_ecb_decrypt(char *str, char *destr);



/* algo_3des_ecb_free
 * 
 * 3des ecb 释放
 * 
 * @param none
 *
 * @return none
 */
void algo_3des_ecb_free();
  
#ifdef __cplusplus
}
#endif

#endif


