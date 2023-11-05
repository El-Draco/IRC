#ifndef TUI
#define TUI

#include <curses.h>
#include <string>
class tui {
  public:
    tui();
    ~tui();
    void refresh();
    int readInput();
    void messageRecieved(std::string message);
    std::string message_buffer;
    void clearInput();
    void endWindow();

  private:
    int term_rows, term_cols;
    WINDOW *chatWindowOuter, *chatWindowInner, *inputWindowOuter,
        *inputWindowInner, *mainWindow;
};

#endif // !TUI
