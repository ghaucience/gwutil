#include "encc.h"
#include "des.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 3DES ECB */
static Des	*des = NULL;

void algo_3des_ecb_init(char *key) {
	if (des != NULL) {
		delete des;
		des = NULL;
	}

	des = new Des(key);
}

int algo_3des_ecb_encrypt(char *str, char *enstr) {
	if (des == NULL) {
		return -1;
	}
		
	std::string es = des->desEncrypt(str);
	
	if (es.compare(nullptr) == 0) {
		return -2;
	}

	strcpy(enstr, es.c_str());

	return strlen(enstr);
}

int algo_3des_ecb_decrypt(char *str, char *destr) {
	if (des == NULL) {
		return -1;
	}

	std::string ds = des->desDecrypt(str);

	if (ds.compare(nullptr) == 0) {
		return -2;
	}

	strcpy(destr, ds.c_str());

	return strlen(destr);
}

void algo_3des_ecb_free() {
	delete des;
	des = NULL;
}
