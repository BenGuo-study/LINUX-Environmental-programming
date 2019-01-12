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

//������

int main(int argc,char *argv[])
{
	//����һ���׽���
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	
	struct sockaddr_in addr;
	socklen_t len = sizeof(addr);
	
	bzero(&addr, len);    //��ʼ���ṹ��
	
	addr.sin_family      = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);  //htonl()��������ת��Ϊ�޷��ų����͵������ֽ���INADDR_ANY��ʾ���е�����IP��������Ϊ��������IP��ַ��˵���˾����Զ���ȡIP
	addr.sin_port        = htons(50001);       //htons()���޷��Ŷ�����ת��Ϊ�����ֽ���
	
	//�����ص�ַ��һ�׽��ֽӿڰ�
	bind(fd, (struct sockaddr *)&addr, len);
	
	
	// ���ü���״̬�����ͬʱ������
	//��δ���ӵ��׽���ת��Ϊһ�������׽���
	listen(fd, 3);  //��������7��Ĭ���Ѿ�����4��
	
	struct sockaddr_in cliaddr;
	socklen_t clilen = sizeof(cliaddr);
	bzero(&cliaddr,clilen);
	
	//�����ȴ��ͻ��˵���������
	int sockfd = accept(fd, (struct sockaddr *)&cliaddr,&clilen);
	//�ȴ��ɹ����ÿͻ����׽��ֵ�IP��ַ�Ͷ˿���Ϣ
	printf("��ӭ[%s:%hu]\n",
			inet_ntoa(cliaddr.sin_addr),  //��һ��32λIP��ַת��Ϊʮ����IP��ַ
			ntohs(cliaddr.sin_port));  	  //��16λ�����ֽ���ת��Ϊ�����ֽ���

	char buf[200];
	while(1)
	{
		bzero(buf,200);
		if(read(sockfd, buf, 200) == 0)
			break;
		
		write(sockfd, buf, strlen(buf));
		
	}
	
	close(sockfd);
	close(fd);
	return 0;
}