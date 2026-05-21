ifneq ($(IXTLI_TOP),)
IXTLI_CPP_FLAGS ?= -I$(IXTLI_TOP)/include 
IXTLI_LD_FLAGS ?= -L$(IXTLI_TOP)/lib -lixtli -Wl,-rpath=$(IXTLI_TOP)/lib
endif

DEST_DIR?=./
TARGET=$(DEST_DIR)/GraphCalculator

OBJ_PATH := .built
SRC_DIR := $(shell find . -type d)
SRC := $(foreach DIR,$(SRC_DIR),$(patsubst ./%,%,$(wildcard $(DIR)/*.cpp)))
OBJ := $(patsubst %.cpp,$(OBJ_PATH)/%.o,$(SRC))
INCLUDES := $(foreach DIR,$(SRC_DIR),-I$(DIR))

CXXFLAGS := -Wall -Wextra -Wno-unused-parameter
CPPFLAGS := $(INCLUDES) $(IXTLI_CPP_FLAGS)

LIBS := -lGL -lglut -luuid
LD_FLAGS := $(IXTLI_LD_FLAGS) $(LIBS)

CXX ?= g++

.PHONY: all clean
all: $(TARGET)

$(OBJ):$(OBJ_PATH)/%.o:%.cpp
	@echo "CXX $<"
	@mkdir -p $(shell dirname $(OBJ_PATH)/$<)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@ 

$(TARGET):$(OBJ)
	@echo "BUILD $@"
	@$(CXX) -std=c++11 $(LDFLAGS) -o $(TARGET) $(OBJ) $(LD_FLAGS) $(LIBS) -lm

clean:
	@rm -rf $(OBJ_PATH) $(TARGET)