#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"

void Function_Count(char *json_string, long file_size);
void Return_Type(char *json_string, long file_size);
void Function_Name(char *json_string, long file_size);
void Function_Param(char *json_string, long file_size);
void IF_Count(char *json_string, long file_size);

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
    //     printf("%c", json_string[i]);
    // }
    /*cJSON으로 root설정*/
    // cJSON *root = cJSON_Parse(json_string);
    // if (root == NULL)
    // {
    //     printf("JSON 파싱실패 root가 존재하지 않음");
    //     free(json_string);
    //     return 0;
    // }
    // cJSON_Delete(root);

    Function_Count(json_string, file_size);
    Return_Type(json_string, file_size);
    Function_Name(json_string, file_size);
    Function_Param(json_string, file_size);
    IF_Count(json_string, file_size);

    free(json_string);

    return 0;
}