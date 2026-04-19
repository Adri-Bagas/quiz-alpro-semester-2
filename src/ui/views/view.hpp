#include <ncurses.h>
#include <string>

namespace BookView {
    std::string draw(WINDOW *main_win);
}

namespace TransactionView {
    std::string draw(WINDOW *main_win);
}

namespace HistoryView {
    std::string draw(WINDOW *main_win);
}

namespace ReportView {
    std::string draw(WINDOW *main_win);
}