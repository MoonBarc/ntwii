#include "log.h"
#include <iostream>

void Log(std::string_view v) {
    std::cerr << "[ntwii] " << v << '\n';
}