#include <stdio.h>

/* Function Prototypes*/

int increase_score(int *score);

int main(){

    int score = 50;
    printf("Score before increase: %d\n", score);
    increase_score(&score);
    printf("Score after increase: %d\n", score);


}

int increase_score(int *score){
    *score += 10;
    printf("Score after increase: %d\n", *score);
    return *score;
}



