CC := g++
CFLAGS := -Wall -Wno-unused-function -Iinclude -lssl -lcrypto #-D_DEBUG -g
SRC := src/gua.cc src/activity_analyze.cc src/net_util.cc
OBJ := bin/gua.o bin/activity_analyze.o bin/net_util.o
OUT := bin/github-activity
TST_SRC := test/test_json.cc
TST_CXXFLAGS := -Wall -Wno-unused-function -Iinclude -g
TST_OBJ := bin/test_json.o
TST_OUT := bin/test_json

$(OUT): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

bin/%.o: src/%.cc | bin
	$(CC) $(CFLAGS) -c $< -o $@

test_json: $(TST_OBJ)
	$(CC) $(TST_CXXFLAGS) $^ -o $(TST_OUT)

$(TST_OBJ): $(TST_SRC)
	$(CC) $(TST_CXXFLAGS) -c $< -o $@

bin:
	@mkdir -p bin

clean:
	rm bin/*

.PHONY: clean bin test_json
