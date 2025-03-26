#include "mal.hpp"
#include <vector>
#include <cstring>

void PacketManipulator::modify_packets(const bdaddr_t& addr) {
}

void PacketManipulator::ddos_attack(const bdaddr_t& addr) {
}
extern "C" void cpp_modify_packets(const bdaddr_t* target_addr) {
    PacketManipulator().modify_packets(*target_addr);
}

extern "C" void cpp_ddos_attack(const bdaddr_t* target_addr) {
    PacketManipulator().ddos_attack(*target_addr);
}
