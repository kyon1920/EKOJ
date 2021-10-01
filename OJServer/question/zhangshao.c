#include <stdio.h>

int main() {
    int a[5];
    char str[100];
    scanf("%s", str);
    for(int i = 0; i < 5; ++i) {
        scanf("%d", a + i);
    }
    printf("%s\n", str);
    for(int i = 0; i < 5; ++i) {
        printf("%d\n", a[i]);
    }
}