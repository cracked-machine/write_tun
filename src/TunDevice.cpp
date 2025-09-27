#include "TunDevice.hpp"

#include <cstring>
#include <fcntl.h>
#include <iostream> // for std::cout in constructor
#include <linux/if_tun.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>

TunDevice::TunDevice( const std::string &dev )
    : fd_( -1 )
{
  fd_ = ::open( "/dev/net/tun", O_RDWR );
  if ( fd_ < 0 )
  {
    perror( "open /dev/net/tun" );
    exit( 1 );
  }

  struct ifreq ifr
  {
  };
  ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
  std::strncpy( ifr.ifr_name, dev.c_str(), IFNAMSIZ );

  if ( ::ioctl( fd_, TUNSETIFF, &ifr ) < 0 )
  {
    perror( "ioctl TUNSETIFF" );
    ::close( fd_ );
    exit( 1 );
  }

  std::cout << "Created TUN device: " << dev << std::endl;
}

TunDevice::~TunDevice()
{
  if ( fd_ >= 0 )
  {
    ::close( fd_ );
    fd_ = -1;
  }
}

TunDevice::TunDevice( TunDevice &&other ) noexcept
    : fd_( other.fd_ )
{
  other.fd_ = -1;
}

TunDevice &TunDevice::operator=( TunDevice &&other ) noexcept
{
  if ( this != &other )
  {
    if ( fd_ >= 0 ) { ::close( fd_ ); }
    fd_ = other.fd_;
    other.fd_ = -1;
  }
  return *this;
}

ssize_t TunDevice::write( const char *data, std::size_t size )
{
  if ( fd_ < 0 ) { return -1; }

  ssize_t result = ::write( fd_, data, size );
  if ( result < 0 ) { perror( "write to TUN device" ); }
  return result;
}

void TunDevice::flush()
{
  // TUN devices don't need explicit flushing
  // This method is provided for stream-like interface compatibility
}

bool TunDevice::good() const { return fd_ >= 0; }

int TunDevice::fd() const { return fd_; }
