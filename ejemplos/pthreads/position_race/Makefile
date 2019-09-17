APPNAME=$(shell basename $(shell pwd))

$(APPNAME): $(APPNAME).c
	cc -g -Wall -Wextra $(APPNAME).c -o $(APPNAME) -pthread

.PHONY: clean
gitignore:
	echo $(APPNAME) > .gitignore
clean:
	rm -f $(APPNAME)
