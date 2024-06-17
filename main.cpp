#include <cstring>
#include <fstream>
#include <ncurses.h>
#include <string>
#include <vector>

#define LINE_NUMBERS_WIDTH 8
#define WORKSPACE_PADDING 1
#define STATUS_BAR_WIDTH 1

using std::string;
using std::vector;


const string mode_names[] = {"Normal", "Insert", "Command", "Visual"};

enum Mode {
  Normal,
  Insert,
  Command,
  Visual,
};

struct State {
  WINDOW*        line_numbers_window;
  WINDOW*        workspace_window;
  WINDOW*        status_bar_window;
  Mode           current_mode;
  vector<string> buffer;
  unsigned int   selected_line = 0;
  unsigned int   selected_column = 0;
  unsigned int   max_row = 0;
  unsigned int   max_col = 0;
};


void init_curses();
void draw_status_bar(State& state);
void draw_line_numbers(State& state);
void draw_workspace(State& state);
void process_keyboard_events(State& state);
void load_file(State& state);
unsigned int count_line_length(State& state);
unsigned int count_lines(State& state);


int main(int argc, char** argv) {
  setlocale(LC_ALL, "");

  State state;

  std::ifstream f;
  f.open(argv[1]);
  if (f.is_open()) {
    string line;

    while (std::getline(f, line)) {
      state.buffer.push_back(line);
    }

    f.close();
  }

  init_curses();

  getmaxyx(stdscr, state.max_row, state.max_col);

  state.line_numbers_window = newwin(state.max_row, LINE_NUMBERS_WIDTH, 0, 0);
  state.workspace_window    = newwin(state.max_row, (state.max_col - LINE_NUMBERS_WIDTH), 0, (LINE_NUMBERS_WIDTH + WORKSPACE_PADDING));
  state.status_bar_window   = newwin(STATUS_BAR_WIDTH, (state.max_col - LINE_NUMBERS_WIDTH), state.max_row - 1, (LINE_NUMBERS_WIDTH + WORKSPACE_PADDING));
  state.current_mode        = Mode::Normal;

  refresh();

  draw_line_numbers(state);
  draw_workspace(state);
  draw_status_bar(state);

  move(0, LINE_NUMBERS_WIDTH + WORKSPACE_PADDING);
  refresh();


  while(true) {
    getmaxyx(stdscr, state.max_row, state.max_col);

    process_keyboard_events(state);
  }

  endwin();
}


void init_curses() {
  initscr();
  start_color();
  keypad(stdscr, TRUE);
  noecho();
}

void draw_status_bar(State& state) {
  mvwprintw(state.status_bar_window, 0, 0, mode_names[state.current_mode].c_str());
  wrefresh(state.status_bar_window);
}

void draw_line_numbers(State& state) {
  for (int i = 0; i < state.buffer.size(); i++) {
    mvwprintw(state.line_numbers_window, i, LINE_NUMBERS_WIDTH - 4, "%d", i+1);
    mvwprintw(state.line_numbers_window, i, LINE_NUMBERS_WIDTH - 1, "|");
  }

  wrefresh(state.line_numbers_window);
}

void draw_workspace(State& state) {
  for (int i = 0; i < state.buffer.size(); i++) {
    mvwprintw(state.workspace_window, i, 0, state.buffer[i].c_str());
  }
  wmove(state.workspace_window, state.selected_line, state.selected_column);

  wrefresh(state.workspace_window);
}

void process_keyboard_events(State& state) {
  int k = getch();

  // Text navigation
  if (k == KEY_RIGHT) {
    if (state.selected_column < count_line_length(state)) {
      wmove(state.workspace_window, state.selected_line, ++state.selected_column);
      draw_workspace(state);
    }
  }

  if (k == KEY_LEFT) {
    if (state.selected_column > 0) {
      wmove(state.workspace_window, state.selected_line, --state.selected_column);
      draw_workspace(state);
    }
  }

  if (k == KEY_UP) {
    if (state.selected_line > 0) {
      wmove(state.workspace_window, --state.selected_line, state.selected_column);
      draw_workspace(state);
    }
  }

  if (k == KEY_DOWN) {
    if (state.selected_line < count_lines(state)) {
      wmove(state.workspace_window, ++state.selected_line, state.selected_column);
      draw_workspace(state);
    }
  }

  // Switch modes
  // TODO: Add Escape button to return into Normal mode
  if (k == 'a' && state.current_mode == Mode::Normal) {
    state.current_mode = Mode::Insert;
    draw_status_bar(state);
    move(state.selected_line, state.selected_column + LINE_NUMBERS_WIDTH + WORKSPACE_PADDING);
  }

  if (state.current_mode == Mode::Insert) {
    echo();
  }
  else {
    noecho();
  }

  // Input
  // TODO: Limit character deleting inside workspace window
  // Fix character deleting
  if (k == KEY_BACKSPACE && state.current_mode == Mode::Insert) {
    mvdelch(state.selected_line, --state.selected_column);
  }
}

unsigned int count_line_length(State &state) {
  return state.buffer[state.selected_line].size();
}

unsigned int count_lines(State &state) {
  return state.buffer.size() - 1;
}
