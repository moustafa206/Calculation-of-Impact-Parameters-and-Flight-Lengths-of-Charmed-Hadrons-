RC           := root-config
ifneq ($(shell which $(RC) 2>&1 | sed -ne "s@.*/$(RC)@$(RC)@p"),$(RC))
ifneq ($(ROOTSYS),)
RC1          := $(ROOTSYS)/bin/root-config
ifneq ($(shell which $(RC1) 2>&1 | sed -ne "s@.*/$(RC)@$(RC)@p"),$(RC))
$(error Please make sure $(RC1) can be found in path)
else
RC           := $(RC1)
endif
else
$(error Please make sure $(RC) can be found in path)
endif
endif

ARCH         := $(shell $(RC) --arch)
ALTCC        := $(shell $(RC) --cc)
ALTCXX       := $(shell $(RC) --cxx)
ALTLD        := $(shell $(RC) --ld)

ObjSuf        = o
SrcSuf        = cpp
ExeSuf        =
DllSuf        = so
OutPutOpt     = -o # keep whitespace after "-o"

ifeq (debug,$(findstring debug,$(ROOTBUILD)))
OPT           = -g
OPT2          = -g
else
ifneq ($(findstring debug, $(strip $(shell $(RC) --config))),)
OPT           = -g
OPT2          = -g
else
OPT           = -O
OPT2          = -O2
endif
endif

ROOTCFLAGS   := $(shell $(RC) --cflags)
ROOTLDFLAGS  := $(shell $(RC) --ldflags)
ROOTLIBS     := $(shell $(RC) --libs)
ROOTGLIBS    := -lEve -lMLP -lSpectrum $(shell $(RC) --glibs) 
HASTHREAD    := $(shell $(RC) --has-thread)
ROOTCINT     := rootcint

ifeq ($(ARCH),linux)
CXX           = g++
CXXFLAGS      = $(OPT2) -Wall -fPIC
LD            = g++
LDFLAGS       = $(OPT2)
endif

ifeq ($(ARCH),linuxx8664gcc)
CXX           = g++
CXXFLAGS      = $(OPT2) -Wall -fPIC
LD            = g++
LDFLAGS       = $(OPT2)
endif

ifeq ($(CXX),)
$(error $(ARCH) invalid architecture)
endif

CXXFLAGS     += $(ROOTCFLAGS)
LDFLAGS      += $(ROOTLDFLAGS)
LIBS          = $(ROOTLIBS) $(SYSLIBS)
GLIBS         = $(ROOTGLIBS) $(SYSLIBS)

ifneq ($(ALTCC),)
   CC  = $(ALTCC)
endif
ifneq ($(ALTCXX),)
   CXX = $(ALTCXX)
endif
ifneq ($(ALTLD),)
   LD  = $(ALTLD)
endif
#------------------------------------------------------------------------------

SRCDIR     := src
SRCDIRS    := $(addprefix ../,$(SRCDIR))

INCDIR     := include
INCDIRS    := $(addprefix ../, $(INCDIR))
INCLUDE    := $(addprefix  -I, $(INCDIRS))

EXENAME    := example1$(ExeSuf)

MAIN       := ../$(EXENAME)

MAINS      := $(wildcard $(addsuffix /*.$(SrcSuf),$(SRCDIRS)))
MAINO      := $(notdir $(patsubst %.$(SrcSuf),%.o,$(MAINS)))
#------------------------------------------------------------------------------

.SUFFIXES: .$(SrcSuf) .$(ObjSuf) .$(DllSuf)

.PHONY = all clean 

all: testdir make 

$(MAIN): $(MAINO)
	$(LD) $(LDFLAGS) $^ $(GLIBS) $(OutPutOpt)$@
	@echo "$@ done"

testdir:
	@test -d "$(ARCH)" || (echo "Creating $(ARCH)"; mkdir $(ARCH))

make:
	@make --directory $(ARCH) -f ../Makefile $(MAIN)

VPATH := $(SRCDIRS)

%.$(ObjSuf): %.$(SrcSuf)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c -MD $< -o $@

clean:
		@rm -rf $(EXENAME) $(ARCH)
