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

#include "Gui.h"

#include <stdarg.h>
#include <algorithm>
#include <iostream>
#include <string.h>

#include "Engine.h"
#include "BearLibTerminal.h"

Log::Log(int sidebar_width) : sidebar_width(sidebar_width), duplicate_count(1) {
  Clear();
  const std::string prompt =
      "Midway upon the journey of your life, you find yourself within a forest dark...";
  messages.push_back(Message(prompt));
  Print("----------------------------------");
  UpdateGeometry();
};

void Log::Print(const char* message, ...) {
  // build the text
  va_list ap;
  char buf[128];
  va_start(ap,message);
  vsprintf(buf,message,ap);
  va_end(ap);
  
  std::string str(buf);
  
  // Compare this message to the last one
  const std::string last_msg = messages.back().text;
  if (str.compare(0,str.size(),last_msg,0,str.size()) == 0) {
    duplicate_count++;
    messages.pop_back();
    str += " [[x" + std::to_string(duplicate_count) + "]]";
  } else {
    duplicate_count = 1;
  }
  
  messages.push_back(Message(str));
  UpdateHeights();
  UpdateGeometry();
}

void Log::Print(const std::string& message) {
  messages.push_back(Message(message));
  UpdateHeights();
  UpdateGeometry();
}

void Log::ProcessInput(int key) {
  if (key == TK_MOUSE_LEFT && terminal_state(TK_MOUSE_X) == scrollbar_column) {
    int py = terminal_state(TK_MOUSE_PIXEL_Y);
    if (py >= scrollbar_offset && py <= scrollbar_offset +
        (scrollbar_height * terminal_state(TK_CELL_HEIGHT))) {
      // Clicked on the scrollbar handle: start dragging
      dragging_scrollbar = true;
      dragging_scrollbar_offset = py - scrollbar_offset;
    } else {
      // Clicked outside of the handle: jump to position
      ScrollToPixel(terminal_state(TK_MOUSE_PIXEL_Y) - scrollbar_height * terminal_state(TK_CELL_HEIGHT) / 2);
    }
  } else if (key == (TK_MOUSE_LEFT|TK_KEY_RELEASED)) {
    dragging_scrollbar = false;
  } else if (key == TK_MOUSE_MOVE) {
    if (dragging_scrollbar)
      ScrollToPixel(terminal_state(TK_MOUSE_PIXEL_Y) - dragging_scrollbar_offset);

    while (terminal_peek() == TK_MOUSE_MOVE)
      terminal_read();
  } else if (key == TK_MOUSE_SCROLL) {
	// Mouse wheel scroll
	frame_offset += mouse_scroll_step * terminal_state(TK_MOUSE_WHEEL);
	frame_offset = std::max(0, std::min(total_messages_height-frame_height, frame_offset));
  } else if (key == TK_RESIZED) {
    UpdateGeometry();
  }

}

void Log::Render() {
  // Frame background
  terminal_layer(0);
  terminal_bkcolor("darkest gray");
  terminal_clear_area(sidebar_start+padding_left, padding_top,
                      frame_width, frame_height);
  terminal_bkcolor("none");

  // Find topmost visible message
  int index = 0, first_line = 0;
  for (; first_line < total_messages_height; index++) {
    auto& message = messages[index];
    if (first_line + message.height >= frame_offset) {
      // This message is partially visible
      break;
    }
    first_line += message.height + line_padding;
  }
  int delta = first_line - frame_offset;

  // Drawing messages (+crop)
  terminal_layer(5);
  for (; index < messages.size() && delta <= frame_height; index++){
    auto& message = messages[index];
    terminal_print_ext(sidebar_start+padding_left, padding_top+delta, 
                       frame_width, 0, TK_ALIGN_DEFAULT, message.text.c_str());
    delta += message.height+line_padding;
  }
  terminal_crop(sidebar_start+padding_left, padding_top,
                frame_width, frame_height);

  // Scroll bar
  terminal_layer(0);
  terminal_bkcolor("darker gray");
  terminal_clear_area(sidebar_start+padding_left+frame_width, padding_top,
                      1, frame_height);
  terminal_layer(6);
  terminal_bkcolor("none");
  terminal_color("dark orange");
  for (int i = 0; i < scrollbar_height; i++) {
    terminal_put_ext(scrollbar_column, i, 0, scrollbar_offset, 0x2588, 0);
  }

  // Put the colors back to their defaults.
  terminal_color("white");
  terminal_bkcolor("black");
};

void Log::Clear() {
  messages.clear();
  frame_offset = 0;
  dragging_scrollbar = false;
}

void Log::Update() {
  scrollbar_column = sidebar_start + frame_width + padding_left;
  scrollbar_offset =
      (padding_top + (frame_height-scrollbar_height) * 
      (frame_offset / (float)(total_messages_height - frame_height))) *
      terminal_state(TK_CELL_HEIGHT);
}

int Log::UpdateHeights() {
	int total_height = 0;
	for (auto& message: messages) {
		message.height = terminal_measure_ext(frame_width, 0, message.text.c_str()).height;
		total_height += message.height;
	}
	
	// Add spaces between lines
	total_height += (messages.size()-1)*line_padding;

	return total_height;
}

void Log::UpdateGeometry() {
  bool at_bottom = ((frame_offset + frame_height) == total_messages_height);
  if (total_messages_height <= frame_height) at_bottom = true;

  // Save current scroll position
  float current_offset_percentage = frame_offset / (float)total_messages_height;

  // Update frame dimensions
  frame_width = sidebar_width - (padding_left + padding_right + 1);
  sidebar_start = terminal_state(TK_WIDTH) - sidebar_width;
  frame_height = terminal_state(TK_HEIGHT) - (padding_top + padding_bottom);

  // Calculate new message list height
  total_messages_height = UpdateHeights();

  // Scrollbar
  scrollbar_height = std::min<int>(std::ceil(frame_height * (frame_height/(float)total_messages_height)), frame_height);

  // Try to recover scroll position
  if (at_bottom) {
    frame_offset = total_messages_height - frame_height;
  } else {
      frame_offset = total_messages_height * current_offset_percentage;
      frame_offset = std::min(frame_offset, total_messages_height - frame_height);
  }
  if (total_messages_height <= frame_height) frame_offset = 0;
}

void Log::ScrollToPixel(int py) {
  py -= padding_top * terminal_state(TK_CELL_HEIGHT);
  float factor = py / ((float)frame_height * terminal_state(TK_CELL_HEIGHT));
  frame_offset = total_messages_height * factor;
  frame_offset = std::max(0, std::min(total_messages_height-frame_height, frame_offset));
}

Gui::Gui(int sidebar_width) : sidebar_width(sidebar_width) {
  log = new Log(sidebar_width);
};

void Gui::Update() {
  log->Update();
};

void Gui::Clear() {
  log->Clear();
};

void Gui::ProcessInput(int key) {
  log->ProcessInput(key);
}

const char* Gui::GetTitle() {
  switch (engine.level) {
    case 1:
      return "[color=dark orange]Acheron: River of Pain";
      break;
    case 2:
      return "[color=dark orange]Cocytus: River of Wailing";
    case 3:
      return "[color=dark orange]Lethe: River of Forgetfulness";
    case 4:
      return "[color=dark orange]Styx: River of Hatred";
    case 5:
      return "[color=dark orange]Phlegethon: River of Fire";
      break;
    default:
      return "ERROR: TITLE NOT FOUND";
  };
};

void Gui::Render() {
  int sidebar_start = terminal_state(TK_WIDTH) - sidebar_width;
  
  terminal_layer(0);
  terminal_bkcolor("darkest gray");
  terminal_clear_area(sidebar_start+1,1,sidebar_width-2,11);
  terminal_layer(2);
  const char* title = GetTitle();
  terminal_print_ext(sidebar_start+1,1, sidebar_width-4, 0, TK_ALIGN_CENTER,
                     title);

  // Help tip
  RenderHelp(sidebar_start+2,3);

  RenderMouseLook(sidebar_start+2, 7);
  
  // health bar
  RenderBar(sidebar_start+1, 13, sidebar_width-2, 7, "Health",
            engine.player->destructible->hp,
            engine.player->destructible->maxHp,Color(136,13,3),Color(106,7,3));
            
  // raft integrity
  RenderBar(sidebar_start+1, 15, sidebar_width-2, 12, "Raft Integrity:",
            engine.raft->destructible->hp,
            engine.raft->destructible->maxHp,Color(129,76,42),Color(73,39,14));
  
  log->Render();
  
  terminal_bkcolor("black");
}

void Gui::RenderBar(int x, int y, int width, int offset, const char *name,
		            float value, float maxValue, const Color barColor,
		            const Color backColor) {
  int block_symbol = 0x2588;
  // Fill in the background.
  terminal_layer(0);
  terminal_bkcolor(backColor.Convert());
  terminal_clear_area(x,y,width,1);
  terminal_bkcolor("none");
  
  // Fill in the bar
  terminal_layer(6);
  int bar_width = (int)(value / maxValue * width);
  if (bar_width > 0) {
    terminal_color(barColor.Convert());
    for (int i=0; i<bar_width; i++) terminal_put(x+i, y, block_symbol);
  }
  
  // Print the text on top of the bar
  terminal_layer(7);
  terminal_color("white");
  terminal_printf(x+width/2-offset, y, "%s : %g/%g", name, value, maxValue);
  
  // Put the colors back to their defaults.
  terminal_bkcolor("black");
}

void Gui::RenderMouseLook(int x, int y) {
  if (engine.CursorOnMap()) {
    char buf[128]=" ";
    bool first=true;
    for (Actor* actor : engine.actors) {
      if (actor->x == engine.mouse->x && actor->y == engine.mouse->y) {
        if (first) {
          first = false;
        } else {
          strcat(buf, ", ");
        };
        strcat(buf, actor->words->name);
      };
    };
    terminal_printf(x, y, "Cursor X: %d  Y: %d", engine.mouse->x, engine.mouse->y);
    terminal_printf(x, y+1, "Under cursor:");
    
    // Check the terrain
  if (engine.map->isWater(engine.mouse->x, engine.mouse->y)) {
      terminal_printf(x, y+2, " river with speed: [[%4.1f, %4.1f]] m/s",
                      engine.map->GetUVelocity(engine.mouse->x, engine.mouse->y),
                      engine.map->GetVVelocity(engine.mouse->x, engine.mouse->y));
  } else if (engine.map->isBeach(engine.mouse->x, engine.mouse->y)) {
    if (engine.level <= 2) {
      terminal_printf(x, y+2, " sand");
    } else {
      terminal_printf(x, y+2, " gravel");
    }
  } else {
    if (engine.level <= 2) {
      terminal_printf(x, y+2, " grass");
    } else {
      terminal_printf(x, y+2, " rock");
    }
  }

// Print the actors
terminal_print_ext(x, y+3, sidebar_width-4, 0, TK_ALIGN_DEFAULT, buf);


}
  
};

void Gui::RenderHelp(int x, int y) {

  if (engine.game_status == Engine::AIMING) {
    terminal_color("yellow");
    terminal_print_ext(x, y, sidebar_width-4, 0, TK_ALIGN_DEFAULT, 
                       "Click any square to aim, or press spacebar to cancel.");
    if (engine.CursorOnMap()) {
        terminal_printf(x,y+2,"That space is %.0f m away.\nYour max range is %d.",
                        engine.player->GetDistance(engine.mouse->x, engine.mouse->y),
                        engine.player->attacker->max_range);
    }
    terminal_color("white");
  } else {
    terminal_print_ext(x, y, sidebar_width-4, 0, TK_ALIGN_DEFAULT, 
                       "Press the arrow/numpad/vi keys to move, or press 'f' to fire.");
  };
};

void Gui::DrawFrame(int x, int y, int width, int height) {
  int MAX_LAYER=10;
  for (int i=0; i<MAX_LAYER+1;i++) {
    terminal_layer(i);
    terminal_bkcolor("black");
    terminal_clear_area(x,y,width,height);
    terminal_bkcolor("darkest gray");
    terminal_clear_area(x+1,y+1,width-2,height-2);
  };
};





