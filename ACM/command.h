#pragma once

#include "framework.h"
#include "winapi.h"
#include "judge.h"

const int UNKNOWN = 0, RUN = 1, COMPARE = 2, CREATE = 3, HELP = 4;
const int COLOR_DEFAULT = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED, COLOR_RED = COLOR_DEFAULT | BACKGROUND_RED;
const int COLOR[] = {0, COLOR_DEFAULT | BACKGROUND_GREEN, COLOR_DEFAULT | BACKGROUND_RED, COLOR_DEFAULT | BACKGROUND_GREEN | BACKGROUND_RED, COLOR_DEFAULT | BACKGROUND_GREEN | BACKGROUND_RED, COLOR_DEFAULT | BACKGROUND_BLUE | BACKGROUND_RED};

void printColor(const String& str, int color, int width) {
	if (color != COLOR_DEFAULT) {
		HANDLE cmd = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(cmd, color);
		cout << std::setw(width) << std::setfill(TEXT(' ')) << str << std::flush;
		SetConsoleTextAttribute(cmd, COLOR_DEFAULT);
		//CloseHandle(cmd);
	} else
		cout << std::setw(width) << std::setfill(TEXT(' ')) << str << std::flush;
}

void printLine(const std::vector<int> width) {
	cout << TEXT('+');
	for (int i : width) {
		for (int j = 1; j <= i; ++j)
			cout << TEXT('-');
		cout << TEXT('+');
	}
	cout << std::endl;
}

void printTable(const std::vector<std::vector<String>>& table, const std::vector<std::vector<int>>& color) {
	std::vector<int> width(table[0].size());
	for (int i = 0; i < table.size(); ++i)
		for (int j = 0; j < table[i].size(); ++j)
			width[j] = max(width[j], table[i][j].length());
	printLine(width);
	for (int i = 0; i < table.size(); ++i) {
		cout << TEXT('|');
		for (int j = 0; j < table[i].size(); ++j)
			printColor(table[i][j], color[i][j], width[j]), cout << TEXT('|') << std::flush;
		cout << std::endl, printLine(width);
	}
}

int analysis(int argc, char* argv[]) {
	if (argc == 1)
		return HELP;
	if (argc != 2)
		return UNKNOWN;
	String command = convert(argv[1]);
	if (command == loadString(IDS_COMMAND_RUN))
		return RUN;
	else if (command == loadString(IDS_COMMAND_COMPARE))
		return COMPARE;
	else if (command == loadString(IDS_COMMAND_CREATE))
		return CREATE;
	else
		return UNKNOWN;
}

void create(String path) {
	generateConfigFile(path);
	std::filesystem::create_directory(path + loadString(IDS_DIRECTORY_CODE));
	std::filesystem::create_directory(path + loadString(IDS_DIRECTORY_DATA));
	std::filesystem::create_directory(path + loadString(IDS_DIRECTORY_GENERATOR));
}

void run(String path) {
	Settings settings = {0};
	if (!loadSettings(path, settings)) {
		cerr << loadString(IDS_CONFIG_INVALID) << std::endl;
		return;
	}

	bool nice = true;
	std::vector<String> files;

	if (!std::filesystem::exists(path + settings.testCode + String(TEXT(".cpp"))))
		nice = false, files.push_back(settings.testCode + String(TEXT(".cpp")));

	for (int i = 1; i <= settings.testCase; ++i)
		if (!std::filesystem::exists(path + replace(settings.inputFile, TEXT('$'), toString(i))))
			nice = false, files.push_back(replace(settings.inputFile, TEXT('$'), toString(i)));

	for (int i = 1; i <= settings.testCase; ++i)
		if (!std::filesystem::exists(path + replace(settings.answerFile, TEXT('$'), toString(i))))
			nice = false, files.push_back(replace(settings.answerFile, TEXT('$'), toString(i)));

	if (!nice) {
		cerr << loadString(IDS_MISSING_FILE) << std::endl;
		for (String s : files)
			cerr << s << std::endl;
		return;
	}

	if (compile(settings.compileCode, path, settings.testCode)) {
		cerr << loadString(IDS_COMPILE_ERROR) << std::endl;
		return;
	}

	ll maxTime = 0, maxMemory = 0;

	std::vector<Result> result(settings.testCase + 1);
	for (int i = 1; i <= settings.testCase; ++i) {
		result[i] = run(path, settings.testCode, replace(settings.inputFile, TEXT('$'), toString(i)), replace(settings.outputFile, TEXT('$'), toString(i)), settings.timeLimit, settings.memoryLimit);
		maxTime = max(maxTime, result[i].time), maxMemory = max(maxMemory, result[i].memory);

		if (result[i].answer == Result::AC && compare(path, replace(settings.outputFile, TEXT('$'), toString(i)), replace(settings.answerFile, TEXT('$'), toString(i))))
			result[i].answer = Result::WA;
		cout << TEXT("Testcase #") << i << TEXT(" OK!") << std::endl;
	}

	std::vector<std::vector<String>> table(settings.testCase + 1);
	std::vector<std::vector<int>> color(settings.testCase + 1);
	table[0].emplace_back(TEXT("Testcase")), table[0].emplace_back(TEXT("Result")), table[0].emplace_back(TEXT("Time(ms)")), table[0].emplace_back(TEXT("Memory(MB)"));
	color[0].push_back(COLOR_DEFAULT), color[0].push_back(COLOR_DEFAULT), color[0].push_back(COLOR_DEFAULT), color[0].push_back(COLOR_DEFAULT);
	for (int i = 1; i <= settings.testCase; ++i) {
		table[i].push_back(toString(i)), table[i].push_back(RESULT[result[i].answer]), table[i].push_back(toString(result[i].time)), table[i].push_back(toString(result[i].memory));
		color[i].push_back(COLOR_DEFAULT), color[i].push_back(COLOR[result[i].answer]), color[i].push_back(result[i].time == maxTime ? COLOR_RED : COLOR_DEFAULT), color[i].push_back(result[i].memory == maxMemory ? COLOR_RED : COLOR_DEFAULT);
	}
	printTable(table, color);
}

void compare(String path) {
	Settings settings = {0};
	if (!loadSettings(path, settings)) {
		cerr << loadString(IDS_CONFIG_INVALID) << std::endl;
		return;
	}

	bool nice = true;
	std::vector<String> files;

	if (!std::filesystem::exists(path + settings.std + String(TEXT(".cpp"))))
		nice = false, files.push_back(settings.std + String(TEXT(".cpp")));

	if (!std::filesystem::exists(path + settings.testCode + String(TEXT(".cpp"))))
		nice = false, files.push_back(settings.testCode + String(TEXT(".cpp")));

	if (!std::filesystem::exists(path + settings.generator + String(TEXT(".cpp"))))
		nice = false, files.push_back(settings.generator + String(TEXT(".cpp")));

	if (!nice) {
		cerr << loadString(IDS_MISSING_FILE) << std::endl;
		for (String s : files)
			cerr << s << std::endl;
		return;
	}

	if (compile(settings.compileCode, path, settings.std)) {
		cerr << loadString(IDS_COMPILE_ERROR) << std::endl;
		return;
	}
	if (compile(settings.compileCode, path, settings.testCode)) {
		cerr << loadString(IDS_COMPILE_ERROR) << std::endl;
		return;
	}
	if (compile(settings.compileCode, path, settings.generator)) {
		cerr << loadString(IDS_COMPILE_ERROR) << std::endl;
		return;
	}

	ll maxTimeStd = 0, maxMemoryStd = 0, maxTimeTest = 0, maxMemoryTest = 0;

	std::vector<Result> resultStd(settings.testCase + 1), resultTest(settings.testCase + 1);
	for (int i = 1; i <= settings.testCase; ++i)
		System(replace(replace(replace(runCommand, TEXT('$'), path + settings.generator), TEXT('@'), TEXT("nul")), TEXT('#'), path + replace(settings.inputFile, TEXT('$'), toString(i))).c_str());

	for (int i = 1; i <= settings.testCase; ++i) {
		resultStd[i] = run(path, settings.std, replace(settings.inputFile, TEXT('$'), toString(i)), replace(settings.answerFile, TEXT('$'), toString(i)), settings.timeLimit, settings.memoryLimit);
		maxTimeStd = max(maxTimeStd, resultStd[i].time), maxMemoryStd = max(maxMemoryStd, resultStd[i].memory);

		resultTest[i] = run(path, settings.testCode, replace(settings.inputFile, TEXT('$'), toString(i)), replace(settings.outputFile, TEXT('$'), toString(i)), settings.timeLimit, settings.memoryLimit);
		maxTimeTest = max(maxTimeTest, resultTest[i].time), maxMemoryTest = max(maxMemoryTest, resultTest[i].memory);

		if (resultTest[i].answer == Result::AC && compare(path, replace(settings.outputFile, TEXT('$'), toString(i)), replace(settings.answerFile, TEXT('$'), toString(i))))
			resultTest[i].answer = Result::WA;
		cout << TEXT("Testcase #") << i << TEXT(" OK!") << std::endl;
	}

	std::vector<std::vector<String>> table(settings.testCase + 1);
	std::vector<std::vector<int>> color(settings.testCase + 1);
	table[0].emplace_back(TEXT("Testcase")), table[0].emplace_back(TEXT("Std Time(ms)")), table[0].emplace_back(TEXT("Std Memory(MB)")), table[0].emplace_back(TEXT("Test Result")), table[0].emplace_back(TEXT("Test Time(ms)")), table[0].emplace_back(TEXT("Test Memory(MB)"));
	color[0].push_back(COLOR_DEFAULT), color[0].push_back(COLOR_DEFAULT), color[0].push_back(COLOR_DEFAULT), color[0].push_back(COLOR_DEFAULT), color[0].push_back(COLOR_DEFAULT), color[0].push_back(COLOR_DEFAULT);
	for (int i = 1; i <= settings.testCase; ++i) {
		table[i].push_back(toString(i)), table[i].push_back(toString(resultStd[i].time)), table[i].push_back(toString(resultStd[i].memory)), table[i].push_back(RESULT[resultTest[i].answer]), table[i].push_back(toString(resultTest[i].time)), table[i].push_back(toString(resultTest[i].memory));
		color[i].push_back(COLOR_DEFAULT), color[i].push_back(resultStd[i].time == maxTimeStd ? COLOR_RED : COLOR_DEFAULT), color[i].push_back(resultStd[i].memory == maxMemoryStd ? COLOR_RED : COLOR_DEFAULT), color[i].push_back(COLOR[resultTest[i].answer]), color[i].push_back(resultTest[i].time == maxTimeTest ? COLOR_RED : COLOR_DEFAULT), color[i].push_back(resultTest[i].memory == maxMemoryTest ? COLOR_RED : COLOR_DEFAULT);
	}
	printTable(table, color);
}