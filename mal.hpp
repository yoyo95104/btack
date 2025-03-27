#pragma once
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/rfcomm.h>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <thread>
#include <chrono>

#ifdef __cplusplus
extern "C" {
#endif

// C-compatible interface
void cpp_modify_packets(const bdaddr_t* target_addr, const char* payload);
void cpp_ddos_attack(const bdaddr_t* target_addr, int duration_sec);

#ifdef __cplusplus
}
#endif

namespace Bluetooth {
    class PacketManipulator {
    private:
        int hci_dev_id = -1;
        int hci_sock = -1;
        
        void ensure_hci_connection();
        void craft_malicious_l2cap(const bdaddr_t& target, const uint8_t* data, size_t len);
        void craft_malicious_rfcomm(const bdaddr_t& target, uint8_t channel, const uint8_t* data, size_t len);
        
    public:
        PacketManipulator();
        ~PacketManipulator();
        
        void inject_packet(const bdaddr_t& target, const std::vector<uint8_t>& payload);
        void flood_device(const bdaddr_t& target, int packets_per_sec, int duration_sec);
    };
}
