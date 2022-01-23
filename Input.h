#ifndef INPUT_H
#define INPUT_H

#include<Windows.h>

	namespace Input
	{

		enum MOUSE_INPUT
		{
			MOUSE_INPUT_LBUTTON = 0,
			MOUSE_INPUT_RBUTTON = 1,
			MOUSE_INPUT_FORCE4  = 0x7fffffff
		};

		enum KEYBOARD_INPUT
		{
			KEYBOARD_INPUT_SPACE = 0,
			KEYBOARD_INPUT_UP    = 1,
			KEYBOARD_INPUT_DOWN  = 2,
			KEYBOARD_INPUT_LEFT  = 3,
			KEYBOARD_INPUT_RIGHT = 4,
			KEYBOARD_INPUT_ENTER = 5,
			KEYBOARD_INPUT_ESC   = 6,
			KEYBOARD_INPUT_FORCE4  = 0x7fffffff
		};


		class InputListener
		{

		public:

			virtual void OnKeyPressed ( KEYBOARD_INPUT )=0;
			virtual void OnKeyReleased( KEYBOARD_INPUT)=0;

			virtual void OnMouseMove( const long, const long )=0;
			virtual void OnMousePressed( const long, const long, MOUSE_INPUT )=0; 
			virtual void OnMouseReleased( const long, const long, MOUSE_INPUT )=0; 
		};

		class InputSignal
		{
  
		public:

			InputSignal();
			~InputSignal();

			static bool Initialize();
			static bool GetInputFromQueue(const LPARAM lParam);
			static void SetListener(InputListener*);

		private:
  
			static InputListener *Listener;
			static unsigned char Data[40];

			static bool MouseLeft;
			static bool MouseRight;
		};

	};

#endif