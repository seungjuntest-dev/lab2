#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>     // getcwd, chdir, write, read, close, rmdir
#include <fcntl.h>      // open, O_CREAT, O_RDWR 등
#include <sys/stat.h>   // mkdir
#include <dirent.h>     // opendir, readdir, closedir

int main() {
    char cwd[256];
    char buf[100] = {0};

    // 1. 현재 작업 디렉토리 확인 (getcwd)
    getcwd(cwd, sizeof(cwd));
    printf("1. 시작 위치: %s\n", cwd);

    // 2. 새 디렉토리 생성 (mkdir) 및 이동 (chdir)
    mkdir("test_dir", 0755);
    chdir("test_dir");
    getcwd(cwd, sizeof(cwd));
    printf("2. 이동 후 위치: %s\n", cwd);

    // 3. 파일 생성 및 데이터 쓰기 (open, write, close)
    int fd = open("hello.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    const char *msg = "Hello Linux File & Directory!";
    write(fd, msg, strlen(msg));
    close(fd);
    printf("3. hello.txt 파일 생성 및 쓰기 완료\n");

    // 4. 파일 다시 열어서 내용 읽기 (open, read, close)
    fd = open("hello.txt", O_RDONLY);
    read(fd, buf, sizeof(buf) - 1);
    close(fd);
    printf("4. 읽은 파일 내용: %s\n", buf);

    // 5. 현재 디렉토리 내부 목록 출력 (opendir, readdir, closedir)
    printf("5. 디렉토리 내부 목록:\n");
    DIR *dir = opendir(".");
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        printf("   - %s\n", entry->d_name);
    }
    closedir(dir);

    // 6. 실습 후 생성한 파일 및 폴더 정리 (unlink, chdir, rmdir)
    unlink("hello.txt"); // 파일 삭제
    chdir("..");         // 상위 폴더로 복귀
    rmdir("test_dir");   // 빈 디렉토리 삭제
    printf("6. 테스트 파일 및 디렉토리 정리 완료\n");

    return 0;
}
