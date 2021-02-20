//Netpackage Format: Opr(3)+Gaddr(32)+Size(13)+Data(64*8)

typedef uint3 net_opr_t;
typedef uint32 net_gaddr_t;
typedef uint13 net_size_t;

#define DATA_LEN 64
#define MEM_RANGE 4
#define BUFFERSIZE_onchip 32

// typedef (void*) net_data_t;

typedef struct {
   net_opr_t opr;
   net_gaddr_t gaddr;
   net_size_t size;
   char data[DATA_LEN];
} Netpackage;

struct {
   char access;
   time_t time;
   net_gaddr_t gaddr;
   char* data[MEM_RANGE];
} BufferOnchip[BUFFERSIZE_onchip];

char * mem_simu;
struct {
	char access;
	time_t time;	
}INFO_MEM[1024*1024/MEM_RANGE];

int i;

void init_period()
{
	//init_dram
	mem_simu = (char *)malloc(DATA_LEN * 1024*1024); // 64MB

	//init_onchip_buf
	for(i = 0;i < BUFFERSIZE_onchip;i++)
	{
		BufferOnchip[i].access = 0;
		BufferOnchip[i].data[0] = (char *)malloc(MEM_RANGE * DATA_LEN);
	}
	const num = 1024*1024/MEM_RANGE;
	for(i = 0;i < num;i++)
	{
		INFO_MEM[i].access = 0;
	}

	return;
}

char get_netpackage(Netpackage* netpack)
{
	Netpackage *new_netpack = (Netpackage *)malloc(sizeof(Netpackage));
out:
	netpack = new_netpack;
	return 0;
error:
	return 1;
}

//return (value of buffer's number + 1)
char is_onchip(Netpackage* netpack)  
{
	for(i = 0;i < BUFFERSIZE_onchip;i++)
	{
		if(netpack->gaddr >= BufferOnchip[i].gaddr && netpack->gaddr < BufferOnchip[i].gaddr+DATA_LEN*MEM_RANGE )
			return (i+1);
	}
	return 0;
}


char migration(Netpackage* netpack)
{
	//update surce_obj access value
	int index = netpack->gaddr - mem_simu;
	char gap = now_time - INFO_MEM[index].time;
	while(gap)
	{
		INFO_MEM[index].access /= 2;
		gap--;
	}


	//TODO
	//find lastest active one obj on chip and swap it
	for(i = 0;i < BUFFERSIZE_onchip;i++)
	{
		//update onchip_obj access value
		gap = now_time - BufferOnchip[i].time;
		while(gap)
		{
			BufferOnchip[i].access /= 2;
			gap--;
		}
	}
	

	

}

void func_READ(Netpackage* netpack , char num)
{
	if(num == 0)
	{
		//func_MEM_READ(netpack);
		memcpy(netpack->gaddr, netpack->data, netpack->size);
		usleep(10);
	}
	else
	{
		//func_CHIP_READ(netpack);
		off_t offset = netpack->gaddr - BufferOnchip[num].gaddr;//app need to force the offset is reasonable
		memcpy(BufferOnchip[num].data[0] + offset, netpack->data, netpack->size);
		usleep(1);
	}
}

void func_WRITE(Netpackage* netpack , char num)
{
	if(num == 0)
	{
		//func_MEM_WRITE(netpack);
		memcpy(netpack->data, netpack->gaddr, netpack->size);
		usleep(10);
	}
	else
	{
		//func_CHIP_WRITE(netpack);
		off_t offset = netpack->gaddr - BufferOnchip[num].gaddr;
		memcpy(netpack->data, BufferOnchip[num].data[0] + offset, netpack->size);
		usleep(1);
	}
}


void deal_netpackage(Netpackage* netpack)
{
	char i = is_onchip(netpack);
	if(i == 0)
		i = migration(netpack);

	uint3 opr_flag = netpack->opr;
	switch(opr_flag)
	{
		case 1: func_READ(netpack, i); //read
		case 2: func_WRITE(netpack, i); //write
	}
}

int main(int argc, char **argv)
{
	init_period();

	Netpackage* netpack;
	while(1)
	{
		if(get_netpackage(netpack))
			break;

		deal_netpackage(netpack);
	}
	return 0;
}

