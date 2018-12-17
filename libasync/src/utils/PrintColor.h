#pragma once

/*
cout << "This ->" << Color::fg_red << "word"<< Color::fg_default << "<- is red." << endl;
*/

namespace wjp::Color {
    enum Code : int {
        fg_black    = 30,
        fg_red      = 31,
        fg_green    = 32,
        fg_yellow   = 33,
        fg_blue     = 34,
        fg_magenta  = 35,
        fg_cyan     = 36,
        fg_white    = 37,
        fg_default  = 39,
        bg_black    = 40,
        bg_red      = 41,
        bg_green    = 42,
        bg_yellow   = 43,
        bg_blue     = 44,
        bg_magenta  = 45,
        bg_cyan     = 46,
        bg_white    = 47,
        bg_default  = 49
    };
    std::ostream& operator<<(std::ostream& os, Code code) {
        return os << "\033[" << static_cast<int>(code) << "m";
    }
}
