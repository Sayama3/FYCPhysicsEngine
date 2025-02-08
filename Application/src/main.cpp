#include "Application.hpp"

int main(int argc, const char** argv)
{
	bool isEditing = false;
	for (int i = 0; i < argc; ++i) {
		std::string_view arg = argv[i];
		if (arg.find("--edit") != std::string_view::npos) isEditing = true;
		if (arg.find("-e") != std::string_view::npos) isEditing = true;
	}

	Application* application = new Application(1600, 900, "Application", isEditing);

	application->Run();

	delete application;

	return 0;
}