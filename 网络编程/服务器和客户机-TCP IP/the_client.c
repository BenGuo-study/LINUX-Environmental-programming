/* �����̵�Ŀ�ľ���ֱ�ӻ��߼��ͨ������Э��ʵ�ּ����֮���ͨѶ
�����������������⣺
��1�����׼ȷ��λ�����ϵļ����
��2���ҵ����������ν������ݴ���

�������е�������ģ���ǿͻ���/�������ṹ����C/S�ṹ

IP��ַ��������ȷ��һ̨���������������ͨѶ�Ķ���׼ȷ�����������������������������еĽ���.

һ���������ж������ִ�У�����IP��ַ�ǲ����ģ���ʱ��������˶˿ںţ�ֻ��IP��ַ�Ͷ˿ںŶ�Ӧ����ȷ��ĳһ�����̡�
����������IP��ַ�Ͷ˿ںŵģ������׽��֣���socket

�׽�����IP/TCPͨѶ�Ļ�����Ԫ���ɽ��׽��ֿ�����ͬ����֮��˫��ͨ�ŵĶ˵�
ÿ������ͨ����Ҫһ���׽��֣��ͻ���һ����������һ�� */

#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>

#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/fb.h>
#include <linux/un.h>

#include <arpa/inet.h>
#include <netinet/in.h>

//�ͻ���

int main(int argc,char *argv[])        /*  ./the_client 192.168.91.63 (��IP������ubuntu��IP��ַһ��)  50001���˿ںţ� */
{
	//�����׽���
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	
	//����Э��ṹ��
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);    //�ṹ����ֽ���
	bzero(&addr,len);                //�ṹ������
	
	addr.sin_family      = AF_INET;   //�������Э���
	addr.sin_addr.s_addr = inet_addr(argv[1]);  //IP��ַ��inet_adddr()��һ��IP��ַ����Ϊһ������ֵ
	addr.sin_port        = htons(atoi(argv[2])); //�˿ںţ�htons()���޷��Ŷ�����ת��Ϊ�����ֽ���
	
	//����������������ӣ���һֱ�����ȴ�
	//��ͼ���ӷ�����~~~
	connect(fd, (struct sockaddr *)&addr, len);
	
	char buf1[200];
	char buf2[200];
	
	while(1)
	{
		bzero(buf1,200);
		fgets(buf1, 200, stdin);
		//�����ݴ���������
		write(fd, buf1, strlen(buf1));
		
		bzero(buf2, 200);
		read(fd, buf2, 200);
		printf("���յ���������Ϣ��%s\n",buf2);
		
	}
	close(fd);
	
	return 0;
	
}











