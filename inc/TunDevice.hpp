/*
 * MIT License
 *
 * Copyright (c) 2025 cracked-machine
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <cstddef>
#include <string>

/**
 * @brief Simple TUN device wrapper class that provides a stream-like interface
 *
 * This class encapsulates the creation and management of a TUN network interface,
 * providing convenient methods for writing packets to the interface.
 */
class TunDevice
{
private:
  int fd_; ///< File descriptor for the TUN device

public:
  /**
   * @brief Construct a new TUN Device object
   *
   * @param dev The name of the TUN device to create (e.g., "tun0")
   * @throws std::runtime_error if device creation fails
   */
  explicit TunDevice( const std::string &dev );

  /**
   * @brief Destroy the TUN Device object
   *
   * Automatically closes the TUN device file descriptor
   */
  ~TunDevice();

  // Disable copy construction and assignment
  TunDevice( const TunDevice & ) = delete;
  TunDevice &operator=( const TunDevice & ) = delete;

  // Enable move construction and assignment
  TunDevice( TunDevice &&other ) noexcept;
  TunDevice &operator=( TunDevice &&other ) noexcept;

  /**
   * @brief Write data to the TUN device
   *
   * @param data Pointer to the data to write
   * @param size Number of bytes to write
   * @return Number of bytes written, or -1 on error
   */
  ssize_t write( const char *data, std::size_t size );

  /**
   * @brief Flush any buffered data
   *
   * Note: TUN devices typically don't require explicit flushing
   */
  void flush();

  /**
   * @brief Check if the device is in a good state
   *
   * @return true if the device is open and ready for I/O
   */
  bool good() const;

  /**
   * @brief Get the file descriptor for the TUN device
   *
   * @return The file descriptor, or -1 if invalid
   */
  int fd() const;
};
