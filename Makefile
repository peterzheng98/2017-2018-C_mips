cc = g++
prom = code
deps = $(shell find ./ -name "*.h")
src = $(shell find ./ -name "*.c")
obj = $(src:%.c=%.o) 

$(prom): $(obj)
    $(cc) -o $(prom) $(obj) -O2

%.o: %.c $(deps)
    $(cc) -c $< -o $@

clean:
    rm -rf $(obj) $(prom)