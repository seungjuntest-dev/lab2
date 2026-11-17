#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MSG_SIZE 128

int main() {
    int p2c_fd[2]; // 부모 -> 자식 전송용 파이프 (0: 읽기, 1: 쓰기)
    int c2p_fd[2]; // 자식 -> 부모 응답용 파이프 (0: 읽기, 1: 쓰기)
    char recv_buf[MSG_SIZE];

    // 1. 두 개의 파이프 생성
    if (pipe(p2c_fd) == -1 || pipe(c2p_fd) == -1) {
        perror("pipe 생성 실패");
        exit(1);
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork 실패");
        exit(1);
    } 
    else if (pid == 0) {
        // [자식 프로세스]
        close(p2c_fd[1]); // 부모->자식 쓰기 끝 닫기
        close(c2p_fd[0]); // 자식->부모 읽기 끝 닫기

        // 부모가 보낸 데이터 읽기
        read(p2c_fd[0], recv_buf, sizeof(recv_buf));
        printf("[자식 프로세스] 수신 데이터: \"%s\"\n", recv_buf);

        // 부모에게 응답 데이터 쓰기
        const char *reply = "정상 수신 완료 (From Child)";
        write(c2p_fd[1], reply, strlen(reply) + 1);

        close(p2c_fd[0]);
        close(c2p_fd[1]);
        exit(0);
    } 
    else {
        // [부모 프로세스]
        close(p2c_fd[0]); // 부모->자식 읽기 끝 닫기
        close(c2p_fd[1]); // 자식->부모 쓰기 끝 닫기

        // 자식에게 데이터 전송
        const char *send_msg = "IPC 파이프 통신 테스트 메시지 (From Parent)";
        printf("[부모 프로세스] 자식으로 데이터 전송 중...\n");
        write(p2c_fd[1], send_msg, strlen(send_msg) + 1);

        // 자식의 응답 대기 및 읽기
        read(c2p_fd[0], recv_buf, sizeof(recv_buf));
        printf("[부모 프로세스] 자식 응답 확인: \"%s\"\n", recv_buf);

        close(p2c_fd[1]);
        close(c2p_fd[0]);
        wait(NULL); // 자식 프로세스 종료 대기
    }

    return 0;
}
