#include "stdafx.h"
#include "MyInput.h"

MyInput::MyInput()
{
	pInput = NULL;
	pKeyboard = NULL;
	pMouse = NULL;
}


MyInput::~MyInput()
{
	pInput = NULL;
	pKeyboard = NULL;
	pMouse = NULL;
}

//functions
int MyInput::CreateInput(HINSTANCE hInstance){
	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&pInput, NULL)))
		return E_FAIL;
	return 0;
}

int MyInput::CreateKeyboard(HWND hWnd){
	if (FAILED(pInput->CreateDevice(GUID_SysKeyboard, &pKeyboard, NULL))){
		DeleteKeyboard();
		return E_FAIL;
	}
	if (FAILED(pKeyboard->SetDataFormat(&c_dfDIKeyboard))){
		DeleteKeyboard();
		return E_FAIL;
	}
	if (FAILED(pKeyboard->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))){
		DeleteKeyboard();
		return E_FAIL;
	}
	if (FAILED(pKeyboard->Acquire())){
		DeleteKeyboard();
		return E_FAIL;
	}
	return 0;
}

int MyInput::CreateMouse(HWND hWnd){
	if (FAILED(pInput -> CreateDevice(GUID_SysMouse, &pMouse, NULL)))
		return E_FAIL;

	if (FAILED(pMouse->SetDataFormat(&c_dfDIMouse)))
		return E_FAIL;

	if (FAILED(pMouse->SetCooperativeLevel(hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE)))
		return E_FAIL;
	
	if (FAILED(pMouse->Acquire()))
		return E_FAIL;
	return 0;
}
void MyInput::DeleteKeyboard(){
	if (pKeyboard){
		pKeyboard->Unacquire();
		pKeyboard->Release();
		pKeyboard = NULL;
	}
}
void MyInput::DeleteMouse(){
	if (pMouse){
		pMouse->Unacquire();
		pMouse->Release();
		pMouse = NULL;
	}
}
HRESULT MyInput::ReadKeyboard(){
	
	if (FAILED(pKeyboard->GetDeviceState(sizeof(keyboard), (LPVOID)&keyboard)))
		return FALSE;
	if (KEYDOWN(keyboard, DIK_LEFT)){
		if (Angle > MaxAngle) Angle -= MaxAngle + MaxAngle;
		Angle += 0.1f;
		return TRUE;
	}
	if (KEYDOWN(keyboard, DIK_RIGHT)){
		if (Angle < -MaxAngle) Angle += MaxAngle + MaxAngle;
		Angle -= 0.1f;
		return TRUE;
	}
	return TRUE;
}
HRESULT MyInput::ReadMouse(){
	
	if (FAILED(pMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&mouse)))
		return FALSE;
	if (CLICK(LEFT_BUTTON)){
		Angle = 0;
		return TRUE;
	}
	if (CLICK(RIGHT_BUTTON)){
		Angle = 3.14f;
		return TRUE;
	}
	GetCursorPos(&position);
	MousePos.x = position.x;
	MousePos.y = position.y;
	return TRUE;
}