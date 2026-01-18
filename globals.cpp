#include "globals.h"
#include <filesystem>

vector<string> getAvailableCommands() {
    vector<string> commands;
    commands.clear();

    for (const auto& entry : filesystem::directory_iterator(COMMANDS_DIR)) {
        if (!entry.is_regular_file()) continue;

        string filename = entry.path().filename().string();

        if (filename.size() > 4 && filename.starts_with("atk-")) {
            filename = filename.substr(4); // remove "atk-"
            commands.push_back(filename);
        }
    }
    return commands;
}