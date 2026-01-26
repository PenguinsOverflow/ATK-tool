#include "../inc/globals.h"
#include "../inc/stringutils.h"
#include <filesystem>
#include <string>
#include <vector>

// **********************
void moveItem(vector<string>& vec, string item, size_t new_pos);
// **********************

vector<string> getAvailableCommands(const string path) {
    vector<string> commands;
    commands.clear();

    for (const auto& entry : filesystem::directory_iterator(path)) {
        if (entry.is_directory()) {
            vector<string> ris = getAvailableCommands(entry.path().string());
            commands.insert(commands.end(), ris.begin(), ris.end());
            continue;
        }

        if (entry.is_regular_file()) {
            string filename = entry.path().filename().string();
    
            if (filename.size() > 4 && filename.starts_with("atk-")) {
                replace_all(filename, "-", " ");
                filename = filename.substr(4); // remove "atk-"
                commands.push_back(filename);
            }
        };

    }

    moveItem(commands, "help", 0);
    moveItem(commands, "version", 1);
    return commands;
}

void moveItem(vector<string>& vec, string item, size_t new_pos) {
    auto it = find(vec.begin(), vec.end(), item);
    if (it != vec.end()) {
        string temp = vec[new_pos];
        vec[new_pos] = item;
        *it = temp;
    }
}