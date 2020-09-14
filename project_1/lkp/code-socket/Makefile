## [M1: point 1]
#  Explain following in here
# This includes all the make files. $(lastword $(MAKEFILE_LIST)) indiactes "Makefile" that
# is the file it is running. adding $realpath to it returns the directory of the Makefile
# dirname is a shel command that returns the directory name by deleleting the tailing slash.
# Therefore, this CUR_DIR variable contains the directory of the Makefile.
#
#  ...
CUR_DIR := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))


## [M2: point 1]
#  Assigning compiler nake to a variable CC and defining flags. 
#  -g allows debug operation, 02 is optimization level 2--code size and execution time.-Wall
#  truns on all the most common warnings. -Werror allow us to treat specified warnings as error.
#
#  ...
CC = gcc
CFLAGS += -g -O2 -Werror -Wall
LDFLAGS +=

## [M3: point 1]
# Atomatically consider all dependecies with follwing preprocessor options:
#  -MD includes system header.
#  -MP option allows supression the errors that would otherwise trigger if a header file is
#  renamed or deleted.
#  -MF sends specify a file that is used to write the dependencies to.
#  Deependency is written to a file extention with .d option
#  ...
DEPS_DIR  := $(CUR_DIR)/.deps$(LIB_SUFFIX)
DEPCFLAGS = -MD -MF $(DEPS_DIR)/$*.d -MP

## [M5: point 1]
# This adds all the .c file to SRC_FILES variable

SRC_FILES = $(wildcard *.c)

## [M6: point 1]
#  output file name into SRC_FILES obejct after stripping the file externsion .c 	
#
#
EXE_FILES = $(SRC_FILES:.c=)

## [M7: point 1]
# In this example, all is dummy tagret printing the names of executables
# But in most cases this is used to put rules that are essential to buid the entire project
#  ...
all: $(EXE_FILES)
	echo $(EXE_FILES)

## [M8: point 1]
#  The folowing pattern rule takes all .c files and all their dependencies into account
#  The rule executes the actuall compilation considering compilar and all dependency flags.
#  Automatic variable $@ pics up target and $< picsup the first prerequisite as 
#  the source (.c files in this case)
%/%.c:%.c $(DEPS_DIR)
	$(CC) $(CFLAGS) $(DEPCFLAGS) -c $@ $<

## [M9: point 1]
#  This command cleans the output files. For this case server and client.
clean:
	rm -f $(EXE_FILES)

## [M10: point 1]
# clean and all are not exactly the name of files rather these are commands needs to be execute 
# if make all/clean is explicitly execute. This avoides conflic with file name and improves 
# performance.
.PHONY: all clean
