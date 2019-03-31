#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <dirent.h>
#include <dlfcn.h>
#include <strings.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/fb.h>
#include <linux/un.h>
#include <linux/input.h>

#include <arpa/inet.h>
#include <netinet/in.h>

//#include "show_jpeg.h"

typedef struct node           //����Ľṹ��
{
	char *name;
	struct node *next;
	struct node *prev;
}*dlinklist;

bool is_empty(dlinklist head);
dlinklist init_dlinklist();
dlinklist node_dlinklist(char *file_name);
void dlinklist_move_tail(dlinklist new,dlinklist head);
void show_dlinklist(dlinklist head);

char *init_lcd(struct fb_var_screeninfo *vinfo)
{
	int lcd = open("/dev/fb0", O_RDWR);

	ioctl(lcd, FBIOGET_VSCREENINFO, vinfo);
	int bpp = vinfo->bits_per_pixel;
	int screensize = vinfo->xres * vinfo->yres * bpp/8;

	char *fbmem = mmap(NULL, screensize, PROT_READ|PROT_WRITE,
			   MAP_SHARED, lcd, 0);
	if(fbmem == MAP_FAILED)
	{
		perror("ӳ���Դ�ʧ��");
		exit(0);
	}
	return fbmem;
}

void usage(int argc, char **argv)
{
	if(argc != 3)
	{
		printf("��������\n");
		exit(0);
	}
}

int main(int argc, char **argv) // ./showimg [image]
{
	// �жϲ����ĺϷ��ԣ�����Ҫ��һ��ͼƬ����������ָ��Ҫ��ʾ��ͼƬ��
	//usage(argc, argv);
	
	//����Ĵ���
	dlinklist head = init_dlinklist();              //�����ʼ��
	
	//��Ŀ¼
	DIR *dp = opendir(argv[1]);
	// ����Ŀ¼
	chdir(argv[1]);
	// ��ȡ�����Ŀ¼�ÿһ���ļ�����һ��Ŀ¼�
	struct dirent *ep;
	while(1)
	{
		ep = readdir(dp);    //��ʼ��ȡĿ¼�е��ļ�
		if(ep == NULL)
			break;
		if(ep->d_name[0] == '.')
			continue;
		dlinklist new = node_dlinklist(ep->d_name);    //���ļ��������½ڵ�
		if(new == NULL)
		{
			perror("�ڵ㷵�ص�ַʧ�ܣ�\n");
			exit(0);
		}
		dlinklist_move_tail(new,head);    //���½ڵ��������β��
		if(ep == NULL)
		{
			printf("Ŀ¼��ȡ���\n");
			break;
		}
	}
	show_dlinklist(head);               //��������Ϣ��ӡ����

	// ���ݲ����ĺ�׺������Ҫ�����ĸ���̬��
	char *lib;
	lib = "libjpg.so";

	// ��̬���ض�̬��
	void *handle = dlopen(lib/*ָ���Ķ�̬������*/, RTLD_NOW/*�����������еķ���*/);
	if(handle == NULL)
	{
		perror("�򿪶�̬��ʧ��");	
		exit(0);
	}
	

	// ������Ҫ�õ��Ŀ��еķ��ţ�������: display
	void (*display)(char *imgfile, char *fbmem,
			struct fb_var_screeninfo *vinfo,
			int xoffset, int yoffset);

	// �� display ָ����еľ���ĺ���
	display = dlsym(handle, "display");
	if(display == NULL)
	{
		perror("���ҷ���ʧ��");
		exit(0);
	}

	// ׼���� LCD �豸
	struct fb_var_screeninfo vinfo;
	bzero(&vinfo, sizeof(vinfo));
	char *fbmem = init_lcd(&vinfo);

	  dlinklist photo = head->next;
	// ��ָ����ͼƬ argv[1] ���� display ����ȥ����
	display(photo->name, fbmem, &vinfo, 0, 0);
	
	
	// 1���򿪴������ļ�
	int tp = open("/dev/input/event0", O_RDONLY);
	
	// 2����ȡ��������Ϣ
	struct input_event buf; // �������ݽṹ��
	int x1 = 0;
	int x2 = 0;
	int x3 = 0;
	int x_0 = 0;
	int x_1 = 0;
	int i=0;
	
while(1)
{

	while(1)
	{
		bzero(&buf, sizeof(buf));
		read(tp, &buf, sizeof(buf)); // ��ȡ���������ݣ��ŵ� buf ��
		if(buf.type == EV_ABS)
		{
			if(buf.code == ABS_X)
			{
				x1 = buf.value;
				//printf("x1 = %d\n",x1);
				break;
			}
		}
	}
	

	while(1)
	{
		bzero(&buf, sizeof(buf));
		read(tp, &buf, sizeof(buf)); // ��ȡ���������ݣ��ŵ� buf ��

		// �����������������¼�
		if(buf.type == EV_ABS)
		{
			// ����x������
				if(buf.code == ABS_X)
			   {
				  if(i%2==0)
				  {
					   x_0 = buf.value;
				     printf("x_0 = %d\n ", buf.value);
				  }
				  
				  
				  if(i%2 == 1)
				  {
					  x_1 = buf.value;
				     printf("x_1 = %d\n ", buf.value);
				  } 
				  
				//printf("%d\n ", buf.value);
			  }
		}
		
		// ���������¼����������̡��������İ�ѹ���������Ҽ�������
		if(buf.type == EV_KEY)
		{
			
			// �����������İ�ѹ�¼�
			if(buf.code == BTN_TOUCH)
			{
				 //�������´�����
				if(buf.value > 0)
					continue;

				// �����ɿ�������
				if(i%2== 0 && buf.value == 0)
				  {
					x_0 = buf.value;
					break;
				  }
					
				if(i%2== 1 && buf.value == 0)
				  {
					x_1 = buf.value;
					break;
				  }
					
			}
		}
		
		i++;
	}
	
	printf("x1 = %d\n",x1);
	printf("x_0 = %d\n",x_0);
	printf("x_1 = %d\n",x_1);
	if(x_0 !=0)
		x2 = x_0;
	if(x_1 != 0)
		x2 = x_1;
	
	printf("x2 = %d\n",x2);
	printf("\n");
	
	if(x3 == x2)
	{
		display(photo->name, fbmem, &vinfo, 0, 0);
		continue;
	}
	x3 = x2;
	
	/*
	if(x_0 - x1 < 20|| x_1 - x1 <20)
		continue;
	if(x1 - x_0 < 20 || x1 - x_1 < 20)
		continue;
	*/
	
	if(x2 - x1 >= 150 )
	{
		if(x_0!=0 ||x_1!=0)
		{
			photo = photo->next;
		   if(photo == head)
		  {
			photo = head->next;
		  }
		   display(photo->name, fbmem, &vinfo, 0, 0);
		}
	}
	
	 if(x2 - x1 <150)
	{
		display(photo->name, fbmem, &vinfo, 0, 0);
	}	
	
	  if(x1 - x2 > 100 )
	{
		if(x_0!=0 || x_1!=0)
		{
			photo = photo->prev;
		   if(photo == head)
		   {
			photo = head->prev;
		   }
		   display(photo->name, fbmem, &vinfo, 0, 0);
		}
	}
	
	 if(x1 - x2 < 100)
	{
		 display(photo->name, fbmem, &vinfo, 0, 0);
	}

		
	//printf("x1 = %d\n",x1);
	//printf("x_0 = %d\n",x_0);
	//printf("x_1 = %d\n",x_1);
	//printf("x2 = %d\n",x2);
	//printf("\n");
	

}
	// 3���ͷ���Դ
	close(tp);
	return 0;
}



bool is_empty(dlinklist head)        //�ж������Ƿ�Ϊ��
{
	if(head->next==head)
		return true;
	else
		return false;
}

dlinklist init_dlinklist()                //��ʼ������
{
	dlinklist head = calloc(1,sizeof(struct node));
	if(head == NULL)
	{
		perror("�����ڴ�����ʧ��!\n");
		exit(0);
	}
	head->next = head;
	head->prev = head;
	return head;
}


dlinklist node_dlinklist(char *file_name)       //��ʼ���½ڵ�
{
	dlinklist new = calloc(1,sizeof(struct node));
	if(new == NULL)
	{
		perror("�ڵ��ڴ�����ʧ�ܣ�\n");
		exit(0);
	}
	new->name = file_name;
	new->next = new;
	new->prev = new;
	return new;
}



void dlinklist_move_tail(dlinklist new,dlinklist head)     //���½ڵ����������β��
{
	dlinklist temp = head->prev;
	
	temp->next = new;
	new->prev = temp;
	head->prev = new;
	new->next = head;
}


void show_dlinklist(dlinklist head)
{
	if(is_empty(head))
	{
		perror("����Ϊ�գ��޷���ӡ��\n");
		exit(0);
	}
	dlinklist temp = head->next;
	while(temp != head)
	{
		printf("%s\n",temp->name);
		temp = temp->next;
	}
}









