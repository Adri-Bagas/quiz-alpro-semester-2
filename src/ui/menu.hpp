#pragma once

#include <ncurses.h>
#include <ncurses/menu.h>
#include <string>
#include <vector>

class MainMenuUI {
  public:
    MainMenuUI(const std::vector<std::string> &items, const std::string_view app_name,
               const std::string_view app_version);
    ~MainMenuUI();

    std::string run();

  private:
    std::vector<std::string> choices;
    std::string selected_text;

    std::string_view app_name, app_version;

    WINDOW *header_win, *menu_win, *main_win, *menu_sub;
    MENU *my_menu;
    ITEM **my_items;

    int header_h;
    int menu_w;
    int req_rows;
    int req_cols;

    chtype ls, rs, ts, bs, tl, tr, bl, br;

    void setup_ncurses();
    void setup_menu();
    void create_ui();
    void destroy_ui();
    void draw_all();
};
