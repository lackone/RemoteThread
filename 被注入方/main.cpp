#include <stdio.h>
#include <locale.h>
#include <windows.h>

int main()
{
	setlocale(LC_ALL, "CHS");

	printf("被注入方，进程运行中，进程ID：%d\n", GetCurrentProcessId());

	//防止进程退出
	getchar();
	return 0;
}