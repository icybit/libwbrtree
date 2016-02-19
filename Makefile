#Reference http://www.iram.fr/~roche/code/c++/AddNumbers.html

CC := gcc
DEBUG := -g
CFLAGS := -Wall -Werror -pedantic $(DEBUG)
LFLAGS := -Wall $(DEBUG)
SRCEXT := c

LIBNAME := librtree

INCPATH := include
SRCPATH := src
OBJPATH := obj
LIBPATH := lib
OUTPATH := $(LIBPATH)

SOURCES := $(shell find $(SRCPATH) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCPATH)/%, $(OBJPATH)/%, $(SOURCES:.$(SRCEXT)=.o))
INCLUDE := -I $(INCPATH)

#http://stackoverflow.com/questions/14806206/what-do-the-makefile-symbols-mean
$(OUTPATH)/$(LIBNAME).so: $(OBJECTS)
	@echo "Creating shared library..."
	@mkdir -p $(LIBPATH)
	$(CC) $(LFLAGS) -shared -o $@ $^

$(OBJPATH)/%.o: $(SRCPATH)/%.$(SRCEXT)
	@echo "Compiling..."
	@mkdir -p $(OBJPATH)
	$(CC) $(CFLAGS) $(INCLUDE) -fpic -c $< -o $@ 
	
clean:
	@echo "Cleaning..."
	$(RM) -r $(OBJPATH) $(OUTPATH)
	
.PHONY: clean