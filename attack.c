#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/rfcomm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void send_packet(bdaddr_t *target_addr, const char *message) {
    int sock;
    struct sockaddr_rc addr = {0};
    sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    if (sock < 0) {
        perror("Failed to create socket");
        exit(1);
    }
    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = 1; 
    bacpy(&addr.rc_bdaddr, target_addr);
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Failed to connect");
        close(sock);
        exit(1);
    }
    if (write(sock, message, strlen(message)) < 0) {
        perror("Failed to send data");
        close(sock);
        exit(1);
    }
    printf("Sent: %s\n", message);
    close(sock);
}
