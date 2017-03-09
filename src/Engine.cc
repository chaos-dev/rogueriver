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
  terminal_set("window: title='Rogue River: Obol of Charon', resizeable=true, minimum-size=80x24");
  terminal_set("font: graphics/VeraMono.ttf, size=8x16");
  //terminal_set("tile font: graphics/Anikki_square_16x16.bmp, size=16x16, align=top-left");
  terminal_bkcolor("black");

  // Initialize engine state
  width = terminal_state(TK_WIDTH);
  height = terminal_state(TK_HEIGHT);
  status = OPEN;

  // Initialize members
  map = new Map(MAP_WIDTH, MAP_HEIGHT);
  map_panel.Update(0, 0, width-SIDEBAR_WIDTH, height);
  camera = new Position(width/2, height/2);
  player = new Actor(width/2, height/2, (int)"@");
};

Engine::~Engine() {
  terminal_close();
};

void Engine::ProcessInput() {
    while (terminal_has_input()) {
      int key = terminal_read();

      if (key == TK_CLOSE || key == TK_ESCAPE) {
        status = CLOSED;
      } else if (key == TK_UP || key == TK_KP_8 
                 || (key == TK_K && !terminal_check(TK_SHIFT))) {
          camera->y += 1; player->y += 1;
      } else if (key == TK_DOWN || key == TK_KP_2 
                 || (key == TK_J && !terminal_check(TK_SHIFT))) {
          camera->y -= 1; player->y -= 1;
      } else if (key == TK_LEFT || key == TK_KP_4 
                 || (key == TK_H && !terminal_check(TK_SHIFT))) {
          camera->x -= 1; player->x -= 1;
      } else if (key == TK_RIGHT || key == TK_KP_6 
                 || (key == TK_L && !terminal_check(TK_SHIFT))) {
          camera->x += 1; player->x += 1;
      } else if (key == TK_KP_1 || (key == TK_B && !terminal_check(TK_SHIFT))){
          camera->x -= 1; player->x -= 1;
          camera->y -= 1; player->y -= 1;
      } else if (key == TK_KP_3 || (key == TK_N && !terminal_check(TK_SHIFT))){
          camera->x += 1; player->x += 1;
          camera->y -= 1; player->y -= 1;
      } else if (key == TK_KP_7 || (key == TK_Y && !terminal_check(TK_SHIFT))){
          camera->x -= 1; player->x -= 1;
          camera->y += 1; player->y += 1;
      } else if (key == TK_KP_9 || (key == TK_U && !terminal_check(TK_SHIFT))){
          camera->x += 1; player->x += 1;
          camera->y += 1; player->y += 1;
      }
  }
};

void Engine::Render() {
  terminal_clear();
  
  // Map
  terminal_layer(0);
  map->Render(map_panel, camera);
  
  // Actors
  terminal_layer(1);
  RenderActor(player);
  
  // Gui
  terminal_layer(2);
  terminal_print(width-SIDEBAR_WIDTH+1, 1, "River: Acheron");
  terminal_printf(width-SIDEBAR_WIDTH+1, 3, "X: %d  Y: %d", player->x, player->y);
  terminal_refresh();
};

void Engine::RenderActor(Actor* actor) {
  const char* symbol = (char*)actor->symbol;
  int term_x = (actor->x - camera->x)*2 + map_panel.width/2;
  int term_y = actor->y - camera->y + map_panel.height/2;
  terminal_bkcolor(terminal_pick_color(term_x, term_y, 0));
  terminal_printf(term_x, term_y, "%s", symbol);
  terminal_bkcolor(color_from_name("black"));
};

void Engine::Update() {
  width = terminal_state(TK_WIDTH);
  height = terminal_state(TK_HEIGHT);
  map_panel.Update(0, 0, width-SIDEBAR_WIDTH, height);
};

void Engine::Run() {
  while (status == OPEN) {
        Update();
        Render();
        ProcessInput();
  }
};
