
#pragma once
#include <chrono>

namespace GameEngine
{
	namespace Utility
	{
		class Timer
		{
		public:
			Timer();
			double GetMilisecondsElapsed() const;
			void Restart();
			bool Stop();
			bool Start();
		private:
			bool isrunning = false;
#ifdef _WIN32
			std::chrono::time_point<std::chrono::steady_clock> start;
			std::chrono::time_point<std::chrono::steady_clock> stop;
#else
			std::chrono::time_point<std::chrono::system_clock> start;
			std::chrono::time_point<std::chrono::system_clock> stop;
#endif
		};
	}
}

