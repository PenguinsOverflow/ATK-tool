#include "../inc/stringutils.h"


void replace_all(string& s, const string& from, const string& to) {
    if (from.empty()) return;

    size_t pos = 0;
    while ((pos = s.find(from, pos)) != string::npos) {
        s.replace(pos, from.length(), to);
        pos += to.length();
    }
}


void ltrim(string& s) {
    auto start = find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !isspace(ch);
    });
    s = string(start, s.end());
}


void rtrim(string& s) {
    auto end = find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !isspace(ch);
    }).base();
    s = string(s.begin(), end);
}


void trim(string& s) {
    rtrim(s);
    ltrim(s);
}