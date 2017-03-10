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

#include "BearLibTerminal.h"

Log::Log(int sidebar_width) : sidebar_width(sidebar_width) {
  Reset();
  const std::string prompt =
      "Use arrow keys or mouse wheel to scroll the list up and down. "
      "Try to resize the window.\n--- --- ---";
  messages.push_back(Message(prompt));
  Print("%d goblins appear!",2);
  UpdateGeometry();
};

void Log::Print(const char* message, ...) {
  // build the text
  va_list ap;
  char buf[128];
  va_start(ap,message);
  vsprintf(buf,message,ap);
  va_end(ap);
  
  const std::string str(buf);
  messages.push_back(Message(str));
}

void Log::Print(const std::string& message) {
  messages.push_back(Message(message));
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
    first_line += message.height + 1;
  }
  int delta = first_line - frame_offset;

  // Drawing messages (+crop)
  terminal_layer(5);
  for (; index < messages.size() && delta <= frame_height; index++){
    auto& message = messages[index];
    terminal_print_ext(sidebar_start+padding_left, padding_top+delta, 
                       frame_width, 0, TK_ALIGN_DEFAULT, message.text.c_str());
    delta += message.height + 1;
  }
  terminal_crop(sidebar_start+padding_left, padding_top,
                frame_width, frame_height);

  // Scroll bar
  terminal_layer(6);
  terminal_bkcolor("darker gray");
  terminal_clear_area(sidebar_start+padding_left+frame_width, padding_top,
                      1, frame_height);
  terminal_bkcolor("none");
  terminal_color("dark orange");
  for (int i = 0; i < scrollbar_height; i++) {
    terminal_put_ext(scrollbar_column, i, 0, scrollbar_offset, 0x2588, 0);
  }

  // Put the colors back to their defaults.
  terminal_color("white");
  terminal_bkcolor("black");
};

void Log::Reset() {
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
	for (auto& message: messages)
	{
		message.height = terminal_measure_ext(frame_width, 0, message.text.c_str()).height;
		total_height += message.height;
	}

	// Add blank lines between messages
	total_height += messages.size()-1;

	return total_height;
}

void Log::UpdateGeometry() {
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
  frame_offset = total_messages_height * current_offset_percentage;
  frame_offset = std::min(frame_offset, total_messages_height - frame_height);
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

void Gui::ProcessInput(int key) {
  log->ProcessInput(key);
}

void Gui::Render() {
  log->Render();
}
