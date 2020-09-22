## [M1: point 1]
#  Creating the variable MODULE and Setting the module name using recurive expand assigment though there is no recursive vriable to expand.
#  ...
MODULE	 = proj2

## [M2: point 1]
# obj-m indicates object files which are built as loadable kernel modules
# += is appending operation, so it appends below .o to already existing values of obj-m (if any)
#  ...
obj-m += $(MODULE).o

## [M3: point 1]
#  The below uses shorthand operator ?=. It sets the variable KERNELDIR only if it's not already set#  before, the belwo is seting kernel directory.Uses shell comand to get present kernel version 
#  ...
KERNELDIR ?= /lib/modules/$(shell uname -r)/build

## [M4: point 1]
#  The present path is set to variable PWD. := is simply expand variable used along with shell command to get path. := expands and assign the value at present time when variable(PWD) is defined
#  ...
PWD := $(shell pwd)

## [M5: point 1]
#  make recipe "all" is set with MODULE variable value that is set above with module name.
#  It is by default used when make is called. $MODULE: recipe that is defined later is called from here
#  ...
all: $(MODULE)


## [M6: point 1]
# It is implicit rule. when make is called, it looks for object files that needs to be created from obj-m then looks for corresponding source file to create object.
# $< indicates dependency while compiling and $@ is target.CC is C compiler. 
#  ...
%.o: %.c
	@echo "  CC      $<"
	@$(CC) -c $< -o $@

## [M7: point 1]
# As this is external module, Here first we are changing the directory from kerneldirectory to present path using -C option
#  Then makefile from $PWD (PWD is assigned variable M) and starts creating .ko and object files in local path 
#  "modules" is default target.It builds the modules locates in current directory with all output files generated in same directory.And kernel source is not updated.
#
#  #  ...
$(MODULE):
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

## [M8: point 1]
#  First we are changing the directory from kerneldirectory to present path using -C option
#  then make from $PWD starts cleaning/deleting all generated files in the module directory/current path/$PWD
#  ...
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean
