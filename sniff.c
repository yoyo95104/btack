#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

void start_sniffing() {
    int sock = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
    if (sock < 0) {
        perror("Failed to open HCI socket");
        exit(1);
    }

    struct hci_filter flt;
    hci_filter_clear(&flt);
    hci_filter_set_ptype(HCI_EVENT_PKT, &flt);
    hci_filter_all_events(&flt);

    if (setsockopt(sock, SOL_HCI, HCI_FILTER, &flt, sizeof(flt)) < 0) {
        perror("Failed to set socket options");
        close(sock);
        exit(1);
    }

    unsigned char buf[1024];
    while (1) {
        int len = read(sock, buf, sizeof(buf));
        if (len > 0) {
            printf("Captured packet: ");
            for (int i = 0; i < len; i++) {
                printf("%02X ", buf[i]);
            }
            printf("\n");
        }
    }

    close(sock);
}
