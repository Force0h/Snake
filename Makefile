snake.o: snake.c 
	clang $^ -lncurses -o $@