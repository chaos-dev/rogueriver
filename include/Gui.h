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
 
#ifndef INCLUDE_GUI_H_
#define INCLUDE_GUI_H_

#include <string>
#include <vector>
#include <cmath>
#include <sstream>

#include "Color.h"
#include "Menu.h"

// A string plus its precalculated height.
struct Message {
	Message() : height(0) { };

	Message(const std::string& text) : text(text), height(0) { };

	std::string text;
	int height;
};

class Log {
 private:
  const int sidebar_width;
  const int padding_left = 1;
  const int padding_right = 1;
  const int padding_top = 17;
  const int padding_bottom = 1;
  const int mouse_scroll_step = 2; // 2 text rows per mouse wheel step.
  const int line_padding = 0;
  int sidebar_start;
  int scrollbar_column;
  int scrollbar_offset;

  std::vector<Message> messages;
  int frame_offset = 0;
  int frame_width;
  int frame_height = 0;
  int total_messages_height = 1;
  int scrollbar_height = 0;
  bool dragging_scrollbar = false;
  int dragging_scrollbar_offset = 0;
  void Reset();
  int UpdateHeights();
  void UpdateGeometry();
  void ScrollToPixel(int py);
  int duplicate_count;

 public:
  Log(int sidebar_width);
  void Print(const char* message, ...);
  void Print(const std::string& message);
  void ProcessInput(int key);
  void Update();
  void Render();
  void Clear();
};

class Gui {
 private:
  const int sidebar_width;
  void RenderBar(int x, int y, int width, int offset, const char *name,
		         float value, float maxValue, const Color barColor,
		         const Color backColor);
  void RenderMouseLook(int x, int y);
  void RenderHelp(int x, int y);
  const char* GetTitle();
 public:
  Log* log;
  Menu menu;
  Gui(int sidebar_width);
  void ProcessInput(int key);
  void Update();
  void Render();
  void Clear();
  void DrawFrame(int x, int y, int width, int height);
};

#endif // INCLUDE_GUI_H
