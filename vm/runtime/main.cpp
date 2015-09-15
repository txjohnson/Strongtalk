# include "incls/_precompiled.incl"
# include "incls/_main.cpp.incl"

#ifdef _WIN32
#include <windows.h>

// in os_nt.cpp
extern HINSTANCE hInstance;
extern HINSTANCE hPrevInstance;
extern int       nCmdShow;

// https://msdn.microsoft.com/en-us/library/bb384843.aspx
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR cmdLine, int cmdShow) {
	// Save all parameters
	hInstance = hInst;
	hPrevInstance = hPrevInst;
	nCmdShow = cmdShow;
	os::set_args(__argc, __argv);
	return vm_main(__argc, __argv);
}
#elif defined(__linux__) || defined(__APPLE__)
int main(int argc, char* argv[]) {
	os::set_args(argc, argv);
	return vm_main(argc, argv);
}
#endif
