CC = cc
CFLAGS = -std=c11 -Wall -Wextra -fsanitize=address,undefined -fno-omit-frame-pointer -g
LDFLAGS = -fsanitize=address,undefined

SRC = src
BUILD = build

OUT = $(BUILD)/tay
OBJ = $(BUILD)/main.o $(BUILD)/tokenizer.o $(BUILD)/parser.o

TEST_TOKENIZER = $(BUILD)/test_tokenizer
TEST_TOKENIZER_OBJ = $(BUILD)/test_tokenizer.o $(BUILD)/tokenizer.o

TEST_PARSER = $(BUILD)/test_parser
TEST_PARSER_OBJ = $(BUILD)/test_parser.o $(BUILD)/tokenizer.o $(BUILD)/parser.o

TESTS = $(TEST_TOKENIZER) $(TEST_PARSER)

all: $(OUT) $(TESTS)

$(OUT): $(OBJ)
	$(CC) $(LDFLAGS) -o $(OUT) $(OBJ)

$(TEST_TOKENIZER): $(TEST_TOKENIZER_OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

$(TEST_PARSER): $(TEST_PARSER_OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

$(BUILD)/test_tokenizer.o $(BUILD)/tokenizer.o: $(SRC)/tokenizer.h
$(BUILD)/test_parser.o: $(SRC)/parser.h $(SRC)/tokenizer.h
$(BUILD)/tokenizer.o: $(SRC)/array.h
$(BUILD)/parser.o: $(SRC)/parser.h $(SRC)/tokenizer.h $(SRC)/array.h

$(BUILD)/%.o: $(SRC)/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: tests/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD):
	mkdir -p $(BUILD)

test: $(TESTS)
	@for bin in $(TESTS); do \
		name=$$(basename $$bin | sed 's/^test_//'); \
		./tests/run.sh ./$$bin tests/inputs tests/$$name || exit 1; \
	done

watch: $(OUT)
	./$(OUT)
	find . -name '*.c' -o -name '*.h' | entr -cs 'make && ./$(OUT)'

clean:
	rm -rf $(BUILD)

.PHONY: all watch clean test
