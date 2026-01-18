#pragma once

#include <string>
#include <vector>

using namespace std;

const string COMMANDS_DIR = "atk-cmd-bin/";
const string VERSION = "1.0.0";

vector<string> getAvailableCommands();