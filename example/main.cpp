
//#include <vld.h>
#include <be/application.hpp>

#include "game.hpp"

int main(int argc, char** argv)
{
	using namespace example;
	be::DefaultLogger logger;
	be::ApplicationRunInfo info = {};
	info.logger = &logger;
	info.argc = &argc;
	info.argv = argv;
	info.createGame = [] { return std::make_unique<Game>(); };
	info.windowTitle = "be example";
	be::Application::run(info);
}
