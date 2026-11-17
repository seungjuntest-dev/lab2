#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

void list_dir_recursive(const char *base_path) {
    DIR *dir;
    struct dirent *entry;
    struct stat statbuf;
    char path[1024];

    // 1. 디렉토리 열기
    if ((dir = opendir(base_path)) == NULL) {
        perror(base_path);
        return;
    }

    // 실제 ls -R 형식에 맞춰 현재 탐색 중인 디렉토리 경로 출력
    printf("%s:\n", base_path);

    // 2. 현재 디렉토리 내의 파일 및 디렉토리 이름 모두 출력
    while ((entry = readdir(dir)) != NULL) {
        // 숨김 파일(., .. 포함) 제외 (실제 ls -R 기본 동작과 동일)
        if (entry->d_name[0] == '.') {
            continue;
        }
        printf("%s  ", entry->d_name);
    }
    printf("\n\n");

    // 3. 디렉토리 스트림을 처음으로 되감아 하위 디렉토리를 찾아 재귀 방문
    rewinddir(dir);
    while ((entry = readdir(dir)) != NULL) {
        // 현재(.), 상위(..) 및 숨김 디렉토리 제외 (무한 루프 방지 필수)
        if (entry->d_name[0] == '.') {
            continue;
        }

        // 전체 경로 조합 (예: ./sub_dir)
        snprintf(path, sizeof(path), "%s/%s", base_path, entry->d_name);

        // 파일 상태 정보를 읽어 디렉토리인지 확인 (lstat 사용 시 심볼릭 링크 무한루프 방지)
        if (lstat(path, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
            list_dir_recursive(path);
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    // 인자가 없으면 현재 디렉토리("."), 있으면 해당 경로를 대상으로 실행
    const char *target_dir = (argc > 1) ? argv[1] : ".";
    list_dir_recursive(target_dir);
    return 0;
}
