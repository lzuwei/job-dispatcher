#
# 'make depend' uses makedepend to automatically generate dependencies 
#               (dependencies are added to end of Makefile)
# 'make'        build executable file 'ipg'
# 'make clean'  removes all .o and executable files
#

# define the C compiler to use
CC = g++

# define any compile-time flags
CFLAGS = -Wall -O2 -fexceptions -DLINUX

# define any directories containing header files other than /usr/include
#
INCLUDES = -I../../include 

# define library paths in addition to /usr/lib
#   if I wanted to include libraries not in /usr/lib I'd specify
#   their path using -Lpath, something like:
LFLAGS = -L/usr/local/lib  -L/usr/lib -s

# define any libraries to link into executable:
#   if I want to link in libraries (libx.so or libx.a) I use the -llibname 
#   option, something like (this will link in libmylib.so and libm.so:
LIBS = -lboost_random -lboost_atomic -lboost_thread -lboost_system -lboost_chrono -lboost_program_options -lboost_iostreams -lboost_filesystem

# define the C source files
SRCS = ../../src/main.cpp \
	../../src/INotifyEvent.cpp \
	../../src/INotifyEventPoller.cpp \
	../../src/INotifyWatch.cpp
	
SRCDIR = ../../src
	
# Object Directory
OBJDIR = obj

# define the C object files 
#
# This uses Suffix Replacement within a macro:
#   $(name:string1=string2)
#         For each word in 'name' replace 'string1' with 'string2'
# Below we are replacing the suffix .c of all words in the macro SRCS
# with the .o suffix
#
OBJS = $(SRCS:.cpp=.o)

# define the executable file 
MAIN = job-dispatcher

#Install Path
INSTALLDIR =.
COND1 = `stat $(MAIN) 2>/dev/null | grep Modify`
COND2 = `stat $(INSTALLDIR)/$(MAIN) 2>/dev/null | grep Modify`

#
# The following part of the makefile is generic; it can be used to 
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

.PHONY: depend clean

all: mkobjpath getobj $(MAIN) putobj

$(MAIN): $(OBJS) 
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@
	
mkobjpath: 
	mkdir -p $(OBJDIR) 2>/dev/null
	
getobj:
	-mv $(OBJDIR)/*.o $(SRCDIR) 2>/dev/null
	
putobj:
	-mv $(SRCDIR)/*.o $(OBJDIR) 2>/dev/null
	
install:
	@if [ "$(COND1)" != "$(COND2)" ]; \
	then \
		cp -p ./$(MAIN) $(INSTALLDIR)/$(MAIN) 2>/dev/null; \
		chmod 700 $(INSTALLDIR)/$(MAIN); \
		echo "Installed in $(INSTALLDIR)/$(MAIN)"; \
	fi

clean:
	$(RM) $(OBJDIR)/*.o *~ $(MAIN)

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it


