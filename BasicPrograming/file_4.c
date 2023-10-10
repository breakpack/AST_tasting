#include <stdio.h>
#include <string.h>
#include <memory.h>


int main(void)
{
	FILE *fp;
	char file_buff[100];
	fp = fopen("test_line.txt", "w");

	if (fp == NULL) printf("파일열기 실패\n");
	else printf("파일열기 성공\n");

	int i;

	for (i = 1; i <= 5; i++) {
		printf("%d라인에 작성할 내용을 입력하세요. \n", i);
		memset(file_buff, 0, sizeof(file_buff));
    // scanf("%s", &file_buff);
		// fgets(file_buff, sizeof(file_buff), stdin); 스페이스바
		file_buff[strlen(file_buff)] = '\n';

		fputs(file_buff, fp);
	}
	fclose(fp);

	fp = fopen("test_line.txt", "r");

	if (fp == NULL) printf("파일열기 실패\n");
	else printf("파일열기 성공\n");

	while(fgets(file_buff, sizeof(file_buff), fp) != NULL){
		printf("%s", file_buff);
		memset(file_buff, 0, sizeof(file_buff));
	}
	fclose(fp);

	return 0;
}