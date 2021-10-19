dbwriter: dbwriter.c
	gcc -o dbwriter dbwriter.c $(shell pkg-config --libs --cflags libmongoc-1.0)

clean:
	rm *.o
	rm dbwriter