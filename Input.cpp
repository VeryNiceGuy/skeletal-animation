#include "Input.h"
#include"ErrorHandler.h"

	Input::InputListener* Input::InputSignal::Listener;
	unsigned char Input::InputSignal::Data[40];

	bool Input::InputSignal::MouseLeft;
	bool Input::InputSignal::MouseRight;

	Input::InputSignal::InputSignal() 
	{
		ErrorHandler::ShowWarning(L"InputSignal is Instantiated!");	
	};

	Input::InputSignal::~InputSignal(){};


	bool Input::InputSignal::GetInputFromQueue(const LPARAM lParam)
	{

		unsigned int dwSize=0;

		if(!Listener) return false;

		GetRawInputData((HRAWINPUT)lParam,
						RID_INPUT,
						0,
						&dwSize,
						sizeof(RAWINPUTHEADER));

		if (GetRawInputData((HRAWINPUT)lParam,
							RID_INPUT,
							Data,
							&dwSize, 
							sizeof(RAWINPUTHEADER)) != dwSize )
							return false;

		RAWINPUT* raw = (RAWINPUT*)Data;

		if (raw->header.dwType == RIM_TYPEKEYBOARD) 
		{ 
			raw->data.keyboard.Flags;
			raw->data.keyboard.VKey;

				switch(raw->data.keyboard.VKey)
				{

					case VK_UP: 

						if(raw->data.keyboard.Flags & RI_KEY_BREAK)
						Listener->OnKeyReleased(KEYBOARD_INPUT_UP);
						else Listener->OnKeyPressed(KEYBOARD_INPUT_UP);

					return true;

					case VK_DOWN: 
				
						if(raw->data.keyboard.Flags & RI_KEY_BREAK)
						Listener->OnKeyReleased(KEYBOARD_INPUT_DOWN);
						else Listener->OnKeyPressed(KEYBOARD_INPUT_DOWN);
   
					return true;

					case VK_LEFT: 
								
						if(raw->data.keyboard.Flags & RI_KEY_BREAK)
						Listener->OnKeyReleased(KEYBOARD_INPUT_LEFT);
						else Listener->OnKeyPressed(KEYBOARD_INPUT_LEFT);

					return true;

					case VK_RIGHT:

						if(raw->data.keyboard.Flags & RI_KEY_BREAK)
						Listener->OnKeyReleased(KEYBOARD_INPUT_RIGHT);
						else Listener->OnKeyPressed(KEYBOARD_INPUT_RIGHT);	   

					return true;
			   
					case VK_SPACE:

						if(raw->data.keyboard.Flags & RI_KEY_BREAK)
						Listener->OnKeyReleased(KEYBOARD_INPUT_SPACE);
						else Listener->OnKeyPressed(KEYBOARD_INPUT_SPACE);

					return true;
			   
					case VK_RETURN:

						if(raw->data.keyboard.Flags & RI_KEY_BREAK)
						Listener->OnKeyReleased(KEYBOARD_INPUT_ENTER);
						else Listener->OnKeyPressed(KEYBOARD_INPUT_ENTER);

					return true;

					case VK_ESCAPE:
								
						if(raw->data.keyboard.Flags & RI_KEY_BREAK)
						Listener->OnKeyReleased(KEYBOARD_INPUT_ESC);
						else Listener->OnKeyPressed(KEYBOARD_INPUT_ESC);
      				   
					return true;

				};
			}

			else if (raw->header.dwType == RIM_TYPEMOUSE) 
			{

				raw->data.mouse.usFlags;

				///////////////////////////// Левая кнопка......

				if(!MouseLeft)
				{

						if(raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) 
						{
								Listener->OnMousePressed(raw->data.mouse.lLastX,
									                     raw->data.mouse.lLastY,
														 MOUSE_INPUT_LBUTTON);
								MouseLeft=true;

								return true;
						};

				}

				else
				{

						if(!(raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)) 
						{
								Listener->OnMouseReleased(raw->data.mouse.lLastX,
															raw->data.mouse.lLastY,
															MOUSE_INPUT_LBUTTON);
								MouseLeft=false;

								return true;
						};


				};

			///////////////////////////// Правая кнопка......

				if(!MouseRight)
				{

						if(raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) 
						{
								Listener->OnMousePressed(raw->data.mouse.lLastX,
														raw->data.mouse.lLastY,
														MOUSE_INPUT_RBUTTON);
								MouseRight=true;

								return true;
						};
				}

				else
				{

						if(!(raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)) 
						{
								Listener->OnMouseReleased(raw->data.mouse.lLastX,
															raw->data.mouse.lLastY,
															MOUSE_INPUT_RBUTTON);
								MouseRight=false;

								return true;
						};

				};

				 // Состояние кнопок неизменилось, просто передаем шаг мыши
				Listener->OnMouseMove(raw->data.mouse.lLastX,raw->data.mouse.lLastY);


			}; 

		   return false;
	};

	void Input::InputSignal::SetListener(InputListener* input)
	{
		Listener = input;
	};

	bool Input::InputSignal::Initialize()
	{

		RAWINPUTDEVICE Rid[2]={0};
        
		// КЛАВИАТУРА
		Rid[0].usUsagePage = 1;
		Rid[0].usUsage = 6;
		Rid[0].dwFlags = 0;
		Rid[0].hwndTarget=NULL;

		// МЫШЬ
		Rid[1].usUsagePage = 1;
		Rid[1].usUsage = 2;
		Rid[1].dwFlags = 0;
		Rid[1].hwndTarget=NULL;


		if(RegisterRawInputDevices(Rid, 2, sizeof(Rid[0])) == false) 
		{
			ErrorHandler::ThreadLastError(); 

			return false;
		};

			return true;
	};