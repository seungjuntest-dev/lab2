#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dbus/dbus.h>

#define BUS_NAME      "org.example.NotifyServer"
#define OBJECT_PATH   "/org/example/NotifyObject"
#define INTERFACE     "org.example.NotifyInterface"
#define SIGNAL_NAME   "Alert"

int main() {
    DBusError err;
    DBusConnection *conn;

    dbus_error_init(&err);

    // 1. 세션 버스에 연결
    conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "DBus 연결 오류: %s\n", err.message);
        dbus_error_free(&err);
        exit(1);
    }

    // 2. 버스 이름 등록
    int ret = dbus_bus_request_name(conn, BUS_NAME, DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "버스 이름 등록 오류: %s\n", err.message);
        dbus_error_free(&err);
        exit(1);
    }
    if (ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
        fprintf(stderr, "기본 소유자 등록 실패\n");
        exit(1);
    }

    // 3. 수신할 시그널 매칭 규칙 등록
    char rule[256];
    snprintf(rule, sizeof(rule), "type='signal',interface='%s'", INTERFACE);
    dbus_bus_add_match(conn, rule, &err);
    dbus_connection_flush(conn);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "Match 규칙 등록 오류: %s\n", err.message);
        dbus_error_free(&err);
        exit(1);
    }

    printf("- D-Bus 알림 수신 서버 대기 중 (종료: Ctrl+C) - \n");

    // 4. 메시지 루프: 클라이언트가 보낸 알림 시그널 수신
    while (1) {
        dbus_connection_read_write(conn, 100); // 100ms 대기하며 읽기
        DBusMessage *msg = dbus_connection_pop_message(conn);

        if (msg == NULL) {
            continue;
        }

        // 지정한 인터페이스와 시그널 이름과 일치하는지 확인
        if (dbus_message_is_signal(msg, INTERFACE, SIGNAL_NAME)) {
            DBusMessageIter args;
            char *recv_text = NULL;

            if (dbus_message_iter_init(msg, &args)) {
                if (DBUS_TYPE_STRING == dbus_message_iter_get_arg_type(&args)) {
                    dbus_message_iter_get_basic(&args, &recv_text);
                    printf("[수신된 알림] >> %s\n", recv_text);
                }
            }
        }
        dbus_message_unref(msg);
    }

    return 0;
}
