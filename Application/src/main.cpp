#include "Application.hpp"

int main(const char* argv[], int argc)
{
	Application* application = new Application();

	application->Run();

	delete application;

	return 0;
}