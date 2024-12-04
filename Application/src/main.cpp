#include "Application.hpp"

int main(int argc, const char** argv)
{
	Application* application = new Application();

	application->Run();

	delete application;

	return 0;
}