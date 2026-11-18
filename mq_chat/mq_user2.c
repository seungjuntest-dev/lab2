#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mqueue.h>
#include <signal.h>
#include <fcntl.h>

#define MQ_RECV "/mq_chat_1to2"
#define MQ_SEND "/mq_chat_2to1"
#define MSG_CAP 256

int main() {
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MSG_CAP;
    attr.mq_curmsgs = 0;

    mqd_t mq_in  = mq_open(MQ_RECV, O_CREAT | O_RDONLY, 0666, &attr);
    mqd_t mq_out = mq_open(MQ_SEND, O_CREAT | O_WRONLY, 0666, &attr);

    if (mq_in == (mqd_t)-1 || mq_out == (mqd_t)-1) {
        perror("mq_open 실패");
        exit(1);
    }

    printf("== [User 2] 채팅 시작 (종료: /bye) ==\n");

    char text_buf[MSG_CAP];
    pid_t pid = fork();

    if (pid == 0) {
        // [자식 프로세스: 메시지 수신 전담]
        while (1) {
            memset(text_buf, 0, MSG_CAP);
            if (mq_receive(mq_in, text_buf, MSG_CAP, NULL) >= 0) {
                if (strcmp(text_buf, "/bye") == 0) {
                    printf("\n[안내] 상대방이 대화방을 나갔습니다.\n");
                    kill(getppid(), SIGTERM);
                    break;
                }
                printf("\r[상대방] : %s\n[나] : ", text_buf);
                fflush(stdout);
            }
        }
        exit(0);
    } else {
        // [부모 프로세스: 키보드 입력 및 송신 전담]
        while (1) {
            printf("[나] : ");
            fflush(stdout);
            if (fgets(text_buf, MSG_CAP, stdin) == NULL) break;
            text_buf[strcspn(text_buf, "\n")] = '\0';

            mq_send(mq_out, text_buf, strlen(text_buf) + 1, 0);

            if (strcmp(text_buf, "/bye") == 0) {
                kill(pid, SIGTERM);
                break;
            }
        }
    }

    mq_close(mq_in);
    mq_close(mq_out);
    return 0;
}
