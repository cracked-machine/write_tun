#include <TunDevice.hpp>

#include <arpa/inet.h>
#include <array>
#include <cstdint>
#include <iostream>
#include <string_view>
#include <vector>

// Internet checksum (safe, no raw pointers)
uint16_t checksum( const std::vector<uint8_t> &data )
{
  uint32_t sum = 0;
  for ( size_t i = 0; i + 1 < data.size(); i += 2 )
  {
    sum += ( data[i] << 8 ) + data[i + 1];
  }
  if ( data.size() & 1 ) sum += data.back() << 8;
  while ( sum >> 16 )
    sum = ( sum & 0xFFFF ) + ( sum >> 16 );
  return static_cast<uint16_t>( ~sum );
}

int main()
{
  TunDevice tun_device( "tun0" );

  constexpr size_t IP_HDR_LEN = 20;
  constexpr size_t ICMP_HDR_LEN = 8;
  constexpr size_t PAYLOAD_LEN = 32;
  const size_t total_len = IP_HDR_LEN + ICMP_HDR_LEN + PAYLOAD_LEN;

  std::vector<uint8_t> packet( total_len, 0 );

  // --- IPv4 header ---
  packet[0] = ( 4 << 4 ) | 5;            // Version=4, IHL=5
  packet[1] = 0;                         // TOS
  packet[2] = ( total_len >> 8 ) & 0xFF; // Total length
  packet[3] = total_len & 0xFF;
  packet[4] = 0x12;
  packet[5] = 0x34; // ID
  packet[6] = 0;
  packet[7] = 0;  // Flags/Frag
  packet[8] = 64; // TTL
  packet[9] = 1;  // Protocol = ICMP

  // Src = 10.0.0.1, Dst = 10.0.3
  std::array<uint8_t, 4> src{};
  std::array<uint8_t, 4> dst{};
  ::inet_pton( AF_INET, "10.0.0.1", src.data() );
  ::inet_pton( AF_INET, "10.0.0.3", dst.data() );
  std::copy( src.begin(), src.end(), packet.begin() + 12 );
  std::copy( dst.begin(), dst.end(), packet.begin() + 16 );

  // IP checksum
  {
    std::vector<uint8_t> iphdr( packet.begin(), packet.begin() + IP_HDR_LEN );
    uint16_t ip_cksum = checksum( iphdr );
    packet[10] = ip_cksum >> 8;
    packet[11] = ip_cksum & 0xFF;
  }

  // --- ICMP header ---
  size_t icmp_offset = IP_HDR_LEN;
  packet[icmp_offset + 0] = 8;    // Type = Echo Request
  packet[icmp_offset + 1] = 0;    // Code = 0
  packet[icmp_offset + 4] = 0x12; // Identifier
  packet[icmp_offset + 5] = 0x34;
  packet[icmp_offset + 6] = 0; // Sequence
  packet[icmp_offset + 7] = 1;

  // Payload
  std::string_view payload = "Hello from C++20 TUN interface!";
  std::copy( payload.begin(), payload.end(), packet.begin() + icmp_offset + ICMP_HDR_LEN );

  // ICMP checksum
  {
    std::vector<uint8_t> icmphdr( packet.begin() + icmp_offset, packet.end() );
    uint16_t icmp_cksum = checksum( icmphdr );
    packet[icmp_offset + 2] = icmp_cksum >> 8;
    packet[icmp_offset + 3] = icmp_cksum & 0xFF;
  }

  // --- Write packet into TUN using stream-like interface ---
  tun_device.write( reinterpret_cast<const char *>( packet.data() ), packet.size() );
  if ( !tun_device.good() )
  {
    std::cerr << "Error writing to TUN device\n";
    return 1;
  }

  tun_device.flush();

  std::cout << "Sent ICMP Echo Request (" << packet.size()
            << " bytes) into tun0 using stream-like interface\n";

  return 0;
}
