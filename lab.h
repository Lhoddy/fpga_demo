#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

typedef char net_opr_t;
typedef uint64_t net_gaddr_t;
typedef size_t net_size_t;

#define DATA_LEN 2
#define MEM_RANGE 2
#define BUFFERSIZE_onchip 3

// typedef (void*) net_data_t;

typedef struct {
   net_opr_t opr;
   net_gaddr_t gaddr;
   net_size_t size;
   char data[DATA_LEN];
} Netpackage;

struct {
   char index;// index for BufferOnchip

   char access;
   time_t *time;
   net_gaddr_t gaddr;
   char* data[MEM_RANGE];
} BufferOnchip[BUFFERSIZE_onchip];
int small_tag;

void * mem_simu;
struct {
	char access;
	time_t time;
}INFO_MEM[1024/MEM_RANGE];

int i;
int i1;
time_t now_time;
time_t now_time1;
