all:
	make clean
	make pder
	make smp
	make alloc

pder:
	g++ -std=c++11 -o poly_der poly_der.cpp

smp:
	g++ -std=c++11 -o smart_pointer smart_pointer.cpp

alloc:
	g++ -std=c++11 -o simple_allocator simple_allocator.cpp

clean:
	@rm -f poly_der
	@rm -f smart_pointer
