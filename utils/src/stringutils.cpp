#include "../inc/stringutils.h"


void replace_all(string& s, const string& from, const string& to) {
    if (from.empty()) return;

    size_t pos = 0;
    while ((pos = s.find(from, pos)) != string::npos) {
        s.replace(pos, from.length(), to);
        pos += to.length();
    }
}