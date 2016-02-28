all:
	make clean
	make pder
	make smp
	make alloc
	make memtester
	make echoserv
	make chatserv
	make signals

pder:
	g++ -std=c++11 -o poly_der poly_der.cpp

smp:
	g++ -std=c++11 -o smart_pointer smart_pointer.cpp

alloc:
	g++ -std=c++11 -Wformat=0 -o simple_allocator simple_allocator.cpp

memtester:
	g++ -std=c++11 -o memory_tester memory_tester.cpp

echoserv:
	g++ -std=c++11 -o echoserver echoserver.cpp

chatserv:
	g++ -std=c++11 -o chatserver chatserver.cpp

signals:
	g++ -std=c++11 -o signals signals.cpp

clean:
	@rm -f poly_der
	@rm -f smart_pointer
	@rm -f simple_allocator
	@rm -f memory_tester
	@rm -f echoserver
	@rm -f chatserver
	@rm -f signals