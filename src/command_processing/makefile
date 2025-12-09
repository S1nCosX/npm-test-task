OBJDIR := ../../obj
SRCS :=
OBJS := $(SRCS:.cpp=.o)
DEPS := $(OBJS:.o=.d)

SUBDIRS := command_descriptor command_processor

SRCDIR := ../../src
INCLUDES := -I$(SRCDIR)

CXX := g++

CXXFLAGS := -std=c++17 -Wall -O2 $(INCLUDES)

all: $(SUBDIRS) $(OBJS)

$(SUBDIRS):
	$(MAKE) -C $@ all;

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $(OBJDIR)/$@

-include $(DEPS)

.PHONY: all $(SUBDIRS)
