CC = g++


dragonshell: commands commands.h utils utils.h
	g++ -g dragonshell.cc commands.o utils.o

commands: utils utils.h
	g++ -g -c commands.cc utils.o -o commands.o

utils:
	g++ -g -c utils.cc -o utils.o

clean: 
	rm -rf *.o; \
	rm -rf dragonshell;