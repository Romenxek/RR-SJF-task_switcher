#include <ctime>
#include "Process.h"
#include "Processes.h"
#pragma once

namespace Lab3OC 
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class MainForm : public System::Windows::Forms::Form
	{
		private: System::Windows::Forms::DataGridView^ dataGridView1;
		private: System::Windows::Forms::Button^ button1;
		private: System::Windows::Forms::Button^ button2;
		private: System::Windows::Forms::Label^ label1;
		private: System::Windows::Forms::Label^ label4;
		private: System::Windows::Forms::Button^ button3;
		private: System::Windows::Forms::Label^ label7;
		private: System::Windows::Forms::MaskedTextBox^ maskedTextBox1;
		private: System::Windows::Forms::MaskedTextBox^ maskedTextBox4;
		private: System::Windows::Forms::Button^ button4;
		private: System::Windows::Forms::Label^ label8;
		private: System::Windows::Forms::MaskedTextBox^ maskedTextBox6;
		private: System::Windows::Forms::MaskedTextBox^ maskedTextBox7;
		private: System::Windows::Forms::Label^ label9;
		private: System::Windows::Forms::Label^ label10;
		private: System::Windows::Forms::Label^ label15;


		private: System::Windows::Forms::Button^ button5;
		private: System::Windows::Forms::DataGridViewTextBoxColumn^ ID;
		private: System::Windows::Forms::DataGridViewTextBoxColumn^ Arrival_Time;
		private: System::Windows::Forms::DataGridViewTextBoxColumn^ Process_State;
		private: System::Windows::Forms::DataGridViewTextBoxColumn^ Execution_Time;
		private: System::Windows::Forms::DataGridViewTextBoxColumn^ ReadyTime;
		private: System::Windows::Forms::Button^ button6;
		private: System::Windows::Forms::MaskedTextBox^ maskedTextBox3;
		private: System::Windows::Forms::Label^ label2;



		private: System::Windows::Forms::MaskedTextBox^ maskedTextBox2;

		private: int systemTime;
		private: Processes^ processes = gcnew Processes();
		private: String^ currentAlgorithm;
		private: bool isChoosed = false;
		private: Timer^ timer = gcnew Timer();

		private: int cycleInQuant;
	private: System::Windows::Forms::Button^ button7;
	private: System::Windows::Forms::Label^ label3;
	private: int numOfQuants;

		private:
			void UpdateCellColor(DataGridViewRow^ row, Process^ processToUpdate)
			{
				if (processToUpdate->PState == Process::State::Waiting) { row->DefaultCellStyle->BackColor = System::Drawing::Color::LightGray; }
				else if (processToUpdate->PState == Process::State::Ready) { row->DefaultCellStyle->BackColor = System::Drawing::Color::Yellow; }
				else if (processToUpdate->PState == Process::State::Running) { row->DefaultCellStyle->BackColor = System::Drawing::Color::Lime; }
				else if (processToUpdate->PState == Process::State::Finished) { row->DefaultCellStyle->BackColor = System::Drawing::Color::DarkGray; }
			}
			void UpdateProcessesStates()
			{
				for each (Process ^ p in processes->ProcessesList)
				{
					Process::State oldState = p->PState;
					p->UpdateState(systemTime);
					Process::State newState = p->PState;
					if (oldState != newState) { processes->_needToUpdateInDGW->Add(p); }
				}
			}
			void AddRowForDataGridView(int ID, int arrivalTime,Process::State state, int executionTime, int readyTime)
			{
				dataGridView1->Rows->Add(ID, arrivalTime, state, executionTime, readyTime);
				dataGridView1->AllowUserToAddRows = false;
			}
			void UpdateDataGridView()
			{
				for each (Process ^ processToUpdate in processes->_needToUpdateInDGW)
				{
					for each (DataGridViewRow ^ row in dataGridView1->Rows)
					{
						if (Convert::ToInt32(row->Cells["ID"]->Value) == processToUpdate->ProcessID)
						{
							row->Cells["ID"]->Value = processToUpdate->ProcessID;
							row->Cells["Execution_Time"]->Value = processToUpdate->ExecutionTime;
							row->Cells["Arrival_Time"]->Value = processToUpdate->ArrivalTime;
							row->Cells["Process_State"]->Value = processToUpdate->PState;
							row->Cells["ReadyTime"]->Value = processToUpdate->ReadyTime;
							UpdateCellColor(row, processToUpdate);
						}
					}
				}
				processes->_needToUpdateInDGW->Clear();
			}
			void UpdateButtonColors()
			{
				if (currentAlgorithm == "RR")
				{
					button1->BackColor = System::Drawing::Color::ForestGreen;
					button2->BackColor = System::Drawing::Color::IndianRed;
					button5->BackColor = System::Drawing::Color::IndianRed;
				}
				else if (currentAlgorithm == "SJF")
				{
					button1->BackColor = System::Drawing::Color::IndianRed;
					button2->BackColor = System::Drawing::Color::ForestGreen;
					button5->BackColor = System::Drawing::Color::IndianRed;
				}
				else if (currentAlgorithm == "none")
				{
					button1->BackColor = System::Drawing::Color::IndianRed;
					button2->BackColor = System::Drawing::Color::IndianRed;
					button5->BackColor = System::Drawing::Color::ForestGreen;
				}
			}
			void UpdateCTButtonColor()
			{
				if (timer->Enabled){button7->BackColor = System::Drawing::Color::Lime;}
				else { button7->BackColor = System::Drawing::Color::DarkGray; }
			}
			void UpdateSTLabel()
			{
				label7->Text = "Текущий такт: " + systemTime.ToString() + " ";
			}
			void UpdateTimerLabel()
			{
				if (timer->Enabled) { label3->Text = "Таймер включен"; }
				else { label3->Text = "Таймер выключен"; }
			}
			void RoundRobin(Processes^ processes, int cycleInQuant, int numOfQuants){ // без простоя по квантам, излишек тактов - простой
				if (processes->FinishedProcessesList->Count == processes->ProcessesList->Count)
				{
					systemTime += cycleInQuant * numOfQuants;
					return;
				}
				while (numOfQuants != 0){ // пока все кванты не потрачены
					numOfQuants -= 1;
					processes->_needToUpdateInDGW->Add(processes->RunningProcess);
					systemTime += cycleInQuant;
					processes->AddReadyTime(cycleInQuant);
					if (cycleInQuant >= processes->RunningProcess->ExecutionTime){ // если квант больше оставшего времени исполнения
						processes->RunningProcess->ExecutionTime = 0;
						processes->RunningProcess->PState = Process::State::Finished;}
					else{ // если квант меньше оставшегося времени исполнения
						processes->RunningProcess->ExecutionTime -= cycleInQuant;}
					UpdateProcessesStates();// обновление статуса в функции
					processes->SetRunningProcess("RR");}}
			void SJF(Processes^ processes, int cycleInQuant, int numOfQuants)
			{
				if (processes->FinishedProcessesList->Count == processes->ProcessesList->Count)
				{
					systemTime += cycleInQuant * numOfQuants;
					return;
				}
				while (numOfQuants != 0) { // пока все кванты не потрачены
					numOfQuants -= 1;
					processes->_needToUpdateInDGW->Add(processes->RunningProcess);
					systemTime += cycleInQuant;
					processes->AddReadyTime(cycleInQuant);
					if (cycleInQuant >= processes->RunningProcess->ExecutionTime) { // если квант больше оставшего времени исполнения
						processes->RunningProcess->ExecutionTime = 0;
						processes->RunningProcess->PState = Process::State::Finished;
					}
					else { // если квант меньше оставшегося времени исполнения
						processes->RunningProcess->ExecutionTime -= cycleInQuant;
					}
					UpdateProcessesStates();// обновление статуса в функции
					processes->SetRunningProcess("SJF");
				}
			}
			void AutoLocate(String^ currentAlgorithm, Processes^ processes, int cycleInQuant, int numOfQuants)
			{
				timer->Start();
				UpdateTimerLabel();
				UpdateCTButtonColor();
			}
			void TimerTick(Object^ sender, EventArgs^ e)
			{
				if (numOfQuants <= 0 || processes->FinishedProcessesList->Count == processes->ProcessesList->Count) {
					timer->Stop();
					UpdateTimerLabel();
					UpdateCTButtonColor();
					return;
				}

				if (currentAlgorithm == "SJF") {
					SJF(processes, cycleInQuant, numOfQuants);
				}
				else if (currentAlgorithm == "RR") {
					RoundRobin(processes, cycleInQuant, numOfQuants);
				}
				else if (currentAlgorithm == "none")
				{
					systemTime += cycleInQuant;
				}

				UpdateProcessesStates();
				UpdateDataGridView();
				UpdateSTLabel();
			}
			void UpdateButton6Colors()
			{
				if (isChoosed == false)
				{
					button6->BackColor = System::Drawing::Color::DarkGray;
				}
				else { button6->BackColor = System::Drawing::Color::LimeGreen; }
			}
		public:
			MainForm(void)
			{
				currentAlgorithm = "none";
				systemTime = 0;
				timer->Tick += gcnew EventHandler(this, &MainForm::TimerTick);

				Process^ kernal = gcnew Process(0, 1000,  0);
				Process^ explorer = gcnew Process(1, 500, 10);
				Process^ abvg = gcnew Process(2, 400, 100);

				processes->AddProcess(kernal);
				processes->AddProcess(explorer);
				processes->AddProcess(abvg);
				InitializeComponent();
				UpdateTimerLabel();
				UpdateButtonColors();
				UpdateButton6Colors();
				UpdateCTButtonColor();
				for each (Process^ current in processes->ProcessesList) 
				{
					current->UpdateState(systemTime);
					AddRowForDataGridView(current->ProcessID, current->ArrivalTime, current->PState, current->ExecutionTime, current->ReadyTime);
				}

				for each (Process ^ process in processes->ProcessesList)
				{
					for each (DataGridViewRow ^ row in dataGridView1->Rows)
					{
						if (Convert::ToInt32(row->Cells["ID"]->Value) == process->ProcessID)
						{
							UpdateCellColor(row, process);
						}
					}
				}
			}

		protected:
			~MainForm()
			{
				if (components)
				{
					delete components;
				}
			}

		protected:


		private:
			System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code
		/// <summary>

		/// </summary>
		void InitializeComponent(void)
		{
			System::Windows::Forms::DataGridViewCellStyle^ dataGridViewCellStyle1 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			this->dataGridView1 = (gcnew System::Windows::Forms::DataGridView());
			this->ID = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Arrival_Time = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Process_State = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Execution_Time = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->ReadyTime = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->maskedTextBox1 = (gcnew System::Windows::Forms::MaskedTextBox());
			this->maskedTextBox4 = (gcnew System::Windows::Forms::MaskedTextBox());
			this->button4 = (gcnew System::Windows::Forms::Button());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->maskedTextBox6 = (gcnew System::Windows::Forms::MaskedTextBox());
			this->maskedTextBox7 = (gcnew System::Windows::Forms::MaskedTextBox());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->label15 = (gcnew System::Windows::Forms::Label());
			this->maskedTextBox2 = (gcnew System::Windows::Forms::MaskedTextBox());
			this->button5 = (gcnew System::Windows::Forms::Button());
			this->button6 = (gcnew System::Windows::Forms::Button());
			this->maskedTextBox3 = (gcnew System::Windows::Forms::MaskedTextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->button7 = (gcnew System::Windows::Forms::Button());
			this->label3 = (gcnew System::Windows::Forms::Label());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView1))->BeginInit();
			this->SuspendLayout();
			// 
			// dataGridView1
			// 
			this->dataGridView1->AllowUserToResizeColumns = false;
			this->dataGridView1->AllowUserToResizeRows = false;
			this->dataGridView1->BorderStyle = System::Windows::Forms::BorderStyle::None;
			dataGridViewCellStyle1->BackColor = System::Drawing::SystemColors::Control;
			dataGridViewCellStyle1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 7.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			dataGridViewCellStyle1->ForeColor = System::Drawing::SystemColors::WindowText;
			dataGridViewCellStyle1->SelectionBackColor = System::Drawing::SystemColors::Highlight;
			dataGridViewCellStyle1->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
			this->dataGridView1->ColumnHeadersDefaultCellStyle = dataGridViewCellStyle1;
			this->dataGridView1->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->dataGridView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(5) {
				this->ID, this->Arrival_Time,
					this->Process_State, this->Execution_Time, this->ReadyTime
			});
			this->dataGridView1->Location = System::Drawing::Point(48, 52);
			this->dataGridView1->Name = L"dataGridView1";
			this->dataGridView1->RowHeadersVisible = false;
			this->dataGridView1->RowHeadersWidth = 10;
			this->dataGridView1->RowHeadersWidthSizeMode = System::Windows::Forms::DataGridViewRowHeadersWidthSizeMode::DisableResizing;
			this->dataGridView1->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->dataGridView1->Size = System::Drawing::Size(1505, 504);
			this->dataGridView1->TabIndex = 0;
			// 
			// ID
			// 
			this->ID->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::None;
			this->ID->HeaderText = L"ID";
			this->ID->MinimumWidth = 6;
			this->ID->Name = L"ID";
			this->ID->Resizable = System::Windows::Forms::DataGridViewTriState::False;
			this->ID->Width = 50;
			// 
			// Arrival_Time
			// 
			this->Arrival_Time->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::AllCells;
			this->Arrival_Time->HeaderText = L"Время начала процесса";
			this->Arrival_Time->MinimumWidth = 6;
			this->Arrival_Time->Name = L"Arrival_Time";
			this->Arrival_Time->Resizable = System::Windows::Forms::DataGridViewTriState::False;
			this->Arrival_Time->Width = 193;
			// 
			// Process_State
			// 
			this->Process_State->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::AllCells;
			this->Process_State->HeaderText = L"Состояние процесса";
			this->Process_State->MinimumWidth = 6;
			this->Process_State->Name = L"Process_State";
			this->Process_State->Width = 171;
			// 
			// Execution_Time
			// 
			this->Execution_Time->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::AllCells;
			this->Execution_Time->HeaderText = L"Оставшееся время выполнения";
			this->Execution_Time->MinimumWidth = 6;
			this->Execution_Time->Name = L"Execution_Time";
			this->Execution_Time->Width = 241;
			// 
			// ReadyTime
			// 
			this->ReadyTime->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::ColumnHeader;
			this->ReadyTime->HeaderText = L"Время готовности";
			this->ReadyTime->MinimumWidth = 6;
			this->ReadyTime->Name = L"ReadyTime";
			this->ReadyTime->Width = 155;
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(48, 583);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(203, 88);
			this->button1->TabIndex = 1;
			this->button1->Text = L"RR";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &MainForm::button1_Click);
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(300, 583);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(203, 88);
			this->button2->TabIndex = 2;
			this->button2->Text = L"SJF (вытесняющий)";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &MainForm::button2_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(1606, 52);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(87, 16);
			this->label1->TabIndex = 3;
			this->label1->Text = L"ID Процесса";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(1606, 118);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(164, 16);
			this->label4->TabIndex = 6;
			this->label4->Text = L"Время начала процесса";
			// 
			// button3
			// 
			this->button3->Location = System::Drawing::Point(1609, 268);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(193, 59);
			this->button3->TabIndex = 8;
			this->button3->Text = L"Создать процесс";
			this->button3->UseVisualStyleBackColor = true;
			this->button3->Click += gcnew System::EventHandler(this, &MainForm::button3_Click);
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 19.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label7->Location = System::Drawing::Point(41, 872);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(438, 38);
			this->label7->TabIndex = 10;
			this->label7->Text = L"Текущее системное время:";
			// 
			// maskedTextBox1
			// 
			this->maskedTextBox1->Location = System::Drawing::Point(1609, 82);
			this->maskedTextBox1->Mask = L"00000";
			this->maskedTextBox1->Name = L"maskedTextBox1";
			this->maskedTextBox1->PromptChar = '.';
			this->maskedTextBox1->Size = System::Drawing::Size(100, 22);
			this->maskedTextBox1->TabIndex = 11;
			this->maskedTextBox1->ValidatingType = System::Int32::typeid;
			// 
			// maskedTextBox4
			// 
			this->maskedTextBox4->Location = System::Drawing::Point(1609, 148);
			this->maskedTextBox4->Mask = L"0000000000";
			this->maskedTextBox4->Name = L"maskedTextBox4";
			this->maskedTextBox4->PromptChar = '.';
			this->maskedTextBox4->Size = System::Drawing::Size(100, 22);
			this->maskedTextBox4->TabIndex = 14;
			this->maskedTextBox4->ValidatingType = System::Int32::typeid;
			// 
			// button4
			// 
			this->button4->Location = System::Drawing::Point(1105, 583);
			this->button4->Name = L"button4";
			this->button4->Size = System::Drawing::Size(309, 88);
			this->button4->TabIndex = 16;
			this->button4->Text = L"Выделить время";
			this->button4->UseVisualStyleBackColor = true;
			this->button4->Click += gcnew System::EventHandler(this, &MainForm::button4_Click);
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->Location = System::Drawing::Point(830, 583);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(197, 16);
			this->label8->TabIndex = 17;
			this->label8->Text = L"Сколько раз выделить время";
			// 
			// maskedTextBox6
			// 
			this->maskedTextBox6->Location = System::Drawing::Point(833, 619);
			this->maskedTextBox6->Mask = L"00000";
			this->maskedTextBox6->Name = L"maskedTextBox6";
			this->maskedTextBox6->Size = System::Drawing::Size(80, 22);
			this->maskedTextBox6->TabIndex = 18;
			this->maskedTextBox6->ValidatingType = System::Int32::typeid;
			this->maskedTextBox6->TextChanged += gcnew System::EventHandler(this, &MainForm::maskedTextBox6_TextChanged);
			// 
			// maskedTextBox7
			// 
			this->maskedTextBox7->Location = System::Drawing::Point(541, 619);
			this->maskedTextBox7->Mask = L"00000";
			this->maskedTextBox7->Name = L"maskedTextBox7";
			this->maskedTextBox7->Size = System::Drawing::Size(100, 22);
			this->maskedTextBox7->TabIndex = 19;
			this->maskedTextBox7->ValidatingType = System::Int32::typeid;
			this->maskedTextBox7->TextChanged += gcnew System::EventHandler(this, &MainForm::maskedTextBox7_TextChanged);
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Location = System::Drawing::Point(538, 583);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(239, 16);
			this->label9->TabIndex = 20;
			this->label9->Text = L"Тактов процессора в одном кванте";
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Location = System::Drawing::Point(45, 33);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(72, 16);
			this->label10->TabIndex = 24;
			this->label10->Text = L"Процессы";
			// 
			// label15
			// 
			this->label15->AutoSize = true;
			this->label15->Location = System::Drawing::Point(1606, 190);
			this->label15->Name = L"label15";
			this->label15->Size = System::Drawing::Size(196, 16);
			this->label15->TabIndex = 31;
			this->label15->Text = L"Время выполнения процесса";
			// 
			// maskedTextBox2
			// 
			this->maskedTextBox2->Location = System::Drawing::Point(1609, 220);
			this->maskedTextBox2->Mask = L"0000000000";
			this->maskedTextBox2->Name = L"maskedTextBox2";
			this->maskedTextBox2->PromptChar = '.';
			this->maskedTextBox2->Size = System::Drawing::Size(100, 22);
			this->maskedTextBox2->TabIndex = 32;
			this->maskedTextBox2->ValidatingType = System::Int32::typeid;
			// 
			// button5
			// 
			this->button5->Location = System::Drawing::Point(48, 701);
			this->button5->Name = L"button5";
			this->button5->Size = System::Drawing::Size(203, 88);
			this->button5->TabIndex = 33;
			this->button5->Text = L"Выделять время";
			this->button5->UseVisualStyleBackColor = true;
			this->button5->Click += gcnew System::EventHandler(this, &MainForm::button5_Click);
			// 
			// button6
			// 
			this->button6->Location = System::Drawing::Point(1105, 692);
			this->button6->Name = L"button6";
			this->button6->Size = System::Drawing::Size(309, 88);
			this->button6->TabIndex = 34;
			this->button6->Text = L"Выделять автоматически";
			this->button6->UseVisualStyleBackColor = true;
			this->button6->Click += gcnew System::EventHandler(this, &MainForm::button6_Click);
			// 
			// maskedTextBox3
			// 
			this->maskedTextBox3->Location = System::Drawing::Point(833, 725);
			this->maskedTextBox3->Mask = L"00000";
			this->maskedTextBox3->Name = L"maskedTextBox3";
			this->maskedTextBox3->Size = System::Drawing::Size(100, 22);
			this->maskedTextBox3->TabIndex = 35;
			this->maskedTextBox3->ValidatingType = System::Int32::typeid;
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(830, 696);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(230, 16);
			this->label2->TabIndex = 36;
			this->label2->Text = L"Время таймера (в миллисекундах)";
			// 
			// button7
			// 
			this->button7->Location = System::Drawing::Point(1461, 583);
			this->button7->Name = L"button7";
			this->button7->Size = System::Drawing::Size(309, 88);
			this->button7->TabIndex = 37;
			this->button7->Text = L"Отключить таймер";
			this->button7->UseVisualStyleBackColor = true;
			this->button7->Click += gcnew System::EventHandler(this, &MainForm::button7_Click);
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 19.8F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label3->Location = System::Drawing::Point(41, 932);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(142, 38);
			this->label3->TabIndex = 38;
			this->label3->Text = L"Таймер:";
			// 
			// MainForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1902, 1033);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->button7);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->maskedTextBox3);
			this->Controls->Add(this->button6);
			this->Controls->Add(this->button5);
			this->Controls->Add(this->maskedTextBox2);
			this->Controls->Add(this->label15);
			this->Controls->Add(this->label10);
			this->Controls->Add(this->label9);
			this->Controls->Add(this->maskedTextBox7);
			this->Controls->Add(this->maskedTextBox6);
			this->Controls->Add(this->label8);
			this->Controls->Add(this->button4);
			this->Controls->Add(this->maskedTextBox4);
			this->Controls->Add(this->maskedTextBox1);
			this->Controls->Add(this->label7);
			this->Controls->Add(this->button3);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->dataGridView1);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->Name = L"MainForm";
			this->Text = L"Планировщик";
			this->WindowState = System::Windows::Forms::FormWindowState::Maximized;
			this->Load += gcnew System::EventHandler(this, &MainForm::MainForm_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView1))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
		private: System::Void MainForm_Load(System::Object^ sender, System::EventArgs^ e) {}
		//RR
		private: System::Void button1_Click(System::Object^ sender, System::EventArgs^ e) 
		{
			currentAlgorithm = "RR";
			processes->SetRunningProcess("RR");

			UpdateDataGridView();
			UpdateButtonColors();
		}
		//sjf
		private: System::Void button2_Click(System::Object^ sender, System::EventArgs^ e) 
		{
			currentAlgorithm = "SJF";
			processes->SetRunningProcess("SJF");

			UpdateDataGridView();
			UpdateButtonColors();
		}
		//выделить время
		private: System::Void button4_Click(System::Object^ sender, System::EventArgs^ e) // Нажатие на Выделить время
		{
			if (timer->Enabled) { return; }
			if (Int32::TryParse(maskedTextBox7->Text, cycleInQuant))
			{
				if (cycleInQuant == 0) 
				{
					MessageBox::Show("Тактов в кванте времени не может быть 0", "Ошибка выделения времени",
						MessageBoxButtons::OK, MessageBoxIcon::Error);
					return;
				};
			}
			else
			{
				MessageBox::Show("Неверное число в выборе тактов.", "Ошибка выделения времени",
					MessageBoxButtons::OK, MessageBoxIcon::Error);
				return;
			}

			if (Int32::TryParse(maskedTextBox6->Text, numOfQuants))
			{
				if (numOfQuants == 0)
				{
					MessageBox::Show("Выделите больше 0 квантов", "Предупреждение",
						MessageBoxButtons::OK, MessageBoxIcon::Warning);
					return;
				};
			}
			else
			{
				MessageBox::Show("Неверное число квантов.", "Ошибка выделения времени", 
					MessageBoxButtons::OK, MessageBoxIcon::Error);
				return;
			}
			
			int timerPeriod;
			if (isChoosed)
			{
				if (Int32::TryParse(maskedTextBox3->Text, timerPeriod))
				{
					if (timerPeriod == 0)
					{
						MessageBox::Show("Выделите больше 0 миллисекунд", "Предупреждение",
							MessageBoxButtons::OK, MessageBoxIcon::Warning);
						return;
					};
				}
				else
				{
					MessageBox::Show("Неверное число времени.", "Ошибка выделения времени",
						MessageBoxButtons::OK, MessageBoxIcon::Error);
					return;
				}
				timer->Interval = timerPeriod;
			}

			if (isChoosed)
			{
				AutoLocate(currentAlgorithm, processes, cycleInQuant, numOfQuants);
			}
			else if (currentAlgorithm == "RR")
			{
				RoundRobin(processes, cycleInQuant, numOfQuants);
			}
			else if (currentAlgorithm == "SJF")
			{
				SJF(processes, cycleInQuant, numOfQuants);
			}
			else if (currentAlgorithm = "none")
			{
				systemTime += cycleInQuant * numOfQuants;
			}
			UpdateProcessesStates();
			UpdateDataGridView();
			UpdateSTLabel();
		}
		//создать процесс
		private: System::Void button3_Click(System::Object^ sender, System::EventArgs^ e) 
		{
			int ID;
			int arrivalTime;
			int executionTime;

			ID = Convert::ToInt32(maskedTextBox1->Text);
			arrivalTime = Convert::ToInt32(maskedTextBox2->Text);
			executionTime = Convert::ToInt32(maskedTextBox4->Text);
			
			Process^ newProcess = gcnew Process(ID, arrivalTime, executionTime);
			
			processes->AddProcess(newProcess);
			AddRowForDataGridView(newProcess->ProcessID, newProcess->ArrivalTime,newProcess->PState, newProcess->ExecutionTime, newProcess->ReadyTime);
			
			UpdateProcessesStates();
			UpdateDataGridView();
		}
		//пропускать время
		private: System::Void button5_Click(System::Object^ sender, System::EventArgs^ e) 
		{
			currentAlgorithm = "none";
			processes->SetRunningProcess("none");

			UpdateDataGridView();
			UpdateButtonColors();
		}
		// выделять автоматически
		private: System::Void button6_Click(System::Object^ sender, System::EventArgs^ e) 
		{
			isChoosed = !isChoosed;
			UpdateButton6Colors();
		}
		// остановить таймер
		private: System::Void button7_Click(System::Object^ sender, System::EventArgs^ e) 
		{
			if (timer->Enabled)
			{
				timer->Stop();
			}
			UpdateTimerLabel();
			UpdateCTButtonColor();
		}
		private: System::Void maskedTextBox6_TextChanged(System::Object^ sender, System::EventArgs^ e) 
		{
			if (timer->Enabled)
			{
				int recentNumOfQuants = numOfQuants;
				if (Int32::TryParse(maskedTextBox6->Text, numOfQuants))
				{
					if (numOfQuants == 0)
					{
						MessageBox::Show("Выделите больше 0 квантов.\nТаймер остановлен", "Предупреждение",
							MessageBoxButtons::OK, MessageBoxIcon::Warning);
						numOfQuants = recentNumOfQuants;
						timer->Stop();
						maskedTextBox6->Text = System::Convert::ToString(numOfQuants);
						UpdateTimerLabel();
						UpdateCTButtonColor();
						return;
					};
				}
				else
				{
					MessageBox::Show("Неверное число квантов.\nТаймер остановлен", "Ошибка выделения времени",
						MessageBoxButtons::OK, MessageBoxIcon::Error);
					numOfQuants = recentNumOfQuants;
					timer->Stop();
					maskedTextBox6->Text = System::Convert::ToString(numOfQuants);
					UpdateTimerLabel();
					UpdateCTButtonColor();
					return;
				};
			}
		}
		private: System::Void maskedTextBox7_TextChanged(System::Object^ sender, System::EventArgs^ e)
		{
			if (timer->Enabled)
			{
				int recentCycleInQuant = cycleInQuant;
				if (Int32::TryParse(maskedTextBox7->Text, cycleInQuant))
				{
					if (cycleInQuant == 0)
					{
						MessageBox::Show("Тактов в кванте времени не может быть 0.\nТаймер остановлен", "Ошибка выделения времени",
							MessageBoxButtons::OK, MessageBoxIcon::Error);
						cycleInQuant = recentCycleInQuant;
						timer->Stop();
						maskedTextBox7->Text = System::Convert::ToString(cycleInQuant);
						UpdateTimerLabel();
						UpdateCTButtonColor();
						return;
					};
				}
				else
				{
					MessageBox::Show("Неверное число в выборе тактов.\nТаймер остановлен", "Ошибка выделения времени",
						MessageBoxButtons::OK, MessageBoxIcon::Error);
					cycleInQuant = recentCycleInQuant;
					timer->Stop();
					maskedTextBox7->Text = System::Convert::ToString(cycleInQuant);
					UpdateTimerLabel();
					UpdateCTButtonColor();
					return;
				}
			}
		}
	};
}