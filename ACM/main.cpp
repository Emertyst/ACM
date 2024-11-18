#include "winapi.h"
#include "framework.h"
#include "judge.h"
#include "command.h"
int main(int argc, char* argv[]) {
	//if (compile("C:/Users/emert/Documents/Code/", "test"))
	//	return 0;
	//Result res = run("C:/Users/emert/Documents/Code/", "test", "1.in", "1.out", 3000, 128);
	//cout << res.answer << " " << res.time << " " << res.memory << "\n";
	//cout << generateConfigFile(TEXT("C:/Users/emert/Documents/Code/"));
	//Settings st = {0};
	//(loadSettings(TEXT("C:/Users/emert/Documents/Code/"), st));
	//wcout << st.std << " " << st.generator << " " << st.testCode << " " << st.testCase << " " << st.compileCode << " " << st.timeLimit << " " << st.memoryLimit << " " << st.inputFile << " " << st.outputFile << " " << st.answerFile << "\n";
	//for (int i = 0; i < argc; ++i)
	//	//wcout << wstring(argv[i]) << "\n";
	//	wcout << convert(argv[i]) << "\n";

	String path = std::filesystem::current_path().c_str() + String(TEXT("\\"));
	switch (analysis(argc, argv)) {
	case CREATE:
		create(path);
		break;
	case RUN:
		run(path);
		break;
	case COMPARE:
		compare(path);
		break;
	}
}













	/*std::ofstream bat("run.bat");
	bat << replace(replace(replace(runCommand, '$', path + exe), '@', path + input), '#', path + output) << std::flush;
	bat.close();

	ShellExecute(NULL, TEXT("open"), TEXT("run.bat"), NULL, NULL, SW_HIDE);

	HANDLE process = NULL;
	while (process == NULL)
		process = getProcessHandle(TEXT("test.exe"));

	std::chrono::time_point<std::chrono::steady_clock> st = std::chrono::steady_clock::now(), ed;

	bool runtime = true;
	ll memory = 0;
	DWORD status = 0;
	while (GetExitCodeProcess(process, &status), status == STILL_ACTIVE) {
		memory = max(memory, getProcessMemory(process));
		ed = std::chrono::steady_clock::now();
		if (std::chrono::duration_cast<std::chrono::milliseconds>(ed - st).count() > timeLimit) {
			result.answer = Result::TLE, runtime = false;
			TerminateProcess(process, 0);
			break;
		} else if (memory > memoryLimit) {
			result.answer = Result::MLE, runtime = false;
			TerminateProcess(process, 0);
			break;
		}
	}
	ed = std::chrono::steady_clock::now();
	CloseHandle(process);
	if (runtime)
		result.answer = status ? Result::RE : Result::AC;*/
