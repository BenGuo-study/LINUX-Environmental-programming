
#include "paino.h"

extern key;
extern voice;
extern suspend;    //��־λ���ж��Ƿ񰴵�����ͣ����Ĭ��û����


void *routine(void *msg)
{
	pthread_detach(pthread_self());
	int key_num = *(int *)msg;
	
	char buf[50];
	system("killall -s SIGKILL madplay");
	snprintf(buf,30,"madplay ./mp3/d%d.mp3 -a %d",key_num,voice);
	system(buf);
	
	pthread_exit(NULL);      //�˳���ǰ���߳�
}



int main(int argc,char *argv[])
{
	
	pthread_t tid;
	
	//��ʾ������
	display_interface();
	
	//��ָ�������٣���ʾ����ȥ��Ч��
	int tp = open("/dev/input/event0",O_RDONLY);
	
	int key_num;
	int key_x;
	
	int former = 0;     //��ʾǰ״̬�İ������
	int now = 0;        //��ʾ����״̬�İ������
	int move = 753;
	
	
	
	while(1)
	{
		//Сͼ�������
		display("./jpg/heart.jpg",move,0);
		move -= 10;
		if(move < 205)
		{
			move = 753;
		}
		display("./jpg/bar.jpg",0,0);
		
		//ǰһ��״̬���ټ����
		former = now;
		//���ص�ǰ״̬���ټ����
		now = touch(tp);    
		
			if(now >=1 && now <= 12)
			{
				printf("The key number of paino = %d\n",now);

				if(now != former)
				{
			
					display("./jpg/key_on.jpg",10+(now-1)*65,47);
					
					//����һ���߳�,ͨ��������ž���ÿ��������������Ƶ
					pthread_create(&tid, NULL, routine, (void *)&now);  
					
					int i,j;
					for(i=0;i<(now-1);i++)
					{
						display("./jpg/key_off.jpg",10+i*65,47);
					}
					for(j=now;j<12;j++)
					{
						display("./jpg/key_off.jpg",10+j*65,47);
					}
				}
			}
	}
	
	 pthread_exit(NULL);     //�˳���ǰ���߳�            
}
