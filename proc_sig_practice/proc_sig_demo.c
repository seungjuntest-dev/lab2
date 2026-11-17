#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

// 시그널 수신 시 실행될 핸들러 함수
void custom_sig_handler(int signo) {
    if (signo == SIGUSR1) {
        printf("\n[자식 프로세스] 부모로부터 SIGUSR1 시그널을 받았습니다!\n");
    } else if (signo == SIGINT) {
        printf("\n[알림] Ctrl+C (SIGINT) 감지 했지만 종료하지 않고 계속 진행합니다.\n");
    }
}

int main() {
    pid_t child_pid;
    int status;

    // 1. 시그널 핸들러 설정 (sigaction 사용)
    struct sigaction sa;
    sa.sa_handler = custom_sig_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);

    printf("=== 프로세스 및 시그널 실습 시작 (부모 PID: %d) ===\n", getpid());

    // 2. 자식 프로세스 생성 (fork)
    child_pid = fork();

    if (child_pid < 0) {
        perror("fork 실패");
        exit(1);
    } 
    else if (child_pid == 0) {
        // [자식 프로세스 영역]
        printf("[자식 프로세스] 생성 완료 (PID: %d, 부모 PID: %d)\n", getpid(), getppid());
        printf("[자식 프로세스] 부모의 시그널을 기다리는 중... (pause)\n");
        
        pause(); // 시그널이 도착할 때까지 대기
        
        printf("[자식 프로세스] 작업을 마치고 종료 코드 42로 종료합니다.\n");
        exit(42);
    } 
    else {
        // [부모 프로세스 영역]
        sleep(1); // 자식이 pause() 상태에 들어갈 때까지 잠시 대기
        
        printf("[부모 프로세스] 자식(PID: %d)에게 SIGUSR1 시그널을 전송합니다.\n", child_pid);
        kill(child_pid, SIGUSR1); // 자식에게 시그널 전송 (kill)

        // 3. 자식 프로세스가 종료될 때까지 대기 및 상태 회수 (wait)
        pid_t ended_pid = wait(&status);
        if (WIFEXITED(status)) {
            printf("[부모 프로세스] 자식(PID: %d) 정상 종료 확인 (반환값: %d)\n", 
                   ended_pid, WEXITSTATUS(status));
        }
    }

    return 0;
}
