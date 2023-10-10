#include <stdio.h>

int main(void)
{
  FILE *fp;
  int c;

  // w, a, r
  fp = fopen("test.txt", "a");

  if(fp == NULL){
    printf("파일열기 실패\n");
  } else {
    printf("파일열기 성공\n");
  }

  fclose(fp);
  return 0;
}

