#pragma once
#define KEYDOWN(name, key) (name[key] &0x80)
#define CLICK(button) (mouse.rgbButtons[button] &0x80)
#define LEFT_BUTTON 0
#define RIGHT_BUTTON 1
#define MIDDLE_BUTTON 2

extern const FLOAT MaxAngle;
extern FLOAT Angle;
extern POINT MousePos;

class MyInput
{
private:
	char keyboard[256];
	DIMOUSESTATE mouse;
	POINT position;
public:
	MyInput();
	~MyInput();
	//interfaces
	LPDIRECTINPUT8 pInput;
	LPDIRECTINPUTDEVICE8 pKeyboard;
	LPDIRECTINPUTDEVICE8 pMouse;
	//functions
	int CreateInput(HINSTANCE);
	int CreateKeyboard(HWND);
	int CreateMouse(HWND);
	void DeleteKeyboard();
	void DeleteMouse();
	HRESULT ReadKeyboard();
	HRESULT ReadMouse();
};

