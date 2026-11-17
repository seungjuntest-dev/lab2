#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <signal.h>

#define FIFO_SEND "fifo_1to2"
#define FIFO_RECV "fifo_2to1"
#define MAX_TEXT 256

int main() {
    char msg_buf[MAX_TEXT];

    // 두 개의 FIFO 파이프 생성 (이미 존재하면 무시)
    mkfifo(FIFO_SEND, 0666);
    mkfifo(FIFO_RECV, 0666);

    printf("== [User 1] 채팅 대기 중 (User 2 접속 시 시작) ==\n");

    // 교착 상태(Deadlock) 방지를 위해 User 1은 쓰기 먼저, 읽기 나중 순서로 오픈
    int fd_out = open(FIFO_SEND, O_WRONLY);
    int fd_in  = open(FIFO_RECV, O_RDONLY);

    printf("== 연결 완료! 메시지를 입력하세요 (종료: /quit) ==\n");

    pid_t pid = fork();

    if (pid == 0) {
        // [자식 프로세스: 상대방 메시지 수신]
        while (1) {
            memset(msg_buf, 0, MAX_TEXT);
            int bytes = read(fd_in, msg_buf, MAX_TEXT - 1);
            if (bytes <= 0 || strcmp(msg_buf, "/quit") == 0) {
                printf("\n[알림] 상대방이 채팅을 종료했습니다.\n");
                kill(getppid(), SIGTERM);
                break;
            }
            printf("\r[User 2] : %s\n[나] : ", msg_buf);
            fflush(stdout);
        }
        exit(0);
    } else {
        // [부모 프로세스: 내 메시지 입력 및 전송]
        while (1) {
            printf("[나] : ");
            fflush(stdout);
            if (fgets(msg_buf, MAX_TEXT, stdin) == NULL) break;
            msg_buf[strcspn(msg_buf, "\n")] = '\0';

            write(fd_out, msg_buf, strlen(msg_buf) + 1);

            if (strcmp(msg_buf, "/quit") == 0) {
                kill(pid, SIGTERM);
                break;
            }
        }
    }

    close(fd_out);
    close(fd_in);
    unlink(FIFO_SEND);
    unlink(FIFO_RECV);
    return 0;
}
