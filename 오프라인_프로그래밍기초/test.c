void any(int a, int b)
{
    int a = 0;
    int size = scanf("%d", &a);
    int *test1 = (int *)malloc(sizeof(int) * size);
    test1[0] = 10;
    printf("%d \n", test1[0]);
}

int main(void)
{
    any(1,2);
}

