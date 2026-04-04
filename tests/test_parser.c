#include <stdio.h>
#include <stdlib.h>

#include "../src/parser.h"
#include "../src/tokenizer.h"

int node_kind2str(TayNode node, char** out) {
    switch (node.kind) {
    case TAY_STRING:
        *out = "STRING";
        break;
    case TAY_BLOCK_STRING:
        *out = "BLOCK_STRING";
        break;
    case TAY_LIST:
        *out = "FLOW_LIST";
        break;
    case TAY_BLOCK_LIST:
        *out = "LIST";
        break;
    case TAY_MAP:
        *out = "MAP";
        break;
    default:
        fprintf(stderr, "Error: unsupported node kind;\n");
        return -1;
    }
    return 0;
}

int print_node(int indentation, TayNode node, bool ln, bool print_val) {
    char* kind = NULL;
    if (node_kind2str(node, &kind)) {
        return -1;
    }
    int   key_len = node.key.len;
    char* key = node.key.str;
    if (print_val && (node.kind == TAY_STRING || node.kind == TAY_BLOCK_STRING)) {
        int   val_len = node.string.len;
        char* val = node.string.str;
        // clang-format off
		return fprintf(stdout, "%*s%s%s%.*s%s\"%.*s\"%s",
  		    indentation,
  		    "",
  		    kind,
  		    key_len ? " " : "",
  		    key_len,
  		    key,
  		    val_len ? " ": "",
  		    val_len,
  		    val,
  		    ln ? "\n" : "");
        // clang-format on
    } else {
        // clang-format off
        return fprintf(stdout, "%*s%s%s%.*s%s",
			indentation, 
			"", 
			kind, 
			key_len ? " " : "", 
			key_len, 
			key,
            ln ? "\n" : "");
        // clang-format on
    };
}

int print_node_recursive(TayNode* node, int indentation, int indentation_step, bool print_val) {
    if (node->kind != TAY_MAP && node->kind != TAY_BLOCK_LIST && node->kind != TAY_LIST) {
        fprintf(stderr, "Error: root node has to be map\n");
        return -1;
    }

    TayNodeChildren* children = node->kind == TAY_MAP ? &node->map : &node->list;

    for (int i = 0; i < (int)children->len; i++) {
        TayNode curr_node = children->items[i];
        char*   node_kind = NULL;
        if (node_kind2str(curr_node, &node_kind)) {
            return -1;
        }

        switch (curr_node.kind) {
        case TAY_STRING:
        case TAY_BLOCK_STRING:
            print_node(indentation, curr_node, true, print_val);
            break;
        case TAY_MAP:
        case TAY_LIST:
        case TAY_BLOCK_LIST: {
            print_node(indentation, curr_node, true, print_val);
            print_node_recursive(&curr_node, indentation + indentation_step * 2, indentation_step,
                                 print_val);
            break;
        }
        }
    }

    return 0;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "Error: expected exactly one input path");
        return -1;
    }
    char* input_file_name = *++argv;

    FILE* input_file = fopen(input_file_name, "r");
    if (!input_file) {
        fprintf(stderr, "Error opening \"%s\"\n. Exit.", input_file_name);
        return 1;
    }

    char* buf = calloc(513, sizeof(char));
    if (!buf) {
        fprintf(stderr, "Error allocating memory. Exit.");
        fclose(input_file);
        return 1;
    }

    size_t len = fread(buf, sizeof(char), 512, input_file);
    if (ferror(input_file)) {
        fprintf(stderr, "Error reading \"%s\". Exit.", input_file_name);
        free(buf);
        fclose(input_file);
        return 1;
    }
    buf[len] = '\0';
    fclose(input_file);

    TayCursor         cursor = {.data = buf, .len = len, .pos = 0, .line = 1, .col = 1};
    static TokenArray tokens = {.len = 0, .cap = 0, .items = NULL};
    if (tokenizer_tokenize(&cursor, &tokens)) {
        free(buf);
        return -1;
    };

    TayNode root = {0};
    size_t  pos = 0;
    if (parser_parse_element(&tokens, &pos, &root))
        return -1;
    ;

    int indent = 2;
    fprintf(stdout, "MAP\n");
    print_node_recursive(&root, indent, indent, true);

    free(buf);
    return 0;
}
