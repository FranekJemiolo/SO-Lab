all: Pascal W


Pascal: Pascal.o err.o
	cc -Wall -o Pascal Pascal.o err.o
  
Pascal.o: Pascal.c err.h
	cc -Wall -c Pascal.c
	
W: W.o err.o
	cc -Wall -o W W.o err.o
  
W.o: W.c err.h
	cc -Wall -c W.c
	
err.o: err.c err.h
	cc -Wall -c err.c
	
clean:
	rm -f *.o Pascal W
