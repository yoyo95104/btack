#ifndef MITM_H
#define MITM_H

#include <bluetooth/bluetooth.h>

void start_mitm_attack(const bdaddr_t *target_addr);

#endif
