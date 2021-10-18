dbwriter:
	gcc -o dbwriter dbwriter.c $(pkg-config --libs --cflags libmongoc-1.0)