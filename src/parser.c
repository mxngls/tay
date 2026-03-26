#include <stdio.h>
#include <stdlib.h>

#include "array.h"
#include "parser.h"
#include "tokenizer.h"

int parser_parse_element(TokenArray* token_arr, size_t* pos, TayNode* out) {
    if (token_arr->items[*pos].kind == TOKEN_INDENT) {
        (*pos)++;
        int err = parser_parse_element(token_arr, pos, out);
        if (err)
            return err;
        (*pos)++;
        return 0;
    }

    if (token_arr->items[*pos].kind == TOKEN_DASH) {
        return parser_parse_list(token_arr, pos, out);
    }

    if (token_arr->len > 2 && token_arr->items[*pos].kind == TOKEN_STRING &&
        token_arr->items[*pos + 1].kind == TOKEN_COLON) {
        return parser_parse_map(token_arr, pos, out);
    }

    (*pos)++;
    return parser_parse_element(token_arr, pos, out);
}

int parser_parse_flow_element(TokenArray* token_arr, size_t* pos, TayNode* out) {
    if (token_arr->items[*pos].kind == TOKEN_LBRACKET) {
        (*pos)++;
        return parser_parse_flow_list(token_arr, pos, out);
    }

    if (token_arr->items[*pos].kind != TOKEN_STRING) {
        fprintf(stderr, "Error: string expected\n");
        return -1;
    }

    *out = ((TayNode){
        .kind = TAY_STRING,
        .string =
            (TayString){
                .str = token_arr->items[*pos].start,
                .len = token_arr->items[*pos].len,
            },
    });

    (*pos)++;

    return 0;
}

int parser_parse_flow_list(TokenArray* token_arr, size_t* pos, TayNode* out) {
    out->kind = TAY_LIST;
    out->list.items = NULL;
    out->list.cap = 0;
    out->list.len = 0;

    while (token_arr->items[*pos].kind != TOKEN_RBRACKET &&
           token_arr->items[*pos].kind != TOKEN_END) {
        array_push(&out->list, (TayNode){0});
        if (parser_parse_flow_element(token_arr, pos, &out->list.items[out->list.len - 1])) {
            return -1;
        }

        if (token_arr->items[*pos].kind == TOKEN_LBRACKET) {
            break;
        }
        if (token_arr->items[*pos].kind != TOKEN_COMMA) {
            fprintf(stderr, "Error: comma expected\n");
            return -1;
        }
        (*pos)++;
    }

    if (token_arr->items[*pos].kind == TOKEN_END) {
        fprintf(stderr, "Error: unterminated flow list\n");
        return -1;
    }

    (*pos)++;
    return 0;
}

// TODO: Finish parser implementation
int parser_parse_map(TokenArray* token_arr, size_t* pos, TayNode* out) { return -1; };
int parser_parse_list(TokenArray* token_arr, size_t* pos, TayNode* out) { return -1; };
