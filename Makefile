predict:
	$(CC) main.c libpredict.c -o predict -lm -lncurses -pthread
