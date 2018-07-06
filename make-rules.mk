# Variables that can be set:

#					 the current direectory
# STATICLIB		   - will create a static library of all .c sources
#					 in the current directory
# SUBDIRS		   - Will call make recursivly on these directories
# EXTRA_CLEAN	   - will rm -f these files on "make clean"
# CC			   - C compiler executable
# LD			   - Linker executable
# CXXFLAGS		   - C compiler flags
# CPPFLAGS		   - C preprocessor flags
# LDFLAGS		   - Linker flags
# AR			   - static library archive executable
# RM			   - executable to remove files
# RMDIR            - executable to remove directory
# PROGRAM_DEPS	   - extra dependencies for PROGRAM
# STATICLIB_DEPS   - extra dependencies for STATICLIB

MAKEFLAGS += --no-builtin-rules

ifndef DEBUG
VARIANT_CXXFLAGS	?= -O2
VARIANT_CPPFLAGS	?= -DNDEBUG
else
VARIANT_CPPFLAGS	?= -DDEBUG
endif

VERSION_STR     = $(shell git describe --tags  --abbrev=5 --always --long)

COMMON_CXXFLAGS	= -Wall -Wextra -Wno-unused-parameter -ggdb -pipe -std=c++14 -Wimplicit-fallthrough=1
COMMON_CPPFLAGS	= -D_FILE_OFFSET_BITS=64 
CXX				?= g++
LD				=  $(CXX)
ALL_CXXFLAGS	?= $(COMMON_CXXFLAGS) $(VARIANT_CXXFLAGS) $(CXXFLAGS)
ALL_CPPFLAGS  	?= $(COMMON_CPPFLAGS) $(VARIANT_CPPFLAGS) $(CPPFLAGS) -DVERSION_STR=\"$(VERSION_STR)\"
ALL_LIBS		?= $(LIBS)
ALL_LDFLAGS		?= $(LDFLAGS)
AR			 	= ar
ARFLAGS			= cru
ALL_ARFLAGS		?= $(ARFLAGS)
RM				= rm -f
RMDIR			= rmdir

OBJDIR=build
OBJECTS = $(addprefix $(OBJDIR)/,$(notdir $(SOURCES:.cpp=.o)))
DEPS	= $(addprefix $(OBJDIR)/,$(notdir $(SOURCES:.cpp=.d)))

.PHONY: default $(SUBDIRS) subdirs
default: subdirs $(PROGRAM) $(STATICLIB)

$(OBJDIR)/%.d: %.cpp
	$(CXX) -MM -MG $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -MF  $@ $<

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -c -o $@ $<

$(STATICLIB): $(OBJECTS) 
	$(AR) $(ALL_ARFLAGS)  $@ $^ 

$(PROGRAM): $(OBJECTS) 
	$(LD) $(ALL_LDFLAGS) -o $@ $^ $(ALL_LIBS)

$(OBJDIR):
	@test -d $(OBJDIR) || mkdir -p $(OBJDIR)

$(OBJECTS): |$(OBJDIR)
$(DEPS): |$(OBJDIR)


.PHONY: clean real_clean
clean:: subdirs 
	-$(RM) $(PROGRAM) $(OBJECTS) $(DEPS) $(STATICLIB)
	-test -d $(OBJDIR) && $(RMDIR) $(OBJDIR) || true
ifneq (,$(EXTRA_CLEAN))
	-$(RM) -f $(EXTRA_CLEAN)
endif

subdirs:
	@for subdir in $(SUBDIRS) ; do \
		$(MAKE) -C "$${subdir}" $(MAKECMDGOALS) ; \
	done

.PHONY: check real_check
check: subdirs default real_check

ifeq (,$(findstring clean, $(MAKECMDGOALS)))
-include $(DEPS)
endif

.SUFFIXES:
SUFFIXES=

