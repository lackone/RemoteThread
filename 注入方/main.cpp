#include <windows.h>
#include <tchar.h>
#include <TlHelp32.h>
#include <locale.h>
#include <stdio.h>

/**
 * 根据进程名称查找进程ID
 */
DWORD FindProcessByName(LPCTSTR processName)
{
	DWORD pid = 0;
	HANDLE hs = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hs == INVALID_HANDLE_VALUE)
	{
		return pid;
	}
	PROCESSENTRY32 ps{ 0 };
	ps.dwSize = sizeof(PROCESSENTRY32);
	BOOL ret = Process32First(hs, &ps);
	while (ret)
	{
		if (_tcsicmp(ps.szExeFile, processName) == 0)
		{
			pid = ps.th32ProcessID;
			return pid;
		}

		ret = Process32Next(hs, &ps);
	}
	return pid;
}

int main()
{
	setlocale(LC_ALL, "CHS");

	//获取要注入进程的hProcess
	DWORD pid = FindProcessByName(TEXT("被注入方.exe"));

	printf("获取注入进程ID: %d\n", pid);

	//获取进程句柄
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	//获取LoadLibraryW地址
	HMODULE kernel32 = LoadLibrary(TEXT("kernel32.dll"));
	FARPROC fn = GetProcAddress(kernel32, "LoadLibraryW");

	//创建参数内存，写入参数
	TCHAR dllPath[] = { TEXT("注入DLL.dll") };
	LPVOID address = VirtualAllocEx(hProcess, NULL, (_tcslen(dllPath) + 1) * sizeof(TCHAR), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	//把字符串写入地址
	WriteProcessMemory(hProcess, address, dllPath, (_tcslen(dllPath) + 1) * sizeof(TCHAR), NULL);

	//创建远程线程
	HANDLE ht = CreateRemoteThread(hProcess, NULL, 0, (PTHREAD_START_ROUTINE)fn, address, 0, NULL);

	//等待线程结束，退出码就是注入后，DLL的加载址址
	WaitForSingleObject(ht, INFINITE);

	DWORD exitCode = 0;

	GetExitCodeThread(ht, &exitCode);

	printf("exitCode: 0x%x\n", exitCode);

	VirtualFreeEx(hProcess, address, 0, MEM_RELEASE);
	CloseHandle(ht);
	CloseHandle(hProcess);

	return 0;
}