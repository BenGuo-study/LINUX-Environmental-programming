#include "show_jpeg.h"

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

// ��jpeg�ļ���ѹ��ͼ�����ݶ������ŵ�jpg_buffer��ȥ�ȴ���ѹ
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

int Stat(const char *filename, struct stat *file_info)
{
	int ret = stat(filename, file_info);

	if(ret == -1)
	{
		fprintf(stderr, "[%d]: stat failed: "
			"%s\n", __LINE__, strerror(errno));
		exit(1);
	}

	return ret;
}

int Open(const char *filename, int mode)
{
	int fd = open(filename, mode);
	if(fd == -1)
	{
		fprintf(stderr, "[%d]: open failed: "
			"%s\n", __LINE__, strerror(errno));
		exit(1);
	}

	return fd;
}

void display(char *jpgfile, char *fbmem,
	     struct fb_var_screeninfo *vinfo,
	     int xoffset, int yoffset)
{
	// ��ȡͼƬ�ļ�������Ϣ
	// ���������С�����ڴ滺����jpg_buffer
	struct stat file_info;
	Stat(jpgfile, &file_info);
	int fd = Open(jpgfile, O_RDONLY);

	unsigned char *jpg_buffer;
	jpg_buffer = (unsigned char *)calloc(1, file_info.st_size);
	read_image_from_file(fd, jpg_buffer, file_info.st_size);


	// ������ѹ���ṹ�壬�Լ��������ṹ��
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	// ʹ��ȱʡ�ĳ���������ʼ����ѹ���ṹ��
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

	// ��ʼ��ѹ
	jpeg_start_decompress(&cinfo);

	struct image_info imageinfo;
	imageinfo.width = cinfo.output_width;
	imageinfo.height = cinfo.output_height;
	imageinfo.pixel_size = cinfo.output_components;

	int row_stride = imageinfo.width * imageinfo.pixel_size;

	// ����ͼƬ�ĳߴ��С������һ����Ӧ���ڴ�rgb_buffer
	// ������Ŵ�jpg_buffer��ѹ������ͼ������
	unsigned long rgb_size;
	unsigned char *rgb_buffer;
	rgb_size = imageinfo.width *
			imageinfo.height * imageinfo.pixel_size;
	rgb_buffer = (unsigned char *)calloc(1, rgb_size);

	// ѭ���ؽ�ͼƬ��ÿһ�ж�������ѹ��rgb_buffer��
	int line = 0;
	while(cinfo.output_scanline < cinfo.output_height)
	{
		unsigned char *buffer_array[1];
		buffer_array[0] = rgb_buffer +
				(cinfo.output_scanline) * row_stride;
		jpeg_read_scanlines(&cinfo, buffer_array, 1);
	}

	// ��ѹ���ˣ���jpeg��ص���Դ�ͷŵ�
 	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	free(jpg_buffer);

	// ����
	bzero(fbmem, vinfo->xres * vinfo->yres * 4);

	// ��rgb_buffer�е�RGBͼ�����ݣ�д��LCD��FRAMEBUFFER��
	char *tmp = fbmem;
	tmp += (yoffset*vinfo->xres + xoffset) * vinfo->bits_per_pixel/8;

	int lcd_w = vinfo->xres - xoffset;
	int lcd_h = vinfo->yres - yoffset;

	int row, column;
	for(row=0; row<lcd_h && row<imageinfo.height; row++)
	{
		for(column=0; column<lcd_w && column<imageinfo.width; column++)
		{
			unsigned long lcd_offset = (vinfo->xres*row + column) * 4;
			unsigned long rgb_offset = (imageinfo.width*row+column) *
						    imageinfo.pixel_size;

			memcpy(tmp + lcd_offset + vinfo->red.offset/8,
			       rgb_buffer + rgb_offset + 0, 1);
			memcpy(tmp + lcd_offset + vinfo->green.offset/8,
			       rgb_buffer + rgb_offset + 1, 1);
			memcpy(tmp + lcd_offset + vinfo->blue.offset/8,
			       rgb_buffer + rgb_offset + 2, 1);
		}
	}

	// �ͷ������Դ
	free(rgb_buffer);
	close(fd);
}















