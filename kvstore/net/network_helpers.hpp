#ifndef NETWORK_HELPER_HPP
#define NETWORK_HELPER_HPP

#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <chrono>
#include <iostream>
#include <optional>
#include <thread>
#include <vector>

#include "common/color.hpp"

using namespace std::chrono;

#define BACKLOG 100

#define ETIMEOUT -2

/*
 * Sends/receives all of the bytes in buf, according to len. Times out after the
 * specified amount if timeout > 0 (in this case, returns ETIMEOUT. Otherwise,
 * returns the result of send/recv).
 */
int sendall(int fd, void* buf, size_t len, int flags,
            milliseconds timeout = 0ms);
int recvall(int fd, void* buf, size_t len, int flags,
            milliseconds timeout = 0ms);

/*
 * Opens a listener socket on the specified address (hostname:port).
 * On success, a file descriptor for the new socket is returned.  On error, -1
 * is returned.
 */
int open_listener_socket(const std::string& address);

/*
 * Establishes a connection to the specified address.
 * On success, a file descriptor for the new socket is returned.  On error, -1
 * is returned.
 */
int connect_to_address(const std::string& address);

/*
 * Creates an address string of hostname:port, from the current host and given
 * port.
 */
std::string get_host_address(const char* port);

#endif /* end of include guard */
