#include "../inc/terminal.h"
#include <unistd.h>

bool isSudo() {
    return geteuid() == 0;
}