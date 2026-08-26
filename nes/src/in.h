#ifndef IN_H_
#define IN_H_

#include "types.h"

uint32 htonl(int val){
  printf("debug: htonl\n");
  return val;
}

uint32 ntohl(uint32 val){
  printf("debug: ntonl\n");
  return val;
}

uint16 htons(uint16 val){
	printf("debug: htons\n");
	return val;
}

#endif
