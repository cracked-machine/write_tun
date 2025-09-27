Use the `build.sh` and `run.sh` scripts.

## Outline

1. Network namespace created (ns1)
1. TUN device created inside the namespace (tun0 with IP 10.0.0.1/24)
1. veth pair created (veth0 in root, veth1 in ns1)
1. C++ application runs inside the namespace and creates a TUN device
1. ICMP packet written to the TUN device successfully
1. tcpdump captures the packet on the TUN interface


## Packet details:
- Source: 10.0.0.1 (TUN device)
- Destination: 10.0.0.3 (veth1 endpoint)
- Type: ICMP echo request
- ID: 4660 (matches your C++ code: 0x1234)
- Sequence: 1
- Length: 60 bytes total (20 IP + 8 ICMP + 32 payload)

