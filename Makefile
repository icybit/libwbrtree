#Reference http://www.iram.fr/~roche/code/c++/AddNumbers.html

LIB_NAME := librtree.so
LIB_VER := 0.1.0
LIB_VER_MAJOR := $(shell echo $(LIB_VER) | cut -f1 -d.)
LIB_VER_MINOR := $(shell echo $(LIB_VER) | cut -f2 -d.)
LIB_PATH := /usr/lib

CC := gcc
DEBUG := -g
CFLAGS := -Wall -Werror -pedantic $(DEBUG) -fvisibility=hidden -DDEBUG=1
LDFLAGS := -Wl,-soname,$(LIB_NAME).$(LIB_VER_MAJOR).$(LIB_VER_MINOR)
SRCEXT := c

INCPATH := include
SRCPATH := src
OBJPATH := obj
TESTPATH := tests
OUTPATH := build

SOURCES := $(shell find $(SRCPATH) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCPATH)/%, $(OBJPATH)/%, $(SOURCES:.$(SRCEXT)=.o))
INCLUDE := -I $(INCPATH)

#http://stackoverflow.com/questions/14806206/what-do-the-makefile-symbols-mean
$(OUTPATH)/$(LIB_NAME).$(LIB_VER): $(OBJECTS)
	@echo "Creating shared library..."
	@mkdir -p $(OUTPATH)
	$(CC) $(LDFLAGS) -shared -o $@ $^

$(OBJPATH)/%.o: $(SRCPATH)/%.$(SRCEXT)
	@echo "Compiling..."
	@mkdir -p $(OBJPATH)
	$(CC) $(CFLAGS) $(INCLUDE) -fpic -c $< -o $@ 

install: $(OUTPATH)/$(LIB_NAME).$(LIB_VER)
	@echo "Installing library..."
	cp -v $< $(LIB_PATH)
	ldconfig -v -n $(LIB_PATH)
	ln -v -sf $(LIB_NAME).$(LIB_VER) $(LIB_PATH)/$(LIB_NAME)
	
test: $(OUTPATH)/$(LIB_NAME).$(LIB_VER)
	@echo "Making tests..."
	cd $(TESTPATH) && $(MAKE)

clean:
	@echo "Cleaning..."
	$(RM) -r $(OBJPATH) $(OUTPATH)
	
.PHONY: clean