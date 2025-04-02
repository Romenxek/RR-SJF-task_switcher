#include "Process.h"
#pragma once
using namespace System::Collections::Generic;
using namespace System;

#ifndef PROCDIC_H
#define PROCDIC_H

public ref class ProcDictionary: Dictionary<Process^,int>
{
private:
	int _handler;
	static ProcDictionary^ instance;
	Process^ _process;
	ProcDictionary() {}

public:
	static property ProcDictionary^ Instance
	{
		ProcDictionary^ get();
	}

	void AddProcess(Process^ process, int ID);
	void Round_Robin(int time_quanta);
	bool ContainsProcessID(int ID);
};

#endif // PROCDIC_H
