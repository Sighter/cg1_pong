# Makefile for unix systems
# this requires GNU make

APPNAME=cg1application

# Compiler flags
ifeq ($(RELEASE), 1)
CFLAGS = -Wall -ansi -pedantic -ffast-math -s -O5 -DNDEBUG
CXXFLAGS = -Wall -ansi -pedantic -ffast-math -s -O5 -DNDEBUG
else
CFLAGS = -ansi -pedantic -g \
		-fno-common \
		-Wall \
		-Wextra \
		-Wformat=2 \
		-Winit-self \
		-Winline \
		-Wpacked \
		-Wp,-D_FORTIFY_SOURCE=2 \
		-Wpointer-arith \
		-Wlarger-than-65500 \
		-Wmissing-declarations \
		-Wmissing-format-attribute \
		-Wmissing-noreturn \
		-Wredundant-decls \
		-Wsign-compare \
		-Wstrict-aliasing=2 \
		-Wswitch-enum \
		-Wundef \
		-Wunreachable-code \
		-Wunsafe-loop-optimizations \
		-Wwrite-strings \
		-Wuninitialized \
		-Weffc++
		#-fmudflap \
		#-fmudflapth \
		#-fmudflapir
CXXFLAGS = -Wall -ansi -pedantic -g
endif

# OpenGL Libraries 
GL_LIBS = -lGL -lGLU -lglut
GL_LIBDIR = -L/usr/X11/lib
LINK_GL = $(GL_LIBDIR) $(GL_LIBS)

# all needed libraries
LINK = $(LINK_GL) -lm -lrt

# Files

CFILES=$(wildcard *.c)
CPPFILES=$(wildcard *.cpp)
INCFILES=$(wildcard *.h)	
SRCFILES = $(CFILES) $(CPPFILES)
PRJFILES = Makefile $(wildcard *.vcproj)
ALLFILES = $(SRCFILES) $(INCFILES) $(PRJFILES)
OBJECTS = $(patsubst %.cpp,%.o,$(CPPFILES)) $(patsubst %.c,%.o,$(CFILES))
	   
# build rules
.PHONY: all
all:	$(APPNAME)

# build and start with "make run"
.PHONY: run
run:	all
	__GL_SYNC_TO_VBLANK=1 ./$(APPNAME)

# automatic dependency generation
# create $(DEPDIR) (and an empty file dir)
# create a .d file for every .c source file which contains
# 		   all dependencies for that file
.PHONY: depend
depend:	$(DEPDIR)/dependencies
DEPDIR   = ./dep
DEPFILES = $(patsubst %.c,$(DEPDIR)/%.d,$(CFILES)) $(patsubst %.cpp,$(DEPDIR)/%.d,$(CPPFILES))
$(DEPDIR)/dependencies: $(DEPDIR)/dir $(DEPFILES)
	@cat $(DEPFILES) > $(DEPDIR)/dependencies
$(DEPDIR)/dir:
	@mkdir -p $(DEPDIR)
	@touch $(DEPDIR)/dir
$(DEPDIR)/%.d: %.c $(DEPDIR)/dir
	@echo rebuilding dependencies for $*
	@set -e; $(CC) -M $(CPPFLAGS) $<	\
	| sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' \
	> $@; [ -s $@ ] || rm -f $@
$(DEPDIR)/%.d: %.cpp $(DEPDIR)/dir
	@echo rebuilding dependencies for $*
	@set -e; $(CXX) -M $(CPPFLAGS) $<	\
	| sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' \
	> $@; [ -s $@ ] || rm -f $@
-include $(DEPDIR)/dependencies

# rule to build application
$(APPNAME): $(OBJECTS) $(DEPDIR)/dependencies
	$(CXX) $(CFLAGS) $(LINK) $(LDFLAGS) $(OBJECTS) -o$(APPNAME)

# remove all unneeded files
.PHONY: clean
clean:
	@echo removing binary: $(APPNAME)
	@rm -f $(APPNAME)
	@echo removing object files: $(OBJECTS)
	@rm -f $(OBJECTS)
	@echo removing dependency files
	@rm -rf $(DEPDIR)
	@echo removing tags
	@rm -f tags

# update the tags file
.PHONY: TAGS
TAGS:	tags

tags:	$(SRCFILES) $(INCFILES) 
	ctags $(SRCFILES) $(INCFILES)

# look for 'TODO' in all relevant files
.PHONY: todo
todo:
	-egrep -in "TODO" $(SRCFILES) $(INCFILES)

