TARGET := test
SOURCES := \
	unit_tests/*.cpp \
	crcany/crc.c \
	crcany/model.c \

TGT_DEFS :=

CPPFLAGS := -g -O0 -Wall -Wextra
TGT_CFLAGS := $(CPPFLAGS) -std=c11
TGT_CXXFLAGS := $(CPPFLAGS) -std=c++17 -pthread

TGT_LDLIBS := -lgtest -lpthread -lgtest_main

.PHONY: tests
tests: $(TARGET_DIR)/$(TARGET)

.PHONY: check
check: $(TARGET_DIR)/$(TARGET)
	$<
