TARGET := demo-load-save
SOURCES := demo/demo-load-save.cpp

TGT_DEFS :=

CPPFLAGS := -g -O0 -Wall -Wextra
TGT_CFLAGS := $(CPPFLAGS) -std=c11
TGT_CXXFLAGS := $(CPPFLAGS) -std=c++17

TGT_LDLIBS :=

.PHONY: demo-load-save
demo-load-save: $(TARGET_DIR)/$(TARGET)

.PHONY: run-demo-load-save
run-demo-load-save: $(TARGET_DIR)/$(TARGET)
	$< $(TARGET_DIR)
