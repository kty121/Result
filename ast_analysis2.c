#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json_c.c"
#include "windows.h"

void get_param(json_value decl) {
    json_value args = json_get(json_get(json_get(decl, "type"), "args"), "params");
    if (args.type == JSON_NULL) {
        printf("\tNOP\n");
        return;
    }
    for (int i = 0; i < json_len(args); i++) {
        json_value arg = json_get(args, i);
        json_value argType = json_get(json_get(arg, "type"), "type");
        printf("\t%s %s\n", json_get_string(json_get(argType, "names"), 0), json_get_string(arg, "declname"));
    }
}

int get_if(json_value block_item) {
    int if_cnt = 0;
    for (int i = 0; i < json_len(block_item); i++) {
        json_value item = json_get(block_item, i);
        const char* nodetype = json_get_string(item, "_nodetype");
        if (strcmp(nodetype, "If") == 0 || strcmp(nodetype, "For") == 0 || strcmp(nodetype, "While") == 0 || strcmp(nodetype, "Stiwh") == 0) {
            if_cnt++;
            json_value stmt = json_get(item, "stmt");
            if (json_get(stmt, "block_items").type != JSON_NULL) {
                if_cnt += get_if(json_get(stmt, "block_items"));
            }
        }
    }
    return if_cnt;
}

int main() {
    FILE* file;
    file = fopen("./JsonFile.json", "rb");
    int file_size, func_cnt = 0, func_if_cnt = 0;
    char* json_data;

    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    rewind(file);

    json_data = (char*)malloc(sizeof(char) * (file_size + 1));
    fread(json_data, 1, file_size, file);
    json_data[file_size] = '\0';
    fclose(file);

    json_value json = json_create(json_data);
    json_value ext = json_get(json, "ext");
    json_value datas, body, decl, type, return_type, block_item;

    for (int i = 0; i < json_len(ext); i++) {
        datas = json_get(ext, i);
        if (strcmp(json_get_string(datas, "_nodetype"), "FuncDef") == 0) {
            func_cnt++;
            decl = json_get(datas, "decl");
            body = json_get(datas, "body");
            block_item = json_get(body, "block_items");
            type = json_get(decl, "type");
            type = json_get(type, "type");
            type = json_get(type, "type");
            return_type = json_get(type, "names");
            printf("========== %s ==========\n", json_get_string(decl, "name"));
            printf("return type: %s\n", json_get_string(return_type, 0));
            printf("function name: %s\n", json_get_string(decl, "name"));
            printf("params: \n");
            get_param(decl);
            printf("\nif count: %d", get_if(block_item));
            printf("\n==============================\n\n");
        }
    }
    printf("all func count: %d\n", func_cnt);
    free(json_data);
}