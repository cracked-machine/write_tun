#!/bin/bash

if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root" 
   exit 1
fi

# setup tun in namespace with veth pair
./scripts/tun.sh

echo "Starting tcpdump on TUN device inside ns1 to monitor ICMP traffic..."
# Monitor the TUN device directly where the packets are being written
ip netns exec ns1 tcpdump -i tun0 -n icmp -v &
TCPDUMP_PID=$!

# Give tcpdump a moment to start
sleep 1

echo "Running C++ app inside network namespace ns1..."
ip netns exec ns1 ./build-x86_64-linux-gnu/bin/CppTestApp

# Give tcpdump time to capture any packets
sleep 2

# Kill tcpdump
echo "Stopping tcpdump..."
kill $TCPDUMP_PID 2>/dev/null || true