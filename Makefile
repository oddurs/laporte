# laporte — a step-by-step telephone exchange, modelled from first principles,
# for no reason.
#
#     make && ./laporte
#
# A C++23 compiler and make. That is the whole list (house rule 5). The one
# sanctioned exception, v0.6's audio edge, is linked only on request.
#
# One translation unit: apps/main.cpp includes everything else. Every part of
# the office is a header small enough to read in one sitting, so there are no
# object files to stale and no link order.
#
# Not -Werror by default, for the fifteen-year reason: a compiler released in
# 2041 will have warnings that do not exist today, and the first thing it will
# do with -Werror is refuse a correct program. `make strict` turns it on.

CXX      ?= c++
CXXFLAGS ?= -std=c++23 -O2 -Iinclude -Iapps \
            -Wall -Wextra -Wpedantic -Wshadow -Wold-style-cast \
            -Wconversion -Wsign-conversion -Wdouble-promotion

SOURCES  := $(wildcard apps/*.cpp)
HEADERS  := $(wildcard include/laporte/*.hpp) $(wildcard apps/*.hpp) \
            $(wildcard apps/checks/*.hpp) $(wildcard apps/platform/*.hpp)

laporte: $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $@

strict: clean
	$(MAKE) CXXFLAGS="$(CXXFLAGS) -Werror" laporte

clean:
	rm -f laporte

.PHONY: strict clean
