#pragma once

#include <string>
#include <vector>

using namespace std;

#ifdef __APPLE__
    const string COMMANDS_DIR = "/usr/local/bin/atk-cmd-bin/";
#elif __linux
    const string COMMANDS_DIR = "/usr/bin/atk-cmd-bin/";
#endif

const string VERSION = "1.0.0";

vector<string> getAvailableCommands(const string path);