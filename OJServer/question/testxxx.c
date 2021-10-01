#include <stdio.h>

int main() {
    printf("Hello world!\n");
    int i, j;
    int x[10];
    int y[100000];
    int k;
    for(k = 0; k < 100000; ++k) {
        // printf("xxxxx\n");
        y[k] = k;
    }
    for(j = 0; j < 10; ++j) {
        scanf("%d", x + j);
        printf("xxxxxx%d\n", x[j]);
    }
    for(i = 0; i < 100000000; ++i) {printf("xxxxxxxxxxxxxx"); }
    return 0;
}
