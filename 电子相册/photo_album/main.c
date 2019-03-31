

#include "show_jpeg.h"
#include "dlinklist.h"


//��ȡ����ʱ��һ������ĺ�����
int swiping_first_point(int tp,struct input_event buf,int x1);
	
//��ȡ�����Ŀ¼�ÿһ���ļ�����һ��Ŀ¼�����Ŀ¼dp�е��ļ�����Ϣ�ŵ�����ڵ���
void store_information(struct dirent *ep,DIR *dp,dlinklist head);  

// ���ݲ����ĺ�׺������Ҫ�����ĸ���̬��,�� display����ָ��ָ����еľ���ĺ���
void Load_dynamic_library();





int main(int argc, char **argv) // ./showimg [image]
{
	//����Ĵ���
	dlinklist head = init_dlinklist();              //�����ʼ��
	
	//��Ŀ¼
	DIR *dp = opendir(argv[1]);
	// ����Ŀ¼
	chdir(argv[1]);

	struct dirent *ep;
	// ��ȡ�����Ŀ¼�ÿһ���ļ�����һ��Ŀ¼�����Ŀ¼dp�е��ļ�����Ϣ�ŵ�����ڵ���
	store_information(ep,dp,head);      
	//��������Ϣ��ӡ����
	show_dlinklist(head);               

	// ���ݲ����ĺ�׺������Ҫ�����ĸ���̬�⣬�� display����ָ��ָ����еľ���ĺ���
	Load_dynamic_library();

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
     x1 = swiping_first_point(tp,buf,x1);      //��ȡ����ʱ��һ������ĺ�����

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
	
	if(x2 - x1 >= 50 )
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
	
	 if(x2 - x1 <50)
	{
		display(photo->name, fbmem, &vinfo, 0, 0);
	}	
	
	  if(x1 - x2 > 50 )
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
	
	 if(x1 - x2 < 50)
	{
		 display(photo->name, fbmem, &vinfo, 0, 0);
	}
}

	// 3���ͷ���Դ
	close(tp);
	return 0;
}







//��ȡ����ʱ��һ������ĺ�����
int swiping_first_point(int tp,struct input_event buf,int x1)
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
	return x1;
}
//��ȡ�����Ŀ¼�ÿһ���ļ�����һ��Ŀ¼�����Ŀ¼dp�е��ļ�����Ϣ�ŵ�����ڵ���
void store_information(struct dirent *ep,DIR *dp,dlinklist head)  
{
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
}

// ���ݲ����ĺ�׺������Ҫ�����ĸ���̬��,�� display����ָ��ָ����еľ���ĺ���
void Load_dynamic_library()
{
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
}








