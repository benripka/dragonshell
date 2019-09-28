CC = g++


dragonshell: commands commands.h
	g++ -g dragonshell.cc commands.o

commands:
	g++ -g -c commands.cc -o commands.o

clean: 
	rm -rf *.o; \
	rm -rf dragonshell;