#include "ProcDictionary.h"
#include <random>

void ProcDictionary::AddProcess(Process^ process, int ID)
{
	int newID = ID;
	while (ContainsProcessID(newID)) 
	{
		newID = rand();
	}
	this->Add(process, newID);
}

void ProcDictionary::Round_Robin(int time_quanta)
{
	for each (KeyValuePair<Process^, int> kvp in this)
	{
		Process^ key = kvp.Key;
		key->ExecutionTime -= time_quanta;
	}
}

bool ProcDictionary::ContainsProcessID(int ID)
{
	for each (KeyValuePair<Process^, int> kvp in this)
	{
		if (kvp.Value == ID) return true;
	}
	return false;
}

ProcDictionary^ ProcDictionary::Instance::get()
{
	if (instance == nullptr) instance = gcnew ProcDictionary();
	return instance;
}