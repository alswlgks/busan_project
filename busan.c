#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define LEN_MIN 15
#define LEN_MAX 50
#define PROB_MIN 10
#define PROB_MAX 90

int train_length, percentile_probability, turn = 1;
int ma, zom, ci;

void initialize_game() {
    srand((unsigned int)time(NULL));
    printf("좀비한테 살아남아라 부산헹~~\n과제 내주신 교수님 너무 서운헹~\n");
    printf("train length(%d~%d)>> ", LEN_MIN, LEN_MAX); 
    scanf("%d", &train_length);
    printf("percentile probability 'p'(%d~%d)>> ", PROB_MIN, PROB_MAX); 
    scanf("%d", &percentile_probability);
    if (train_length < LEN_MIN || train_length > LEN_MAX || percentile_probability < PROB_MIN || percentile_probability > PROB_MAX) exit(1);
    ma = train_length - 2;
    zom = train_length - 3;
    ci = train_length - 6;
}

void print_train() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < train_length; j++) {
            if (i == 0 || i == 2 || j == 0 || j == train_length - 1) printf("#");
            else if (j == ma) printf("M");
            else if (j == zom) printf("Z");
            else if (j == ci) printf("C");
            else printf(" ");
            if (j == train_length - 1) printf("\n");
        }
    }
    printf("\n");
}

void move_entities() {
    int randam = rand() % 101;
    if (turn % 2 != 0 && randam <= percentile_probability && zom > 1) zom--;
    randam = rand() % 101;
    if (randam <= 100 - percentile_probability && ci > 1) ci--;
}

void print_status(int before_ci, int before_zom) {
    printf("citizen: %s %d\n", before_ci == ci ? "stay" : "move", ci);
    if (turn % 2 != 0)
        printf("zombie: %s %d\n\n", before_zom == zom ? "stay" : "move", zom);
    else
        printf("zombie: stay %d (cannot move)\n\n", zom);
}

int main(void) {
    initialize_game();
    while (1) {
        int before_ci = ci, before_zom = zom;
        move_entities();
        print_train();
        print_status(before_ci, before_zom);
        if (ci == 1) { printf("탈출 성공~!"); break; }
        if (ci == zom - 1) { printf("탈출 실패ㅠ~ㅠ"); break; }
        turn++;
        sleep(4);
    }
    return 0;
}
