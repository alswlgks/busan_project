#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LEN_MIN 15 // 기차 길이 최소값
#define LEN_MAX 50 // 기차 길이 최대값
#define STM_MIN 0  // 마동석 체력 최소값
#define STM_MAX 5  // 마동석 체력 최대값
#define PROB_MIN 10 // 확률 최소값
#define PROB_MAX 90 // 확률 최대값
#define AGGRO_MIN 1 // 어그로 범위 최소값
#define AGGRO_MAX 5 // 어그로 범위 최대값

// 마동석 이동 방향 정의
#define MOVE_LEFT 1
#define MOVE_STAY 0

// 마동석 행동 정의
#define ACTION_REST 1
#define ACTION_PROVO 2

int train_length, madongseok_stamina, percentile_probability, turn = 1;
int ma, zom, ci;
int aggro = 1; // 시민의 어그로 값
int madongseok_aggro = 1; // 마동석의 어그로 값
int madongseok_action_success = 0; // 마동석의 '붙들기' 성공 여부 (기본값 : false)

// 게임 초기화 함수
void initialize_game() {
    srand((unsigned int)time(NULL)); // 랜덤 시드 설정
    printf("좀비한테 살아남아라 부산헹~~\n과제 내주신 교수님 너무 서운헹~\n");

    // 기차 길이 입력받기
    do {
        printf("train length(%d~%d)>> ", LEN_MIN, LEN_MAX);
        scanf("%d", &train_length);
    } while (train_length < LEN_MIN || train_length > LEN_MAX);

    // 마동석 체력 입력받기
    do {
        printf("madongseok stamina(%d~%d)>> ", STM_MIN, STM_MAX);
        scanf("%d", &madongseok_stamina);
    } while (madongseok_stamina < STM_MIN || madongseok_stamina > STM_MAX);
    
    // 확률 입력받기
    do {
        printf("percentile probability 'p'(%d~%d)>> ", PROB_MIN, PROB_MAX);
        scanf("%d", &percentile_probability);
    } while (percentile_probability < PROB_MIN || percentile_probability > PROB_MAX);

    // 초기 위치 설정
    ma = train_length - 2;
    zom = train_length - 3;
    ci = train_length - 6;
}

// 기차 상태를 출력하는 함수
void print_train() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < train_length; j++) {
            if (i == 0 || i == 2 || j == 0 || j == train_length - 1)
                printf("#"); // 기차의 경계 출력
            else if (j == ma)
                printf("M"); // 마동석 위치 출력
            else if (j == zom)
                printf("Z"); // 좀비 위치 출력
            else if (j == ci)
                printf("C"); // 시민 위치 출력
            else
                printf(" "); // 빈 공간 출력
        }
        printf("\n");
    }
    printf("\n");
}

// 시민 이동 규칙
void m_citizen() {
    int r_move_c = rand() % 100;
    if (r_move_c <= 100 - percentile_probability) {
        --ci;
        aggro = aggro < AGGRO_MAX ? aggro + 1 : AGGRO_MAX; // 어그로 증가
    } else {
        aggro = aggro > AGGRO_MIN ? aggro - 1 : AGGRO_MIN; // 어그로 감소
    }
}

// 좀비 이동 규칙
void move_zombie() {
    if (turn % 2 != 1 && !madongseok_action_success) { // 홀수 턴이고 마동석이 붙들기 성공하지 않은 경우
      
        if (aggro <=  madongseok_aggro && zom > 1){
            zom--; // 시민의 어그로가 높으면 시민 쪽으로 이동
        }
        else if (ma < zom && zom > 1)
            zom--; // 마동석 쪽으로 이동
             if (ci == zom - 1 || ma == zom + 1) {// 좀비가 시민 또는 마동석과 인접해 있는 경우 이동하지 않음 
            return;
        }
        
    }
}

// 좀비 공격 규칙
void zombie_attack() {
    if (zom == ci + 1 && zom == ma - 1) { // 좀비가 시민과 마동석 둘 다 인접한 경우
        if (aggro >= madongseok_aggro) {
            printf("좀비가 시민을 공격했습니다! 게임 오버\n");
            exit(0); // 시민이 공격당하면 게임 오버
        } else {
            printf("좀비가 마동석을 공격했습니다!\n");
            madongseok_stamina--; // 마동석 체력 감소
        }
    } else if (zom == ci + 1) { // 좀비가 시민과 인접한 경우
        printf("좀비가 시민을 공격했습니다! 게임 오버\n");
        exit(0); // 시민이 공격당하면 게임 오버
    } else if (zom == ma - 1) { // 좀비가 마동석과 인접한 경우
        printf("좀비가 마동석을 공격했습니다!\n");
        madongseok_stamina--; // 마동석 체력 감소
    }
    if (madongseok_stamina <= STM_MIN) {
        printf("마동석의 체력이 바닥났습니다. 게임 오버\n");
        exit(0); // 마동석 체력이 최소값에 도달하면 게임 오버
    }
}

// 마동석 이동 규칙
void move_madongseok() {
    int move;
    do {
        printf("madongseok move(0:stay, 1:left)>> ");
        scanf("%d", &move);
    } while (move != MOVE_STAY && move != MOVE_LEFT);

    if (move == MOVE_LEFT && ma > 1) {
        ma--; // 왼쪽으로 이동
    }
}

// 마동석 행동 규칙
void action_madongseok() {
    int action;
    if (ma != zom + 1) { // 좀비와 인접하지 않은 경우에만 선택 가능
        do {
            printf("madongseok action(0:rest, 1:provoke)>> ");
            scanf("%d", &action);
        } while (action != ACTION_REST && action != ACTION_PROVO);

        if (action == ACTION_REST) { // 휴식
            madongseok_aggro = madongseok_aggro > AGGRO_MIN ? madongseok_aggro - 1 : AGGRO_MIN; // 어그로 감소
            madongseok_stamina = madongseok_stamina < STM_MAX ? madongseok_stamina + 1 : STM_MAX; // 체력 증가
        } else if (action == ACTION_PROVO) { // 도발
            madongseok_aggro = AGGRO_MAX; // 어그로 최대치로 증가
        }
    }
}

// 현재 시민의 상태를 출력하는 함수
void citizen_status(int before_ci ) {
    printf("citizen: %s %d -> %d (aggro: %d)\n", before_ci == ci ? "stay" : "move", before_ci, ci, aggro);
}
// 현재 좀비의 상태를 출력하는 함수
void zom_status(int before_zom){
    if (turn % 2 != 0)
        printf("zombie: %s %d\n", before_zom == zom ? "stay" : "move", zom);
    else
        printf("zombie: stay %d (cannot move)\n", zom);
}
//현재 마동석의 상태를 출력하는 함수
void ma_status(int before_ma){
    printf("madongseok: %s %d (aggro: %d, stamina: %d)\n\n",
     before_ma == ma ? "stay" : "move", ma, madongseok_aggro, madongseok_stamina);
}
// 메인 함수
int main(void) {
    initialize_game(); // 게임 초기화
    print_train(); // 초기 기차 상태 출력

    while (1) {
        int before_ci = ci, before_zom = zom, before_ma = ma;
        
        
        
        m_citizen(); // 시민 이동
       
        before_ci = ci; // 이동 후 상태 저장
        
        move_zombie(); // 좀비 이동
       
        before_zom = zom; // 이동 후 상태 저장

        print_train(); // 이동 후 기차 상태 출력
        citizen_status(before_ci); // 시민 상태 출력
        zom_status(before_zom); // 좀비 상태 출력

        move_madongseok(); // 마동석 이동
        
        before_ma = ma; // 이동 후 상태 저장
        
        print_train(); // 이동 후 기차 상태 출력
        ma_status(before_ma); // 마동석 상태 출력
        
        citizen_status(before_ci); // 시민 상태 출력
        zom_status(before_zom); // 좀비 상태 출력
        
        zombie_attack(); // 좀비 공격
        
        action_madongseok(); // 마동석 행동
        ma_status(before_ma); // 마동석 상태 출력

        if (ci == 1) { printf("탈출 성공~!\n"); break; } // 시민이 탈출했을 경우
        if (ci == zom - 1) { printf("탈출 실패ㅠ~ㅠ\n"); break; } // 좀비에게 잡혔을 경우
        turn++; // 턴 증가
    }
    return 0;
}
