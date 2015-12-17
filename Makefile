all:
	make clean
	make pder
	make smp

pder:
	g++ -std=c++11 -o poly_der poly_der.cpp

smp:
	g++ -std=c++11 -o smart_pointer smart_pointer.cpp

clean:
	@rm -f poly_der
	@rm -f smart_pointer
