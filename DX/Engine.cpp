#include "Engine.h"
#include <ostream>
#include <Windows.h>
#include <iostream>
#include <sstream>

#define DBOUT( s )            \
{                             \
   std::wostringstream os_;    \
   os_ << s;                   \
   OutputDebugStringW( os_.str().c_str() );  \
}
namespace GameEngine
{

	bool Engine::Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height)
	{
		timer.Start();

		if (!this->render_window.Initialize(this, hInstance, window_title, window_class, width, height))
			return false;

		if(!gfx.Initialize(render_window.GetHWND(), width, height))
		{
			return false;
		}

		return true;
	}

	bool Engine::ProcessMessages()
	{
		return this->render_window.ProcessMessages();
	}

	void Engine::Update()
	{
		float dt = timer.GetMilisecondsElapsed();
		timer.Restart();

		while (!keyboard.CharBufferIsEmpty())
		{
			unsigned char ch = keyboard.ReadChar();
		}

		while (!keyboard.KeyBufferIsEmpty())
		{
			KeyboardEvent kbe = keyboard.ReadKey();
			unsigned char keycode = kbe.GetKeyCode();
		}

		while (!mouse.EventBufferIsEmpty())
		{
			MouseEvent me = mouse.ReadEvent();

		}

	}

	void Engine::RenderFrame()
	{
		gfx.RenderFrame();
	}
}