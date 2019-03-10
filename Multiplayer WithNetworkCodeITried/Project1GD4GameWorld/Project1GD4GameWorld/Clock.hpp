#pragma once
#define TIMER_H
#include <ctime>
#include <string>

class Stopwatch
{
public:
	explicit Stopwatch(bool start_immediately = false);
	void Start(bool reset = false);
	void Stop();
	unsigned long Elapsed() const;
	std::string getTimeMins();
private:
	std::clock_t start, stop;
	bool running;
};

//This code was taken from https://www.daniweb.com/programming/software-development/code/428040/simple-timer-class-c
