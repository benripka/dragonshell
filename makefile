CC = g++


dragonshell:
	g++ -g dragonshell.cc

clean: 
	rm -rf *.o; \
	rm -rf a.out;