mmu: main.cpp random.cpp readinput.cpp datastructures.h simulate.cpp pager.h
	g++ -o mmu main.cpp random.cpp readinput.cpp datastructures.h simulate.cpp pager.h

clean:
	rm mmu