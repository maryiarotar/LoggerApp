#pragma once
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
using namespace std;

#define OnMinsClicked 1
#define OnSaveDirClicked 2
#define OnLoadClicked 3
#define OnStopClicked 4
#define OnSaveDefaultClicked 5
#define DgIndex 15

HWND bufferedTextHwnd;
HWND loadedTextHwnd;
HWND tempHwnd = NULL;
HWND clockHwnd;

int mins = 5;
const int bufferSize = 500;
char buffer[bufferSize];


wchar_t filename[260];
OPENFILENAME ofname;

HANDLE file; //variable for opening&closing files
HANDLE readThread; //for threads

volatile bool isThreading = false;

DWORD dwAttributes;
LPCSTR path = "D:\\logBattery.txt";

wchar_t defaultFileName[11] = L"logBattery";
wchar_t defaultDirectory[4] = L"D:\\";

//template for creating window classess
WNDCLASS NewWindowClass(HBRUSH bgColor, HCURSOR cursor, HINSTANCE hInst, HICON icon, LPCWSTR name, WNDPROC procedure);

//WNDPOC
LRESULT CALLBACK WndProcedure(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

//thread function. It opens or creates a file and begins logging in it
DWORD WINAPI whereToSaveLog(LPVOID lpParameter);

//it displays main widgets
void setMinutesAndDirectoryWidgwets(HWND hWnd);

//it opens a file, reads from it and shows last some strings to the main window
void loadLog(LPCSTR path);

//it sets parametrs for opening new [dir]/[file]
void setOpenFileParams(HWND hwnd);

//it shows help message in the main window
void toShowMainMessage(HWND hwnd);

//it writes info of battery level every n minutes in a [dir]/[file]
void logger(HANDLE localFile);

//it converts wchar_t* to char* ('filename' to 'path')
void transformPath();

//it stops working thread
void toStopLogging();
