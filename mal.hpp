#pragma once
#include <bluetooth/bluetooth.h>

#ifdef __cplusplus
extern "C" {
#endif

void cpp_modify_packets(const bdaddr_t* target_addr);
void cpp_ddos_attack(const bdaddr_t* target_addr);

#ifdef __cplusplus
}
#endif

class PacketManipulator {
public:
    void modify_packets(const bdaddr_t& addr);
    void ddos_attack(const bdaddr_t& addr);
};
