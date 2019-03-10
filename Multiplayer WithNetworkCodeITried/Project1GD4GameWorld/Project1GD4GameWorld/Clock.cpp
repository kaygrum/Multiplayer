#include "Clock.hpp"


Stopwatch::Stopwatch(bool start_immediately)
	: start(0), stop(0), running(false)
{
	if (start_immediately)
	{
		Start(true);
	}
}
void Stopwatch::Start(bool reset)
{
	if (!running)
	{
		if (reset)
		{
			start = std::clock();
		}
		running = true;
	}
}
void Stopwatch::Stop()
{
	if (running)
	{
		stop = std::clock();
		running = false;
	}
}

unsigned long Stopwatch::Elapsed() const
{
	clock_t ticks = (running ? std::clock() : stop) - start;
	double seconds = (double)ticks / CLOCKS_PER_SEC;
	return seconds;
}

std::string Stopwatch::getTimeMins()
{
	int seconds = Elapsed();
	int mins = seconds / 60;
	seconds = (seconds % 60);
	if (seconds == 60)
	{
		seconds = 0;
		mins += 1;
	}

	return std::to_string(mins) + ":" + std::to_string(seconds) + " Minutes";
}
