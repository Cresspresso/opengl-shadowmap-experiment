
#pragma once

namespace be
{
	// Interface
	class Game
	{
	public:
		virtual ~Game() noexcept = default;
	protected:
		Game() = default;
		Game(Game&&) noexcept = default;
		Game& operator=(Game&&) noexcept = default;
		Game(Game const&) = default;
		Game& operator=(Game const&) = default;

	private:
		// INTERFACE METHODS
		virtual void Update() {}
		virtual void Render() {}
		virtual void OnWindowSizeChanged(int width, int height) {}
		virtual void OnWindowPositionChanged(int x, int y) {}
		virtual void OnMousePositionInWindowChanged(int x, int y) {}
		virtual void OnKeyGoingDown(unsigned char key) {}
		virtual void OnKeyGoingUp(unsigned char key) {}
		virtual void OnSpecialGoingDown(int keycode) {}
		virtual void OnSpecialGoingUp(int keycode) {}
		virtual void OnMouseButtonGoingDown(int button) {}
		virtual void OnMouseButtonGoingUp(int button) {}
		virtual void OnMouseWheelPositive(int wheel) {}
		virtual void OnMouseWheelNegative(int wheel) {}
		virtual void OnMouseMoveWhileAnyDown() {}
		virtual void OnMouseMoveWhileAllUp() {}
		virtual void OnMouseEnteredWindow() {}
		virtual void OnMouseLeftWindow() {}

	public:
		void update() { Update(); }
		void render() { Render(); }
		void onWindowSizeChanged(int width, int height) { OnWindowSizeChanged(width, height); }
		void onWindowPositionChanged(int x, int y) { OnWindowPositionChanged(x, y); }
		void onMousePositionInWindowChanged(int x, int y) { OnMousePositionInWindowChanged(x, y); }
		void onKeyGoingDown(unsigned char key) { OnKeyGoingDown(key); }
		void onKeyGoingUp(unsigned char key) { OnKeyGoingUp(key); }
		void onSpecialGoingDown(int keycode) { OnSpecialGoingDown(keycode); }
		void onSpecialGoingUp(int keycode) { OnSpecialGoingUp(keycode); }
		void onMouseButtonGoingDown(int button) { OnMouseButtonGoingDown(button); }
		void onMouseButtonGoingUp(int button) { OnMouseButtonGoingUp(button); }
		void onMouseWheelPositive(int wheel) { OnMouseWheelPositive(wheel); }
		void onMouseWheelNegative(int wheel) { OnMouseWheelNegative(wheel); }
		void onMouseMoveWhileAnyDown() { OnMouseMoveWhileAnyDown(); }
		void onMouseMoveWhileAllUp() { OnMouseMoveWhileAllUp(); }
		void onMouseEnteredWindow() { OnMouseEnteredWindow(); }
		void onMouseLeftWindow() { OnMouseLeftWindow(); }
	};
}
