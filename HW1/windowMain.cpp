#include "stdafx.h"
#include "student.h"

#define MAX_COUNT 8
#define STUDENT_NUM 10

using namespace std;

string name[STUDENT_NUM] = { "������", "������","�̼���","������","�㰡��","�����","������", "�̵���", "�ҿ���","�ڹ̳�" };

vector<student> vStudent;

HWND MainWindowHandle = 0;

bool checkName[STUDENT_NUM] = { false, };

// ǥ�õ� ������ ��ġ
int recx = 450; int recy = 400;
// ����ڰ� Ŭ���� ��ġ
int xpos = 0; int ypos = 0;

/* --------------------------------------------------------------------------- */

// (1) ������ �Լ�
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nShowCmd)
{
	if (!InitWindowsApp(hInstance, nShowCmd))
	{
		::MessageBox(0, L"Init - Failed", L"Error", MB_OK);
		return 0;
	}
	return Run();
}

// (2) ���� ���α׷��� �����츦 �����ϴ� �Լ�
bool InitWindowsApp(HINSTANCE instanceHandle, int show)
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = instanceHandle;
	wc.hIcon = ::LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = ::LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = static_cast<HBRUSH>(::GetStockObject(WHITE_BRUSH));
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"Hello";
	if (!::RegisterClass(&wc)) {
		::MessageBox(0, L"RegisterClass - Failed", 0, 0);
		return false;
	}
	MainWindowHandle = ::CreateWindow(
		L"Hello", L"Hello", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		0, 0, instanceHandle, 0);
	if (MainWindowHandle == 0) {
		::MessageBox(0, L"CreateWindow - Failed", 0, 0);
		return false;
	}
	::ShowWindow(MainWindowHandle, show);
	::UpdateWindow(MainWindowHandle);
	return true;
}

// (3) â�� ������ ���Ŀ� â���� ���� �޽����� �޾Ƽ� �̸� ������ ���ν����� ������ �Լ�
int Run()
{
	MSG msg;
	::ZeroMemory(&msg, sizeof(MSG));

	while (::GetMessage(&msg, 0, 0, 0)) {
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
	return msg.wParam;
}

// GID�� �簢�� �׸��� �Լ� (�� ������Ʈ������ ������ ����)
/* LRESULT OnPaint(HWND hWnd)
{
	PAINTSTRUCT ps;

	BeginPaint(hWnd, &ps);

	RECT rc; GetClientRect(hWnd, &rc);

	FillRect(ps.hdc, &rc, GetSysColorBrush(COLOR_HIGHLIGHT));

	HPEN hMyRedPen = CreatePen(PS_SOLID, 3, RGB(255, 0, 0));
	HGDIOBJ hOldPen = SelectObject(ps.hdc, hMyRedPen);

	Rectangle(ps.hdc, rc.left + 100, rc.top + 100, rc.right - 100, rc.bottom - 100);

	SelectObject(ps.hdc, hOldPen);
	DeleteObject(hMyRedPen);
	EndPaint(hWnd, &ps);
	return 0;
} */

// Win32 ���α׷����� printf()�� ���� ������ �ϴ� �Լ�
void TRACE(LPCTSTR lpszFormat, ...) {
	TCHAR lpszBuffer[1024]; // 0x160
	va_list fmtList;
	va_start(fmtList, lpszFormat);
	_vstprintf(lpszBuffer, sizeof(lpszBuffer), lpszFormat, fmtList);
	va_end(fmtList);
	::OutputDebugString(lpszBuffer);
}

/* --------------------------------------------------------------------------- */

// string to wstring for TextOut
// TextOut������ wstring���� ��ȯ�� �Ŀ� LPCWSTR�� �ٽ� �ٲ㼭 ���
wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

// ������ ������ ������� ĥ�ϴ� �Լ�
LRESULT drawRectangleWhite(HWND windowHandle, HDC hdc) {
	COLORREF color = RGB(255, 255, 255);

	HBRUSH myBrush = CreateSolidBrush(color);
	HPEN myPen = CreatePen(PS_SOLID, 1, color);
	HGDIOBJ oldBrush = SelectObject(hdc, myBrush);
	HGDIOBJ oldPen = (HBRUSH)SelectObject(hdc, myPen);

	Rectangle(hdc, recx, recy, recx + 200, recy + 50);

	SelectObject(hdc, oldBrush);
	SelectObject(hdc, oldPen);
	DeleteObject(myBrush);
	DeleteObject(myPen);

	return 0;
}

// �л��� �� �� �߰��� ������ ���簢���� �׸��� �Լ�
LRESULT drawRectangle(HWND windowHandle, HDC hdc)
{
	COLORREF color = NULL;

	int colorNum = 0;

	for (vector<student>::iterator iterDraw = vStudent.begin(); iterDraw != vStudent.end(); iterDraw++) {

		/* �л��� �̸��� ���� */
		wstring tmpName = s2ws(iterDraw->name);
		wstring tmpScore = s2ws(to_string(iterDraw->score));

		LPCWSTR name = tmpName.c_str();
		LPCWSTR score = tmpScore.c_str();

		/* ���簢���� ǥ�õ� �� */
		switch (colorNum) {

		case 0:
			color = RGB(255, 87, 127);
			break;
		case 1:
			color = RGB(255, 136, 75);
			break;
		case 2:
			color = RGB(255, 199, 100);
			break;
		case 3:
			color = RGB(205, 255, 252);
			break;
		case 4:
			color = RGB(74, 71, 163);
			break;
		case 5:
			color = RGB(74, 71, 0);
			break;
		case 6:
			color = RGB(112, 159, 176);
			break;
		case 7:
			color = RGB(167, 197, 235);
			break;
		default:
			break;
		}

		/* ���� �׵θ� ���� ��ġ��Ŵ */
		HBRUSH myBrush = CreateSolidBrush(color);
		HPEN myPen = CreatePen(PS_SOLID, 1, color);
		HGDIOBJ oldBrush = SelectObject(hdc, myBrush);
		HGDIOBJ oldPen = (HBRUSH)SelectObject(hdc, myPen);

		/* ���簢���� ��ġ */
		Rectangle(hdc, recx, recy, recx + 200, recy + 50);

		/* ���簢�� �ȿ� �̸��� ���� ǥ�� */
		TextOut(hdc, recx + 20, recy + 10, name, sizeof(name) - sizeof(' '));
		TextOut(hdc, recx + 20, recy + 30, score, to_string(iterDraw->score).length());

		/* ������ ��Ÿ���� ���簢���� ��ġ */
		recy -= 50;

		SelectObject(hdc, oldBrush);
		SelectObject(hdc, oldPen);
		DeleteObject(myBrush);
		DeleteObject(myPen);

		colorNum++;
	}
	return 0;
}

// �л����� ������ ���� ������� ����
bool cmp(const student& p1, const student& p2) {
	if (p1.score < p2.score) return true;
	else return false;
}

// (4) ������ ���ν��� �Լ�
LRESULT CALLBACK WndProc(HWND windowHandle, UINT msg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (msg) {

		/* ���� ���콺 Ŭ�� */
	case WM_LBUTTONDOWN:
		/* Ŭ���� ��ġ ���� */
		xpos = GET_X_LPARAM(lParam);
		ypos = GET_Y_LPARAM(lParam);

		/* ����� �����Ͱ� ���� ��, ���ο� �л� ������ ���� */
		if (vStudent.size() == 0) {
			int nameIndex = 0;

			student tmpStudent;
			srand((unsigned)time(NULL));
			nameIndex = rand() % STUDENT_NUM;

			// student[]�� ����� �̸� �� �ϳ�
			tmpStudent.name = name[nameIndex];
			// ������ ������ 50~99��
			tmpStudent.score = rand() % 50 + 50;

			// �� �̸��� ���Ǿ����� ǥ��
			checkName[nameIndex] = true;
			// ���Ϳ� �߰�
			vStudent.push_back(tmpStudent);

			InvalidateRect(windowHandle, NULL, FALSE);
			::MessageBox(0, L"You made a box.", L"Making", MB_OK);
		}
		/* ����� �����Ͱ� ���� �� */
		else {
			// �� ���� �ִ� �ڽ� Ŭ�� �� ����
			if (((recx <= xpos) && (xpos <= recx + 200) && (recy + 50 <= ypos) && (ypos <= recy + 100)) && (recy <= 600)) {
				RECT temp;

				temp.left = recx;
				temp.top = recy + 50;
				temp.right = recx + 200;
				temp.bottom = recy + 100;

				recy += 50;

				// ���Ϳ��� ������ ���� ���� �л� ����
				vStudent.erase(vStudent.end() - 1);
				// �� �̻� �̸��� ������ �����Ƿ� false�� ǥ��
				checkName[vStudent.size()] = false;

				InvalidateRect(windowHandle, &temp, TRUE);

				// ������ ������ ������� ǥ��
				hdc = BeginPaint(windowHandle, &ps);
				drawRectangleWhite(windowHandle, hdc);
				EndPaint(windowHandle, &ps);
				::MessageBox(0, L"Delete Box", L"Delete", MB_OK);
			}
			// �� �� ���� Ŭ�� �� �߰�
			else {
				/* ������ �ִ� ������� 8�̹Ƿ� �� �̻� �߰� �Ұ� */
				if (vStudent.size() == 8) {
					::MessageBox(0, L"Size of the struct is 8. So you can't make it.", L"Out of Size", MB_OK);
					return 0;
				}
				/* �߰� */
				else {
					student tmpStudent;
					srand((unsigned)time(NULL));

					int nameIndex = 0;

					// �̸� �ߺ��� �˻�
					while (true) {
						nameIndex = rand() % STUDENT_NUM;
						if (!checkName[nameIndex]) break;
					}
					checkName[nameIndex] = true;

					tmpStudent.name = name[nameIndex];


					// ������ 50~99�� ����
					tmpStudent.score = rand() % 50 + 50;

					vStudent.push_back(tmpStudent);;

					// y��ǥ �ʱ�ȭ�Ͽ� ó������ �׸� �� �ֵ��� ����
					recy = 400;

					InvalidateRect(windowHandle, NULL, TRUE);

					::MessageBox(0, L"You made a box.", L"Make Box", MB_OK);
				}
			}
		}
		return 0;
		/* ȭ�鿡 �׸��� */
	case WM_PAINT:
		hdc = BeginPaint(windowHandle, &ps);
		stable_sort(vStudent.begin(), vStudent.end(), cmp);
		drawRectangle(windowHandle, hdc);
		EndPaint(windowHandle, &ps);
		break;
	case WM_RBUTTONDOWN:
		::MessageBox(0, L"You should click the Left Button.", L"Error", MB_ICONERROR);
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) DestroyWindow(MainWindowHandle);
		else ::MessageBox(0, L"If you want to exit, please press the ESC.", L"Error", MB_ICONERROR);
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProc(windowHandle, msg, wParam, lParam);
}