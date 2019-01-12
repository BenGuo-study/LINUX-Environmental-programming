

#include "head.h"
#include "cJSON.h"

int Socket(int domain, int type, int protocol)
{
	int fd = socket(domain, type, protocol);
	if(fd == -1)
	{
		perror("�׽��ִ���ʧ��!\n");
		exit(0);
	}
	return fd;
}


int Connect(int fd, struct sockaddr *addr, socklen_t len)
{
	int ret = connect(fd, addr, len);
	if(ret == -1)
	{
		perror("���ӷ�����ʧ�ܣ�\n");
		exit(0);
	}
	return ret;
}

char *http_request(char *city)
{
    static char buf[500];    //�������static ����������Ѻ������ر��ر����ĵ�ַ��ִ��ʱ�����ᷢ���δ���
	bzero(buf, 500);
	
    /* �����ڰ�������һ���ṩ����Ԥ���Ʒ����API�ƶˣ��������������Թ��ߣ����µ���Ϣ�����������ҵ���
	GET ��Http Method
	/spot-to-weather ��Path��ʽ
	HTTP/1.1 ָ����HTTPЭ��
	ali-weather.showapi.com   ���ṩ������ƶ˵�����
	17e8b5a89e2544b289d4ad1a2fee4052 �������ڹ��ﳵ���߹������̨�ҵ� 
	��Ϊ�Ӽ��������city�����к���\n�ַ���������strtok����������\nȥ�� */
	
	snprintf(buf, 500, "GET /spot-to-weather?area=%s HTTP/1.1\r\n"			
						"Host: ali-weather.showapi.com\r\n"
						"Authorization:APPCODE 17e8b5a89e2544b289d4ad1a2fee4052\r\n\r\n",
						strtok(city,"\n"));
						
	return buf;
}


void Write(int fd, char *req, int req_len)
{
	int m;
	while(req_len > 0)              //���ϵش��׽�����д��������
	{
		m = write(fd, req, req_len);
		req_len -= m;
		req += m;
	}
}


char *Read(int fd, char *json, int json_len)
{
	int total = 0;
	while(json_len > 0)
	{
		int n = read(fd, json+total, json_len);      //���ϵش��׽����ж�ȡcJSON��Ϣ
		json_len -= n;
		total += n;
	}
	return json;
}
























