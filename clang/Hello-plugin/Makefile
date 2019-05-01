CXX := clang++

DISTRO=$(shell cat /etc/os-release | grep -E "^ID=.*$$" | cut -d"=" -f2)

ifndef FORCE_USE_ABI
ifeq ($(DISTRO), ubuntu)
        FORCE_USE_ABI = 0
else
        FORCE_USE_ABI = 1
endif
endif

# NOTE: Need to include headers for compiler builtins for tools to work.
CLANG_INC_DIRS := $(shell echo $$( i=1; for d in $$(${CXX} -E -x c++ - -v < /dev/null 2>&1 | awk "/\#include <...> search starts here:/{f=1;next} /End of search list./{f=0} f"); do echo -DCLANG_INC_DIR$${i}=\"$${d}\"; i=$$(( i+=1 )); done))

CXXFLAGS := -fno-rtti -O0 -g -std=c++11 -D_GLIBCXX_USE_CXX11_ABI=${FORCE_USE_ABI} ${CLANG_INC_DIRS}

PLUGIN_CXXFLAGS := -fpic
PLUGIN_LDFLAGS := -shared

ifndef LLVMCONFIG
	LLVMCONFIG = llvm-config
endif

LLVM_CXXFLAGS := $(shell ${LLVMCONFIG} --cxxflags | sed -e "s/\-fno\-exceptions//")
LLVM_LDFLAGS := $(shell ${LLVMCONFIG} --ldflags --libs --system-libs)
LLVM_LDFLAGS_NOLIBS := $(shell ${LLVMCONFIG} --ldflags)
LLVM_INC := $(shell ${LLVMCONFIG} --includedir)

TARGET := hello-plugin.so
BUILDDIR := build

OBJDIR := $(BUILDDIR)/obj

SRCDIR = src
vpath %.cpp $(SRCDIR)
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

DEPS := $(OBJS:%.o=%.d)

INC = -Iincludes

CLANG_INC = -I$(shell ${LLVMCONFIG} --src-root)/tools/clang/include \
					  -I$(shell ${LLVMCONFIG} --obj-root)/tools/clang/include

CLANG_LIBS := \
	-Wl,--start-group \
	-lclangAST \
	-lclangASTMatchers \
	-lclangAnalysis \
	-lclangBasic \
	-lclangDriver \
	-lclangEdit \
	-lclangFrontend \
	-lclangFrontendTool \
	-lclangLex \
	-lclangParse \
	-lclangSema \
	-lclangEdit \
	-lclangRewrite \
	-lclangRewriteFrontend \
	-lclangStaticAnalyzerFrontend \
	-lclangStaticAnalyzerCheckers \
	-lclangStaticAnalyzerCore \
	-lclangSerialization \
	-lclangToolingCore \
	-lclangTooling \
	-lclangFormat \
	-Wl,--end-group


.PHONY: all
all:	build \
			$(BUILDDIR)/$(TARGET)

.PHONY: build
build:
	@test -d $(BUILDDIR) || mkdir -p $(BUILDDIR) && mkdir -p $(OBJDIR)

$(BUILDDIR)/$(TARGET): $(OBJS)
	$(CXX) $(PLUGIN_CXXFLAGS) $(CXXFLAGS) $(INC) $(LLVM_CXXFLAGS) $(CLANG_INC) $^ $(PLUGIN_LDFLAGS) $(LLVM_LDFLAGS_NOLIBS) -o $@

-include $(DEPS)

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(PLUGIN_CXXFLAGS) $(CXXFLAGS) $(INC) $(LLVM_CXXFLAGS) $(CLANG_INC) -MMD -o $@ -c $<

.PHONY: clean
clean:
	rm -rf $(BUILDDIR)
