#include <stdlib.h>
#include "lab.h"
/*
 * �Ը��������г�ʼ��
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
 * �жϽ��ܵ���������������Ƿ���Ƭ�ϻ���buffer
 * return:Ƭ�ϻ���buffer������index+1��0��ʾ����Ƭ�ϻ�����
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
 * �ۼӷ��ʴ������ж��ȶ��Ƿ����Ƭ�ϻ����������ݿ鲢����Ǩ�ơ�
 * return:Ƭ�ϻ���buffer������index+1��0��ʾ�����ݲ���Ƭ�ϻ�����
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
 * ����Ƭ�ϻ�����ȶ���Ϣ��˳���ϵ����̬ά���ȶ����顣
 * return:Ƭ�ϻ���buffer�����ݵ�index+1
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
 * �����ݺ��������ݽ��ܵĲ����ж���Ҫ���ڴ��ж�ȡ���ݻ���ֱ�Ӵ�Ƭ�ϻ�������ȡ���ݡ�
 * Note������ʱ�����ƣ�δ�ܽ�host�ڴ��PYQN��Խӣ����Դ˴�����������߼��жϣ����������ݷ��ʲ�����
 * return:��
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
 * д���ݺ��������ݽ��ܵĲ����ж���Ҫ���ڴ�д�����ݻ���ֱ����Ƭ�ϻ�����д�����ݡ�
 * Note������ʱ�����ƣ�δ�ܽ�host�ڴ��PYQN��Խӣ����Դ˴�����������߼��жϣ����������ݷ��ʲ�����
 * return:��
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
