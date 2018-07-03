cc = g++
prom = code
deps = main.cpp

$(prom): $(deps)
	g++ -o $(prom) $(deps) -O2 -std=c++14

%.o: %.c $(deps)
	$(cc) -c $< -o $@

clean:
	rm -rf $(obj) $(prom)