#ifndef ATTACK_H
#define ATTACK_H

#include <bluetooth/bluetooth.h>
void send_packet(bdaddr_t *target_addr , const char *message);

#endif
