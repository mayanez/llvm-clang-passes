DISTRO=$(shell cat /etc/os-release | grep -E "^ID=.*$$" | cut -d"=" -f2)

ifndef FORCE_USE_ABI
ifeq ($(DISTRO), ubuntu)
        FORCE_USE_ABI = 0
else
        FORCE_USE_ABI = 1
endif
endif

ifndef LLVMCONFIG
	LLVMCONFIG = llvm-config
endif

$(info -----------------------------------------------)
$(info Using LLVMCONFIG = $(LLVMCONFIG))
$(info Using FORCE_USE_ABI = $(FORCE_USE_ABI))
$(info -----------------------------------------------)

CXX := clang++
CXXFLAGS := -fno-rtti -O0 -g -D_GLIBCXX_USE_CXX11_ABI=${FORCE_USE_ABI}
PLUGIN_CXXFLAGS := -fpic

LLVM_CXXFLAGS := $(shell ${LLVMCONFIG} --cxxflags)
LLVM_LDFLAGS := $(shell ${LLVMCONFIG} --ldflags --libs --system-libs)
LLVM_INC := $(shell ${LLVMCONFIG} --includedir)

# Plugins shouldn't link LLVM and Clang libs statically, because they are
# already linked into the main executable (opt or clang). LLVM doesn't like its
# libs to be linked more than once because it uses globals for configuration
# and plugin registration, and these trample over each other.
LLVM_LDFLAGS_NOLIBS := $(shell ${LLVMCONFIG} --ldflags)
PLUGIN_LDFLAGS := -shared

TARGET := HelloPass.so
BUILDDIR := build

OBJDIR := $(BUILDDIR)/obj

SRCDIR = src
vpath %.cpp $(SRCDIR)
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

DEPS := $(OBJS:%.o=%.d)

INC = -Iincludes

.PHONY: all
all:	build \
			$(BUILDDIR)/$(TARGET)

.PHONY: build
build:
	@test -d $(BUILDDIR) || mkdir -p $(BUILDDIR) && mkdir -p $(OBJDIR)

$(BUILDDIR)/$(TARGET): $(OBJS)
	$(CXX) $(PLUGIN_CXXFLAGS) $(CXXFLAGS) $(INC) $(LLVM_CXXFLAGS) $^ $(PLUGIN_LDFLAGS) $(LLVM_LDFLAGS_NO_LIBS) -o $@

-include $(DEPS)

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(PLUGIN_CXXFLAGS) $(CXXFLAGS) $(INC) $(LLVM_CXXFLAGS) -MMD -o $@ -c $<

.PHONY: clean format

clean:
	rm -rf $(BUILDDIR)/*

format:
	find . -name "*.cpp" | xargs clang-format -style=file -i
