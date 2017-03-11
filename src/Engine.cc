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

Engine::Engine() : status(OPEN), game_status(STARTUP), level(1), 
    player(nullptr), raft(nullptr), map(nullptr) {
  terminal_open();
  // Terminal settings
  terminal_set("window: title='Rogue River: Obol of Charon', resizeable=true, size=132x43, minimum-size=80x24");
  terminal_set("font: graphics/VeraMono.ttf, size=8x16");
  terminal_set("tile font: graphics/Anikki_square_16x16.bmp, size=16x16, codepage=437, align=top-left");
  terminal_set("input.filter={keyboard, mouse+}, precise-mouse=true");
  terminal_composition(TK_ON);
  terminal_bkcolor("black");

  // Initialize engine state
  width = terminal_state(TK_WIDTH);
  height = terminal_state(TK_HEIGHT);
  status = OPEN;
  mouse = new Position(terminal_state(TK_MOUSE_X), terminal_state(TK_MOUSE_Y));

  gui = new Gui(SIDEBAR_WIDTH);
};

Engine::~Engine() {
  Term();
  if (gui) delete gui;
  terminal_close();
};

void Engine::Init() {
  // Seed RNG
  rng.seed(std::random_device()());

  // Initialize members
  map = new Map(MAP_WIDTH, MAP_HEIGHT);
  map->Init(true);
  map_panel.Update(0, 0, width-SIDEBAR_WIDTH, height);
  Position player_start = map->GetPlayerStart();
  camera = new Position(player_start.x, player_start.y);
  
  // Create player
  player = new Actor(player_start.x, player_start.y, (int)'@', Color(240,240,240), 1);
  player->words = new Words("you","You","your corpse","your","spear","robes");
  player->ai = new PlayerAi();
  player->destructible=new PlayerDestructible(45,9);
  player->attacker = new Attacker(15,16,19,32);
  engine.actors.push_back(player);
  
  // Create raft
  raft = new Actor(player_start.x, player_start.y-2, (int)'#', Color(129,76,42), 1);
  raft->words = new Words("raft","Raft","pile of logs"," "," ","thick wood");
  raft->destructible = new RaftDestructible(45,9);
  raft->blocks = false;
  engine.actors.push_front(raft);
};

void Engine::Term() {
    actors.clear();
    if (map) delete map;
    if (camera) delete camera;
    gui->Clear();
}

void Engine::ProcessInput() {
    while (terminal_has_input()) {
      int key = terminal_read();
      bool shift = terminal_check(TK_SHIFT);
      if (game_status == AIMING) {
        int x, y;
        engine.PickATile(key, &x, &y, player->attacker->max_range);
      } else {
        player->ProcessInput(key, shift);
      }
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
  if (game_status != AIMING) {
      game_status = IDLE;
  }
  player->Update();
  if (game_status == NEW_TURN) {
    for (Actor* actor : actors) {
        if (actor != player) actor->Update();
    }
  }

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

bool Engine::CursorOnMap() {
    if (terminal_state(TK_MOUSE_X) < map_panel.width-1) return true;
    return false;
};

bool Engine::PickATile(int key, int *x, int *y, int max_range) {

  if (key == TK_MOUSE_LEFT && CursorOnMap()) {
    float distance = player->GetDistance(mouse->x, mouse->y);
    if (distance < max_range) {
      for (Actor* actor : actors) {
        if (actor == player) continue;
        if (actor->x == mouse->x && actor->y == mouse->y &&
            actor->destructible && !actor->destructible->isDead()) {
          player->attacker->SetAim(actor);
          return true;
        };   
      }
    } else {
      engine.gui->log->Print("Your max range is %d m.\nThat space is %.1f m away.",
                             max_range, distance);
    }
  } else if (key == TK_SPACE) {
    engine.gui->log->Print("Firing canceled.");
    engine.game_status = IDLE;
  }
  return false;
}

void Engine::NextLevel() {
  level++;
  engine.gui->log->Print("[color=yellow]You carry your raft past the waterfall to the next section of the river.");
  delete map;
  
  // delete all actors but the player and the raft
  for (unsigned int i=0; i<actors.size(); i++) {
    if (actors[i] != player && actors[i] != raft) {
      actors.erase(actors.begin()+i);
      i--;
    };
  };
  
  // create a new map
  map = new Map(MAP_WIDTH, MAP_HEIGHT);
  map->Init(true);
  map_panel.Update(0, 0, width-SIDEBAR_WIDTH, height);
  Position player_start = map->GetPlayerStart();
  player->x = player_start.x; player->y = player_start.y-1;
  raft->x = player_start.x; raft->y = player_start.y - 2;
  camera->x = player_start.x; camera->y = player_start.y-1;
};
