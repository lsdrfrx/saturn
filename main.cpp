#include <ncurses.h>

int main() {
  initscr();
  start_color();
  curs_set(0);
  noecho();

  int max_row, max_col;

  getmaxyx(stdscr, max_row, max_col);

  move(max_row / 2, max_col / 2);

  printw("HELLO, SATURN!");

  getch();

  endwin();
}
