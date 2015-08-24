// Win32Project1.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "DirectXapp.h"

#define MAX_LOADSTRING 100

// Global Variables:
D3DDISPLAYMODE Display;
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

LPDIRECT3D9 pDirect3D = NULL;
LPDIRECT3DDEVICE9 pDirect3DDevice = NULL;

LPD3DXFONT pFont = NULL;
RECT TextRect;

LPD3DXMESH pMesh = NULL;
LPD3DXBUFFER pMeshBuffer = NULL;
D3DMATERIAL9* pMeshMaterials = NULL;
LPDIRECT3DTEXTURE9* pMeshTexture = NULL;
DWORD dwNumber;

MyInput* pInput;
const FLOAT MaxAngle = D3DX_PI;
FLOAT Angle = 0;
POINT MousePos = { 0, 0 };

struct CUSTOMVERTEX {
	FLOAT X, Y, Z;
	FLOAT nx, ny, nz;
	FLOAT tu, tv;
	//DWORD color;
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

//----------------------------------------------------------------
// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT				InitialDirect3D(HWND hwnd);
VOID				Matrix();
VOID				DrawMyText(LPDIRECT3DDEVICE9 pDirect3DDevice, LPCWSTR MyText, int x, int y, int x1, int y1, D3DCOLOR MyColor);
VOID				RenderingDirect3D();
VOID				ReleaseDirect3D();
HRESULT				InitialMesh();
VOID				DrawMyMesh();
HRESULT				InitialInput();


//InitialDirect3D
HRESULT InitialDirect3D(HWND hwnd){
	if ((pDirect3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
		return E_FAIL;
	if (FAILED(pDirect3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &Display)))
		return E_FAIL;
	D3DPRESENT_PARAMETERS Direct3DParameters;
	ZeroMemory(&Direct3DParameters, sizeof(Direct3DParameters));
	Direct3DParameters.Windowed = FALSE;
	Direct3DParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	Direct3DParameters.BackBufferFormat = Display.Format;
	Direct3DParameters.EnableAutoDepthStencil = TRUE;
	Direct3DParameters.AutoDepthStencilFormat = D3DFMT_D16;
	//fullscreen-----------------------------
	Direct3DParameters.BackBufferWidth = GetSystemMetrics(SM_CXSCREEN);
	Direct3DParameters.BackBufferHeight = GetSystemMetrics(SM_CYSCREEN);
	Direct3DParameters.BackBufferCount = 3;
	Direct3DParameters.FullScreen_RefreshRateInHz = Display.RefreshRate;
	//---------------------------------------
	if (FAILED(pDirect3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING, &Direct3DParameters, &pDirect3DDevice)))
		return E_FAIL;
	//свет
	pDirect3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	pDirect3DDevice->SetRenderState(D3DRS_AMBIENT, 0xffffffff);
	//создание буфера глубины
	pDirect3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);
	
	//создаем шрифт-----------------------------
	D3DXCreateFont(pDirect3DDevice, 30, 10, 0, 0, FALSE, UNICODE, 0, 0, DEFAULT_PITCH | FF_MODERN, L"Arial", &pFont);
	return S_OK;
}

HRESULT InitialMesh(){
	if (FAILED(D3DXLoadMeshFromX(L"tiger.x", D3DXMESH_SYSTEMMEM, pDirect3DDevice, NULL, &pMeshBuffer, NULL, &dwNumber, &pMesh)))
		return E_FAIL;
	D3DXMATERIAL* D3DXMeshMaterials;
	D3DXMeshMaterials = (D3DXMATERIAL*)pMeshBuffer->GetBufferPointer();

	pMeshMaterials = new D3DMATERIAL9[dwNumber];
	pMeshTexture = new LPDIRECT3DTEXTURE9[dwNumber];
	for (DWORD i = 0; i < dwNumber; i++){
		//copying materials
		pMeshMaterials[i] = D3DXMeshMaterials[i].MatD3D;
		//seting environment light
		pMeshMaterials[i].Ambient = pMeshMaterials[i].Diffuse;
		//loading texture
		if (FAILED(D3DXCreateTextureFromFile(pDirect3DDevice, L"tiger.bmp", &pMeshTexture[i])))
			pMeshTexture[i] = NULL;
	}
	pMeshBuffer->Release();
	return S_OK;
}

VOID DrawMyMesh(){
	for (DWORD i = 0; i < dwNumber; i++){
		pDirect3DDevice->SetMaterial(&pMeshMaterials[i]);
		pDirect3DDevice->SetTexture(0, pMeshTexture[i]);
		pMesh->DrawSubset(i);
	}
}

//Matrix
VOID Matrix(){
	D3DXMATRIX MatrixWorld;
	D3DXMATRIX MatrixWorldX;
	D3DXMATRIX MatrixWorldY;
	D3DXMATRIX MatrixView;
	D3DXMATRIX MatrixProjection;

	//MatrixWorld
	//UINT Time = timeGetTime() % 5000;
	//FLOAT Angle = Time * (2.0f * D3DX_PI) / 5000.0f;
	//D3DXMatrixRotationX(&MatrixWorldX, Angle);
	D3DXMatrixRotationY(&MatrixWorldY, Angle);
	//D3DXMatrixMultiply(&MatrixWorld, &MatrixWorldX, &MatrixWorldY);
	pDirect3DDevice->SetTransform(D3DTS_WORLD, &MatrixWorldY);
	//MatrixView
	D3DXMatrixLookAtLH(&MatrixView, &D3DXVECTOR3(0.0f, 0.0f, -5.0f), &D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		&D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	pDirect3DDevice->SetTransform(D3DTS_VIEW, &MatrixView);
	//MatrixProjection
	D3DXMatrixPerspectiveFovLH(&MatrixProjection, D3DX_PI / 4, (float)Display.Width / Display.Height, 1.0f, 100.0f);
	pDirect3DDevice->SetTransform(D3DTS_PROJECTION, &MatrixProjection);
}

//Light Material
VOID LightMaterial(){
	D3DMATERIAL9 Material;
	D3DLIGHT9 Light;

	ZeroMemory(&Material, sizeof(D3DMATERIAL9));

	Material.Diffuse.r = Material.Ambient.r = 1.0f;
	Material.Diffuse.g = Material.Ambient.g = 1.0f;
	Material.Diffuse.b = Material.Ambient.b = 1.0f;
	Material.Diffuse.a = Material.Ambient.a = 1.0f;
	pDirect3DDevice->SetMaterial(&Material);

	//Устанавливаем свет
	ZeroMemory(&Light, sizeof(D3DLIGHT9));
	Light.Type = D3DLIGHT_DIRECTIONAL;
	Light.Diffuse.r = 1.0f;
	Light.Diffuse.g = 1.0f;
	Light.Diffuse.b = 1.0f;
	Light.Range = 1000.0f;

	//нормализация...
	D3DXVECTOR3 LightDir(0.0f, 0.0f, 1.0f);
	D3DXVec3Normalize((D3DXVECTOR3*)&Light.Direction, &LightDir);

	pDirect3DDevice->SetLight(0, &Light);
	pDirect3DDevice->LightEnable(0, TRUE);

	pDirect3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	pDirect3DDevice->SetRenderState(D3DRS_AMBIENT, 0);
}

VOID DrawMyText(LPDIRECT3DDEVICE9 pDirect3DDevice, LPCWSTR MyText, int x, int y, int x1, int y1, D3DCOLOR MyColor){
	//text rect pos
	TextRect.left = x;
	TextRect.top = y;
	TextRect.right = x1;
	TextRect.bottom = y1;
	//Draw
	pFont->DrawText(0, MyText, -1, &TextRect, DT_WORDBREAK, MyColor);
}

VOID RenderingDirect3D(){
	if (pDirect3DDevice == NULL)
		return;
	pDirect3DDevice -> Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(60, 100, 150), 1.0f, 0);
	pDirect3DDevice->BeginScene();
	//здесь происходит прорисовка сцены
	Matrix();
	DrawMyMesh();

	DrawMyText(pDirect3DDevice, (std::to_wstring(Angle)).c_str(), 50, 50, 200, 200, D3DCOLOR_RGBA(250, 30, 30, 250));
	DrawMyText(pDirect3DDevice, (std::to_wstring(MousePos.x)).c_str(), 50, 70, 200, 200, D3DCOLOR_RGBA(250, 30, 30, 250));
	DrawMyText(pDirect3DDevice, (std::to_wstring(MousePos.y)).c_str(), 50, 90, 200, 200, D3DCOLOR_RGBA(250, 30, 30, 250));
		
	pDirect3DDevice->EndScene();
	pDirect3DDevice->Present(NULL, NULL, NULL, NULL);
}

VOID ReleaseDirect3D(){
	if (pMeshMaterials) delete[] pMeshMaterials;
	if (pMeshTexture) {
		for (DWORD i = 0; i < dwNumber; i++){
			if (pMeshTexture[i]) pMeshTexture[i]->Release();
		}
		delete[] pMeshTexture;
	}
	if (pMesh) pMesh->Release();
	if (pFont) pFont->Release();
	if (pDirect3DDevice) pDirect3DDevice->Release();
	if (pDirect3D) pDirect3D->Release();
}

//function WinMain
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	
	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_DIRECTXAPP, szWindowClass, MAX_LOADSTRING);

	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
		return FALSE;
	ZeroMemory(&msg, sizeof(msg));
	
	// Main message loop:
	while (msg.message != WM_QUIT){
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
			pInput->ReadKeyboard();
			pInput->ReadMouse();
			RenderingDirect3D();
	}
	return (int) msg.wParam;
}

// создаем и регистрируем WindowClass
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DIRECTXAPP));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+5);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hWnd = CreateWindow(szWindowClass, L"Базовое окно для DirectX", WS_OVERLAPPEDWINDOW | WS_VISIBLE,
      300, 150, 500, 500, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   } 
   if (FAILED(InitialDirect3D(hWnd))) return FALSE;
  
   if (FAILED(pInput = new MyInput())) return FALSE;
   if (FAILED(pInput->CreateInput(hInstance))) return FALSE;
   if (FAILED(pInput->CreateKeyboard(hWnd))) return FALSE;
   if (FAILED(pInput->CreateMouse(hWnd))) return FALSE;
  
   if (FAILED(InitialMesh())) return FALSE;
   
   //if (FAILED(InitialObject())) return FALSE;
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
  
   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	
	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		
		// Parse the menu selections:
		switch (wmId){
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		ReleaseDirect3D();
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			PostQuitMessage(0);
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

