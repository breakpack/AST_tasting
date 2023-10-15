#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include "cJSON.c"

// void Function_Count(char *json_string, long file_size);
void Return_Type(char *json_string, long file_size);
// void Function_Name(char *json_string, long file_size);
// void Function_Param(char *json_string, long file_size);
// void IF_Count(char *json_string, long file_size);

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

    /*읽어온 파일 확인*/
    // for (int i = 0; i < file_size; i++)
    // {
    //      printf("%c", json_string[i]);
    // }
    /*cJSON으로 root설정*/
    cJSON *root = cJSON_Parse(json_string);
    if (root == NULL)
    {
        printf("JSON 파싱실패 root가 존재하지 않음");
        free(json_string);
        return 0;
    }
    Return_Type(json_string, file_size);
    cJSON_Delete(root);

    // Function_Count(json_string, file_size);
    
    // Function_Name(json_string, file_size);
    // Function_Param(json_string, file_size);
    // IF_Count(json_string, file_size);

    free(json_string);

    return 0;
}

void Return_Type(char *json_string, long file_size)
{
    cJSON *root = cJSON_Parse(json_string);
    cJSON *funcExt = cJSON_GetObjectItem(root, "ext");
    int j = cJSON_GetArraySize(funcExt); // 함수의 갯수를 i에 저장
    for(int i = 0;i < j; i++){ // 함수를 순서대로 탐색
        cJSON* funcBlock = cJSON_GetObjectItem(cJSON_GetObjectItem(cJSON_GetArrayItem(funcExt,i), "body"), "block_items");
        int line = cJSON_GetArraySize(funcBlock);
        for(int k = 0; k < line; k++){
            cJSON* funcBlockPosition = cJSON_GetArrayItem(funcBlock,k);
            cJSON* funcFirstName = cJSON_GetObjectItem(funcBlockPosition, "_nodetype");
            if(strcmp(funcFirstName->valuestring,"Return") == 0)
            {
                cJSON* funcName = cJSON_GetObjectItem(cJSON_GetObjectItem(funcBlockPosition, "expr"),"_nodetype");
                if(strcmp(funcName->valuestring,"Constant") == 0) //return을 어떤 값으로 보내줄 때 그 보내는 값의 type을 출력
                {
                    cJSON* funcType = cJSON_GetObjectItem(cJSON_GetObjectItem(funcBlockPosition, "expr"),"type");
                    printf("return type: %s\n", funcType->valuestring);
                }
                else if(strcmp(funcName->valuestring,"ID") == 0) //return을 변수를 이용해 보내줄 때 그 변수명을 출력
                {
                    cJSON* funcName = cJSON_GetObjectItem(cJSON_GetObjectItem(funcBlockPosition, "expr"),"name");
                    printf("return name: %s\n", funcName->valuestring);
                }
                else if(strcmp(funcName->valuestring,"FuncCall") == 0) //return으로 함수 호출 할 때 호출한 함수 이름 출력
                {
                    cJSON* funcFuncName = cJSON_GetObjectItem(cJSON_GetObjectItem(cJSON_GetObjectItem(funcBlockPosition, "expr"),"name"),"name");
                    printf("return func name: %s\n", funcFuncName->valuestring);
                }
                else if(strcmp(funcName->valuestring,"BinaryOp") == 0) //return이 ||와 같이 여러개로 나누어져 있을 경우(현재는 패스함)
                {
                    continue;
                }
                else if(strcmp(funcName->valuestring,"UnaryOp") == 0) //return이 단항 연산자가 있을 경우 expr에 한 번 더 들어가서 판단
                {
                    cJSON* funcUnary = cJSON_GetObjectItem(funcBlockPosition, "expr");
                    cJSON* funcUnaryName = cJSON_GetObjectItem(funcUnary,"_nodetype");
                    if(strcmp(funcUnaryName->valuestring,"Constant") == 0) //return을 어떤 값으로 보내줄 때 그 보내는 값의 type을 출력
                    {
                        cJSON* funcType = cJSON_GetObjectItem(funcUnary,"type");
                        printf("return type: %s\n", funcType->valuestring);
                    }
                    else if(strcmp(funcUnaryName->valuestring,"ID") == 0) //return을 변수를 이용해 보내줄 때 그 변수명을 출력
                    {
                        cJSON* funcName = cJSON_GetObjectItem(funcUnary,"name");
                        printf("return name: %s\n", funcName->valuestring);
                    }
                    else if(strcmp(funcUnaryName->valuestring,"FuncCall") == 0) //return으로 함수 호출 할 때 호출한 함수 이름 출력
                    {
                        cJSON* funcFuncName = cJSON_GetObjectItem(funcUnary,"name");
                        printf("return func name: %s\n", funcFuncName->valuestring);
                    }
                    else if(strcmp(funcUnaryName->valuestring,"BinaryOp") == 0) //return이 ||와 같이 여러개로 나누어져 있을 경우(현재는 패스함)
                    {
                        continue;
                    }
                    printf("-------this is Unary------\n");
                }
                else if(strcmp(funcName->valuestring,"Case") == 0)
                {
                    cJSON* funcStmtsName = cJSON_GetObjectItem(cJSON_GetArrayItem(cJSON_GetObjectItem(cJSON_GetObjectItem(funcBlockPosition, "expr"),"stmts"),0),"expr");
                     if(strcmp(funcStmtsName->valuestring,"Constant") == 0) //return을 어떤 값으로 보내줄 때 그 보내는 값의 type을 출력
                    {
                        cJSON* funcType = cJSON_GetObjectItem(cJSON_GetObjectItem(funcBlockPosition, "expr"),"type");
                        printf("return type: %s\n", funcType->valuestring);
                    }
                    else if(strcmp(funcStmtsName->valuestring,"ID") == 0) //return을 변수를 이용해 보내줄 때 그 변수명을 출력
                    {
                        cJSON* funcName = cJSON_GetObjectItem(cJSON_GetObjectItem(funcBlockPosition, "expr"),"name");
                        printf("return name: %s\n", funcName->valuestring);
                    }
                    else if(strcmp(funcStmtsName->valuestring,"FuncCall") == 0) //return으로 함수 호출 할 때 호출한 함수 이름 출력
                    {
                        cJSON* funcFuncName = cJSON_GetObjectItem(cJSON_GetObjectItem(cJSON_GetObjectItem(funcBlockPosition, "expr"),"name"),"name");
                        printf("return func name: %s\n", funcFuncName->valuestring);
                    }
                    else if(strcmp(funcStmtsName->valuestring,"BinaryOp") == 0) //return이 ||와 같이 여러개로 나누어져 있을 경우(현재는 패스함)
                    {
                        continue;
                    }
                }
                else if(strcmp(funcName->valuestring,"if") == 0)
                {
                    cJSON* funcIffalse = cJSON_GetObjectItem(cJSON_GetObjectItem(cJSON_GetObjectItem(funcBlockPosition, "expr"),"iffalse"),"block_items");
                    cJSON* funcIftrue = cJSON_GetObjectItem(cJSON_GetObjectItem(cJSON_GetObjectItem(funcBlockPosition, "expr"),"iftrue"),"block_items");
                    int iffalseLine = cJSON_GetArraySize(funcIffalse);
                    int iftrueLine = cJSON_GetArraySize(funcIftrue);
                    for(int n = 0; n < iffalseLine; n++){
                        cJSON* funcIffalsePosition = cJSON_GetArrayItem(funcIffalse,n);
                        cJSON* funcIfName = cJSON_GetObjectItem(funcIffalsePosition,"iffalse");
                        if(cJSON_IsNull(funcIfName)){
                            continue;
                        }
                        else{
                            cJSON* funcIfBlock = cJSON_GetArrayItem(cJSON_GetObjectItem(funcIfName,"block_items"),0); // 0이라는건 단일 if문이라는것 if문이 중첩되면 block_items의 크기를 알아와서 다시 if, iffalse, iftrue를 구분히야함
                            cJSON* funcIfType = cJSON_GetObjectItem(cJSON_GetObjectItem(funcIfBlock,"expr"),"type");
                            printf("%s\n",funcIfType->valuestring);
                        }

                    }
                    for(int m = 0; m < iftrueLine; m++){
                        cJSON* funcIftruePosition = cJSON_GetArrayItem(funcIftrue,m);
                        cJSON* funcIfName = cJSON_GetObjectItem(funcIftruePosition,"iftrue");
                        if(cJSON_IsNull(funcIfName)){
                            continue;
                        }
                        else{
                            cJSON* funcIfBlock = cJSON_GetArrayItem(cJSON_GetObjectItem(funcIfName,"block_items"),0); // 0이라는건 단일 if문이라는것 if문이 중첩되면 block_items의 크기를 알아와서 다시 if, iffalse, iftrue를 구분히야함
                            cJSON* funcIfType = cJSON_GetObjectItem(cJSON_GetObjectItem(funcIfBlock,"expr"),"type");
                            printf("%s\n",funcIfType->valuestring);
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
    }
    

    cJSON_Delete(root);
}
