#include "network_helpers.hpp"

int sendall(int fd, void* buf, size_t len, int flags, milliseconds timeout) {
  size_t n_sent = 0, n_to_send = len;
  char* data = (char*)buf;
  auto begin = system_clock::now();
  while (n_sent < n_to_send) {
    // If desired, check if timed out
    if (timeout > 0ms &&
        duration_cast<milliseconds>(system_clock::now() - begin) > timeout) {
      return ETIMEOUT;
    }

    int curr = send(fd, data + n_sent, n_to_send - n_sent, flags);
    if (curr <= 0) {
      return curr;
    }

    // update bytes sent; if not completed, network is likely saturated, so wait
    n_sent += curr;
    if (n_sent < n_to_send) {
      // There might be a better way to do this; sleeping feels un-optimal, but
      // I'm pretty sure setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, ...) is
      // functionally the same as blocking on the thread. Investigate more
      // later.
      if (timeout > 0ms) std::this_thread::sleep_for(timeout / 10);
    }
  }
  return n_sent;
}

int recvall(int fd, void* buf, size_t len, int flags, milliseconds timeout) {
  size_t n_recvd = 0, n_to_recv = len;
  char* data = (char*)buf;
  auto begin = system_clock::now();
  while (n_recvd < n_to_recv) {
    // Check if timed out
    if (timeout > 0ms &&
        duration_cast<milliseconds>(system_clock::now() - begin) > timeout) {
      return ETIMEOUT;
    }

    int curr = recv(fd, data + n_recvd, n_to_recv - n_recvd, flags);
    if (curr <= 0) {
      return curr;
    }

    // update bytes sent; if not completed, network is likely saturated, so wait
    n_recvd += curr;
    if (n_recvd < n_to_recv) {
      if (timeout > 0ms) std::this_thread::sleep_for(timeout / 10);
    }
  }
  return n_recvd;
}

int open_listener_socket(const std::string& address) {
  size_t splitIdx = address.find(':');
  if (splitIdx == std::string::npos) {
    cerr_color(RED, "Invalid address: ", address);
    return -1;
  }
  std::string hostname = address.substr(0, splitIdx);
  std::string port = address.substr(splitIdx + 1);

  int ret, listener_fd;
  struct addrinfo hints, *res;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;        // INET -> v4, INET6 -> v6, UNSPEC -> both
  hints.ai_socktype = SOCK_STREAM;  // STREAM -> TCP, DGRAM -> UDP
  hints.ai_flags = AI_PASSIVE;      // set -> bind; not set -> connect
  // if ((ret = getaddrinfo(hostname.c_str(), port.c_str(), &hints, &res)) != 0)
  // {
  if ((ret = getaddrinfo(NULL, port.c_str(), &hints, &res)) != 0) {
    cerr_color(RED, "getaddrinfo: ", gai_strerror(ret));
    return -1;
  }

  struct addrinfo* cur;
  // loop over all potential results
  for (cur = res; cur != NULL; cur = cur->ai_next) {
    int yes = 1;
    // create socket w/ addr family (INET/6), type (STREAM/DGRAM), and protocol
    // (can specify IPPROTO_UDP or TCP, but here either is ok)
    listener_fd = socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol);
    if (listener_fd == -1) {
      perror_color(YELLOW, "socket");
      continue;
    }

    // setsockopt [socket] [level (e.g. socket, or for a protocol)] [OPTIONS]
    // [OPTION VALUE] [OPTION LEN]
    // Here, SO_REUSEADDR allows us to instantly re-use port, rather than
    // waiting for TCP TIME_WAIT to expire
    if ((ret = setsockopt(listener_fd, SOL_SOCKET, SO_REUSEADDR, &yes,
                          sizeof(yes))) == -1) {
      close(listener_fd);
      perror_color(YELLOW, "setsockopt");
      continue;
    }

    // assign name to the desired socket
    if ((ret = bind(listener_fd, cur->ai_addr, cur->ai_addrlen)) == -1) {
      close(listener_fd);
      perror_color(YELLOW, "bind");
      continue;
    }
    break;
  }

  // if no available results found, quit
  if (!cur) {
    if (res) freeaddrinfo(res);
    cerr_color(RED, "no available sockets on port ", port);
    return -1;
  }

  freeaddrinfo(res);
  // configure listening backlog
  if ((ret = listen(listener_fd, BACKLOG)) < 0) {
    close(listener_fd);
    perror_color(RED, "listen");
    return -1;
  }

  return listener_fd;
};

int connect_to_address(const std::string& address) {
  size_t splitIdx = address.find(':');
  if (splitIdx == std::string::npos) {
    cerr_color(RED, "Invalid address: ", address);
    return -1;
  }
  std::string hostname = address.substr(0, splitIdx);
  std::string port = address.substr(splitIdx + 1);

  int ret, cfd;
  struct addrinfo hints, *res;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;        // INET -> v4, INET6 -> v6, UNSPEC -> both
  hints.ai_socktype = SOCK_STREAM;  // STREAM -> TCP, DGRAM -> UDP
  if ((ret = getaddrinfo(hostname.c_str(), port.c_str(), &hints, &res)) != 0) {
    cerr_color(RED, "getaddrinfo: ", gai_strerror(ret));
    return -1;
  }

  struct addrinfo* cur;
  // loop over all potential results
  for (cur = res; cur != NULL; cur = cur->ai_next) {
    cfd = socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol);
    if (cfd == -1) {
      perror_color(YELLOW, "socket");
      continue;
    }

    if ((ret = connect(cfd, cur->ai_addr, cur->ai_addrlen)) == -1) {
      close(cfd);
      perror_color(YELLOW, "connect");
      continue;
    }
    break;
  }

  if (!cur) {
    return -1;
  }

  return cfd;
}

std::string get_host_address(const char* port) {
  // Get our hostname for readability
  char hostnamebuf[256] = {0};
  if (gethostname(hostnamebuf, 256) < 0) {
    perror_color(RED, "gethostname");
    exit(EXIT_FAILURE);
  }
  // construct addresses
  std::string hostname(hostnamebuf);
  std::string port_str(port);

  std::string addr = hostname + ":" + port_str;
  return addr;
}
