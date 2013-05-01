all: 
	g++ *.cpp -o p2 -lboost_regex
cl:
	rm -f p2 *.o
run:
	./p2
test:
	perl ./difftest.pl -1 "./rpal FILE" -2 "./p2 FILE" -t ~/rpal/tests/
log:
	g++ *.cpp -o p2 > log.out 2>&1
archive:
	tar -zcvf proj2_RRC.tar.gz *.c *.h BUGS.txt README.txt rpal Makefile difftest.pl
