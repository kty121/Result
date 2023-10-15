#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"

void extractFunctionInfo(cJSON *jsonObject) {
    if (jsonObject == NULL || !cJSON_IsArray(jsonObject)) {
        printf("Invalid JSON format.\n");
        return;
    }

    int functionCount = cJSON_GetArraySize(jsonObject);
    printf("Number of functions: %d\n", functionCount);

    for (int i = 0; i < functionCount; ++i) {
        cJSON *function = cJSON_GetArrayItem(jsonObject, i);
        cJSON *returnType = cJSON_GetObjectItem(function, "return_type");
        cJSON *name = cJSON_GetObjectItem(function, "name");
        cJSON *parameters = cJSON_GetObjectItem(function, "parameters");
        cJSON *ifConditions = cJSON_GetObjectItem(function, "if_conditions");

        if (returnType && name && parameters && ifConditions) {
            printf("Function %d:\n", i + 1);
            printf("Return Type: %s\n", returnType->valuestring);
            printf("Name: %s\n", name->valuestring);

            cJSON *paramArray = cJSON_GetObjectItem(parameters, "params");
            int paramCount = cJSON_GetArraySize(paramArray);
            printf("Number of Parameters: %d\n", paramCount);
            printf("Parameters:\n");
            for (int j = 0; j < paramCount; ++j) {
                cJSON *param = cJSON_GetArrayItem(paramArray, j);
                cJSON *paramType = cJSON_GetObjectItem(param, "type");
                cJSON *paramName = cJSON_GetObjectItem(param, "name");
                printf("    Type: %s, Name: %s\n", paramType->valuestring, paramName->valuestring);
            }

            int ifCount = cJSON_GetArraySize(ifConditions);
            printf("Number of if conditions: %d\n", ifCount);
        } else {
            printf("Invalid function format at index %d.\n", i);
        }
        printf("\n");
    }
}

int main() {
    FILE *file = fopen("JsonFile.json", "rb");
    if (file == NULL) {
        perror("Error opening JSON file");
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *jsonString = (char *)malloc(fileSize + 1);
    fread(jsonString, 1, fileSize, file);
    jsonString[fileSize] = '\0';
    fclose(file);
    //puts(jsonString);

    cJSON *jsonObject = cJSON_Parse(jsonString);
    if (jsonObject == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        free(jsonString);
        return 1;
    }

    cJSON *functions = cJSON_GetObjectItem(jsonObject, "functions");
    extractFunctionInfo(functions);

    cJSON_Delete(jsonObject);
    free(jsonString);

    return 0;
}