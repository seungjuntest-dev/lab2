#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dbus/dbus.h>

#define OBJECT_PATH   "/org/example/NotifyObject"
#define INTERFACE     "org.example.NotifyInterface"
#define SIGNAL_NAME   "Alert"

int main(int argc, char *argv[]) {
    const char *alert_msg = (argc > 1) ? argv[1] : "기본 알림: 시스템 점검이 완료되었습니다.";

    DBusError err;
    DBusConnection *conn;
    DBusMessage *msg;
    DBusMessageIter args;

    dbus_error_init(&err);

    // 1. 세션 버스에 연결
    conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
    if (dbus_error_is_set(&err)) {
        fprintf(stderr, "DBus 연결 오류: %s\n", err.message);
        dbus_error_free(&err);
        exit(1);
    }

    // 2. 새로운 시그널 메시지 생성
    msg = dbus_message_new_signal(OBJECT_PATH, INTERFACE, SIGNAL_NAME);
    if (msg == NULL) {
        fprintf(stderr, "시그널 메시지 생성 실패\n");
        exit(1);
    }

    // 3. 메시지에 문자열 인자 추가
    dbus_message_iter_init_append(msg, &args);
    if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_STRING, &alert_msg)) {
        fprintf(stderr, "메모리 부족으로 인자 추가 실패\n");
        exit(1);
    }

    // 4. 버스 전송 및 플러시
    if (!dbus_connection_send(conn, msg, NULL)) {
        fprintf(stderr, "메시지 전송 실패\n");
        exit(1);
    }
    dbus_connection_flush(conn);

    printf("[전송 완료] 알림 메시지: \"%s\"\n", alert_msg);

    dbus_message_unref(msg);
    return 0;
}
