all : comp decomp

comp : comp.o fastlz.o
	gcc -o comp comp.o fastlz.o
decomp :decomp.o fastlz.o
	gcc -o decomp decomp.o fastlz.o


comp.o : lib_fastlz/fastlz.h comp.c
	gcc -c comp.c

decomp.o : lib_fastlz/fastlz.h decomp.c
	gcc -c  decomp.c

fastlz.o : lib_fastlz/fastlz.h lib_fastlz/fastlz.c
	gcc -c lib_fastlz/fastlz.c

clean : 
	rm -f *.o
	rm -f comp decomp
