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
 * ����Ϊ���߼��Ĳ��ԣ�˳��������鳤��Ϊ4���ڴ��ַ��
 * ѭ����һ��10��֮�������ڴ����ν���Ƭ�ϻ��棬�ȶ�����ֱ�Ϊ1/2/3
 * ���еڶ���ѭ��10��֮�󣬵�һ���ڴ�ķ��ʴ���С�ڵڶ����ڴ�(if��i>10��i=3),�ȶ�����Ϊ2/1/3
 * ���е�����ѭ��10��֮�󣬵�һ���ڴ�ķ��ʴ���С�ڵ������ڴ棬�ȶ�����Ϊ2/3/1
 * ���ͼ�е�BufferOnchip.indexֵ�ı仯����������仯���ơ�
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
