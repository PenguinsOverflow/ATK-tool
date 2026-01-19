#include "globals.h"
#include <unistd.h>
#include <string>
#include <vector>
#include <format>

using namespace std;

// **********************
string calcDirectory(const string cmd);
// **********************

int main(int argc, char* argv[]) {

    if (argc > 1) {
        string arg = argv[1];
        string str_args = "";
        for (int i = 2; i < argc; i++) {
            str_args += string(argv[i]) + " ";
        }

        if (arg == "-h" || arg == "--help") {
            system(format("./{}atk-help", COMMANDS_DIR).c_str());
            return 0;
        } else if (arg == "-v" || arg == "--version") {
            system(format("./{}atk-version", COMMANDS_DIR).c_str());
            return 0;
        } else {
            str_args = arg + " " + str_args;
            vector<string> commands = getAvailableCommands(COMMANDS_DIR);
            bool found = false;
            for (string cmd : commands) {
                if ((str_args).starts_with(cmd)) {
                    found = true;
                    string str_directory = calcDirectory(cmd);
                    str_args = str_args.substr(cmd.size());
                    replace_all(cmd, " ", "-");
                    system(format("./{}{}atk-{} {}", COMMANDS_DIR, str_directory, cmd, str_args).c_str());
                }
            }

            if (!found) {
                printf("Unknown option: %s\n", arg.c_str());
                printf("Use -h or --help for usage information.\n");
                return 1;
            }
        }
    } else {
        printf("No arguments provided. Use -h or --help for usage information.\n");
        return 1;
    }
    
    return 0;
}


string calcDirectory(const string cmd) {
    string directory = "";
    size_t pos = cmd.rfind(' ');
    directory = cmd.substr(0, pos == string::npos ? 0 : pos);
    replace_all(directory, " ", "/");
    directory += "/";
    return directory;
}