#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include "cJSON.c"

/*각 기능들에 대한 파라미터 타입과 이름을 추출하는 함수*/
void Function_Param(char *json_string, long i)
{
    cJSON *json = cJSON_Parse(json_string);              // JSON 문자열 파싱
    cJSON *functions = cJSON_GetObjectItem(json, "ext"); // 'ext' 항목 가져오기

    // 각 함수에서 'args' 객체와 그 안의 'params' 배열 가져오기
    cJSON *funcDecl =
        cJSON_GetObjectItem(cJSON_GetObjectItem(cJSON_GetArrayItem(functions, i), "decl"), "type");
    cJSON *parameters =
        cJSON_GetObjectItem(funcDecl, "args");

    if (parameters != NULL)
    { // args가 있는 경우만 처리
        parameters = cJSON_GetObjectItem(parameters, "params");

        // 각 파라미터를 순회하며 타입과 이름 출력하기
        for (int j = 0; j < cJSON_GetArraySize(parameters); j++)
        {
            printf("\tParameter %d type: %s\n", j + 1,
                   cJSON_PrintUnformatted(
                       cJSON_GetArrayItem(
                           cJSON_GetObjectItem(
                               cJSON_GetObjectItem(
                                   cJSON_GetArrayItem(parameters, j),
                                   "type"),
                               "names"),
                           0)));

            printf("\tParameter %d name: %s\n", j + 1,
                   cJSON_PrintUnformatted(
                       cJSON_GetObjectItem(cJSON_GetArrayItem(parameters, j), "name")));
        }
    }

    // JSON 객체 삭제하기
    cJSON_Delete(json);
}

/*각 함수의 if문 카운트*/
void IF_Count(char *json_string, long file_size, long idx)
{
    cJSON *root = cJSON_Parse(json_string);
    if (root == NULL)
    {
        printf("JSON 파싱실패 root가 존재하지 않음");
        free(json_string);
    }
    else
    {
        cJSON *ext = cJSON_GetObjectItem(root, "ext");
        if (ext == NULL)
        {
            printf("ext가 존재하지 않음");
            cJSON_Delete(root);
        }
        long arr_size = cJSON_GetArraySize(ext);
        cJSON *idx_JSON = cJSON_GetArrayItem(ext, idx);
        if (idx_JSON == NULL)
        {
            printf("idx_JSON이 존재하지 않음");
            cJSON_Delete(root);
        }

        cJSON *nodetype = cJSON_GetObjectItem(idx_JSON, "_nodetype");
        if (strcmp(nodetype->valuestring, "FuncDef") == 0)
        {
            cJSON *decl = cJSON_GetObjectItem(idx_JSON, "decl");
            cJSON *body = cJSON_GetObjectItem(idx_JSON, "body");
            cJSON *block_items = cJSON_GetObjectItem(body, "block_items");

            long sizeof_block_items = cJSON_GetArraySize(block_items);
            int count_if = 0;
            int count_elseif = 0;

            for (long block_items_idx = 0; block_items_idx < sizeof_block_items; block_items_idx++)
            {
                cJSON *items = cJSON_GetArrayItem(block_items, block_items_idx);
                cJSON *block_items_nodetype = cJSON_GetObjectItem(items, "_nodetype");
                if (strcmp(block_items_nodetype->valuestring, "If") == 0)
                {
                    count_if++;
                }
                if (count_if > 0)
                {
                    cJSON *iffalse = cJSON_GetObjectItem(items, "iffalse");
                    if (!cJSON_IsNull(iffalse))
                    {
                        cJSON *block_items_nodetype = cJSON_GetObjectItem(iffalse, "_nodetype");
                        if (cJSON_IsString(block_items_nodetype))
                        {
                            if (strcmp(block_items_nodetype->valuestring, "If") == 0)
                            {

                                count_elseif++;
                            }
                        }
                    }
                }
            }
            cJSON *name = cJSON_GetObjectItem(decl, "name");
            printf("function name : %s\n\tcount if = %d\n\tcount else if = %d\n", cJSON_Print(name), count_if, count_elseif);
        }
    }

    cJSON_Delete(root);
};

void Return_Type(char *json_string, long file_size, long idx)
{
    cJSON *root = cJSON_Parse(json_string);
    cJSON *funcExt = cJSON_GetObjectItem(root, "ext");
    cJSON *funcBlock = cJSON_GetObjectItem(cJSON_GetObjectItem(cJSON_GetArrayItem(funcExt, idx), "body"), "block_items");
    int line = cJSON_GetArraySize(funcBlock);
    for (int k = 0; k < line; k++)
    {
        cJSON *funcBlockPosition = cJSON_GetArrayItem(funcBlock, k);
        cJSON *funcFirstName = cJSON_GetObjectItem(funcBlockPosition, "_nodetype");
        if (strcmp(funcFirstName->valuestring, "Return") == 0)
        {
            cJSON *funcName = cJSON_GetObjectItem(cJSON_GetObjectItem(funcBlockPosition, "expr"), "_nodetype");
            if (strcmp(funcName->valuestring, "Constant") == 0) // return을 어떤 값으로 보내줄 때 그 보내는 값의 type을 출력
            {
                cJSON *funcType = cJSON_GetObjectItem(cJSON_GetObjectItem(funcBlockPosition, "expr"), "type");
                printf("return type: %s\n", funcType->valuestring);
            }
            else if (strcmp(funcName->valuestring, "ID") == 0) // return을 변수를 이용해 보내줄 때 그 변수명을 출력
            {
                cJSON *funcName = cJSON_GetObjectItem(cJSON_GetObjectItem(funcBlockPosition, "expr"), "name");
                printf("return name: %s\n", funcName->valuestring);
            }
            else if (strcmp(funcName->valuestring, "FuncCall") == 0) // return으로 함수 호출 할 때 호출한 함수 이름 출력
            {
                cJSON *funcFuncName = cJSON_GetObjectItem(cJSON_GetObjectItem(cJSON_GetObjectItem(funcBlockPosition, "expr"), "name"), "name");
                printf("return func name: %s\n", funcFuncName->valuestring);
            }
            else if (strcmp(funcName->valuestring, "BinaryOp") == 0) // return이 ||와 같이 여러개로 나누어져 있을 경우(현재는 패스함)
            {
                continue;
            }
            else if (strcmp(funcName->valuestring, "UnaryOp") == 0) // return이 단항 연산자가 있을 경우 expr에 한 번 더 들어가서 판단
            {
                cJSON *funcUnary = cJSON_GetObjectItem(funcBlockPosition, "expr");
                cJSON *funcUnaryName = cJSON_GetObjectItem(funcUnary, "_nodetype");
                if (strcmp(funcUnaryName->valuestring, "Constant") == 0) // return을 어떤 값으로 보내줄 때 그 보내는 값의 type을 출력
                {
                    cJSON *funcType = cJSON_GetObjectItem(funcUnary, "type");
                    printf("return type: %s\n", funcType->valuestring);
                }
                else if (strcmp(funcUnaryName->valuestring, "ID") == 0) // return을 변수를 이용해 보내줄 때 그 변수명을 출력
                {
                    cJSON *funcName = cJSON_GetObjectItem(funcUnary, "name");
                    printf("return name: %s\n", funcName->valuestring);
                }
                else if (strcmp(funcUnaryName->valuestring, "FuncCall") == 0) // return으로 함수 호출 할 때 호출한 함수 이름 출력
                {
                    cJSON *funcFuncName = cJSON_GetObjectItem(funcUnary, "name");
                    printf("return func name: %s\n", funcFuncName->valuestring);
                }
                else if (strcmp(funcUnaryName->valuestring, "BinaryOp") == 0) // return이 ||와 같이 여러개로 나누어져 있을 경우(현재는 패스함)
                {
                    continue;
                }
                printf("-------this is Unary------\n");
            }
            else if (strcmp(funcName->valuestring, "Case") == 0)
            {
                cJSON *funcStmtsName = cJSON_GetObjectItem(cJSON_GetArrayItem(cJSON_GetObjectItem(cJSON_GetObjectItem(funcBlockPosition, "expr"), "stmts"), 0), "expr");
                if (strcmp(funcStmtsName->valuestring, "Constant") == 0) // return을 어떤 값으로 보내줄 때 그 보내는 값의 type을 출력
                {
                    cJSON *funcType = cJSON_GetObjectItem(cJSON_GetObjectItem(funcBlockPosition, "expr"), "type");
                    printf("return type: %s\n", funcType->valuestring);
                }
                else if (strcmp(funcStmtsName->valuestring, "ID") == 0) // return을 변수를 이용해 보내줄 때 그 변수명을 출력
                {
                    cJSON *funcName = cJSON_GetObjectItem(cJSON_GetObjectItem(funcBlockPosition, "expr"), "name");
                    printf("return name: %s\n", funcName->valuestring);
                }
                else if (strcmp(funcStmtsName->valuestring, "FuncCall") == 0) // return으로 함수 호출 할 때 호출한 함수 이름 출력
                {
                    cJSON *funcFuncName = cJSON_GetObjectItem(cJSON_GetObjectItem(cJSON_GetObjectItem(funcBlockPosition, "expr"), "name"), "name");
                    printf("return func name: %s\n", funcFuncName->valuestring);
                }
                else if (strcmp(funcStmtsName->valuestring, "BinaryOp") == 0) // return이 ||와 같이 여러개로 나누어져 있을 경우(현재는 패스함)
                {
                    continue;
                }
            }
            else if (strcmp(funcName->valuestring, "if") == 0)
            {
                cJSON *funcIffalse = cJSON_GetObjectItem(cJSON_GetObjectItem(cJSON_GetObjectItem(funcBlockPosition, "expr"), "iffalse"), "block_items");
                cJSON *funcIftrue = cJSON_GetObjectItem(cJSON_GetObjectItem(cJSON_GetObjectItem(funcBlockPosition, "expr"), "iftrue"), "block_items");
                int iffalseLine = cJSON_GetArraySize(funcIffalse);
                int iftrueLine = cJSON_GetArraySize(funcIftrue);
                for (int n = 0; n < iffalseLine; n++)
                {
                    cJSON *funcIffalsePosition = cJSON_GetArrayItem(funcIffalse, n);
                    cJSON *funcIfName = cJSON_GetObjectItem(funcIffalsePosition, "iffalse");
                    if (cJSON_IsNull(funcIfName))
                    {
                        continue;
                    }
                    else
                    {
                        cJSON *funcIfBlock = cJSON_GetArrayItem(cJSON_GetObjectItem(funcIfName, "block_items"), 0); // 0이라는건 단일 if문이라는것 if문이 중첩되면 block_items의 크기를 알아와서 다시 if, iffalse, iftrue를 구분히야함
                        cJSON *funcIfType = cJSON_GetObjectItem(cJSON_GetObjectItem(funcIfBlock, "expr"), "type");
                        printf("%s\n", funcIfType->valuestring);
                    }
                }
                for (int m = 0; m < iftrueLine; m++)
                {
                    cJSON *funcIftruePosition = cJSON_GetArrayItem(funcIftrue, m);
                    cJSON *funcIfName = cJSON_GetObjectItem(funcIftruePosition, "iftrue");
                    if (cJSON_IsNull(funcIfName))
                    {
                        continue;
                    }
                    else
                    {
                        cJSON *funcIfBlock = cJSON_GetArrayItem(cJSON_GetObjectItem(funcIfName, "block_items"), 0); // 0이라는건 단일 if문이라는것 if문이 중첩되면 block_items의 크기를 알아와서 다시 if, iffalse, iftrue를 구분히야함
                        cJSON *funcIfType = cJSON_GetObjectItem(cJSON_GetObjectItem(funcIfBlock, "expr"), "type");
                        printf("%s\n", funcIfType->valuestring);
                    }
                }
            }
        }

        else
        {
            continue;
        }
    }
    // for(int k = 0; k < line; k++){
    //     cJSON* funcRetrun = cJSON_GetObjectItem(cJSON_GetArrayItem(funcBlock,k), "expr");
    //     if(funcRetrun != NULL){
    //         cJSON* funcType = cJSON_GetObjectItem(funcRetrun,"type");
    //         if(funcType != NULL){
    //             printf("%s\n", funcType->valuestring);
    //         }
    //         else{
    //             printf("(null)\n");
    //         }
    //     }
    // }

    cJSON_Delete(root);
}

int main(int argc, char *argv[])
{
    /*파일 열기*/
    FILE *file = fopen(argv[1], "r");
    if (file == NULL)
    {
        printf("json파일 열기 실패");
        return 0;
    }

    /*json 메모리 할당*/
    fseek(file, 0, SEEK_END);                          // 스트림 위치 끝으로 이동
    long file_size = ftell(file);                      // 스트림 현재 위치 확인
    fseek(file, 0, SEEK_SET);                          // 스트림 위치 처음으로 이동
    char *json_string = (char *)malloc(file_size + 1); // json_string에 메모리 할당

    if (json_string == NULL)
    {
        fclose(file);
        printf("메모리 할당 실패");
        return 0;
    }

    /*읽어오고 문자열 종료설정*/
    fread(json_string, 1, file_size, file); // file에서 1부터 file_size까지 읽고 json_string에 저장
    json_string[file_size] = '\0';          // 마지막 글자를 종료문자로 설정
    fclose(file);
    cJSON *root = cJSON_Parse(json_string);
    if (root == NULL)
    {
        printf("JSON 파싱실패 root가 존재하지 않음");
        free(json_string);
        return 0;
    }
    cJSON *ext = cJSON_GetObjectItem(root, "ext");
    if (ext == NULL)
    {
        printf("ext가 존재하지 않음");
        cJSON_Delete(root);
        return 0;
    }
    long arr_size = cJSON_GetArraySize(ext);
    for (long idx = 0; idx < arr_size; idx++)
    {
        printf("[*]%ld\n", idx);

        IF_Count(json_string, file_size, idx);
        Function_Param(json_string, idx);
        Return_Type(json_string, file_size, idx);
    }

    free(json_string);

    return 0;
}
