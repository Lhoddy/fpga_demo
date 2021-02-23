#include "lab.h"

//char get_netpackage(Netpackage* netpack)
//{
//	Netpackage *new_netpack = (Netpackage *)malloc(sizeof(Netpackage));
//	new_netpack->opr = 1;
//	new_netpack->gaddr = 64;
//	new_netpack->size = 8;
//out:
//	netpack = new_netpack;
//	return 0;
//error:
//	return 1;
//}

/*
 * 仅作为对逻辑的测试，顺序访问三块长度为4的内存地址，
 * 循环第一个10次之后，三块内存依次进入片上缓存，热度排序分别为1/2/3
 * 进行第二个循环10次之后，第一块内存的访问次数小于第二块内存(if（i>10）i=3),热度排序为2/1/3
 * 进行第三个循环10次之后，第一块内存的访问次数小于第三块内存，热度排序为2/3/1
 * 结果图中的BufferOnchip.index值的变化体现了这个变化趋势。
 */
int main(int argc, char **argv)
{
	Netpackage netpack;
	netpack.opr = 1;
//	netpack.size = 8;
//	netpack.data[0] = 1;
	int i = 0;
	while(1)
	{
		//if(get_netpackage(netpack))
		//	break;
		netpack.gaddr = i;
		i += 1;
		if(i > 10) i = 3;
		deal_netpackage(netpack);
	}
	return 0;
}
