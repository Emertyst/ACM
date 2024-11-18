#pragma once

#include "framework.h"
const int MAX_LOADSTRING = 100;

HANDLE getProcessHandle(PCTSTR processName) {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
		return NULL;

	PROCESSENTRY32 processEntry = {0};
	processEntry.dwSize = sizeof(PROCESSENTRY32);

	for (bool nice = Process32First(hSnapshot, &processEntry); nice; nice = Process32Next(hSnapshot, &processEntry))
		if (!wcscmp(processName, processEntry.szExeFile))
			return CloseHandle(hSnapshot) ? OpenProcess(PROCESS_ALL_ACCESS, FALSE, processEntry.th32ProcessID) : NULL;
	return NULL;
}

ll getProcessMemory(HANDLE process) {
	PROCESS_MEMORY_COUNTERS processMemoryCounter = {0};
	GetProcessMemoryInfo(process, &processMemoryCounter, sizeof(PROCESS_MEMORY_COUNTERS));
	return 1ull * processMemoryCounter.PeakWorkingSetSize;
}

bool writeConfig(PCTSTR fileName, PCTSTR key, PCTSTR value) {
	TCHAR app[MAX_LOADSTRING];
	LoadString(NULL, IDS_APP_NAME, app, MAX_LOADSTRING);
	return WritePrivateProfileString(app, key, value, fileName);
}

String loadString(int id) {
	TCHAR res[MAX_LOADSTRING];
	LoadString(NULL, id, res, MAX_LOADSTRING);
	return String(res);
}
String readString(String path, int id) {
	path += loadString(IDS_CONFIG_FILE);
	TCHAR res[MAX_LOADSTRING];
	GetPrivateProfileString(loadString(IDS_APP_NAME).c_str(), loadString(id).c_str(), TEXT(""), res, MAX_LOADSTRING, path.c_str());
	return String(res);
}


bool generateConfigFile(String path) {
	path += loadString(IDS_CONFIG_FILE);
	return writeConfig(path.c_str(), loadString(IDS_KEY_STD).c_str(), loadString(IDS_DEFAULT_STD).c_str())
		&& writeConfig(path.c_str(), loadString(IDS_KEY_GENERATOR).c_str(), loadString(IDS_DEFAULT_GENERATOR).c_str())
		&& writeConfig(path.c_str(), loadString(IDS_KEY_TEST_CODE).c_str(), loadString(IDS_DEFAULT_TEST_CODE).c_str())
		&& writeConfig(path.c_str(), loadString(IDS_KEY_TESTCASE_COUNT).c_str(), loadString(IDS_DEFAULT_TESTCASE_COUNT).c_str())
		&& writeConfig(path.c_str(), loadString(IDS_KEY_COMPILE_COMMAND).c_str(), loadString(IDS_DEFAULT_COMPILE_COMMAND).c_str())
		&& writeConfig(path.c_str(), loadString(IDS_KEY_TIME_LIMIT).c_str(), loadString(IDS_DEFAULT_TIME_LIMIT).c_str())
		&& writeConfig(path.c_str(), loadString(IDS_KEY_MEMORY_LIMIT).c_str(), loadString(IDS_DEFAULT_MEMORY_LIMIT).c_str())
		&& writeConfig(path.c_str(), loadString(IDS_KEY_INPUT_FILE_NAME).c_str(), loadString(IDS_DEFAULT_INPUT_FILE_NAME).c_str())
		&& writeConfig(path.c_str(), loadString(IDS_KEY_OUTPUT_FILE_NAME).c_str(), loadString(IDS_DEFAULT_OUTPUT_FILE_NAME).c_str())
		&& writeConfig(path.c_str(), loadString(IDS_KEY_ANSWER_FILE_NAME).c_str(), loadString(IDS_DEFAULT_ANSWER_FILE_NAME).c_str());
}