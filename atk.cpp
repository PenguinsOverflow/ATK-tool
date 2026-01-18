#include "atk.h"
#include "globals.h"
#include <format>
#include <vector>
#include <unistd.h>

using namespace std;

int main(int argc, char* argv[]) {

    if (argc > 1) {
        string arg = argv[1];
        string str_args = "";
        for (int i = 2; i < argc; i++) {
            str_args += string(argv[i]) + " ";
        }

        if (arg == "-h" || arg == "--help") {
            system(format("./{}atk-help {}", COMMANDS_DIR, str_args).c_str());
            return 0;
        } else if (arg == "-v" || arg == "--version") {
            system(format("./{}atk-version {}", COMMANDS_DIR, str_args).c_str());
            return 0;
        } else {

            vector<string> commands = getAvailableCommands();
            bool found = false;
            for (string cmd : commands) {
                if (arg.compare(cmd) == 0) {
                    found = true;
                    system(format("./{}atk-{} {}", COMMANDS_DIR, cmd, str_args).c_str());
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
