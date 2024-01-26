#include <windows.h>
#include <tchar.h>
#include <TlHelp32.h>
#include <locale.h>
#include <stdio.h>

/**
 * ���ݽ������Ʋ��ҽ���ID
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

	//��ȡҪע����̵�hProcess
	DWORD pid = FindProcessByName(TEXT("��ע�뷽.exe"));

	printf("��ȡע�����ID: %d\n", pid);

	//��ȡ���̾��
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	//��ȡLoadLibraryW��ַ
	HMODULE kernel32 = LoadLibrary(TEXT("kernel32.dll"));
	FARPROC fn = GetProcAddress(kernel32, "LoadLibraryW");

	//���������ڴ棬д�����
	TCHAR dllPath[] = { TEXT("ע��DLL.dll") };
	LPVOID address = VirtualAllocEx(hProcess, NULL, (_tcslen(dllPath) + 1) * sizeof(TCHAR), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	//���ַ���д���ַ
	WriteProcessMemory(hProcess, address, dllPath, (_tcslen(dllPath) + 1) * sizeof(TCHAR), NULL);

	//����Զ���߳�
	HANDLE ht = CreateRemoteThread(hProcess, NULL, 0, (PTHREAD_START_ROUTINE)fn, address, 0, NULL);

	//�ȴ��߳̽������˳������ע���DLL�ļ���ַַ
	WaitForSingleObject(ht, INFINITE);

	DWORD exitCode = 0;

	GetExitCodeThread(ht, &exitCode);

	printf("exitCode: 0x%x\n", exitCode);

	VirtualFreeEx(hProcess, address, 0, MEM_RELEASE);
	CloseHandle(ht);
	CloseHandle(hProcess);

	return 0;
}