#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

void util_viewbuf(char *buffer, int len) {
  int i = 0;
  for (i = 0; i < len; i++) {
    printf("[%02X]", buffer[i]&0xff);
    if ( (i + 1) % 16 == 0) {
      printf("\n");
    }
  }
  cout << endl;
}
