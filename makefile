
VERSION?=release

PWD = $(shell pwd)
SRCS := $(shell ls *.cpp 2> /dev/null)
OBJS = $(SRCS:%.cpp=$(VERSION)/%.o)

ifeq ($(VERSION),debug)
VERSION_FLAGS?=-O0 -DDEBUG
else ifeq ($(VERSION),release)
VERSION_FLAGS?=-O3 -DNDEBUG
endif

CXX?=g++
CPPFLAGS?=-std=c++11 -g -MD
LDFLAGS?=-lm -lpthread

all: $(VERSION)/posterize

$(VERSION):
	mkdir $(VERSION)

$(OBJS): $(VERSION)/%.o: $(PWD)/%.cpp
	$(CXX) -c $(VERSION_FLAGS) $(CPPFLAGS) $< -o $@

$(VERSION)/posterize: $(VERSION) $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)

clean:
	rm -rf $(VERSION)

DEPS = $(SRCS:%.cpp=$(VERSION)/%.d)
sinclude $(DEPS)
