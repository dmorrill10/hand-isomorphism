#
# Makefile
# author: Kevin Waugh (waugh@cs.cmu.edu)
# date: April 7, 2013
#
# see README.md and LICENSE.txt for more details.
#

CFLAGS     ?=-std=c99 -Wall -g -O2
LDLIBS     ?=-lm

SRC        :=$(addprefix src/,deck.c hand_index.c)
OBJ        :=$(SRC:.c=.o)

TARGETS    :=src/check
TARGET_SRC :=$(addprefix src/,$(TARGETS:=-main.c))
TARGET_OBJ :=$(TARGET_SRC:.c=.o)

.PHONY: all clean check

all: $(TARGETS)

check: src/check
	./src/check

src/check: src/check-main.o $(OBJ)

$(TARGETS):
	$(CC) $(LDFLAGS) -o $@ $^ $(LOADLIBES) $(LDLIBS)

src/check-main.o src/hand_index.o: src/deck.h src/hand_index.h
src/deck.o: src/deck.h


# More precise but typically not necessary and more verbose
# THIS_DIR := $(abspath $(CURDIR)/$(dir $(lastword $(MAKEFILE_LIST))))
THIS_DIR := .
SRC_DIR :=$(THIS_DIR)/src

VENDOR_DIR :=$(THIS_DIR)/vendor
INCLUDES =-I$(SRC_DIR) -I$(VENDOR_DIR)

CLANG_OPTIONS =-fcolor-diagnostics
CXX =clang++ -std=c++11 $(CLANG_OPTIONS)
CXXFLAGS ?=-std=c++11 $(CLANG_OPTIONS)

DISABLED_WARNINGS =-Wno-vla-extension -Wno-unused-function -Wno-unknown-pragmas
WARNINGS =-Wall -Wextra -Wredundant-decls -Wfatal-errors -Wshadow

### Source files
C_SRC = $(shell find -L $(SRC_DIR) -type f -name '*.c' 2>/dev/null)
C_HEADERS =$(shell find -L $(SRC_DIR) -type f -name '*.h' 2>/dev/null)
CXX_SRC = $(shell find -L $(SRC_DIR) -type f -name '*.cpp' 2>/dev/null)
CXX_HEADERS =$(shell find -L $(SRC_DIR) -type f -name '*.hpp' 2>/dev/null)

### Intermediate files
C_OBJ :=$(C_SRC:%.c=%.c.o)
CXX_OBJ :=$(CXX_SRC:%.cpp=%.cpp.o)

MAIN_OBJ :=$(filter %-main.cpp.o,$(CXX_OBJ))

C_LIB_OBJ :=$(filter-out %-main.c.o,$(C_OBJ))
CXX_LIB_OBJ :=$(filter-out %-main.cpp.o,$(CXX_OBJ))

TO_FILE =-o

%.c.o: %.c
	@echo [CC] $@
	$(CC) -c $(CFLAGS) $(TO_FILE) $@ $^ $(INCLUDES)

%.cpp.o: %.cpp
	@echo [CXX] $@
	$(CXX) -c $(CXXFLAGS) $(TO_FILE) $@ $^ $(INCLUDES)

# Utilities
#==========

run-%: %
	./$*

# @todo This is a neat rule but it's not used properly in this file yet
## commands : Display available commands.
commands : Makefile
	@sed -n 's/^##//p' $<

# Clean
#-------
.PHONY: clean
clean:
	rm -f $(TARGETS)
	rm -f $(MAIN_OBJ) src/check-main.o $(OBJ)

.PHONY: cleandep
cleandep:
	-rm $(DEP)


# Debugging
#----------
print-%:
	@echo $* = $($*)


# Testing
#===========

# Definitions
#------------
CATCH_DIR :=$(VENDOR_DIR)/Catch/single_include
TEST_DIR :=$(THIS_DIR)/test
TEST_SUPPORT_DIR :=$(TEST_DIR)/support
TEST_SRC_EXTENSION =.cpp
TEST_EXTENSION =.out
TEST_PREFIX =test_
TEST_SUBDIRS :=$(filter-out $(TEST_SUPPORT_DIR) $(TEST_DIR)/data,$(wildcard $(TEST_DIR)/*))
TESTS        :=$(shell find $(TEST_SUBDIRS) -type f -name '$(TEST_PREFIX)*$(TEST_SRC_EXTENSION)' 2>/dev/null)
TEST_OBJ :=$(TESTS:%$(TEST_SRC_EXTENSION)=%$(TEST_SRC_EXTENSION).o)
TEST_EXES :=$(TESTS:%$(TEST_SRC_EXTENSION)=%$(TEST_EXTENSION))
TEST_SUPPORT_SRC :=$(TEST_SUPPORT_DIR)/test_helper$(TEST_SRC_EXTENSION)
TEST_SUPPORT_OBJ :=$(TEST_SUPPORT_DIR)/test_helper$(TEST_SRC_EXTENSION).o

# Rules
#------
$(TEST_SUPPORT_DIR):
	@echo [mkdir] $@
	@mkdir -p $@

$(TEST_SUPPORT_SRC): | $(TEST_SUPPORT_DIR)
	@echo [touch] $@
	@touch $@

%$(TEST_EXTENSION): OPT =-O0
%$(TEST_EXTENSION): CFLAGS +=$(OPT) $(WARNINGS) $(DISABLED_WARNINGS) $(DEBUG_SYMBOLS)
%$(TEST_EXTENSION): CXXFLAGS +=$(OPT) $(WARNINGS) $(DISABLED_WARNINGS) $(DEBUG_SYMBOLS)
%$(TEST_EXTENSION): INCLUDES +=-I$(CATCH_DIR) -I$(TEST_SUPPORT_DIR)
%$(TEST_EXTENSION): $(TEST_SUPPORT_OBJ) %$(TEST_SRC_EXTENSION).o $(CXX_LIB_OBJ) $(C_LIB_OBJ) $(VENDOR_OBJS) | %$(TEST_SRC_EXTENSION) $(C_HEADERS) $(CXX_HEADERS)
	@echo [LD] $@
	$(CXX) $(CXXFLAGS) $(TO_FILE) $@ $^ $(INCLUDES) $(LDLIBS)
	@chmod 755 $@

.PHONY: compiletest
compiletest: $(TEST_EXES)

.PHONY: test
test: $(TEST_EXES)
	@for test in $^; do echo ; echo [TEST] $$test; \
		echo "===============================================================\n";\
		$$test | grep -v -P '^\s*\#'; done

.PHONY: cleantest
cleantest:
	-rm -f $(TEST_SUPPORT_OBJ) $(TEST_OBJ) $(TEST_EXES)

test-%: $(TEST_DIR)/$(TEST_PREFIX)%$(TEST_EXTENSION)
	./$(TEST_DIR)/$(TEST_PREFIX)$*$(TEST_EXTENSION)
