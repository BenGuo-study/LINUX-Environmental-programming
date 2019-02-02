

#include "paino.h"

bool key = 0;
int voice = -20;      //�������� -50
int suspend = 0;        //��־λ���ж��Ƿ񰴵�����ͣ����Ĭ��û����

//��ʾ������
void display_interface()         
{
	display("./jpg/background.jpg",0,0);      //����
	display("./jpg/bar.jpg",0,0);         //ͷ��
	
	int key_num;
	for(key_num=0;key_num<12;key_num++)
	{
		display("./jpg/key_off.jpg",10+key_num*65,47);     //���ٰ���,δ����ʱ
	}
	display("./jpg/logo.jpg",230,370);             //logoͼƬ
	
	display("./jpg/voice01.jpg",10,370);          //�ϵ�������
	display("./jpg/voice02.jpg",100,370);         //�µ�������
	
}


int touch(int tp)
{
	if(tp == -1)
	{
		perror("�������ļ���ʧ�ܣ�\n");
		exit(0);
	}
	
	struct input_event buf;
	
	int key_x;
	int key_y;
	int num;             //������ʾ�������
	
	while(1)
	{
        bool xdone;
	 
		bzero(&buf,sizeof(buf));
	    read(tp,&buf,sizeof(buf));
		
		if(buf.type == EV_ABS)
		{
			if(buf.code == ABS_X)
			{
				key_x = buf.value;
				xdone = 1;
				//printf("(%d ,",key_x);
			}
				
			if(buf.code ==ABS_Y)
			{
				key_y = buf.value;
				//printf(" %d)\n",key_y);
			}
		}

		if(key_y >=47 && key_y <=327 && xdone == 1)
		{
			switch(key_x)
			{

				case 0 ... 75:
						key = 1;       //��ʾ��ָ������
						num = 1;
						return num;
				case 76 ... 724:
						key = 1; 
						num = (key_x/65)+1;
				        return num;
				case 725 ... 800:
						key = 1;
						num = 12;
						return num;	
				default:
					return 0;
			}
		}
		
		if(key_x >= 10 && key_x <= 75 &&
		   key_y >= 370 && key_y <= 435)
		{
		    voice++;	                          //����+1
			printf("voice = %d\n",voice);
		}
		if(key_x >= 100 && key_x <= 165 &&
		   key_y >= 370 && key_y <= 435)
		{
			   voice--;						     //����-1
			   printf("voice = %d\n",voice);
		}
	
		
		if(key_x >= 230 && key_x <= 572 &&       //�˳�����
		   key_y >=370 && key_y <= 436)
		{
			printf("The ending!\n");
			display("./jpg/cat02.jpg",0,0);
			close(tp);
			break;
		}
	    
	
	
	   
		  if(buf.type == EV_KEY && buf.code == BTN_TOUCH && buf.value == 0)
		{
			key = 0;                             //��ʾ��ָ�Ѿ��뿪
			 int i;
			for(i=0;i<12;i++)
			{
				display("./jpg/key_off.jpg",10+i*65,47);     
			} 
			return num;	
		}   	
	}
}





// ��jpeg�ļ���ѹ��ͼ�����ݶ������ŵ�jpg_buffer��ȥ�ȴ�����
unsigned long read_image_from_file(int fd,
				   unsigned char *jpg_buffer,
				   unsigned long jpg_size)
{
	unsigned long nread = 0;
	unsigned long total = 0;

	while(jpg_size > 0)
	{
		nread = read(fd, jpg_buffer, jpg_size);

		jpg_size -= nread;
		jpg_buffer += nread;
		total += nread;
	}
	close(fd);

	return total;
}

//��ʾͼƬ��ָ������
void display(char *filename,int x,int y)    //ͼƬ��  x����   y����
{
	// ***** 1����ȡJPG��ʽ��ͼƬ�ļ� ****** //

	// ��ȡͼƬ�ļ�������Ϣ
	// ���������С�����ڴ滺����jpg_buffer
	struct stat file_info;
	int ret_stat = stat(filename, &file_info);
	if(ret_stat == -1)
	{
		perror("�ṹ��stat��ȡ����ʧ�ܣ�\n");
		exit(0);
	}
	int fd = open(filename, O_RDONLY);
	if(fd == -1)
	{
		perror("ͼƬ�ļ���ʧ�ܣ�\n");
		exit(0);
	}

	unsigned char *jpg_buffer;
	jpg_buffer = (unsigned char *)calloc(1, file_info.st_size);
	read_image_from_file(fd, jpg_buffer, file_info.st_size);


	// ***** 2����JPG��ʽ�����RGB ****** //

	// �����������ṹ�壬�Լ��������ṹ��
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	// ʹ��ȱʡ�ĳ���������ʼ���������ṹ��
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	// ���ø�cinfo��ʹ���jpg_buffer�ж�ȡjpg_size���ֽ�
	// ��Щ���ݱ�����������JPEG����
	jpeg_mem_src(&cinfo, jpg_buffer, file_info.st_size);


	// ��ȡJPEG�ļ���ͷ�����ж����ʽ�Ƿ�Ϸ�
	int ret = jpeg_read_header(&cinfo, true);
	if(ret != 1)
	{
		fprintf(stderr, "[%d]: jpeg_read_header failed: "
			"%s\n", __LINE__, strerror(errno));
		exit(1);
	}

	// ��ʼ����
	jpeg_start_decompress(&cinfo);

	struct image_info imageinfo;
	imageinfo.width = cinfo.output_width;
	imageinfo.height = cinfo.output_height;
	imageinfo.pixel_size = cinfo.output_components;

	int row_stride = imageinfo.width * imageinfo.pixel_size;

	// ����ͼƬ�ĳߴ��С������һ����Ӧ���ڴ�rgb_buffer
	// ������Ŵ�jpg_buffer���������ͼ������
	unsigned long rgb_size;
	unsigned char *rgb_buffer;
	rgb_size = imageinfo.width *
			imageinfo.height * imageinfo.pixel_size;
	rgb_buffer = (unsigned char *)calloc(1, rgb_size);

	// ѭ���ؽ�ͼƬ��ÿһ�ж��������뵽bmp_buffer��
	int line = 0;
	while(cinfo.output_scanline < cinfo.output_height)
	{
		unsigned char *buffer_array[1];
		buffer_array[0] = rgb_buffer +
				(cinfo.output_scanline) * row_stride;
		jpeg_read_scanlines(&cinfo, buffer_array, 1);
	}

	// �������ˣ���jpeg��ص���Դ�ͷŵ�
 	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	free(jpg_buffer);
	
	// ***** 3����RGB���ݣ����Ƶطŵ��Դ��� ****** //
	
	// ׼��LCD��Ļ
	int lcd = open("/dev/fb0", O_RDWR);
	if(lcd == -1)
	{
		perror("lcd�ļ���ʧ�ܣ�\n");
		exit(0);
	}

	// ��ȡLCD�豸�ĵ�ǰ����
	struct fb_var_screeninfo vinfo;
	ioctl(lcd, FBIOGET_VSCREENINFO, &vinfo);

	// ���ݵ�ǰLCD�豸���������ʵ���С��FRAMEBUFFR
	unsigned char *FB;
	unsigned long bpp = vinfo.bits_per_pixel;
	FB = mmap(NULL, vinfo.xres * vinfo.yres * bpp/8,
		  PROT_READ|PROT_WRITE, MAP_SHARED, lcd, 0);
		  
	// ��bmp_buffer�е�RGBͼ�����ݣ�д��LCD��FRAMEBUFFER��
	//write_lcd(bmp_buffer, &imageinfo, FB, &vinfo);
	
	//bzero(FB, vinfo.xres * vinfo.yres * 4);
	
	FB += (vinfo.xres * y + x)* vinfo.bits_per_pixel/8;
	
	int lcd_w = imageinfo.width;
	int lcd_h = imageinfo.height;
	
	int row,column;

	for(row=0; row<lcd_h ; row++)                  //���� ��ӡ���ص�
	    {
		   for(column=0; column<lcd_w ; column++)   //���� ��ӡ���ص�
		    {
			unsigned long lcd_offset = (vinfo.xres*row + column) * 4;
			unsigned long rgb_offset = (imageinfo.width*row+column) *
						    imageinfo.pixel_size;

			memcpy(FB + lcd_offset + vinfo.red.offset/8,
			       rgb_buffer + rgb_offset + 0, 1);
			memcpy(FB + lcd_offset + vinfo.green.offset/8,
			       rgb_buffer + rgb_offset + 1, 1);
			memcpy(FB + lcd_offset + vinfo.blue.offset/8,
			       rgb_buffer + rgb_offset + 2, 1);
		    }
	    }
	close(lcd);
}