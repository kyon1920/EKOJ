src=$(wildcard ./*.cc)
target=$(patsubst ./%.cc,./%,$(src))

object:$(target)

%:%.cc
	g++ $< -o $@ -std=c++17 -lpthread

clean:
	rm -rf $(target)