#include <cliext/vector>
#include "MainForm.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace Lab3OC;

[STAThread]
void main()
{
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);
    Application::Run(gcnew MainForm());
}
