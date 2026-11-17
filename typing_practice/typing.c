#include <stdio.h>
#include <string.h>
#include <time.h>

#define SENTENCE_CNT 3
#define BUF_SIZE 256

int main() {
    const char *sample_texts[SENTENCE_CNT] = {
        "System programming requires understanding of OS internals.",
        "Processes communicate via pipes, message queues, and shared memory.",
        "Always check return values of Linux system calls."
    };

    char user_buf[BUF_SIZE];
    int wrong_cnt = 0;
    int total_chars = 0;
    struct timespec ts_start, ts_end;

    printf("=== 리눅스 타자 연습기 ===\n");
    printf("총 %d개의 문장을 따라 입력하세요.\n\n", SENTENCE_CNT);

    clock_gettime(CLOCK_MONOTONIC, &ts_start); // 시작 시간 측정

    for (int i = 0; i < SENTENCE_CNT; i++) {
        printf("문장 %d: %s\n", i + 1, sample_texts[i]);
        printf("입력  : ");

        if (fgets(user_buf, sizeof(user_buf), stdin) == NULL) {
            break;
        }
        user_buf[strcspn(user_buf, "\n")] = '\0';

        int origin_len = strlen(sample_texts[i]);
        int typed_len = strlen(user_buf);
        int limit = (origin_len > typed_len) ? origin_len : typed_len;
        int line_err = 0;

        for (int j = 0; j < limit; j++) {
            if (j >= origin_len || j >= typed_len || sample_texts[i][j] != user_buf[j]) {
                line_err++;
            }
        }

        wrong_cnt += line_err;
        total_chars += typed_len;
        printf(" -> 오타: %d개\n\n", line_err);
    }

    clock_gettime(CLOCK_MONOTONIC, &ts_end); // 종료 시간 측정

    // 초(tv_sec)와 나노초(tv_nsec)를 합산하여 경과 시간 계산
    double duration = (ts_end.tv_sec - ts_start.tv_sec) + 
                      (ts_end.tv_nsec - ts_start.tv_nsec) / 1e9;
    double cpm = (duration > 0) ? (total_chars / duration) * 60.0 : 0.0;

    printf("=== 측정 결과 ===\n");
    printf("소요 시간   : %.2f초\n", duration);
    printf("총 오타 수  : %d회\n", wrong_cnt);
    printf("평균 타수   : %.1f 타/분\n", cpm);

    return 0;
}
