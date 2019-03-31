#include "dlinklist.h"



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