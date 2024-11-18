#pragma once

#include "framework.h"
#include "winapi.h"

struct Result {
	static const int AC = 1, WA = 2, TLE = 3, MLE = 4, RE = 5;
	int answer;
	ll time, memory;
};
const String RESULT[] = {TEXT(""), TEXT("[AC]"), TEXT("[WA]"), TEXT("[TLE]"), TEXT("[MLE]"), TEXT("[RE]")};

struct Settings {
	int testCase;
	String std, generator, testCode, compileCode, inputFile, outputFile, answerFile;
	ll timeLimit, memoryLimit;
};

const String runCommand(TEXT("$.exe <@ ># 2>nul")), compareCommand(TEXT("fc @ # 1>nul 2>nul"));

const std::wregex NUMBER(TEXT("\\d{1,9}"));

String replace(String str, TCHAR c, const String src) {
	while (str.find(c) != String::npos)
		str.replace(str.find(c), 1, src);
	return str;
}

int compile(const String& compileCommand, const String& path, const String& fileName) { return System(replace(compileCommand, TEXT('$'), path + fileName).c_str()); }

Result run(const String& path, const String& exe, const String& input, const String& output, ll timeLimit, ll memoryLimit) {
	Result result = {0};
	memoryLimit <<= 20;
	ll memory = 0;

	std::future<int> tsk = std::async([](String command)->int { return System(command.c_str()); }, replace(replace(replace(runCommand, TEXT('$'), path + exe), TEXT('@'), path + input), TEXT('#'), path + output));
	std::chrono::time_point<std::chrono::steady_clock> st = std::chrono::steady_clock::now(), ed = std::chrono::steady_clock::now();

	for (; std::chrono::duration_cast<std::chrono::milliseconds>(ed - st).count() <= timeLimit; ed = std::chrono::steady_clock::now()) {
		switch (tsk.wait_for(std::chrono::milliseconds(1))) {
		case std::future_status::ready:
			result.answer = tsk.get() ? Result::RE : Result::AC;
			goto END;
		case std::future_status::timeout:
		{
			HANDLE process = getProcessHandle(TEXT("test.exe"));
			if (process != NULL)
				memory = max(memory, getProcessMemory(process)), CloseHandle(process);
			if (memory > memoryLimit) {
				result.answer = Result::MLE;
				goto END;
			}
			break;
		}
		}
	}

	{
		result.answer = Result::TLE;
		HANDLE process = getProcessHandle(TEXT("test.exe"));
		if (process != NULL)
			TerminateProcess(process, 0), CloseHandle(process);
	}

END:
	result.time = std::chrono::duration_cast<std::chrono::milliseconds>(ed - st).count(), result.memory = memory >> 20;
	return result;
}

//int compare(const String& path, const String& file1, const  String& file2) { return System(replace(replace(compareCommand, '@', path + file1), '#', path + file2).c_str()); }

void read(std::wifstream& in, std::vector<String>& str) {
	String s;
	while (!in.eof()) {
		TCHAR c = in.get();
		if (c > 32 && c < 127)
			s.push_back(c);
		else if (!s.empty())
			str.push_back(s), s.clear();
	}
}

bool compare(const String& path, const String& file1, const  String& file2) {
	std::wifstream in1(path + file1), in2(path + file2);
	std::vector<String> str1, str2;
	read(in1, str1), read(in2, str2);
	if (str1.size() != str2.size())
		return true;
	for (int i = 0; i < str1.size(); ++i)
		if (str1[i] != str2[i])
			return true;
	return false;
}

bool loadSettings(String path, Settings& settings) {
	String value;

	settings.std = loadString(IDS_DIRECTORY_CODE) + readString(path, IDS_KEY_STD);

	settings.generator = loadString(IDS_DIRECTORY_GENERATOR) + readString(path, IDS_KEY_GENERATOR);

	settings.testCode = loadString(IDS_DIRECTORY_CODE) + readString(path, IDS_KEY_TEST_CODE);

	value = readString(path, IDS_KEY_TESTCASE_COUNT);
	if (!std::regex_match(value, NUMBER))
		return false;
	settings.testCase = std::stoi(value);

	settings.compileCode = readString(path, IDS_KEY_COMPILE_COMMAND);
	if (settings.testCode.empty())
		return false;

	value = readString(path, IDS_KEY_TIME_LIMIT);
	if (!std::regex_match(value, NUMBER))
		return false;
	settings.timeLimit = std::stoi(value);

	value = readString(path, IDS_KEY_MEMORY_LIMIT);
	if (!std::regex_match(value, NUMBER))
		return false;
	settings.memoryLimit = std::stoi(value);

	settings.inputFile = loadString(IDS_DIRECTORY_DATA) + readString(path, IDS_KEY_INPUT_FILE_NAME);

	settings.outputFile = loadString(IDS_DIRECTORY_DATA) + readString(path, IDS_KEY_OUTPUT_FILE_NAME);

	settings.answerFile = loadString(IDS_DIRECTORY_DATA) + readString(path, IDS_KEY_ANSWER_FILE_NAME);

	return true;
}