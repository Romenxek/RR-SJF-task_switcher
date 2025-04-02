#ifndef PROCESS_H
#define PROCESS_H

using namespace System;
using namespace System::Windows::Forms;
public ref class Process
{
public:
    enum class State { None, Waiting, Ready, Running, Finished };
private:
    int _processID;
    int _arrivalTime;
    int _executionTime;
    bool _isRunning;
    State _state;
    int _readyTime;
    int _runningTime;
public:
    Process(int processID, int executionTime, int arrivalTime);
    property int ProcessID
    {
        int get();
        void set(int ID);
    }
    property int ExecutionTime
    {
        int get();
        void set(int executionTime);
    }
    property int ArrivalTime
    {
        int get();
        void set(int arrivalTime);
    }
    property State PState
    {
        State get();
        void set(State newState);
    }
    property int ReadyTime
    {
        int get();
        void set(int readyTime);
    }
    void UpdateState(int systemTime);
};

#endif // PROCESS_H