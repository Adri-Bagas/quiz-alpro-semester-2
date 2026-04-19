#include "menu.hpp"
#include "./views/view.hpp"
#include "theme.hpp"
#include <ncurses.h>

MainMenuUI::MainMenuUI(const std::vector<std::string> &items, const std::string_view app_name,
                       const std::string_view app_version)
    : choices(items), header_h(3), menu_w(25), selected_text("") {

    this->app_name = app_name;
    this->app_version = app_version;

    ls = BorderTheme::ls;
    rs = BorderTheme::rs;
    ts = BorderTheme::ts;
    bs = BorderTheme::bs;
    tl = BorderTheme::tl;
    tr = BorderTheme::tr;
    bl = BorderTheme::bl;
    br = BorderTheme::br;

    setup_ncurses();
    setup_menu();
    create_ui();
    draw_all();
}

MainMenuUI::~MainMenuUI() {
    destroy_ui();
    free_menu(my_menu);
    for (size_t i = 0; i < choices.size(); ++i) {
        free_item(my_items[i]);
    }
    delete[] my_items;
    endwin();
}

void MainMenuUI::setup_ncurses() {
    initscr();
    set_escdelay(25);
    cbreak();
    noecho();
    curs_set(0);
}

void MainMenuUI::setup_menu() {
    my_items = new ITEM *[choices.size() + 1];
    for (size_t i = 0; i < choices.size(); ++i) {
        my_items[i] = new_item(choices[i].c_str(), nullptr);
    }
    my_items[choices.size()] = nullptr;

    my_menu = new_menu(my_items);
    set_menu_format(my_menu, choices.size(), 1);
    set_menu_mark(my_menu, "> ");
    scale_menu(my_menu, &req_rows, &req_cols);
}

void MainMenuUI::create_ui() {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    header_win = newwin(header_h, max_x, 0, 0);
    menu_win = newwin(max_y - header_h, menu_w, header_h, 0);
    main_win = newwin(max_y - header_h, max_x - menu_w, header_h, menu_w);

    menu_sub = derwin(menu_win, req_rows, req_cols, 3, 2);

    set_menu_win(my_menu, menu_win);
    set_menu_sub(my_menu, menu_sub);
    keypad(menu_win, TRUE);
}

void MainMenuUI::destroy_ui() {
    unpost_menu(my_menu);
    delwin(menu_sub);
    delwin(header_win);
    delwin(menu_win);
    delwin(main_win);
}

void MainMenuUI::draw_all() {
    wborder(header_win, ls, rs, ts, bs, tl, tr, bl, br);
    mvwprintw(header_win, 1, 2, "%s - v%s", app_name.data(), app_version.data());
    wrefresh(header_win);

    wborder(main_win, ls, rs, ts, bs, tl, tr, bl, br);
    if (!selected_text.empty()) {
        mvwprintw(main_win, 2, 2, "Selected: %s", selected_text.c_str());
    }
    wrefresh(main_win);

    wborder(menu_win, ls, rs, ts, bs, tl, tr, bl, br);
    mvwprintw(menu_win, 1, 2, "Menu");
    post_menu(my_menu);
    wrefresh(menu_win);
}

std::string MainMenuUI::run() {
    int c;
    bool running = true;
    while (running && (c = wgetch(menu_win))) {
        switch (c) {
        case KEY_DOWN:
            menu_driver(my_menu, REQ_DOWN_ITEM);
            wrefresh(menu_win);
            break;
        case KEY_UP:
            menu_driver(my_menu, REQ_UP_ITEM);
            wrefresh(menu_win);
            break;
        case 10: // Enter
        {
            ITEM *cur = current_item(my_menu);
            std::string sel = item_name(cur);
            if (sel == "Exit") {
                return ""; // Empty string indicates exit
            } else if (sel == choices[0]) {

                while (true) {
                    std::string result = BookView::draw(main_win);

                    if (result == "RESIZE") {
                        // Rebuild the outer shell
                        destroy_ui();
                        clear();
                        refresh();
                        create_ui();
                        draw_all();
                    } else if (result == "EXIT") {
                        werase(main_win);
                        draw_all();
                        break;
                    }
                }

            } else if (sel == choices[1]) {

                while (true) {
                    std::string result = TransactionView::draw(main_win);

                    if (result == "RESIZE") {
                        // Rebuild the outer shell
                        destroy_ui();
                        clear();
                        refresh();
                        create_ui();
                        draw_all();
                    } else if (result == "EXIT" || result == "Continue" || result == "Cancel") {
                        werase(main_win);
                        draw_all();
                        break;
                    }
                }

            } else if (sel == choices[2]) {
                while (true) {
                    std::string result = HistoryView::draw(main_win);

                    if (result == "RESIZE") {

                        destroy_ui();
                        clear();
                        refresh();
                        create_ui();
                        draw_all();
                    } else if (result == "Continue" || result == "EXIT") {
                        werase(main_win);
                        draw_all();
                        break;
                    }
                }
            } else if (sel == choices[3]) { 
                while (true) {
                    std::string result = ReportView::draw(main_win);

                    if (result == "RESIZE") {

                        destroy_ui();
                        clear();
                        refresh();
                        create_ui();
                        draw_all();
                    } else {
                        // User pressed a key. Exit view.
                        werase(main_win);
                        draw_all();
                        break;
                    }
                }
            } else {
                selected_text = sel;
                werase(main_win);
                wborder(main_win, ls, rs, ts, bs, tl, tr, bl, br);
                mvwprintw(main_win, 2, 2, "Selected: %s", sel.c_str());
                wrefresh(main_win);
            }
            break;
        }
        case KEY_RESIZE:
            destroy_ui();
            clear();
            refresh();
            create_ui();
            draw_all();
            break;
        }
    }
    return selected_text;
}