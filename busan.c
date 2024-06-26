#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define LEN_MIN 15 // 기차 길이 최소값
#define LEN_MAX 50 // 기차 길이 최대값
#define STM_MIN 0  // 마동석 체력 최소값
#define STM_MAX 5  // 마동석 체력 최대값
#define PROB_MIN 10 // 확률 최소값
#define PROB_MAX 90 // 확률 최대값
#define AGGRO_MIN 0 // 어그로 범위 최소값
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
int zombie_attacked_nobody = 0; // 좀비가 아무도 공격하지 않은 경우를 표시하는 플래그
int citizen_does_nothing = 0; // 시민이 아무 것도 하지 않은 경우를 표시하는 플래그

// 게임 초기화 함수
void initialize_game() {
    srand((unsigned int)time(NULL)); // 랜덤 시드 설정
    printf("좀비한테 살아남아라 부산헹~~\n과제 내주신 교수님 너무 서운헹~\n");

    // 기차 길이 입력받기
    do {
        printf("train length(%d~%d)>> ", LEN_MIN, LEN_MAX);
        scanf_s("%d", &train_length);
    } while (train_length < LEN_MIN || train_length > LEN_MAX);

    // 마동석 체력 입력받기
    do {
        printf("madongseok stamina(%d~%d)>> ", STM_MIN, STM_MAX);
        scanf_s("%d", &madongseok_stamina);
    } while (madongseok_stamina < STM_MIN || madongseok_stamina > STM_MAX);
    
    // 확률 입력받기
    do {
        printf("percentile probability 'p'(%d~%d)>> ", PROB_MIN, PROB_MAX);
        scanf_s("%d", &percentile_probability);
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
        citizen_does_nothing = 0;
    } else {
        aggro = aggro > AGGRO_MIN ? aggro - 1 : AGGRO_MIN; // 어그로 감소
        citizen_does_nothing = 1;
    }
}

// 좀비 이동 규칙
void move_zombie() {
    if (turn % 2 == 1 && !madongseok_action_success) { // 홀수 턴이고 마동석이 붙들기 성공하지 않은 경우
        if (aggro >= madongseok_aggro && zom > 1) {
            zom--; // 시민의 어그로가 높거나 같은 경우 시민 쪽으로 이동
        } else if (aggro < madongseok_aggro && ma < zom && zom > 1) {
            zom--; // 마동석 쪽으로 이동
        }
    }
}

// 좀비 공격 규칙
void zombie_attack() {
    int attacked = 0; // 좀비가 공격했는지 여부

    if (zom == ci + 1 && zom == ma - 1) { // 좀비가 시민과 마동석 둘 다 인접한 경우
        if (aggro >= madongseok_aggro) {
            printf("좀비가 시민을 공격했습니다! 게임 오버\n");
            exit(0); // 시민이 공격당하면 게임 오버
        } else {
            printf("좀비가 마동석을 공격했습니다!\n");
            madongseok_stamina--; // 마동석 체력 감소
            attacked = 1;
        }
    } else if (zom == ci + 1) { // 좀비가 시민과 인접한 경우
        printf("좀비가 시민을 공격했습니다! 게임 오버\n");
        exit(0); // 시민이 공격당하면 게임 오버
    } else if (zom == ma - 1) { // 좀비가 마동석과 인접한 경우
        printf("좀비가 마동석을 공격했습니다!\n");
        madongseok_stamina--; // 마동석 체력 감소
        attacked = 1;
    }

    if (!attacked) {
        printf("Zombie attacked nobody.\n");
    }

    if (madongseok_stamina <= STM_MIN) {
        printf("마동석의 체력이 바닥났습니다. 게임 오버\n");
        exit(0); // 마동석 체력이 최소값에 도달하면 게임 오버
    }
}

// 마동석 이동 규칙
void move_madongseok() {
    int move;
    if (ma == zom + 1) {
        // 마동석이 좀비와 인접해 있으면 대기만 가능
        printf("madongseok move(0:stay)>> ");
        scanf_s("%d", &move);
        while (move != MOVE_STAY) {
            printf("마동석이 좀비와 인접해 있어 대기만 가능합니다. 다시 입력하세요.\n");
            printf("madongseok move(0:stay)>> ");
            scanf_s("%d", &move);
        }
    } else {
        // 마동석이 좀비와 인접해 있지 않으면 이동 가능
        printf("madongseok move(0:stay, 1:left)>> ");
        scanf_s("%d", &move);
        while (move != MOVE_STAY && move != MOVE_LEFT) {
            printf("잘못된 입력입니다. 다시 입력하세요.\n");
            printf("madongseok move(0:stay, 1:left)>> ");
            scanf_s("%d", &move);
        }

        if (move == MOVE_LEFT && ma > 1) {
            ma--; // 왼쪽으로 이동
            madongseok_aggro = madongseok_aggro < AGGRO_MAX ? madongseok_aggro + 1 : AGGRO_MAX; // 어그로 증가
        }
    }
}

// 마동석 행동 규칙
void action_madongseok() {
    int action;
    if (ma != zom + 1) { // 좀비와 인접하지 않은 경우에만 선택 가능
        printf("madongseok action(0:rest, 1:provoke)>> ");
        scanf_s("%d", &action);
        while (action != ACTION_REST && action != ACTION_PROVO) {
            printf("잘못된 입력입니다. 다시 입력하세요.\n");
            printf("madongseok action(0:rest, 1:provoke)>> ");
            scanf_s("%d", &action);
        }

        if (action == ACTION_REST) { // 휴식
            madongseok_aggro = madongseok_aggro > AGGRO_MIN ? madongseok_aggro - 1 : AGGRO_MIN; // 어그로 감소
            madongseok_stamina = madongseok_stamina < STM_MAX ? madongseok_stamina + 1 : STM_MAX; // 체력 증가
        } else if (action == ACTION_PROVO) { // 도발
            madongseok_aggro = AGGRO_MAX; // 어그로 최대치로 증가
        }
    }
}

// 현재 시민의 상태를 출력하는 함수
void citizen_status(int before_ci) {
    printf("citizen: %s %d -> %d (aggro: %d)\n", before_ci == ci ? "stay" : "move", before_ci, ci, aggro);
}
// 현재 좀비의 상태를 출력하는 함수
void zom_status(int before_zom) {
    if (turn % 2 != 0)
        printf("zombie: %s %d -> %d\n", before_zom == zom ? "stay" : "move", before_zom, zom);
    else
        printf("zombie: stay %d (cannot move)\n", zom);
}
// 현재 마동석의 상태를 출력하는 함수
void ma_status(int before_ma) {
    printf("madongseok: %s %d -> %d (aggro: %d, stamina: %d)\n\n",
     before_ma == ma ? "stay" : "move", before_ma, ma, madongseok_aggro, madongseok_stamina);
}

// 메인 함수
int main(void) {
    initialize_game(); // 게임 초기화
    print_train(); // 초기 기차 상태 출력

    while (1) {
        int before_ci = ci, before_zom = zom, before_ma = ma;

        before_ci = ci; // 이동 전 상태 저장
        m_citizen(); // 시민 이동
        
        before_zom = zom; // 이동 전 상태 저장
        move_zombie(); // 좀비 이동

        print_train(); // 이동 후 기차 상태 출력
        citizen_status(before_ci); // 시민 상태 출력
        zom_status(before_zom); // 좀비 상태 출력

        before_ma = ma; // 이동 전 상태 저장
        move_madongseok(); // 마동석 이동

        print_train(); // 이동 후 기차 상태 출력
        ma_status(before_ma); // 마동석 상태 출력

        zombie_attack(); // 좀비 공격

        if (citizen_does_nothing) {
            printf("citizen does nothing.\n");
        }

        if (zombie_attacked_nobody) {
            printf("Zombie attacked nobody.\n");
        }

        action_madongseok(); // 마동석 행동
        ma_status(before_ma); // 마동석 상태 출력

        if (ci == 1) {
            printf("탈출 성공~!\n");
            break; // 시민이 탈출했을 경우
        }
        if (ci == zom - 1) {
            printf("탈출 실패ㅠ~ㅠ\n");
            break; // 좀비에게 잡혔을 경우
        }

        turn++; // 턴 증가
    }
    return 0;
}
