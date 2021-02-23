#include <stdlib.h>
#include "lab.h"
/*
 * 对各类对象进行初始化
 */
void init_period()
{
	//init_dram
	if(i1 == 1)
		return ;
	i1 = 1;

	//mem_simu = (char *)malloc(DATA_LEN * 1024); // 64KB

	//init_onchip_buf
	small_tag = -1;
	for(i = 0;i < BUFFERSIZE_onchip;i++)
	{
		BufferOnchip[i].access = 0;
		//BufferOnchip[i].data[0] = (char *)malloc(MEM_RANGE * DATA_LEN);
		BufferOnchip[i].time = &now_time1;
		BufferOnchip[i].index = i;
		BufferOnchip[i].gaddr = -1;
	}
	*(BufferOnchip[0].time) = 0;
	const num = 1024*1024/MEM_RANGE;
	for(i = 0;i < num;i++)
	{
		INFO_MEM[i].access = 0;
	}

	return;
}

/*
 * 判断接受的网络包操作数据是否在片上缓存buffer
 * return:片上缓存buffer索引的index+1，0表示不在片上缓存内
 */
char is_onchip(Netpackage* netpack)
{
	//update onchip_obj access value
	if(now_time != *(BufferOnchip[0].time) )
	{
		*(BufferOnchip[0].time) = now_time;
		for(i = 0;i < small_tag;i++)
			BufferOnchip[i].access /= 2;
	}

	char index, i;
	for(i = 0;i < BUFFERSIZE_onchip;i++)
	{
		index = BufferOnchip[i].index;
		if(BufferOnchip[index].gaddr == -1) continue;
		if(netpack->gaddr >= BufferOnchip[index].gaddr)
		{
			net_gaddr_t  temp = BufferOnchip[index].gaddr+DATA_LEN*MEM_RANGE;
			if(netpack->gaddr <temp)
			{
				BufferOnchip[index].access ++;
				return (i+1);
			}
		}
	}
	return 0;
}


/*
 * 累加访问次数，判断热度是否大于片上缓存最冷数据块并进行迁移。
 * return:片上缓存buffer索引的index+1，0表示新数据不在片上缓存内
 */
char migration(Netpackage* netpack)
{
	//update surce_obj access value
	int index = netpack->gaddr / MEM_RANGE / DATA_LEN;
	char gap = now_time - INFO_MEM[index].time;
	while(gap)
	{
		INFO_MEM[index].access /= 2;
		gap--;
		if(! INFO_MEM[index].access)
			break;
	}
	INFO_MEM[index].access ++;

	//find lastest active one obj on chip and swap it
	char tag = 0;
	if(small_tag + 1 < BUFFERSIZE_onchip)
	{
		small_tag ++;
		tag = 1;
	}
	int Onchip_index = BufferOnchip[small_tag].index;
	if(INFO_MEM[index].access > BufferOnchip[Onchip_index].access) //find one
		tag = 2;
	if(tag > 0)
	{
		//write from chip to MEM
		if(tag == 2)
		{
			char mem_index = BufferOnchip[Onchip_index].gaddr / MEM_RANGE / DATA_LEN;
			//memcpy(BufferOnchip[Onchip_index].gaddr + mem_simu, BufferOnchip[Onchip_index].data[0], DATA_LEN * MEM_RANGE);
			INFO_MEM[mem_index].time = *(BufferOnchip[0].time);
			INFO_MEM[mem_index].access = BufferOnchip[Onchip_index].access;
		}
		//read from MEM to chip
		BufferOnchip[Onchip_index].access = INFO_MEM[index].access;
		BufferOnchip[Onchip_index].gaddr = index * MEM_RANGE * DATA_LEN;
		//memcpy(BufferOnchip[Onchip_index].data[0], BufferOnchip[Onchip_index].gaddr + mem_simu, DATA_LEN * MEM_RANGE);
		return small_tag+1;
	}
	return 0;
}

/*
 * 更新片上缓存的热度信息和顺序关系，动态维护热度数组。
 * return:片上缓存buffer的数据的index+1
 */
char update_bufferonchip(Netpackage* netpack, char index)
{
	char i = 1;
	int Onchip_index_forward, Onchip_index;
	while((index - i) >= 0)
	{
		Onchip_index = BufferOnchip[index - (i-1)].index;
		Onchip_index_forward = BufferOnchip[index - i].index;
		if(BufferOnchip[Onchip_index].access > BufferOnchip[Onchip_index_forward].access)
		{
			BufferOnchip[index - i].index = Onchip_index;
			BufferOnchip[index - (i-1)].index = Onchip_index_forward;
		}
		i++;
	}
	if(index == -1) return 0;
	return BufferOnchip[index].index+1;
}

/*
 * 读数据函数，根据接受的参数判断需要从内存中读取数据还是直接从片上缓存区读取数据。
 * Note：由于时间限制，未能将host内存和PYQN版对接，所以此处代码仅进行逻辑判断，不进行数据访问操作，
 * return:无
 */
void func_READ(Netpackage* netpack , char num)
{
	if(num == 0)
	{
		//func_MEM_READ(netpack);
		//memcpy(netpack->gaddr + mem_simu, netpack->data, netpack->size);
		//usleep(10);
	}
	else
	{
		num --;
		//func_CHIP_READ(netpack);
		off_t offset = netpack->gaddr - BufferOnchip[num].gaddr;//app need to force the offset is reasonable
		//memcpy(BufferOnchip[num].data[0] + offset, netpack->data, netpack->size);
		//usleep(1);
	}
}

/*
 * 写数据函数，根据接受的参数判断需要向内存写入数据还是直接向片上缓存区写入数据。
 * Note：由于时间限制，未能将host内存和PYQN版对接，所以此处代码仅进行逻辑判断，不进行数据访问操作，
 * return:无
 */
void func_WRITE(Netpackage* netpack , char num)
{
	if(num == 0)
	{
		//func_MEM_WRITE(netpack);
		//memcpy(netpack->data, netpack->gaddr + mem_simu, netpack->size);
		//usleep(10);
	}
	else
	{
		num --;
		//func_CHIP_WRITE(netpack);
		off_t offset = netpack->gaddr - BufferOnchip[num].gaddr;
		//memcpy(netpack->data, BufferOnchip[num].data[0] + offset, netpack->size);
		//usleep(1);
	}
}


void deal_netpackage(Netpackage* netpack)
{

	init_period();

	char i;
	i = is_onchip(netpack);
	if(i == 0)
		i = migration(netpack);
	i = update_bufferonchip(netpack, i-1); //i(index of index) to i(index of data)

	net_opr_t opr_flag = netpack->opr;
	switch(opr_flag)
	{
		case 1:
			func_READ(netpack, i);//read
			break;
		case 2:
			func_WRITE(netpack, i); //write
			break;
	}
	return;
}
