char plus(char a){
    return 'c';
}
int sub(int a){
    int b =a;
    return b;
}
int check(){
    int check = 100;
    if(check <= 50){
        return 50;
    }
    else{
        if(check == 100){
            return 100;
        }
        return 75;
    }

}


int main(){
    char aa = 'a';
    aa = plus(aa);
    printf("%c",aa);
    return 0;
}