#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define SHM_NAME "/my_file_copy_shm"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "사용법: %s <원본_파일> <대상_파일>\n", argv[0]);
        exit(1);
    }

    const char *src_path = argv[1];
    const char *dst_path = argv[2];

    // 1. 원본 파일 열기 및 크기 확인
    int src_fd = open(src_path, O_RDONLY);
    if (src_fd < 0) {
        perror("원본 파일 열기 실패");
        exit(1);
    }

    struct stat st;
    if (fstat(src_fd, &st) < 0) {
        perror("fstat 실패");
        close(src_fd);
        exit(1);
    }
    size_t file_size = st.st_size;

    if (file_size == 0) {
        // 빈 파일인 경우 대상 파일만 생성 후 종료
        int empty_fd = open(dst_path, O_CREAT | O_WRONLY | O_TRUNC, 0644);
        close(empty_fd);
        close(src_fd);
        printf("빈 파일 복사 완료.\n");
        return 0;
    }

    // 2. POSIX 공유 메모리 객체 생성 및 크기 할당
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd < 0) {
        perror("shm_open 실패");
        close(src_fd);
        exit(1);
    }
    ftruncate(shm_fd, file_size);

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork 실패");
        exit(1);
    } 
    else if (pid == 0) {
        // [자식 프로세스: 원본 파일을 mmap으로 읽어 공유 메모리에 기록]
        void *src_map = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, src_fd, 0);
        void *shm_map = mmap(NULL, file_size, PROT_WRITE, MAP_SHARED, shm_fd, 0);

        if (src_map == MAP_FAILED || shm_map == MAP_FAILED) {
            perror("자식 mmap 실패");
            exit(1);
        }

        memcpy(shm_map, src_map, file_size);

        munmap(src_map, file_size);
        munmap(shm_map, file_size);
        close(src_fd);
        close(shm_fd);
        exit(0);
    } 
    else {
        // [부모 프로세스: 자식의 쓰기 완료 대기 후 공유 메모리 -> 대상 파일로 mmap 복사]
        wait(NULL);

        int dst_fd = open(dst_path, O_CREAT | O_RDWR | O_TRUNC, st.st_mode & 0777);
        if (dst_fd < 0) {
            perror("대상 파일 생성 실패");
            exit(1);
        }
        ftruncate(dst_fd, file_size);

        void *shm_map = mmap(NULL, file_size, PROT_READ, MAP_SHARED, shm_fd, 0);
        void *dst_map = mmap(NULL, file_size, PROT_WRITE, MAP_SHARED, dst_fd, 0);

        if (shm_map == MAP_FAILED || dst_map == MAP_FAILED) {
            perror("부모 mmap 실패");
            exit(1);
        }

        memcpy(dst_map, shm_map, file_size);
        msync(dst_map, file_size, MS_SYNC); // 메모리 변경 사항을 실제 디스크에 동기화

        munmap(shm_map, file_size);
        munmap(dst_map, file_size);
        close(dst_fd);
        close(src_fd);
        close(shm_fd);
        shm_unlink(SHM_NAME); // 공유 메모리 객체 해제

        printf("파일 복사 완료: %s -> %s (%zu bytes)\n", src_path, dst_path, file_size);
    }

    return 0;
}
