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
  enum Status {OPEN, 
               CLOSED} status;
  void ProcessInput();
  void Update();
  void Render();
 public:
  Engine();
  ~Engine();
  void Run();
};

Engine::Engine() {
	terminal_open();
	terminal_set("window: title='Rogue River: Obol of Charon', resizeable=true, minimum-size=80x24");
	width = terminal_state(TK_WIDTH);
    height = terminal_state(TK_HEIGHT);
    status = OPEN;
};

Engine::~Engine() {
	terminal_close();
};

void Engine::ProcessInput() {
	int key = terminal_read();

	if (key == TK_CLOSE || key == TK_ESCAPE) {
		status = CLOSED;
	}
};

void Engine::Render() {
	terminal_clear();
	for (int x=0; x<width; x++) {
		terminal_put(x, 0, x%2? symbol: (int)'#');
		terminal_put(x, height-1, x%2? symbol: (int)'#');
	}
	for (int y=0; y<height; y++) {
		terminal_put(0, y, y%2? symbol: (int)'#');
		terminal_put(width-1, y, y%2? symbol: (int)'#');
	}
	terminal_printf(3, 2, "Terminal size is %dx%d", width, height);
	terminal_refresh();
};

void Engine::Update() {
	width = terminal_state(TK_WIDTH);
	height = terminal_state(TK_HEIGHT);
};

void Engine::Run() {
	while (status == OPEN) {
        Update();
        Render();
        ProcessInput();
	}
};

int main()
{
   Engine engine;
   engine.Run();
	
   return 0;
}
