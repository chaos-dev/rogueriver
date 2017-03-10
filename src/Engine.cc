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

#include <iostream>

#include "Actor.h"
#include "Ai.h"
#include "Attacker.h"

#include "BearLibTerminal.h"

Engine::Engine() {
  terminal_open();
  // Terminal settings
  terminal_set("window: title='Rogue River: Obol of Charon', resizeable=true, size=132x43, minimum-size=80x24");
  terminal_set("font: graphics/VeraMono.ttf, size=8x16");
  terminal_set("tile font: graphics/Anikki_square_16x16.bmp, size=16x16, codepage=437, align=top-left");
  terminal_set("input.filter={keyboard, mouse+}, precise-mouse=true");
  terminal_bkcolor("black");

  // Initialize engine state
  width = terminal_state(TK_WIDTH);
  height = terminal_state(TK_HEIGHT);
  status = OPEN;
  mouse = new Position(terminal_state(TK_MOUSE_X), terminal_state(TK_MOUSE_Y));

  // Seed RNG
  rng.seed(std::random_device()());

  // Initialize members
  gui = new Gui(SIDEBAR_WIDTH);
  map = new Map(MAP_WIDTH, MAP_HEIGHT);
  map_panel.Update(0, 0, width-SIDEBAR_WIDTH, height);
  Position player_start = map->GetPlayerStart();
  camera = new Position(player_start.x, player_start.y);
  
  player = new Actor(player_start.x, player_start.y, (int)'@', "player");
  player->ai = new PlayerAi();
  player->destructible=new PlayerDestructible(50,5,"your cadaver");
  player->attacker = new Attacker(15,15,15);
  engine.actors.push_back(player);
};

Engine::~Engine() {
  terminal_close();
};

void Engine::ProcessInput() {
    while (terminal_has_input()) {
      int key = terminal_read();
      bool shift = terminal_check(TK_SHIFT);
      player->ProcessInput(key, shift);
      gui->ProcessInput(key);
      if (key == TK_CLOSE || key == TK_ESCAPE) {
        status = CLOSED;
      } else if (key == TK_MOUSE_MOVE) {
        mouse->x = terminal_state(TK_MOUSE_X)/2 + camera->x - map_panel.width/4;
        mouse->y = -terminal_state(TK_MOUSE_Y) + camera->y + map_panel.height/2;
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
  for (Actor* actor : actors) {
    RenderActor(actor);
  };
  
  // Gui
  terminal_layer(2);
  terminal_print(width-SIDEBAR_WIDTH+1, 1, "River: Acheron");
  terminal_printf(width-SIDEBAR_WIDTH+1, 3, "Player X: %d  Y: %d", player->x, player->y);
  terminal_printf(width-SIDEBAR_WIDTH+1, 5, "Cursor X: %d  Y: %d", mouse->x, mouse->y);
  terminal_printf(width-SIDEBAR_WIDTH+1, 7, "River Velocity Under Cursor:");
  terminal_printf(width-SIDEBAR_WIDTH+1, 8, "    [[%.1f, %.1f]] m/s",
                  map->GetUVelocity(mouse->x, mouse->y),
                  map->GetVVelocity(mouse->x, mouse->y));
  gui->Render();

  // Print out results
  terminal_refresh();

};

void Engine::RenderActor(Actor* actor) {
  int term_x = (actor->x - camera->x)*2 + map_panel.width/2;
  int term_y = -actor->y + camera->y + map_panel.height/2;
  if (term_x < map_panel.width-1 && term_y < map_panel.height) {
      terminal_color(actor->color.Convert());
      terminal_bkcolor(terminal_pick_color(term_x, term_y, 0));
      terminal_printf(term_x, term_y, "[font=tile]%c", (char*)actor->symbol);
      terminal_color(color_from_name("white"));
      terminal_bkcolor(color_from_name("black"));
  };
};

void Engine::Update() {
  game_status = IDLE;
  // Update the actors
  player->Update();
  if (game_status == NEW_TURN) {
    for (Actor* actor : actors) {
        if (actor != player) actor->Update();
    };
  };

  // Update the map
  width = terminal_state(TK_WIDTH);
  height = terminal_state(TK_HEIGHT);
  map_panel.Update(0, 0, width-SIDEBAR_WIDTH, height);

  // Update the gui
  gui->Update();
};

void Engine::Run() {
  while (status == OPEN) {
        Update();
        Render();
        ProcessInput();
  }
};
