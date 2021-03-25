
#include <glew/glew.h>
#include <freeglut/freeglut.h>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>

#include "be/application.hpp"

namespace be
{
	RunException::RunException(char const* what) : std::runtime_error(what) {}
	RunException::RunException(std::string const& what) : std::runtime_error(what) {}

	RunAgainException::RunAgainException(char const* what) : RunException(what) {}
	RunAgainException::RunAgainException(std::string const& what) : RunException(what) {}

	RunInfoException::RunInfoException(char const* what) : RunException(what) {}
	RunInfoException::RunInfoException(std::string const& what) : RunException(what) {}

	CreateGameException::CreateGameException(char const* what) : RunException(what) {}
	CreateGameException::CreateGameException(std::string const& what) : RunException(what) {}

	GlewInitException::GlewInitException(char const* what) : RunException(what) {}
	GlewInitException::GlewInitException(std::string const& what) : RunException(what) {}



	namespace Application
	{
		namespace detail
		{
			enum class ClosingState
			{
				BeforeStart,
				Running,
				WantingToClose,
				Closing,
				Finished
			};



			static std::mutex s_closingMutex;
			static std::atomic<detail::ClosingState> s_closing{ detail::ClosingState::BeforeStart };
			static Logger* s_logger = nullptr;
			static std::unique_ptr<Game> s_game = nullptr;



			static Game* getGame() noexcept;
			static Logger* getLogger() noexcept;
			static void logException() noexcept;

			static BOOL WINAPI onConsoleClose(DWORD ctrl);

			static void close() noexcept;

			static void update() noexcept;
			static void render() noexcept;
			static void onReshape(int width, int height) noexcept;
			static void onKeyGoingDown(unsigned char key, int x, int y) noexcept;
			static void onKeyGoingUp(unsigned char key, int x, int y) noexcept;
			static void onSpecialGoingDown(int key, int x, int y) noexcept;
			static void onSpecialGoingUp(int key, int x, int y) noexcept;
			static void onMouseButton(int button, int state, int x, int y) noexcept;
			static void onMouseMoveWhileAllUp(int x, int y) noexcept;
			static void onMouseMoveWhileAnyDown(int x, int y) noexcept;
			static void onMouseWheel(int wheel, int direction, int x, int y) noexcept;
			static void onMouseEntry(int state) noexcept;
			static void onPosition(int x, int y) noexcept;

			static void init(ApplicationRunInfo const& info);
			static void run(ApplicationRunInfo const& info) noexcept;

			static bool wantsToExit() noexcept;
			static void exit() noexcept;
		}
	}



	Game* Application::detail::getGame() noexcept
	{
		return s_game.get();
	}

	Logger* Application::detail::getLogger() noexcept
	{
		return s_logger;
	}

	void Application::detail::logException() noexcept
	{
		if (s_logger) { s_logger->logException(); }
	}



	BOOL WINAPI Application::detail::onConsoleClose(DWORD ctrl)
	{
		try
		{
			switch (ctrl)
			{
			case CTRL_CLOSE_EVENT:
			{
				{
					std::scoped_lock<std::mutex> _{ s_closingMutex };
					auto const closing = s_closing.load();
					if (detail::ClosingState::Closing == closing
						|| detail::ClosingState::Finished == closing)
					{
						break;
					}

					s_closing.store(detail::ClosingState::WantingToClose);
				}

				// wait until s_closing is Finished or the timer expires.
				constexpr long long maxWaitMilliseconds = 3'000;
				constexpr long long interval = 100;
				constexpr std::chrono::milliseconds ms{ interval };

				for (long long i = maxWaitMilliseconds / interval; i > 0; --i)
				{
					if (detail::ClosingState::Finished == s_closing.load())
					{
						break;
					}
					std::this_thread::sleep_for(ms);
				}

				// should have finished closing by now.
				//assert(detail::ClosingState::Finished == s_closing.load());
				break;
			}
			default:
				break;
			}
			return FALSE;
		}
		catch (...)
		{
			logException();
			std::terminate();
		}
	}



	void Application::detail::close() noexcept
	{
		try
		{
			bool cleaning = false;

			{
				std::scoped_lock<std::mutex> _{ s_closingMutex };
				auto const closing = s_closing.load();
				if (detail::ClosingState::BeforeStart == closing
					|| detail::ClosingState::Running == closing
					|| detail::ClosingState::WantingToClose == closing)
				{
					cleaning = true;
					s_closing.store(detail::ClosingState::Closing);
				}
			}

			if (cleaning)
			{
				s_game.reset();

				{
					std::scoped_lock<std::mutex> _{ s_closingMutex };
					s_closing.store(detail::ClosingState::Finished);
				}
			}
		}
		catch (...)
		{
			logException();
			std::terminate();
		}
	}



	void Application::detail::update() noexcept
	{
		try
		{
			if (wantsToExit())
			{
				exit();
				return;
			}

			auto& app = *getGame();
			try { app.update(); }
			catch (...) { logException(); }

			glutPostRedisplay();
		}
		catch (...)
		{
			logException();
			std::terminate();
		}
	}



	void Application::detail::render() noexcept
	{
		try
		{
			auto& app = *getGame();
			try { app.render(); }
			catch (...) { logException(); }

			glutSwapBuffers();
		}
		catch (...)
		{
			logException();
			std::terminate();
		}
	}



	void Application::detail::onReshape(int width, int height) noexcept
	{
		try
		{
			glViewport(0, 0, width, height);

			auto& app = *getGame();
			try { app.onWindowSizeChanged(width, height); }
			catch (...) { logException(); }
		}
		catch (...) { logException(); std::terminate(); }
	}



	void Application::detail::onKeyGoingDown(unsigned char key, int x, int y) noexcept
	{
		try
		{
			auto& app = *getGame();
			try { app.onMousePositionInWindowChanged(x, y); }
			catch (...) { logException(); }
			try { app.onKeyGoingDown(key); }
			catch (...) { logException(); }
		}
		catch (...) { logException(); std::terminate(); }
	}
	void Application::detail::onKeyGoingUp(unsigned char key, int x, int y) noexcept
	{
		try
		{
			auto& app = *getGame();
			try { app.onMousePositionInWindowChanged(x, y); }
			catch (...) { logException(); }
			try { app.onKeyGoingUp(key); }
			catch (...) { logException(); }
		}
		catch (...) { logException(); std::terminate(); }
	}
	void Application::detail::onSpecialGoingDown(int key, int x, int y) noexcept
	{
		try
		{
			auto& app = *getGame();
			try { app.onMousePositionInWindowChanged(x, y); }
			catch (...) { logException(); }
			try { app.onSpecialGoingDown(key); }
			catch (...) { logException(); }
		}
		catch (...) { logException(); std::terminate(); }
	}
	void Application::detail::onSpecialGoingUp(int key, int x, int y) noexcept
	{
		try
		{
			auto& app = *getGame();
			try { app.onMousePositionInWindowChanged(x, y); }
			catch (...) { logException(); }
			try { app.onSpecialGoingUp(key); }
			catch (...) { logException(); }
		}
		catch (...) { logException(); std::terminate(); }
	}
	void Application::detail::onMouseButton(int button, int state, int x, int y) noexcept
	{
		try
		{
			auto& app = *getGame();

			try { app.onMousePositionInWindowChanged(x, y); }
			catch (...) { logException(); }

			switch (state)
			{
			case GLUT_DOWN:
			{
				try { app.onMouseButtonGoingDown(button); }
				catch (...) { logException(); }
			}
			break;
			case GLUT_UP:
			{
				try { app.onMouseButtonGoingUp(button); }
				catch (...) { logException(); }
			}
			break;
			}
		}
		catch (...)
		{
			logException();
			std::terminate();
		}
	}
	void Application::detail::onMouseMoveWhileAllUp(int x, int y) noexcept
	{
		try
		{
			auto& app = *getGame();
			try { app.onMousePositionInWindowChanged(x, y); }
			catch (...) { logException(); }
			try { app.onMouseMoveWhileAllUp(); }
			catch (...) { logException(); }
		}
		catch (...) { logException(); std::terminate(); }
	}
	void Application::detail::onMouseMoveWhileAnyDown(int x, int y) noexcept
	{
		try
		{
			auto& app = *getGame();
			try { app.onMousePositionInWindowChanged(x, y); }
			catch (...) { logException(); }
			try { app.onMouseMoveWhileAnyDown(); }
			catch (...) { logException(); }
		}
		catch (...) { logException(); std::terminate(); }
	}
	void Application::detail::onMouseWheel(int wheel, int direction, int x, int y) noexcept
	{
		try
		{
			auto& app = *getGame();
			try { app.onMousePositionInWindowChanged(x, y); }
			catch (...) { logException(); }
			if (direction < 0)
			{
				try { app.onMouseWheelNegative(wheel); }
				catch (...) { logException(); }
			}
			else
			{
				try { app.onMouseWheelPositive(wheel); }
				catch (...) { logException(); }
			}
		}
		catch (...) { logException(); std::terminate(); }
	}
	void Application::detail::onMouseEntry(int state) noexcept
	{
		try
		{
			auto& app = *getGame();
			switch (state)
			{
			case GLUT_LEFT:
			{
				try { app.onMouseLeftWindow(); }
				catch (...) { logException(); }
			}
			break;
			case GLUT_ENTERED:
			{
				try { app.onMouseEnteredWindow(); }
				catch (...) { logException(); }
			}
			break;
			}
		}
		catch (...) { logException(); std::terminate(); }
	}
	void Application::detail::onPosition(int x, int y) noexcept
	{
		try
		{
			auto& app = *getGame();
			try { app.onWindowPositionChanged(x, y); }
			catch (...) { logException(); }
		}
		catch (...) { logException(); std::terminate(); }
	}



	void Application::detail::init(ApplicationRunInfo const& info)
	{
		glutInit(info.argc, info.argv);
		glutInitDisplayMode(
			GLUT_DOUBLE
			| GLUT_RGBA
			| GLUT_DEPTH
			| GLUT_STENCIL
			//| GLUT_MULTISAMPLE
		);

		glutInitWindowPosition(
			glutGet(GLUT_SCREEN_WIDTH) / 2 - info.windowWidth / 2,
			glutGet(GLUT_SCREEN_HEIGHT) / 2 - info.windowHeight / 2
		);
		glutInitWindowSize(info.windowWidth, info.windowHeight);
		glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
		glutCreateWindow(info.windowTitle); // title

		if (0 != glewInit()) { throw GlewInitException(); }

		// callbacks
		SetConsoleCtrlHandler(onConsoleClose, TRUE);

		glutCloseFunc(close);

		glutIdleFunc(update);
		glutDisplayFunc(render);
		glutReshapeFunc(onReshape);

		glutKeyboardFunc(onKeyGoingDown);
		glutKeyboardUpFunc(onKeyGoingUp);
		glutSpecialFunc(onSpecialGoingDown);
		glutSpecialUpFunc(onSpecialGoingUp);
		glutMouseFunc(onMouseButton);
		glutPassiveMotionFunc(onMouseMoveWhileAllUp);
		glutMotionFunc(onMouseMoveWhileAnyDown);
		glutMouseWheelFunc(onMouseWheel);
		glutEntryFunc(onMouseEntry);
		glutPositionFunc(onPosition);

		// initial GL properties
		glFrontFace(GL_CCW);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		//glutSetOption(GLUT_MULTISAMPLE, 8); // anti aliasing
		//glEnable(GL_MULTISAMPLE); // anti aliasing
	}



	void Application::detail::run(ApplicationRunInfo const& info) noexcept
	{
		s_logger = info.logger;

		try
		{
			if (info.argc == nullptr
				|| info.argv == nullptr
				|| info.windowWidth <= 0
				|| info.windowHeight <= 0
				|| info.windowTitle == nullptr
				|| info.createGame == nullptr)
			{
				throw RunInfoException();
			}

			if (ClosingState::BeforeStart != s_closing.load())
			{
				throw RunAgainException();
			}

			init(info);

			s_game = info.createGame();
			if (s_game == nullptr)
			{
				throw CreateGameException();
			}

			glutMainLoop();
			close();
		}
		catch (...)
		{
			logException();
		}

		s_logger = nullptr;
	}



	bool Application::detail::wantsToExit() noexcept
	{
		return ClosingState::WantingToClose == s_closing.load();
	}

	void Application::detail::exit() noexcept
	{
		glutLeaveMainLoop();
	}



	void Application::run(ApplicationRunInfo const& info) noexcept
	{
		detail::run(info);
	}

	void Application::exit() noexcept
	{
		detail::exit();
	}

	Game* Application::getGame() noexcept
	{
		return detail::getGame();
	}

	Logger* Application::getLogger() noexcept
	{
		return detail::getLogger();
	}

	void Application::logException() noexcept
	{
		detail::logException();
	}
}
