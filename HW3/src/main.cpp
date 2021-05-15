#pragma comment(lib, "d2d1")
#pragma comment(lib, "Dwrite")

#include "HW3.h"
#include "Animation.h"

#define BASE_X 500
#define BLOCK 10
#define USER_VELOCITY 18.0f
#define COM_VELOCITY 10.0f

float spaceBar = 0.0f;
float changedVelocity = USER_VELOCITY;


int WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/)
{
	if (SUCCEEDED(CoInitialize(NULL)))
	{
		{
			DemoApp app;

			if (SUCCEEDED(app.Initialize()))
			{
				app.RunMessageLoop();
			}
		}
		CoUninitialize();
	}

	return 0;
}

DemoApp::DemoApp() :
	m_hwnd(NULL),
	m_pD2DFactory(NULL),
	m_pRT(NULL),
	m_pPathGeometry(NULL),
	m_pObjectGeometry(NULL),
	m_pPathGeometry2(NULL),
	m_pObjectGeometry2(NULL),
	m_pRedBrush(NULL),
	m_pYellowBrush(NULL),
	m_pGreenBrush(NULL),
	//m_nFrequency(NULL),
	//m_nPrevTime(NULL),
	m_pBitmapMask(NULL),
	m_pBitmapMaskBrush(NULL),
	m_pFernBitmap(NULL),
	m_pFernBitmapBrush(NULL),
	m_pWICFactory(NULL),
	m_pTextBrush(NULL),
	m_pTextFormat(NULL),
	m_pDWriteFactory(NULL),
	m_Animation(),
	m_Animation2()
{
}

DemoApp::~DemoApp()
{
	SAFE_RELEASE(m_pD2DFactory);
	SAFE_RELEASE(m_pRT);
	SAFE_RELEASE(m_pPathGeometry);
	SAFE_RELEASE(m_pObjectGeometry);
	SAFE_RELEASE(m_pPathGeometry2);
	SAFE_RELEASE(m_pObjectGeometry2);
	SAFE_RELEASE(m_pRedBrush);
	SAFE_RELEASE(m_pYellowBrush);
	SAFE_RELEASE(m_pGreenBrush);
	SAFE_RELEASE(m_pBitmapMask);
	SAFE_RELEASE(m_pBitmapMaskBrush);
	SAFE_RELEASE(m_pFernBitmap);
	SAFE_RELEASE(m_pFernBitmapBrush);
	SAFE_RELEASE(m_pWICFactory);
	SAFE_RELEASE(m_pTextBrush);
	SAFE_RELEASE(m_pTextFormat);
	SAFE_RELEASE(m_pDWriteFactory);
}

HRESULT DemoApp::Initialize()
{
	HRESULT hr;

	//register window class
	WNDCLASSEX wcex = { sizeof(WNDCLASSEX) };
	/* â�� ����� ����Ǹ� �ٽ� �׸� */
	/* https://docs.microsoft.com/ko-kr/windows/win32/winmsg/window-class-styles */
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = DemoApp::WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(LONG_PTR);
	wcex.hInstance = HINST_THISCOMPONENT;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"D2DDemoApp";
	RegisterClassEx(&wcex);

	hr = CreateDeviceIndependentResources();
	if (SUCCEEDED(hr))
	{
		/* ������ ������ ���� */
		m_hwnd = CreateWindow(
			L"D2DDemoApp", L"HW3_201801567(������)", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
			1024, 512, NULL, NULL, HINST_THISCOMPONENT, this);
		hr = m_hwnd ? S_OK : E_FAIL;
		if (SUCCEEDED(hr))
		{
			float length = 0;
			hr = m_pPathGeometry->ComputeLength(NULL, &length);

			if (SUCCEEDED(hr))
			{
				/* -------------------- ����� �ﰢ�� �ִϸ��̼� -------------------- */
				m_Animation.SetStart(0); //start at beginning of path
				m_Animation.SetEnd(length); //length at end of path
				m_Animation.SetDuration(COM_VELOCITY); //seconds

				/* -------------------- ���λ� �ﰢ�� �ִϸ��̼� -------------------- */
				m_Animation2.SetStart(0); //start at beginning of path
				m_Animation2.SetEnd(length); //length at end of path
				m_Animation2.SetDuration(0.0f); //seconds

				ShowWindow(m_hwnd, SW_SHOWNORMAL);
				UpdateWindow(m_hwnd);
			}
		}
	}

	QueryPerformanceFrequency(&m_nFrequency);
	QueryPerformanceCounter(&m_nPrevTime);

	return hr;
}

HRESULT DemoApp::CreateDeviceIndependentResources()
{
	HRESULT hr;
	ID2D1GeometrySink* pSink = NULL;

	// D2D ���丮�� ������.
	hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

	/* ---------------------- �ؽ�Ʈ ���� ----------------------------- */
	if (SUCCEEDED(hr))
	{
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown**>(&m_pDWriteFactory)
		);
	}

	if (SUCCEEDED(hr))
	{
		hr = m_pDWriteFactory->CreateTextFormat(
			L"Verdana",
			NULL,
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			13.0f,
			L"ko-kr",
			&m_pTextFormat
		);
	}

	/* ---------------------------- ��Ʈ�� ----------------------------- */
	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&m_pWICFactory));
	}

	/* -------------- ���簢�� ����� ��� ���ϸ� ������. -------------- */
	if (SUCCEEDED(hr))
	{
		hr = m_pD2DFactory->CreatePathGeometry(&m_pPathGeometry);
	}
	if (SUCCEEDED(hr))
	{
		hr = m_pPathGeometry->Open(&pSink);
	}
	if (SUCCEEDED(hr))
	{
		D2D1_POINT_2F currentLocation = { 0, 0 };

		pSink->BeginFigure(currentLocation, D2D1_FIGURE_BEGIN_FILLED);

		float x = currentLocation.x, y = currentLocation.y;
		bool isX = true;
		for (int i = 1; i < 20; i++) {

			D2D1_POINT_2F movedPoint = D2D1::Point2F(x, y);
			pSink->AddLine(movedPoint);
			if (isX) {
				x = BLOCK * i;
				if ((i - 3) % 4 == 0) x = (-1) * x;
				isX = false;
			}
			else {
				y = x;
				isX = true;
			}
		}

		pSink->EndFigure(D2D1_FIGURE_END_OPEN);

		hr = pSink->Close();
	}

	SAFE_RELEASE(pSink);

	/* ----------------- �������� ������ �����̴� �� --------------- */
	if (SUCCEEDED(hr))
	{
		hr = m_pD2DFactory->CreatePathGeometry(&m_pPathGeometry2);
	}
	if (SUCCEEDED(hr))
	{
		hr = m_pPathGeometry2->Open(&pSink);
	}
	if (SUCCEEDED(hr))
	{
		D2D1_POINT_2F currentLocation = { BASE_X, 0 };

		pSink->BeginFigure(currentLocation, D2D1_FIGURE_BEGIN_FILLED);

		float x = currentLocation.x, y = currentLocation.y;
		bool isX = true;
		for (int i = 1; i < 20; i++) {

			D2D1_POINT_2F movedPoint = D2D1::Point2F(x, y);
			pSink->AddLine(movedPoint);
			if (isX) {
				x = BLOCK * i;
				if ((i - 3) % 4 == 0) x = (-1) * x;
				x += BASE_X;
				isX = false;
			}
			else {
				y = x - BASE_X;
				isX = true;
			}
		}

		pSink->EndFigure(D2D1_FIGURE_END_OPEN);

		hr = pSink->Close();
	}

	SAFE_RELEASE(pSink);

	/* ------------------ ����� �ﰢ�� (��ǻ��) ------------------ */
	if (SUCCEEDED(hr))
	{
		hr = m_pD2DFactory->CreatePathGeometry(&m_pObjectGeometry);
	}
	if (SUCCEEDED(hr))
	{
		hr = m_pObjectGeometry->Open(&pSink);
	}
	if (SUCCEEDED(hr))
	{
		pSink->BeginFigure(D2D1::Point2F(0.0f, 0.0f), D2D1_FIGURE_BEGIN_FILLED);

		const D2D1_POINT_2F ptTriangle[] = { {-10.0f, -10.0f}, {-10.0f, 10.0f}, {0.0f, 0.0f} };
		pSink->AddLines(ptTriangle, 3);

		pSink->EndFigure(D2D1_FIGURE_END_OPEN);

		hr = pSink->Close();
	}

	SAFE_RELEASE(pSink);

	/* ------------------ ���λ� �ﰢ�� (����) ------------------ */
	if (SUCCEEDED(hr))
	{
		hr = m_pD2DFactory->CreatePathGeometry(&m_pObjectGeometry2);
	}
	if (SUCCEEDED(hr))
	{
		hr = m_pObjectGeometry2->Open(&pSink);
	}
	if (SUCCEEDED(hr))
	{
		pSink->BeginFigure(D2D1::Point2F(0.0f, 0.0f), D2D1_FIGURE_BEGIN_FILLED);

		const D2D1_POINT_2F ptTriangle[] = { {-10.0f, -10.0f}, {-10.0f, 10.0f}, {0.0f, 0.0f} };
		pSink->AddLines(ptTriangle, 3);

		pSink->EndFigure(D2D1_FIGURE_END_OPEN);

		hr = pSink->Close();
	}

	SAFE_RELEASE(pSink);

	return hr;
}

HRESULT DemoApp::LoadBitmapFromResource(ID2D1RenderTarget* pRenderTarget, IWICImagingFactory* pIWICFactory, PCWSTR resourceName, PCWSTR resourceType, ID2D1Bitmap** ppBitmap)
{
	HRESULT hr = S_OK;
	IWICBitmapDecoder* pDecoder = NULL;
	IWICBitmapFrameDecode* pSource = NULL;
	IWICStream* pStream = NULL;
	IWICFormatConverter* pConverter = NULL;

	HRSRC imageResHandle = NULL;
	HGLOBAL imageResDataHandle = NULL;
	void* pImageFile = NULL;
	DWORD imageFileSize = 0;

	// Locate the resource.
	imageResHandle = FindResourceW(HINST_THISCOMPONENT, resourceName, resourceType);

	hr = imageResHandle ? S_OK : E_FAIL;
	if (SUCCEEDED(hr))
	{
		// Load the resource.
		imageResDataHandle = LoadResource(HINST_THISCOMPONENT, imageResHandle);

		hr = imageResDataHandle ? S_OK : E_FAIL;
	}

	if (SUCCEEDED(hr))
	{
		// Lock it to get a system memory pointer.
		pImageFile = LockResource(imageResDataHandle);

		hr = pImageFile ? S_OK : E_FAIL;
	}

	if (SUCCEEDED(hr))
	{
		// Calculate the size.
		imageFileSize = SizeofResource(HINST_THISCOMPONENT, imageResHandle);

		hr = imageFileSize ? S_OK : E_FAIL;
	}

	if (SUCCEEDED(hr))
	{
		// Create a WIC stream to map onto the memory.
		hr = pIWICFactory->CreateStream(&pStream);
	}

	if (SUCCEEDED(hr))
	{
		// Initialize the stream with the memory pointer and size.
		hr = pStream->InitializeFromMemory(reinterpret_cast<BYTE*>(pImageFile), imageFileSize);
	}

	if (SUCCEEDED(hr))
	{
		// Create a decoder for the stream.
		hr = pIWICFactory->CreateDecoderFromStream(pStream, NULL, WICDecodeMetadataCacheOnLoad, &pDecoder);
	}

	if (SUCCEEDED(hr))
	{
		// Create the initial frame.
		hr = pDecoder->GetFrame(0, &pSource);
	}

	if (SUCCEEDED(hr))
	{
		// Convert the image format to 32bppPBGRA
		// (DXGI_FORMAT_B8G8R8A8_UNORM + D2D1_ALPHA_MODE_PREMULTIPLIED).
		hr = pIWICFactory->CreateFormatConverter(&pConverter);
	}

	if (SUCCEEDED(hr))
	{
		hr = pConverter->Initialize(pSource, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);
	}

	if (SUCCEEDED(hr))
	{
		// Create a Direct2D bitmap from the WIC bitmap.
		hr = pRenderTarget->CreateBitmapFromWicBitmap(pConverter, NULL, ppBitmap);
	}

	SAFE_RELEASE(pDecoder);
	SAFE_RELEASE(pSource);
	SAFE_RELEASE(pStream);
	SAFE_RELEASE(pConverter);

	return hr;
}


HRESULT DemoApp::CreateDeviceResources()
{
	HRESULT hr = S_OK;

	if (!m_pRT)
	{
		RECT rc;
		GetClientRect(m_hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

		// D2D ����Ÿ���� ������.
		hr = m_pD2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(m_hwnd, size), &m_pRT);

		// ������ ���� ������.
		if (SUCCEEDED(hr))
		{
			hr = m_pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &m_pRedBrush);
		}
		// �Ͼ�� ���� ������.
		if (SUCCEEDED(hr))
		{
			hr = m_pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_pWhiteBrush);
		}
		// ����� ���� ������.
		if (SUCCEEDED(hr))
		{
			hr = m_pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &m_pYellowBrush);
		}
		// �ʷϻ� ���� ������.
		if (SUCCEEDED(hr))
		{
			hr = m_pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::GreenYellow), &m_pGreenBrush);
		}
		// �ؽ�Ʈ �� ���� ������
		if (SUCCEEDED(hr))
		{
			hr = m_pRT->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_pTextBrush);
		}

		// ������ ����ũ
		if (SUCCEEDED(hr))
		{
			hr = LoadBitmapFromResource(m_pRT, m_pWICFactory, L"Background", L"PNG", &m_pFernBitmap);
		}

		if (SUCCEEDED(hr))
		{
			hr = LoadBitmapFromResource(m_pRT, m_pWICFactory, L"BackgroundMask2", L"PNG", &m_pBitmapMask);
		}

		if (SUCCEEDED(hr))
		{
			D2D1_BITMAP_BRUSH_PROPERTIES propertiesXClampYClamp = D2D1::BitmapBrushProperties(D2D1_EXTEND_MODE_CLAMP, D2D1_EXTEND_MODE_CLAMP, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);

			hr = m_pRT->CreateBitmapBrush(m_pFernBitmap, propertiesXClampYClamp, &m_pFernBitmapBrush);

			if (SUCCEEDED(hr))
			{
				hr = m_pRT->CreateBitmapBrush(m_pBitmapMask, propertiesXClampYClamp, &m_pBitmapMaskBrush);
			}
		}
	}

	return hr;
}

void DemoApp::DiscardDeviceResources()
{
	SAFE_RELEASE(m_pRT);
	SAFE_RELEASE(m_pRedBrush);
	SAFE_RELEASE(m_pWhiteBrush);
	SAFE_RELEASE(m_pYellowBrush);
	SAFE_RELEASE(m_pGreenBrush);
	SAFE_RELEASE(m_pFernBitmap);
	SAFE_RELEASE(m_pBitmapMask);
	SAFE_RELEASE(m_pFernBitmapBrush);
	SAFE_RELEASE(m_pBitmapMaskBrush);
	SAFE_RELEASE(m_pTextBrush);
}

void DemoApp::RunMessageLoop()
{
	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

HRESULT DemoApp::OnRender()
{
	HRESULT hr;

	hr = CreateDeviceResources();
	if (SUCCEEDED(hr))
	{
		D2D1_POINT_2F point;
		D2D1_POINT_2F tangent;

		D2D1_POINT_2F point2 = { 0, 0 };
		D2D1_POINT_2F tangent2 = { 0, 0 };

		D2D1_MATRIX_3X2_F triangleMatrix;
		D2D1_SIZE_F rtSize = m_pRT->GetSize();
		float minWidthHeightScale = min(rtSize.width, rtSize.height) / 512;

		D2D1::Matrix3x2F scale = D2D1::Matrix3x2F::Scale(minWidthHeightScale, minWidthHeightScale);

		D2D1::Matrix3x2F translation = D2D1::Matrix3x2F::Translation(rtSize.width / 4, rtSize.height / 2);

		// �׸��⸦ �غ���.
		m_pRT->BeginDraw();

		// ��ȯ�� �׵���ķ� ������.
		m_pRT->SetTransform(D2D1::Matrix3x2F::Identity());

		// ����Ÿ���� Ŭ������.
		m_pRT->Clear(D2D1::ColorF(D2D1::ColorF::Black));

		/* ------------------- ��� ���� - ���� -------------------------- */
		// ������ ä�� �簢�� ����� ������.
		D2D1_RECT_F rcBrushRect = D2D1::RectF(0, 0, 1024, 512);

		// �簢���� m_pFernBitmapBrush ������ ä��鼭 m_pBitmapMaskBrush�� ������ ����ũ�� �����.
		m_pRT->SetTransform(D2D1::Matrix3x2F::Translation(0, 0));

		// FillOpacityMask �Լ��� �ùٸ��� �����Ϸ��� ������ D2D1_ANTIALIAS_MODE_ALIASED�� �����ؾ� ��.
		m_pRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
		m_pRT->FillOpacityMask(m_pBitmapMask, m_pFernBitmapBrush, D2D1_OPACITY_MASK_CONTENT_GRAPHICS, &rcBrushRect);
		m_pRT->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

		/* ---------------------------- �ؽ�Ʈ ---------------------------- */
		m_pRT->SetTransform(D2D1::Matrix3x2F::Identity());
		WCHAR szText[100];
		swprintf_s(szText, L"�����̽� �ٸ� �����ּ���!\n��ǻ�� �ӵ� : %.1f\n�÷��̾� �ӵ� : %.1f\n", COM_VELOCITY, changedVelocity);

		m_pRT->DrawText(
			szText,
			wcslen(szText),
			m_pTextFormat,
			D2D1::RectF(430.0f, 400.0f, 700.0f, 500.0f),
			m_pTextBrush
		);

		/* ---------------------------- ������ ---------------------------- */

		// �̵� ���� ���� ��ΰ� ȭ�� �߽ɿ��� ���� ���ʿ� �׷������� ��.
		m_pRT->SetTransform(scale * translation);

		// �̵� ������ ���������� �׸�.
		m_pRT->DrawGeometry(m_pPathGeometry, m_pRedBrush);

		m_pRT->DrawGeometry(m_pPathGeometry2, m_pWhiteBrush);

		/* ---------------------------- ��� �ﰢ�� ---------------------------- */

		static float anim_time = 0.0f;

		float length = m_Animation.GetValue(anim_time);

		// ���� �ð��� �ش��ϴ� ���� ���̿� ��ġ�ϴ� �̵� ���� ���� ������ ����.
		m_pPathGeometry->ComputePointAtLength(length, NULL, &point, &tangent);

		// �ﰢ���� ������ �����Ͽ� �̵� ������ ���󰡴� ������ �ǵ��� ��.
		triangleMatrix = D2D1::Matrix3x2F(
			tangent.x, tangent.y,
			-tangent.y, tangent.x,
			point.x, point.y);

		m_pRT->SetTransform(triangleMatrix * scale * translation);

		// �ﰢ���� ��������� �׸�.
		m_pRT->FillGeometry(m_pObjectGeometry, m_pYellowBrush);

		/* ---------------------------- �ӽ� ���λ� ---------------------------- */

		float length2 = m_Animation2.GetValue(anim_time);

		translation = D2D1::Matrix3x2F::Translation(rtSize.width / 4 + BASE_X - 38, rtSize.height / 2);

		// ���� �ð��� �ش��ϴ� ���� ���̿� ��ġ�ϴ� �̵� ���� ���� ������ ����.
		m_pPathGeometry->ComputePointAtLength(length2, NULL, &point2, &tangent2);

		// �ﰢ���� ������ �����Ͽ� �̵� ������ ���󰡴� ������ �ǵ��� ��.
		triangleMatrix = D2D1::Matrix3x2F(
			tangent2.x, tangent2.y,
			-tangent2.y, tangent2.x,
			point2.x, point2.y);

		m_pRT->SetTransform(triangleMatrix * scale * translation);

		// �ﰢ���� ���λ����� �׸�.
		m_pRT->FillGeometry(m_pObjectGeometry2, m_pGreenBrush);


		// �׸��� ������� ������.
		hr = m_pRT->EndDraw();

		if (hr == D2DERR_RECREATE_TARGET)
		{
			hr = S_OK;
			DiscardDeviceResources();
		}

		/* -------------------- ��� �ﰢ�� �ִϸ��̼� -------------------- */

		// �ִϸ��̼��� ���� �����ϸ� �ٽ� ó������ �ǵ����� �ݺ��ǵ��� ��.
		if (anim_time >= m_Animation.GetDuration())
		{
			anim_time = 0.0f;
		}
		else
		{
			LARGE_INTEGER CurrentTime;
			QueryPerformanceCounter(&CurrentTime);

			float elapsedTime = (float)((double)(CurrentTime.QuadPart - m_nPrevTime.QuadPart) / (double)(m_nFrequency.QuadPart));
			m_nPrevTime = CurrentTime;

			anim_time += elapsedTime;
		}
	}

	return hr;
}

void DemoApp::OnResize(UINT width, UINT height)
{
	if (m_pRT)
	{
		D2D1_SIZE_U size;
		size.width = width;
		size.height = height;

		m_pRT->Resize(size);
	}
}

LRESULT CALLBACK DemoApp::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	if (message == WM_CREATE)
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		DemoApp* pDemoApp = (DemoApp*)pcs->lpCreateParams;

		SetWindowLongPtrW(hwnd, GWLP_USERDATA, PtrToUlong(pDemoApp));

		result = 1;
	}
	else
	{
		DemoApp* pDemoApp = reinterpret_cast<DemoApp*>(static_cast<LONG_PTR>(GetWindowLongPtrW(hwnd, GWLP_USERDATA)));

		bool wasHandled = false;

		if (pDemoApp)
		{
			switch (message)
			{
			case WM_SIZE:
			{
				UINT width = LOWORD(lParam);
				UINT height = HIWORD(lParam);
				pDemoApp->OnResize(width, height);
			}
			result = 0;
			wasHandled = true;
			break;

			case WM_DISPLAYCHANGE:
			{
				InvalidateRect(hwnd, NULL, FALSE);
			}
			result = 0;
			wasHandled = true;
			break;

			case WM_KEYDOWN:
			{
				if (wParam == VK_ESCAPE) PostQuitMessage(0);
				else if (wParam == VK_SPACE) {
					if (changedVelocity > 13.6f)
					{
						spaceBar += 0.1;
						changedVelocity = USER_VELOCITY - spaceBar;
						pDemoApp->m_Animation2.SetDuration(changedVelocity);
					}
				}
				else
					pDemoApp->m_Animation2.SetDuration(0.0f);
				break;
			}

			case WM_PAINT:
			{
				pDemoApp->OnRender();

				// ���⿡�� ValidateRect�� ȣ������ ���ƾ� OnRender �Լ��� ��� �ݺ� ȣ���.
			}
			result = 0;
			wasHandled = true;
			break;

			case WM_DESTROY:
			{
				PostQuitMessage(0);
			}
			result = 1;
			wasHandled = true;
			break;
			}
		}

		if (!wasHandled)
		{
			result = DefWindowProc(hwnd, message, wParam, lParam);
		}
	}

	return result;
}
