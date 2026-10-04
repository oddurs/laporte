// main.cpp — the front of the building. It dispatches to instruments and does
// nothing else; the office knows nothing about this file.

#include <cstdio>
#include <string_view>

#include "checks/units_compile.hpp"

int main(int argc, char** argv) {
    if (argc < 2 || std::string_view{argv[1]} == "help") {
        std::puts("laporte — a step-by-step telephone exchange, for no reason.\n\n"
                  "  ./laporte help      this\n");
        return 0;
    }
    std::fprintf(stderr, "laporte: unknown command '%s'\n", argv[1]);
    return 1;
}
