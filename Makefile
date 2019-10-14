CXX				:= g++-9

RM				:= rm
SH				:= bash

SRCDIR			:= src
BUILDDIR		:= build
BINDIR			:= $(BUILDDIR)/bin
LIBDIR			:= $(BUILDDIR)/lib
OBJDIR			:= $(BUILDDIR)/obj
PROGRAM			:= $(BINDIR)/program

INCLUDE			:= -I/usr/local/include -I/usr/local/include/eigen3
CXXFLAGS		:= -std=c++17 -Wall -O2 $(INCLUDE) $(SRCDIR)
LDFLAGS			:= -L/usr/local/lib -lglfw -framework OpenGL -framework CoreVideo -framework IOKit -framework Cocoa

SRC				:= $(shell find $(SRCDIR) -name "*.cpp")
HDR				:= $(shell find $(SRCDIR) -name "*.h")
CUTSRC			:= $(subst $(SRCDIR),.,$(SRC))
OBJS			:= $(addprefix $(OBJDIR)/, $(SRC:.cpp=.o))

.PHONY: all
all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	-mkdir -p $(BINDIR)
	$(CXX) $^ $(LDFLAGS) -o $@

$(OBJDIR)/%.o: %.cpp $(HDR)
	-mkdir -p $(dir $@)
	$(CXX) -c $< $(CXXFLAGS) -o $@

.PHONY: clean
clean:
	$(RM) -f $(OBJS) $(PROGRAM)
