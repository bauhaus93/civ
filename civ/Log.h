#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <exception>
#include <chrono>
#include <ctime>
#include <iomanip>

#include "CivException.h"

namespace Logger{
	void Write(const std::string& msg);
	void Write(const std::exception& e);
}


class Log{

	static Log* instance;

	ofstream file;
	ostream& out;

public:
	Log(std::string& name);
	Log();
	~Log();



	void Write(const std::string& msg);
	void Write(const exception& e);

	static void CreateStd(std::string& name);
	static void CreateStd();
	static void CloseStd();
	static Log& Instance(void);

};

