#include <stdio.h>
#include <locale.h>
#include <windows.h>

int main()
{
	setlocale(LC_ALL, "CHS");

	printf("��ע�뷽�����������У�����ID��%d\n", GetCurrentProcessId());

	//��ֹ�����˳�
	getchar();
	return 0;
}