using namespace System;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::Windows::Forms;

#include "Process.h"
#pragma once

#ifndef PROCESSES_H
#define PROCESSES_H

ref class Processes
{
private:
	List<Process^>^ _processesList;
	List<Process^>^ _readyProcessesList;
	List<Process^>^ _finishedProcessesList;
	List<Process^>^ _waitingProcessesList;
	Process^ _runningProcess;
public:
	List<Process^>^ _needToUpdateInDGW;
public:
	Processes();
	property List<Process^>^ ProcessesList
	{
		List<Process^>^ get();
		void set(List<Process^>^);
	}
	property List<Process^>^ ReadyProcessesList
	{
		List<Process^>^ get();
	}
	property List<Process^>^ FinishedProcessesList
	{
		List<Process^>^ get();
	}
	property Process^ RunningProcess
	{
		Process^ get();
		void set(Process^ process);
	}
	void SetRunningProcess(String^ algorithmType);
	void AddProcess(Process^ newProcess);
	void AddReadyTime(int time);
	void SetRunningToReady();
	void SetMinIDAsRunning(List<Process^>^ list);
	//void RoundRobin(int cycleInQuant, int numOfQuants);
	//void SJF(int cycleInQuant, int numOfQuants);
};

#endif 