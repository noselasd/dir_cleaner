.SUFFIXES:
SUFFIXES=
SUBDIRS = src 

.PHONY: all clean check $(SUBDIRS) subdirs
     
all: subdirs

clean: subdirs

check: subdirs

subdirs:
	@for subdir in $(SUBDIRS) ; do \
		$(MAKE) -C "$${subdir}" $(MAKECMDGOALS) ; \
	done

