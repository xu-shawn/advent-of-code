all:
	g++ -std=c++17 -O3 solution.cpp -o run -fprofile-generate="solution_pgo"
	./run
	g++ -std=c++17 -O3 solution.cpp -o run -fprofile-use="solution_pgo"
	rm -rf solution_pgo
