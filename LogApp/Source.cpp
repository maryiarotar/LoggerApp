#include <Windows.h>
#include "Prototypes.h"

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR args, int ncmdshow) {

	//to make main class
	WNDCLASS AppMainClass = NewWindowClass((HBRUSH)GetStockObject(GRAY_BRUSH), LoadCursor(NULL, IDC_ARROW),
		hInst, LoadIcon(NULL, IDI_INFORMATION), L"AppClass", WndProcedure);

	//it registers main class
	if (!RegisterClassW(&AppMainClass)) {
		return -1;
	}

	MSG theMainMessage = { 0 };

	CreateWindow(L"AppClass", L"Logger", WS_MINIMIZEBOX | WS_OVERLAPPEDWINDOW | WS_VISIBLE, 500, 200, 305, 300, NULL, NULL, NULL, NULL);

	while (GetMessage(&theMainMessage, NULL, NULL, NULL)) {
		TranslateMessage(&theMainMessage);
		DispatchMessage(&theMainMessage);
	}

	TerminateThread(readThread, 0); //forced closing of working thread 

	return 0;
}

WNDCLASS NewWindowClass(HBRUSH bgColor, HCURSOR cursor, HINSTANCE hInst, HICON icon, LPCWSTR name, WNDPROC procedure) {

	//empty class structure
	WNDCLASS NWC = { 0 };

	//class attributes 
	NWC.hbrBackground = bgColor;
	NWC.hCursor = cursor;
	NWC.hInstance = hInst;
	NWC.hIcon = icon;
	NWC.lpszClassName = name;
	NWC.lpfnWndProc = procedure;

	return NWC;
}


LRESULT CALLBACK WndProcedure(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	int textLength;
	char* text;

	switch (message) {
	case WM_COMMAND:
		switch (wparam) {
		case OnMinsClicked:
			textLength = GetWindowTextLength(bufferedTextHwnd);
			text = new char[textLength + 1];
			GetWindowTextA(bufferedTextHwnd, text, textLength + 1);
			if (textLength != 0 && *text != '0') {
				mins = std::atoi(text);
				SetWindowTextA(tempHwnd, "Ok!");
			}
			else {
				SetWindowTextA(tempHwnd, "Err");
			}; break;
		case OnSaveDirClicked:
			if (GetSaveFileName(&ofname) || !GetSaveFileName(&ofname)) {
				isThreading = true;
				transformPath();
				toShowMainMessage(hwnd);
				readThread = CreateThread(NULL, 0, whereToSaveLog, NULL, 0, NULL);
			};
			break;
		case OnSaveDefaultClicked:
			isThreading = true;
			toShowMainMessage(hwnd);
			readThread = CreateThread(NULL, 0, whereToSaveLog, NULL, 0, NULL);
			break;
		case OnLoadClicked:
			loadLog(path);
			break;
		case OnStopClicked:
			toStopLogging();
			toShowMainMessage(hwnd);
			break;
		default: break;
		};
		break;
	case WM_CREATE:
		setMinutesAndDirectoryWidgwets(hwnd);
		setOpenFileParams(hwnd);
		break;
	case WM_DESTROY:
		toStopLogging();
		PostQuitMessage(0);
		break;

	default: return DefWindowProc(hwnd, message, wparam, lparam); //default message handler
	}
}

void setMinutesAndDirectoryWidgwets(HWND hwnd) {

	CreateWindowA("static", "mins between checks:", WS_VISIBLE | WS_CHILD, 5, 40, 150, 22, hwnd, NULL, NULL, NULL);

	bufferedTextHwnd = CreateWindowA("edit", "", WS_VISIBLE | WS_CHILD | ES_CENTER | ES_NUMBER, 165, 40, 25, 22, hwnd, NULL, NULL, NULL);

	CreateWindowA("button", "save", WS_VISIBLE | WS_CHILD, 200, 40, 50, 24, hwnd, (HMENU)OnMinsClicked, NULL, NULL);

	tempHwnd = CreateWindowA("static", " ", WS_VISIBLE | WS_CHILD, 260, 40, 25, 22, hwnd, NULL, NULL, NULL);

	CreateWindowA("static", "save in:", WS_VISIBLE | WS_CHILD, 5, 80, 60, 22, hwnd, NULL, NULL, NULL);

	CreateWindowA("button", "default dir", WS_VISIBLE | WS_CHILD, 75, 80, 80, 24, hwnd, (HMENU)OnSaveDefaultClicked, NULL, NULL);

	CreateWindowA("button", "new dir", WS_VISIBLE | WS_CHILD, 160, 80, 70, 24, hwnd, (HMENU)OnSaveDirClicked, NULL, NULL);

	CreateWindowA("button", "load", WS_VISIBLE | WS_CHILD, 235, 80, 50, 24, hwnd, (HMENU)OnLoadClicked, NULL, NULL);

	toShowMainMessage(hwnd);

	CreateWindowA("button", "stop", WS_VISIBLE | WS_CHILD, 80, 220, 50, 24, hwnd, (HMENU)OnStopClicked, NULL, NULL);

}

void transformPath() {

	// wchar_t* -> char*
	size_t size = sizeof(filename);
	char* tempLpcstr = new char[size];
	wcstombs_s(&size, tempLpcstr, size, filename, size);
	path = tempLpcstr;

	delete[] tempLpcstr;

}


DWORD WINAPI whereToSaveLog(LPVOID lpParameter) {

	dwAttributes = GetFileAttributesA(path);

	if (dwAttributes == INVALID_FILE_ATTRIBUTES) {

		file = CreateFileA(
			path,
			GENERIC_WRITE,
			FILE_SHARE_WRITE,
			NULL,
			CREATE_NEW,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);
	}
	else {
		file = CreateFileA(
			path,
			FILE_APPEND_DATA,
			FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL
		);
	}



	while (isThreading) {

		if (file == NULL) {
			file = CreateFileA(
				path,
				FILE_APPEND_DATA,
				FILE_SHARE_WRITE,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				NULL
			);
		}

		logger(file);

	}

	if (file != NULL) {
		CloseHandle(file);
	}

	return 0;
}


void loadLog(LPCSTR path1) {

	file = CreateFileA(
		path1,
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	//it shows last 360 symbols from the end of file (last some strings)
	SetFilePointer(file, -360, NULL, FILE_END);

	DWORD readedBytes;

	bool b = ReadFile(file, buffer, bufferSize, &readedBytes, NULL);

	SetWindowTextA(loadedTextHwnd, "_");
	SetWindowTextA(loadedTextHwnd, buffer);

	CloseHandle(file);
	file = NULL;


}


void setOpenFileParams(HWND hwnd) {
	//clear
	ZeroMemory(&ofname, sizeof(ofname));

	ofname.lStructSize = sizeof ofname;
	ofname.hwndOwner = hwnd;
	ofname.lpstrFile = filename; //set the [dir]/[file] 
	ofname.nMaxFile = sizeof(filename); //set its size
	ofname.lpstrFilter = L"*.txt"; //file types
	ofname.lpstrFileTitle = defaultFileName; //default name
	ofname.nMaxFileTitle = sizeof(defaultFileName);
	ofname.lpstrInitialDir = defaultDirectory;
	ofname.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

}

void toShowMainMessage(HWND hwnd) {

	char tmpMsg[260] = "press Load to see last 10 logs...\r\n(if doesn't work then resave [file].txt manually..)\r\n";
	strcat_s(tmpMsg, 260, "current path --> ");
	strcat_s(tmpMsg, 260, path);

	if (isThreading) {
		strcat_s(tmpMsg, 260, "\r\n");
		strcat_s(tmpMsg, 260, "<<-- LOGGER IS WORKING -->>>");
	}

	loadedTextHwnd = CreateWindowA("edit", tmpMsg,
		WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_VSCROLL, 5, 110, 275, 100, hwnd, NULL, NULL, NULL);
}




void logger(HANDLE localFile) {

	//it converts current time&date to the string
	time_t seconds; 
	tm timeinfo[26];
	char* s = new char[26];

	seconds = time(NULL);
	localtime_s(timeinfo, &seconds); 
	asctime_s(s, 26, timeinfo); 

	//buffer for the info string
	DWORD length = 60;
	char* bufStr = new char[length];
	memset(bufStr, NULL, length * sizeof(char));

	strcpy_s(bufStr, length, "\r\n");
	strcat_s(bufStr, length, s);

	//battery info
	SYSTEM_POWER_STATUS spsPwr;
	BYTE buttLP = NULL;
	//unsigned int buttLT = NULL;

	if (GetSystemPowerStatus(&spsPwr)) {
		buttLP = spsPwr.BatteryLifePercent;
		//buttLT = (unsigned int) spsPwr.BatteryLifeTime;
		//int p = static_cast<int>(buttLP);
		std::string tmpst = std::to_string(buttLP);
		char const* num_char = tmpst.c_str();

		strcat_s(bufStr, length, " - ");
		strcat_s(bufStr, length, num_char);
	}
	strcat_s(bufStr, length, "  % of battery level ");

	WriteFile(localFile, bufStr, length, &length, NULL);


	delete[] bufStr;
	delete[] s;

	if (file != NULL) {
		CloseHandle(localFile);
		file = NULL;
	}

	this_thread::sleep_for(chrono::minutes(mins));

}


void toStopLogging() {

	if (isThreading) {
		CloseHandle(readThread); //close working thread
		isThreading = false;
	}

}