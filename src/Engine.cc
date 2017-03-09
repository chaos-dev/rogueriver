/**
 *  \brief  
 *  Copyright (C) 2017  Chaos-Dev
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Engine.h"

#include "BearLibTerminal.h"

Engine::Engine() {
  terminal_open();
  // Terminal settings
  terminal_set("window: title='Rogue River: Obol of Charon', resizeable=true, minimum-size=80x24; font: graphics/VeraMono.ttf, size=16x16");
  terminal_bkcolor("black");

  // Initialize engine state
  width = terminal_state(TK_WIDTH);
    height = terminal_state(TK_HEIGHT);
    status = OPEN;

    // Initialize members
    map = new Map(MAP_WIDTH, MAP_HEIGHT);
  map_panel.Update(0, 0, width, height);
  camera = new Position(width/2, height/2);
    player = new Actor(width/2, height/2, (int)'@');
};

Engine::~Engine() {
  terminal_close();
};

void Engine::ProcessInput() {
    while (terminal_has_input()) {
      int key = terminal_read();

      if (key == TK_CLOSE || key == TK_ESCAPE) {
        status = CLOSED;
      } else if (key == TK_UP) {
          camera->y += 1; player->y += 1;
      } else if (key == TK_DOWN) {
          camera->y -= 1; player->y -= 1;
      } else if (key == TK_LEFT) {
          camera->x -= 1; player->x -= 1;
      } else if (key == TK_RIGHT) {
          camera->x += 1; player->x += 1;
      }
  }
};

void Engine::Render() {
  terminal_clear();
  map->Render(map_panel, camera);
  terminal_put(player->x - camera->x + map_panel.width/2,
               player->y - camera->y + map_panel.height/2, player->symbol);
  terminal_printf(map_panel.width/2-5, 0, "X: %d  Y: %d", player->x, player->y);
  terminal_refresh();
};

void Engine::Update() {
  width = terminal_state(TK_WIDTH);
  height = terminal_state(TK_HEIGHT);
  map_panel.Update(0, 0, width, height);
};

void Engine::Run() {
  while (status == OPEN) {
        Update();
        Render();
        ProcessInput();
  }
};
