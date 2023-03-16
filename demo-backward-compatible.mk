TARGET := demo-backward-compatible
SOURCES := demo/demo-backward-compatible.cpp

TGT_DEFS :=

CPPFLAGS := -g -O0 -Wall -Wextra
TGT_CFLAGS := $(CPPFLAGS) -std=c11
TGT_CXXFLAGS := $(CPPFLAGS) -std=c++17

TGT_LDLIBS :=

.PHONY: demo-backward-compatible
demo-backward-compatible: $(TARGET_DIR)/$(TARGET)

.PHONY: run-demo-backward-compatible
run-demo-backward-compatible: $(TARGET_DIR)/$(TARGET)
	$< $(TARGET_DIR)
