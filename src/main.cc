/*
 * WindowResize.cpp
 *
 *  Created on: Jan 9, 2014
 *      Author: cfyz
 */

#include "BearLibTerminal.h"

class Engine {
 protected:
  int width, height;
  const int symbol = 0x2588;
 public:
  Engine();
  ~Engine();
  void Run();
};

Engine::Engine() {
	terminal_open();
	terminal_set("window: title='Rogue River: Obol of Charon', resizeable=true, minimum-size=27x5");
};

Engine::~Engine() {
	terminal_close();
};

void Engine::Run() {
	while (true)
	{
		terminal_clear();
		int w = terminal_state(TK_WIDTH);
		int h = terminal_state(TK_HEIGHT);
		for (int x=0; x<w; x++)
		{
			terminal_put(x, 0, x%2? symbol: (int)'#');
			terminal_put(x, h-1, x%2? symbol: (int)'#');
		}
		for (int y=0; y<h; y++)
		{
			terminal_put(0, y, y%2? symbol: (int)'#');
			terminal_put(w-1, y, y%2? symbol: (int)'#');
		}
		terminal_printf(3, 2, "Terminal size is %dx%d", w, h);
		terminal_refresh();

		int key = terminal_read();

		if (key == TK_CLOSE || key == TK_ESCAPE)
		{
			break;
		}
	}
};

int main()
{
   Engine engine;
   engine.Run();
	
   return 0;
}
