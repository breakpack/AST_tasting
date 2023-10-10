#include <stdio.h>

int main(void)
{
  FILE *fp;
  int c;

  fp = fopen("test.txt", "r");

  if(fp == NULL){
    printf("파일열기 실패\n");
  } else {
    printf("파일열기 성공\n");
  }
  
  // 한문자씩 읽는 fgetc
  // int count = 0;
  // while((c = fgetc(fp)) != EOF){
  //   putchar(c);
  //   count++;
  // }
  // printf("%d \n", count);

  // while(1)
  // {
  //   char temp[2];
  //   char *tStr = fgets(temp, 2, fp);
  //   if(tStr == NULL)
  //   {
  //     break;
  //   }
  //   printf("%s", temp);
  // }

  fclose(fp);
  return 0;
}



