#include "Processes.h"
#include "Process.h"
#include <climits> // для выявления наименьшего в Processes::SetCurrentProcess::set(String^ algorithmType)
#pragma once

Processes::Processes() 
{
	_processesList = gcnew List<Process^>();
	_readyProcessesList = gcnew List<Process^>();
	_finishedProcessesList = gcnew List<Process^>();
	_needToUpdateInDGW = gcnew List<Process^>();
	_runningProcess;
}

List<Process^>^ Processes::ProcessesList::get() 
{
	return _processesList;
}

void Processes::ProcessesList::set(List<Process^>^)
{
	_processesList = gcnew List<Process^>();
}

List<Process^>^ Processes::ReadyProcessesList::get()
{
	_readyProcessesList->Clear();

	for each (Process ^ p in ProcessesList)
	{
		if (p->PState == Process::State::Ready) { _readyProcessesList->Add(p); }
	}
	return _readyProcessesList;
}

List<Process^>^ Processes::FinishedProcessesList::get()
{
	_finishedProcessesList->Clear();

	for each (Process ^ p in ProcessesList)
	{
		if (p->PState == Process::State::Finished) { _finishedProcessesList->Add(p); }
	}

	return _finishedProcessesList;
}

Process^ Processes::RunningProcess::get()
{
	return _runningProcess;
}

void Processes::RunningProcess::set(Process^ process)
{
	_runningProcess = process;
}

void Processes::SetRunningToReady()
{
	if (_runningProcess != nullptr && _runningProcess->PState != Process::State::Finished) // если текущий выбран и не завершён, ставим в Ready и обновляем
	{
		_runningProcess->PState = Process::State::Ready;
		if (!_needToUpdateInDGW->Contains(_runningProcess)) { _needToUpdateInDGW->Add(_runningProcess); }
	}

	_runningProcess = nullptr;
}

void Processes::SetMinIDAsRunning(List<Process^>^ list)
{
	int minID = INT_MAX;
	Process^ processWithMinID;
	for each (Process ^ p in list)
	{
		if (p->ProcessID <= minID)
		{
			processWithMinID = p;
			minID = p->ProcessID;
		}
	}
	_runningProcess = processWithMinID;
	_runningProcess->PState = Process::State::Running;
	_runningProcess->ReadyTime = 0;
}

void Processes::SetRunningProcess(String^ algorithmType)
{
	SetRunningToReady();
	if (ReadyProcessesList->Count == 0) { return; }

	if (algorithmType == "RR") // если выбран RR
	{
		Process^ processWithMaxReadyTime;
		List<Process^>^ processesWithEqReadyTime = gcnew List<Process^>;
		Process^ processWithMinID;

		int minID = INT_MAX;
		int maxReadyTime = 0;

		for each (Process ^ p in ReadyProcessesList) // для каждого готового ищем максимальное время
		{
			if (p->ReadyTime > maxReadyTime)
			{
				processesWithEqReadyTime->Clear();
				processWithMaxReadyTime = p;
				maxReadyTime = p->ReadyTime;
				processesWithEqReadyTime->Add(p);
			}
			else if (p->ReadyTime == maxReadyTime)
			{
				processesWithEqReadyTime->Add(p);
			}
		}

		if (processesWithEqReadyTime->Count != 1) // если два процесса с одним временем готовности ищем с минимальным id и устанавливаем
		{
			SetMinIDAsRunning(processesWithEqReadyTime);
		}
		else if (processesWithEqReadyTime->Count == 1) // если только один процесс с минимальным временем готовности
		{
			_runningProcess = processesWithEqReadyTime[0];
			_runningProcess->PState = Process::State::Running;
			_runningProcess->ReadyTime = 0;
		}
	}
	else if (algorithmType == "SJF")
	{
		Process^ processWithMinExecutionTime;
		List<Process^>^ processesWithEqExT = gcnew List<Process^>;
		int minExT = INT_MAX;

		Process^ processWithMinID;
		int minID = INT_MAX;

		for each (Process ^ p in ReadyProcessesList) // для каждого готового ищем максимальное время
		{
			if (p->ExecutionTime < minExT)
			{
				processesWithEqExT->Clear();
				processWithMinExecutionTime = p;
				minExT = p->ExecutionTime;
				processesWithEqExT->Add(p);
			}
			else if (p->ExecutionTime == minExT)
			{
				processesWithEqExT->Add(p);
			}
		}

		if (processesWithEqExT->Count != 1) // если два процесса с одним временем готовности ищем с минимальным id и устанавливаем
		{
			SetMinIDAsRunning(processesWithEqExT);
		}
		else if (processesWithEqExT->Count == 1) // если только один процесс с минимальным временем готовности
		{
			_runningProcess = processesWithEqExT[0];
			_runningProcess->PState = Process::State::Running;
			_runningProcess->ReadyTime = 0;
		}
	}

	if (!_needToUpdateInDGW->Contains(_runningProcess) && _runningProcess != nullptr)
	{
		_needToUpdateInDGW->Add(_runningProcess);
	}
	return;
}

void Processes::AddProcess(Process^ newProcess)
{
	for each (Process ^ p in ProcessesList)
	{
		if (p->ProcessID == newProcess->ProcessID)
		{
			MessageBox::Show("Процесс с таким PID уже существует.", "Ошибка создания процесса",
				MessageBoxButtons::OK, MessageBoxIcon::Error);
			return;
		}
	}
	ProcessesList->Add(newProcess);
	return;
}

void Processes::AddReadyTime(int time)
{
	for each (Process ^ p in this->ReadyProcessesList)
	{
		p->ReadyTime += time;
		if (!this->_needToUpdateInDGW->Contains(p))
		{
			this->_needToUpdateInDGW->Add(p);
		}
	}
}
/*void Processes::RoundRobin(int cycleInQuant, int numOfQuants) // без простоя по квантам, излишек тактов - простой
{
	while (numOfQuants != 0) // пока все кванты не потрачены
	{
		if (ReadyProcessesList->Count != 0) 
		{
			if (cycleInQuant >= _runningProcess->ExecutionTime) // если такт больше оставшего времени исполнения
			{
				numOfQuants -= 1;
				_runningProcess->ExecutionTime = 0;
				_runningProcess->PState = Process::State::Finished;
				_needToUpdateInDGW->Add(_runningProcess);
			}
			else // если такт меньше оставшегося времени исполнения
			{
				_runningProcess->ExecutionTime -= cycleInQuant;
				numOfQuants -= 1;
				SetRunningProcess("RR"); // обновление статуса в функции
			}
		}
		else // если у нас нет готовых процессов, то ждём их
		{
			numOfQuants -= 1;
			for each (Process ^ p in _processesList)
			{
				
			}
		} 

	}
	while (cycleInQuant * numOfQuants != 0)
	{
		if (cycleInQuant * numOfQuants >= _runningProcess->ExecutionTime)
		{
			int quantsForProcess = (_runningProcess->ExecutionTime - 1 + cycleInQuant) / cycleInQuant;//формула для деления с округлением к верхнему целому
			numOfQuants -= quantsForProcess;
			_runningProcess->ExecutionTime = 0;
			_runningProcess->PState = Process::State::Finished;
			_needToUpdateInDGW->Add(_runningProcess);
			if (this->ReadyProcessesList->Count != 0)
			{
				this->SetRunningProcess("RR");
			}
			else (numOfQuants = 0);
		}
		if (cycleInQuant * numOfQuants < _runningProcess->ExecutionTime)
		{
			for each (Process ^ p in ReadyProcessesList)
			{
				p->ReadyTime += cycleInQuant * numOfQuants;
				if (!_needToUpdateInDGW->Contains(p))
				{
					_needToUpdateInDGW->Add(p);
				}
			}
			_runningProcess->ExecutionTime -= cycleInQuant * numOfQuants;
			_needToUpdateInDGW->Add(_runningProcess);
			if (this->ReadyProcessesList->Count != 0)
			{
				this->SetRunningProcess("RR");
			}
			numOfQuants = 0;
		}
	}
}*/

/*void Processes::SJF(int cycleInQuant, int numOfQuants)
{
	while (cycleInQuant * numOfQuants != 0)
	{
		if (cycleInQuant * numOfQuants >= _runningProcess->ExecutionTime)
		{
			int quantsForProcess = (_runningProcess->ExecutionTime - 1 + cycleInQuant) / cycleInQuant;//формула для деления с округлением к верхнему целому
			numOfQuants -= quantsForProcess;
			_runningProcess->ExecutionTime = 0;
			_runningProcess->PState = Process::State::Finished;
			_needToUpdateInDGW->Add(_runningProcess);
			if (this->ReadyProcessesList->Count != 0)
			{
				this->SetRunningProcess("SJF");
			}
			else (numOfQuants = 0);
		}
		if (cycleInQuant * numOfQuants < _runningProcess->ExecutionTime)
		{
			_runningProcess->ExecutionTime -= cycleInQuant * numOfQuants;
			numOfQuants = 0;
			_needToUpdateInDGW->Add(_runningProcess);
		}
	}
}*/