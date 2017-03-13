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
#include "Menu.h"

#include "BearLibTerminal.h"

Engine::Engine() : status(OPEN), game_status(STARTUP), level(1), 
    player(nullptr), raft(nullptr), map(nullptr) {
  terminal_open();
  // Terminal settings
  terminal_set("window: title='Rogue River: Obol of Charon', resizeable=true, size=132x43, minimum-size=80x24");
  terminal_set("font: graphics/VeraMono.ttf, size=8x16");
  terminal_set("tile font: graphics/Anikki_square_16x16.bmp, codepage=437, size=16x16, align=top-left");
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
  level=1;
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
  player->words = new Words("you","You","your corpse","your","sling","robes");
  player->ai = new PlayerAi();
  player->destructible=new PlayerDestructible(20,3);
  player->attacker = new Attacker(15,16,3,12);
  engine.actors.push_back(player);
  
  // Create raft
  raft = new Actor(player_start.x, player_start.y-2, (int)'#', Color(129,76,42), 1);
  raft->words = new Words("raft","Raft","pile of logs"," "," ","thick wood");
  raft->destructible = new RaftDestructible(15,9);
  raft->blocks = false;
  engine.actors.push_front(raft);
  
  Actor* charon = new Actor(player_start.x-4, player_start.y-1, (int)'@', Color(240,230,140),1);
  charon->words = new Words("Charon","Charon"," "," "," "," ");
  engine.actors.push_back(charon);
  Actor* boatl = new Actor(player_start.x-5, player_start.y-1, (int)'{', Color(129,76,42),1);
  boatl->words = new Words("Charon's boat","Charon's boat"," "," "," "," ");
  engine.actors.push_back(boatl);
  Actor* boatr = new Actor(player_start.x-3, player_start.y-1, (int)'}', Color(129,76,42),1);
  boatr->words = new Words("Charon's boat","Charon's boat"," "," "," "," ");
  engine.actors.push_back(boatr);
  Actor* hermes = new Actor(player_start.x-2, player_start.y+2, (int)'@', Color(240,230,140),1);
  hermes->words = new Words("Hermes","Hermes"," "," "," "," ");
  engine.actors.push_back(hermes);
  
  engine.Update();
  engine.Render();
  engine.gui->MessageBox("Your loved one has been taken to the Underworld by Hades, and it is now up to you to save her! Hermes has shown you the way to Acheron, the river leading into the Underworld.");
  engine.gui->MessageBox("Hermes: That there is Charon, the ferryman of the underworld.");
  engine.gui->MessageBox("Charon: One coin will buy you passage down my river.");
  engine.gui->MessageBox("You: I... I don't have any coins on me.");
  engine.gui->MessageBox("Charon: Well, if you don't have a coin, then you're stuck here with these wandering ghosts. No obol, no passage.");
  engine.gui->MessageBox("Hermes: It looks like you'll have to find another way down the river...");
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
    gui->ProcessInput(key);
    if (key == TK_CLOSE) {
      status = CLOSED;
    } else if (key == TK_ESCAPE && game_status != AIMING) {
      engine.gui->menu.clear();
	    engine.gui->menu.addItem(Menu::RESUME,"Resume");
	    engine.gui->menu.addItem(Menu::NEW_GAME,"New game");
	    engine.gui->menu.addItem(Menu::EXIT,"Exit");
	    Menu::MenuItemCode menuItem=engine.gui->menu.pick(Menu::PAUSE);
      if ( menuItem == Menu::EXIT ) {
		    status = CLOSED;
	    } else if ( menuItem == Menu::NEW_GAME ) {
		    // New game
		    game_status = STARTUP;
		    engine.Term();
		    engine.Init();
	    }
    } else if (key == TK_MOUSE_MOVE) {
      UpdateMouse(); // This is actually redundant.
    }
    if (game_status == AIMING) {
      int x, y;
      engine.PickATile(key, &x, &y, player->attacker->max_range);
    } else if (game_status == IDLE || game_status == STARTUP) {
      player->ProcessInput(key, shift);
    }
  }
};

void Engine::UpdateMouse() {
  mouse->x = terminal_state(TK_MOUSE_X)/2 + camera->x - map_panel.width/4;
  mouse->y = -terminal_state(TK_MOUSE_Y) + camera->y + map_panel.height/2;
};

void Engine::Render() {
  terminal_clear();
  
  // Map
  terminal_layer(MAP);
  map->Render(map_panel, camera);
  terminal_crop(0,0,map_panel.width-1, map_panel.height);
  
  // Actors
  terminal_layer(ACTORS);
  for (Actor* actor : actors) {
    RenderActor(actor);
  };
  terminal_crop(0,0,map_panel.width-1, map_panel.height);
  
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
  if (game_status == NEW_TURN || game_status == STARTUP || game_status == IDLE) {
    game_status = IDLE;
  } 
  if (game_status == NEW_TURN || game_status == STARTUP || 
      game_status == IDLE     || game_status == AIMING) {
    player->Update();
    if (game_status == NEW_TURN) {
      UpdateMouse(); // Map may have moved...
      for (Actor* actor : actors) {
          if (actor != player) actor->Update();
      }
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

void Engine::Load(bool pause) {
  engine.gui->menu.clear();
	engine.gui->menu.addItem(Menu::NEW_GAME,"New game");
	engine.gui->menu.addItem(Menu::EXIT,"Exit");
	
	Menu::MenuItemCode menuItem=engine.gui->menu.pick(
	    pause ? Menu::PAUSE : Menu::MAIN);
  if ( menuItem == Menu::EXIT || menuItem == Menu::NONE ) {
		// Exit or window closed
		exit(0);
	} else if ( menuItem == Menu::NEW_GAME ) {
		// New game
	  engine.gui->menu.addItem(Menu::RESUME,"Resume");
		engine.Term();
		engine.Init();
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
  } else if (key == TK_SPACE || key == TK_ESCAPE) {
    engine.gui->log->Print("Firing canceled.");
    engine.game_status = IDLE;
  }
  return false;
}

void Engine::NextLevel() {
  level++;
  switch (level) {
    case 2:
      engine.gui->MessageBox("You: Up ahead are the cliffs leading down to the underworld.  I'll have to find a way around the waterfall.");
      engine.gui->log->Print("[color=amber]You carry your raft past the waterfall to the next section of the river.");
      break;
    case 3:
      engine.gui->MessageBox("You: Here it is! It's the cave leading down into the depths of the kingdom of Hades.");
      engine.gui->log->Print("[color=amber]You paddle ahead to where the river enters a dark cave.");
      break;
    case 4:
      engine.gui->MessageBox("You: That fork up ahead must be the one Hermes told me about.");
      engine.gui->log->Print("[color=amber]You race away from the harpies, entering a dark fork of the cave.");
      break;
    case 5:
      engine.gui->MessageBox("You: I've reached the last fork.  If I cut through here, I'll be right at the throne of Hades.");
      engine.gui->log->Print("[color=amber]You enter another fork of the cave, where the air is hot and the water hotter.");
      break;
   };
   
  if (level == 6) {
      engine.gui->MessageBox("Hades: Well done. I have quite enjoyed the show! You are indeed a mighty warrior. And you've managed to find some of the old relics I have been borrowing...");
      engine.gui->MessageBox("You: I'll fight you too, if need be!");
      engine.gui->MessageBox("Hades: There's no need for that.  You can take your place in the Elysian fields, among the other fallen warriors.");
      engine.gui->MessageBox("You: What do you mean? I'm here to take my love back to the surface.");
      engine.gui->MessageBox("Your love: My dear, I'm right here.");
      engine.gui->MessageBox("Hades: There seems to be a misunderstanding.  You see, you're already...");
      engine.gui->MessageBox("Your love: Our house burned down last night.  We both died in the fire.");
      engine.gui->MessageBox("You: So that means...I'm...");
      engine.gui->MessageBox("Hades: A powerful hero, who has earned his place among the champions of our time.");
      engine.gui->MessageBox("               THE END               ");
      engine.gui->menu.clear();
	    engine.gui->menu.addItem(Menu::NEW_GAME,"New game");
	    engine.gui->menu.addItem(Menu::EXIT,"Exit");
	    Menu::MenuItemCode menuItem=engine.gui->menu.pick(Menu::PAUSE);
      if ( menuItem == Menu::EXIT || menuItem == Menu::NONE ) {
		    status = CLOSED;
	    } else if ( menuItem == Menu::NEW_GAME ) {
		    // New game
		    game_status = STARTUP;
		    engine.Term();
		    engine.Init();
	    }
  } else {
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
  }
};
