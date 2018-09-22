// include the basic windows header file
#include <windowsx.h>
#include <Windows.h>
#include <direct.h>
#include <stdio.h>
#include <iostream>
#include <string>

//Convert bytes to MB
#define DIV 1048576

//Method to check no other instance is running, returns false if another instance is found.
bool isOnlyInstance(LPCTSTR gameTitle) {
	HANDLE handle = CreateMutex(NULL, TRUE, gameTitle);
	if (GetLastError() != ERROR_SUCCESS) {
		HWND hWnd = FindWindow(gameTitle, NULL);
		if (hWnd) {
			// An instance of your game is already running.
			ShowWindow(hWnd, SW_SHOWNORMAL);
			SetFocus(hWnd);
			SetForegroundWindow(hWnd);
			SetActiveWindow(hWnd);
			// display message
			int msgboxID = MessageBox(
				hWnd,
				"An instance of the application is already running",
				"Multiple Instances Detected",
				MB_ICONWARNING
			);
			return false;
		}
	}

	return true;
}

//Method to check if there is enough available storage, outputs to console the results.
bool checkStorage(const DWORDLONG diskSpaceNeeded) {
	int const drive = _getdrive();
	struct _diskfree_t diskfree;
	_getdiskfree(drive, &diskfree);

	unsigned __int64 const neededClusters = diskSpaceNeeded /
		(diskfree.sectors_per_cluster * diskfree.bytes_per_sector);

	if (diskfree.avail_clusters < neededClusters) {
		return 0;
	}
	else {
		return 1;
	}
}

//Method to check how much physical and virtual memory is available on the system.
void checkMemory(const DWORDLONG physicalRAMNeeded, const DWORDLONG virtualRAMNeeded) {
	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
	GlobalMemoryStatusEx(&status);

	if (status.ullAvailPhys < physicalRAMNeeded || status.ullAvailVirtual < virtualRAMNeeded) {
		int msgboxID3 = MessageBox(
			NULL,
			"Failed, Toss Comp.",
			"Check Memory",
			MB_ICONWARNING
		);
	}
	else {
		int msgboxID3 = MessageBox(
			NULL,
			"You have passed the requirements!",
			"Check Memory",
			MB_ICONWARNING
		);
	}

	char szTest1[100];
	sprintf_s(szTest1, "You have : %d MB of available Physical Memory", status.ullAvailPhys / DIV);
	MessageBox(NULL, szTest1, "CheckMemory", MB_OK);

	char szTest2[100];
	sprintf_s(szTest2, "You have : %d MB of available Virtual Memory", status.ullAvailVirtual / DIV);
	MessageBox(NULL, szTest2, "CheckMemory", MB_OK);
}

//Displays the CPU speed and architecture to the console. 
void checkCPUStats() {
	int ArchNum;
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	
	DWORD BufSize = sizeof(DWORD);
	DWORD dwMHz = 0;
	DWORD type = REG_DWORD;
	HKEY hKey;
	// open the key where the proc speed is hidden:
	long lError = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
		0,
		KEY_READ,
		&hKey);
	if (lError == ERROR_SUCCESS) {
		// query the key:
		RegQueryValueEx(hKey,
			"~MHz",
			NULL,
			&type,
			(LPBYTE)&dwMHz,
			&BufSize);
	}
	char szTest[50];
	sprintf_s(szTest, "The CPU Speed we get is : %d MegaHertz\n", dwMHz);
	MessageBox(NULL, szTest, "CPU Speed Test", MB_OK);

	ArchNum = systemInfo.wProcessorArchitecture;
	switch (ArchNum)
	{
	case 0: MessageBox(NULL, "Intel x86\n", "Processor Architecture", MB_OK);
		break;
	case 5: MessageBox(NULL, "ARM\n", "Processor Architecture", MB_OK);
		break;
	case 6: MessageBox(NULL, "Intel Itanium based", "Processor Architecture", MB_OK);
		break;
	case 9: MessageBox(NULL, "x64(AMD or Intel)", "Processor Architecture", MB_OK);
		break;
	case 12: MessageBox(NULL, "ARM64\n", "Processor Architecture", MB_OK);
		break;
	default: MessageBox(NULL, "Unknown architecture", "Processor Architecture", MB_OK);
		break;
	}
}

// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam);

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	// the handle for the window, filled by a function
	HWND hWnd;
	// this struct holds information for the window class
	WNDCLASSEX wc;

	// clear out the window class for use
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	// fill in the struct with the needed information
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpszClassName = "WindowClass1";

	// register the window class
	RegisterClassEx(&wc);

	// create the window and use the result as the handle
	hWnd = CreateWindowEx(NULL,
		"WindowClass1",    // name of the window class
		"Our First Windowed Program",   // title of the window
		WS_OVERLAPPEDWINDOW,    // window style
		300,    // x-position of the window
		300,    // y-position of the window
		500,    // width of the window
		400,    // height of the window
		NULL,    // we have no parent window, NULL
		NULL,    // we aren't using menus, NULL
		hInstance,    // application handle
		NULL);    // used with multiple windows, NULL

// display the window on the screen
	ShowWindow(hWnd, nCmdShow);
	isOnlyInstance("WindowClass1");
	// display message
	// enter the main loop:
	if (checkStorage(300000000) == false)
	{
		int msgboxID3 = MessageBox(
			hWnd,
			"checkStorage Failure : Not enough physical storage.",
			"Check Storage",
			MB_ICONWARNING
		);
	}
	else {
		int msgboxID4 = MessageBox(
			hWnd,
			"checkStorage Success: You got the cash!.",
			"Check Storage",
			MB_ICONWARNING
		);
	}
	checkMemory(1000 * DIV, 1000 * DIV);
	checkCPUStats();
	
	// this struct holds Windows event messages
	MSG msg;

	// wait for the next message in the queue, store the result in 'msg'
	while (GetMessage(&msg, NULL, 0, 0))
	{
		// translate keystroke messages into the right format
		TranslateMessage(&msg);

		// send the message to the WindowProc function
		DispatchMessage(&msg);
	}

	// return this part of the WM_QUIT message to Windows
	return msg.wParam;

}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// sort through and find what code to run for the message given
	switch (message)
	{
		// this message is read when the window is closed
	case WM_DESTROY:
	{
		// close the application entirely
		PostQuitMessage(0);
		return 0;
	} break;
	}

	// Handle any messages the switch statement didn't
	return DefWindowProc(hWnd, message, wParam, lParam);
}