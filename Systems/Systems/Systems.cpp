﻿// Systems.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "Systems.h"

LRESULT  CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(
	HINSTANCE hInstance, 
	HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("SysMetsl");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndClass;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = szAppName;

	if (!RegisterClass(&wndClass))
	{
		MessageBox(
			NULL, 
			TEXT("This program requires Windows NT!"), 
			szAppName, 
			MB_ICONERROR);
		return 0;
	}
	hwnd = CreateWindow(
		szAppName,
		TEXT("Get System MEtrics No.1"),
		WS_OVERLAPPEDWINDOW | WS_VSCROLL | WS_HSCROLL,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL
	);
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int cxChar, cxCaps, cyChar, cxClient, cyClient, iMaxWidth;
	HDC hdc;
	PAINTSTRUCT ps;
	TCHAR szBuffer[10];
	TEXTMETRIC tm;
	int a = NUMLINES;
	int i, x, y, iVertPos, iHorzPos, iPaintBeg, iPaintEnd;
	SCROLLINFO si;
	switch (message)
	{
	case WM_SIZE:
		cxClient = LOWORD(lParam);
		cyClient = HIWORD(wParam);
		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_PAGE;
		si.nMin = 0;
		si.nMax = NUMLINES - 1;
		si.nPage = cyClient / cyChar;
		SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_PAGE;
		si.nMin = 0;
		si.nMax = 2 + iMaxWidth / cxChar;
		si.nPage = cxClient / cxChar;
		SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
		return 0;
	case WM_CREATE:
		hdc = GetDC(hwnd);
		GetTextMetrics(hdc, &tm);
		cxChar = tm.tmAveCharWidth;
		cxCaps = (tm.tmPitchAndFamily & 1 ? 3 : 2) * cxChar / 2;
		cyChar = tm.tmHeight + tm.tmExternalLeading;
		ReleaseDC(hwnd, hdc);
		//SetScrollRange(hwnd, SB_VERT, 0, NUMLINES - 1, FALSE);
		//SetScrollPos(hwnd, SB_VERT, iVscrollPos, TRUE);
		iMaxWidth = 40 * cxChar + 22 * cxChar;
		return 0;

	case WM_VSCROLL: 
		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;
		GetScrollInfo(hwnd, SB_VERT, &si);
		iVertPos = si.nPos;
		switch (LOWORD(wParam))
		{
		case SB_TOP:
			si.nPos = si.nMin;
			break;
		case SB_BOTTOM:
			si.nPos = si.nMax;
			break;
		case SB_LINEUP:
			si.nPos -= 1;
			break;
		case SB_LINEDOWN:
			si.nPos += 1;
			break;
		case SB_PAGEUP:
			si.nPos -= si.nPage;
			break;
		case SB_PAGEDOWN:
			si.nPos += si.nPage;
			break;
		case SB_THUMBTRACK:
			si.nPos = si.nTrackPos;
			break;
		default:break;
		}

		si.fMask = SIF_POS;
		SetScrollInfo(hwnd, SB_VERT, &si, TRUE);
		GetScrollInfo(hwnd, SB_VERT, &si);
		if (si.nPos != iVertPos)
		{
			ScrollWindow(hwnd, 0, cyChar * (iVertPos - si.nPos), NULL, NULL);
			UpdateWindow(hwnd);
		}
		return 0;
	case WM_HSCROLL:
		si.cbSize = sizeof(si);
		si.fMask = SIF_ALL;
		GetScrollInfo(hwnd, SB_HORZ, &si);
		iHorzPos = si.nPos;
		switch (LOWORD(wParam))
		{
		case SB_LINELEFT:
			si.nPos -= 1;
			break;
		case SB_LINERIGHT:
			si.nPos += 1;
			break;
		case SB_PAGERIGHT:
			si.nPos += si.nPage;
			break;
		case SB_PAGELEFT:
			si.nPos -= si.nPage;
			break;
		case SB_THUMBPOSITION:
			si.nPos = si.nTrackPos;
			break;
		default:
			break;
		}
		si.fMask = SIF_POS;
		SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
		GetScrollInfo(hwnd, SB_HORZ, &si);
		if (si.nPos != iHorzPos)
		{
			ScrollWindow(hwnd, cxChar * (iHorzPos - si.nPos),0, NULL, NULL);
		}
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		si.cbSize = sizeof(si);
		si.fMask = SIF_POS;
		GetScrollInfo(hwnd, SB_VERT, &si);
		iVertPos = si.nPos;
		GetScrollInfo(hwnd, SB_HORZ, &si);
		iHorzPos = si.nPos;
		iPaintBeg = max(0, iVertPos + ps.rcPaint.top / cyChar);
		iPaintEnd = min(NUMLINES - 1, iVertPos + ps.rcPaint.bottom / cyChar);
		for (i = iPaintBeg; i <= iPaintEnd; i++)
		{
			x = cxChar * (1 - iHorzPos);
			y = cyChar * (i - iVertPos);
			TextOut(
				hdc,
				x, y,
				sysmetrics[i].szLabel,
				lstrlen(sysmetrics[i].szLabel)
			);

			TextOut(
				hdc,
				x + 22 * cxCaps, y,
				sysmetrics[i].szDesc,
				lstrlen(sysmetrics[i].szDesc)
			);

			SetTextAlign(hdc, TA_RIGHT | TA_TOP);

			TextOut(hdc, x + 22 * cxCaps + 40 * cxChar, y, szBuffer,
				wsprintf(szBuffer, TEXT("%5d"),
					GetSystemMetrics(sysmetrics[i].Index)));
			SetTextAlign(hdc, TA_LEFT | TA_TOP);
		}
		EndPaint(hwnd, &ps);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}