all: smartPointers
solution: smartPointers.sol

clean:
	rm -f *o *so smartPointers *~ smartPointers.sol

% : %.cpp
	$(CXX) -g -std=c++2a -Wall -Wextra -o $@ $<

%.sol : solution/%.sol.cpp
	$(CXX) -g -std=c++2a -Wall -Wextra -o $@ $<
