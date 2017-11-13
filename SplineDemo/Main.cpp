
#include "CApplication.h"

using namespace ion;


int main()
{
	Log::AddDefaultOutputs();

	SingletonPointer<CApplication> Application;
	Application->Run();
	return 0;
}
