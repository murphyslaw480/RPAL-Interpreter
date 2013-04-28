all: 
	g++ *.cpp -o p2 -lboost_regex
cl:
	rm -f p2 *.o
run:
	./p2
test:
	perl ./difftest.pl -1 "./rpal -st -noout FILE" -2 "./p2 -st FILE" -t ~/rpal/tests/
log:
	g++ *.cpp -o p2 > log.out 2>&1
