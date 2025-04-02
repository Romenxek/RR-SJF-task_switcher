#include "Process.h"

Process::Process(int processID, int executionTime, int arrivalTime)
{
    ProcessID = processID;
    ArrivalTime = arrivalTime;
    ExecutionTime = executionTime;
    _isRunning = true;
    _state = PState;
    ReadyTime = 0;
    PState = Process::State::Waiting;
    //SetRunningTime
}

int Process::ProcessID::get() 
{
    return _processID;
}

void Process::ProcessID::set(int ID)
{
    _processID = ID;
}

int Process::ExecutionTime::get()
{
    return _executionTime;
}

void Process::ExecutionTime::set(int executionTime)
{
    _executionTime = executionTime;
}

int Process::ArrivalTime::get()
{
    return _arrivalTime;
}

void Process::ArrivalTime::set(int arrivalTime)
{
    _arrivalTime = arrivalTime;
}

Process::State Process::PState::get()
{
    return _state;
}

void Process::PState::set(State newState)
{
    _state = newState;
}

int Process::ReadyTime::get()
{
    return _readyTime;
}

void Process::ReadyTime::set(int Time)
{
    if (this->PState == Process::State::Ready) { _readyTime = Time; }
    else { _readyTime = 0; };
}

void Process::UpdateState(int systemTime) // ќбновление состо€ни€ с ожидани€ на готовность
{
    if (this->ArrivalTime > systemTime)
    {
        if (this->PState == State::None) { this->PState = State::Waiting; }
        else if (this->PState == State::Waiting) { this->PState = State::Waiting; }
        else if (this->PState == State::Ready) { this->PState = State::Waiting; }
        else if (this->PState == State::Finished) { this->PState = State::Finished; }
    }
    else if (this->ArrivalTime <= systemTime)
    {
        if (this->PState == State::None) { this->PState = State::Ready; }
        else if (this->PState == State::Waiting) { this->PState = State::Ready; }
        else if (this->PState == State::Ready) { this->PState = State::Ready; }
        else if (this->PState == State::Finished) { this->PState = State::Finished; }
    }
}
