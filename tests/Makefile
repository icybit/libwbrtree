#Reference http://www.iram.fr/~roche/code/c++/AddNumbers.html

CC := gcc
DEBUG := -g
CFLAGS := -Wall -Werror -pedantic $(DEBUG) -DWB_RTREE_COMPILATION=1
LDFLAGS :=
SRCEXT := c

OUTNAME := libwbrtree-test

INCPATH := .
SRCPATH := .
OBJPATH := ../obj
OUTPATH := ../build

SOURCES := $(shell find $(SRCPATH) -type f -name '*.$(SRCEXT)')
OBJECTS := $(patsubst $(SRCPATH)/%, $(OBJPATH)/%, $(SOURCES:.$(SRCEXT)=.o))
LIBS := -lm -lcmocka
INCLUDE := -I $(INCPATH) -I../include

#http://stackoverflow.com/questions/14806206/what-do-the-makefile-symbols-mean
run: $(OUTPATH)/$(OUTNAME)
	@echo "Running tests..."
	./$<

$(OUTPATH)/$(OUTNAME): $(OBJECTS) $(shell find $(OBJPATH) -type f -name '*.o')
	@echo "Linking..."
	@mkdir -p $(OUTPATH)
	$(CC) $(LFLAGS) $^ $(LIBS) -o $@

$(OBJPATH)/%.o: $(SRCPATH)/%.$(SRCEXT)
	@echo "Compiling..."
	@mkdir -p $(OBJPATH)
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

.PHONY: run