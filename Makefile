CC = cc
CFLAGS = -std=c11 -Wall -Wextra -fsanitize=address,undefined -fno-omit-frame-pointer -g
LDFLAGS = -fsanitize=address,undefined

BUILD = build

OUT = $(BUILD)/tay
OBJ = $(BUILD)/main.o $(BUILD)/tokenizer.o

TEST_TOKENIZER = $(BUILD)/test_tokenizer
TEST_TOKENIZER_OBJ = $(BUILD)/test_tokenizer.o $(BUILD)/tokenizer.o

$(OUT): $(OBJ)
	$(CC) $(LDFLAGS) -o $(OUT) $(OBJ)

$(TEST_TOKENIZER): $(TEST_TOKENIZER_OBJ)
	$(CC) $(LDFLAGS) -o $(TEST_TOKENIZER) $(TEST_TOKENIZER_OBJ)

$(BUILD)/tokenizer.o $(BUILD)/test_tokenizer.o: tokenizer.h

$(BUILD)/%.o: %.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD):
	mkdir -p $(BUILD)

test: $(TEST_TOKENIZER)
	@./tests/run.sh ./$(TEST_TOKENIZER) tests/tokenizer

watch: $(OUT)
	./$(OUT)
	find . -name '*.c' -o -name '*.h' | entr -cs 'make && ./$(OUT)'

clean:
	rm -rf $(BUILD)

.PHONY: watch clean test
