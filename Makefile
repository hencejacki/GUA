CC := g++
CFLAGS := -Wall
SRC := gua.cc activity_analyze.cc
OBJ := bin/gua.o bin/activity_analyze.o
OUT := bin/github-activity

$(OUT): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

bin/%.o: %.cc | bin
	$(CC) $(CFLAGS) -c $< -o $@

bin:
	@mkdir -p bin

clean:
	rm bin/*

.PHONY: clean bin
