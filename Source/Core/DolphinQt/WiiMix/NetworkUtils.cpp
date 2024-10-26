#include <DolphinQt/WiiMix/NetworkUtils.h>

WiiMixNetworkUtils::WiiMixNetworkUtils() {};

void* WiiMixNetworkUtils::GetInAddr(struct sockaddr *sa) {
  // IPv4
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }

  // IPv6
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}