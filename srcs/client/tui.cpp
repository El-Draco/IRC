#include "../../inc/client/tui.hpp"
#include <curses.h>
#include <ncurses.h>
#include <string>

tui::tui() {
  mainWindow = initscr();
  keypad(mainWindow, TRUE);
  raw();
  noecho();
  cbreak();
  start_color();
  use_default_colors();
  init_pair(1, -1, -1);
  init_pair(2, COLOR_CYAN, -1);
  init_pair(3, COLOR_RED, -1);
  init_pair(4, COLOR_GREEN, -1);
  getmaxyx(stdscr, term_rows, term_cols);
  chatWindowOuter = subwin(mainWindow, term_rows - 6, term_cols, 0, 0);
  inputWindowOuter = subwin(mainWindow, 6, term_cols, term_rows - 6, 0);

  box(inputWindowOuter, 0, 0);
  box(chatWindowOuter, 0, 0);

  chatWindowInner = subwin(chatWindowOuter, term_rows - 8, term_cols - 2, 1, 1);
  inputWindowInner =
      subwin(inputWindowOuter, 4, term_cols - 2, term_rows - 5, 1);
  scrollok(chatWindowInner, TRUE);
  scrollok(inputWindowInner, TRUE);
  // wcursyncup(inputWindowInner);
  wmove(inputWindowInner, 0, 0);
  wclear(inputWindowInner);

  wrefresh(mainWindow);
}

void tui::refresh() {
  wrefresh(chatWindowInner);
  wrefresh(inputWindowInner);
}

void tui::messageRecieved(std::string message) {
  wrefresh(chatWindowInner);
  size_t userSep = message.find(":");
  if (userSep == std::string::npos) {
    return;
  }
  std::string user = message.substr(0, userSep);
  std::string theMessage = message.substr(userSep + 1);

  if (user == "SERVER") {
    wattron(chatWindowInner, COLOR_PAIR(4));
  } else if (user == "ERROR") {
    wattron(chatWindowInner, COLOR_PAIR(3));
  } else
    wattron(chatWindowInner, COLOR_PAIR(2));
  wprintw(chatWindowInner, "[%s] ", user.c_str());
  if (user == "SERVER") {
    wattroff(chatWindowInner, COLOR_PAIR(4));
  } else if (user == "ERROR") {
    wattroff(chatWindowInner, COLOR_PAIR(3));
  } else
    wattroff(chatWindowInner, COLOR_PAIR(2));

  wprintw(chatWindowInner, "%s\n", theMessage.c_str());
  wrefresh(chatWindowInner);
  wrefresh(inputWindowInner);
};

int tui::readInput() {
  int ch = getch();
  if (ch == 8 || ch == 127 || ch == KEY_LEFT || ch == KEY_BACKSPACE) {
    if (!message_buffer.empty())
      message_buffer.pop_back();
    wprintw(inputWindowInner, "\b \b");
  } else if (ch != '\n') {
    wprintw(inputWindowInner, "%c", char(ch));
    message_buffer.push_back(char(ch));
  } else if (ch == '\n') {
    return 1;
  }

  wrefresh(mainWindow);
  wrefresh(inputWindowInner);
  wrefresh(chatWindowInner);
  wrefresh(inputWindowInner);
  return 0;
}

void tui::clearInput() {
  message_buffer.clear();
  wclear(inputWindowOuter);
  wrefresh(inputWindowInner);

  wmove(inputWindowInner, 0, 0);
  box(inputWindowOuter, 0, 0);

  wrefresh(inputWindowInner);
}

tui::~tui() {
  curs_set(1);
  endwin();
}
