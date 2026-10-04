// main.cpp — the front of the building. It dispatches to instruments and does
// nothing else; the office knows nothing about this file.

#include "checks/registry.hpp"
#include "verify.hpp"

#include <cstdio>
#include <string_view>

#include "checks/units_compile.hpp"

int main(int argc, char** argv) {
    if (argc < 2 || std::string_view{argv[1]} == "help") {
        std::puts("laporte — a step-by-step telephone exchange, for no reason.\n\n"
                  "  ./laporte help             this\n"
                  "  ./laporte verify           judge every claim; exit status is the failures\n"
                  "  ./laporte verify <check>   judge one\n");
        return 0;
    }
    if (std::string_view{argv[1]} == "verify")
        return laporte::verify::main(argc - 2, argv + 2);
    std::fprintf(stderr, "laporte: unknown command '%s'\n", argv[1]);
    return 1;
}
