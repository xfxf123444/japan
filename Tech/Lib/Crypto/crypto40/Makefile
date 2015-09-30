CXXFLAGS = -O2 -w -pipe
ARFLAGS = crs

ifeq ($(OSTYPE),)	# for DJGPP, where OSTYPE doesn't exist
CXXFLAGS := $(CXXFLAGS) -mbnu210
endif

ifeq ($(OSTYPE),solaris)
LDLIBS = -lnsl -lsocket
endif

ifeq ($(CXX),gcc)	# for some reason CXX is gcc on cygwin 1.1.4
CXX = g++
endif

SRCS = $(wildcard *.cpp)

ifeq ($(SRCS),)				# workaround wildcard function bug in GNU Make 3.77
SRCS = $(shell ls *.cpp)
endif

OBJS = $(SRCS:.cpp=.o)
# test.o needs to be after bench.o for cygwin 1.1.4 (possible ld bug?)
TESTOBJS = bench.o test.o validat1.o validat2.o validat3.o
LIBOBJS = $(filter-out $(TESTOBJS),$(OBJS))

all: cryptest.exe

clean:
	$(RM) cryptest.exe libcrypt.a $(LIBOBJS) $(TESTOBJS)

libcrypt.a: $(LIBOBJS)
	$(AR) $(ARFLAGS) $@ $(LIBOBJS)

cryptest.exe: libcrypt.a $(TESTOBJS)
	$(CXX) -o $@ $(CXXFLAGS) $(TESTOBJS) -L. -lcrypt $(LDFLAGS) $(LDLIBS)

.SUFFIXES: .cpp

.cpp.o:
	$(CXX) -fpermissive $(CXXFLAGS) -c $<
