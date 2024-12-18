#include "Application.hpp"

int main(int argc, const char** argv)
{
	Application* application = new Application(1600, 900);

	application->Run();

	delete application;

	return 0;
}