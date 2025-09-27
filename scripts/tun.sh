#!/bin/bash

if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root" 
   exit 1
fi

# teardown occurs when you exit the container
# but this line allows you to run the script multiple times
ip netns del ns1 2>/dev/null || true

# Create namespace
ip netns add ns1

# Create TUN device inside ns1
ip netns exec ns1 ip tuntap add mode tun dev tun0
ip netns exec ns1 ip addr add 10.0.0.1/24 dev tun0
ip netns exec ns1 ip link set tun0 up

# Create veth pair
ip link add veth0 type veth peer name veth1

# Move one side into ns1
ip link set veth1 netns ns1

# Root side (monitor only)
ip addr add 10.0.0.2/24 dev veth0
ip link set veth0 up

# Inside ns1
ip netns exec ns1 ip addr add 10.0.0.3/24 dev veth1
ip netns exec ns1 ip link set veth1 up
ip netns exec ns1 ip link set lo up

# Add routing so packets from TUN (10.0.0.1) to destination (10.0.0.3) 
# get routed through veth1 to veth0 where tcpdump can see them
ip netns exec ns1 ip route add 10.0.0.3/32 dev veth1

# Enable IP forwarding in the namespace (if needed)
ip netns exec ns1 sysctl -w net.ipv4.ip_forward=1 >/dev/null

echo "=== Network setup complete ==="
echo "Namespace ns1 interfaces:"
ip netns exec ns1 ip addr show
echo "Root namespace veth0:"
ip addr show veth0