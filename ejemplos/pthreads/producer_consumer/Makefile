APPNAME=$(shell basename $(shell pwd))

CC=cc
CCX=g++
FLAGS=-g -Wall -Wextra  
CFLAGS=$(FLAGS)
CXXFLAGS=$(FLAGS)
LIBS=-pthread
CLIBS=$(LIBS)
CXXLIBS=$(LIBS)

$(APPNAME): $(APPNAME).c
	$(CC) $(FLAGS) $^ -o $@ $(CLIBS)
	
#$(APPNAME): $(APPNAME).cpp
#	$(CXX) $(CXXFLAGS) $^ -o $@ $(CXXCLIBS)

.PHONY: gitignore
gitignore:
	echo $(APPNAME) > .gitignore

.PHONY: clean
clean:
	rm -f $(APPNAME)
