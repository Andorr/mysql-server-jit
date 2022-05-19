#if __linux__

#include <stdlib.h>
#include <algorithm>
#include <cerrno>
#include <cstring>
#include <numeric>
#include <tuple>
#include <vector>

#include <asm/unistd.h>
#include <linux/perf_event.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <inttypes.h>
#include <sys/types.h>

#include "benchmark.h"

static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                            int cpu, int group_fd, unsigned long flags) {
  int ret;

  ret = syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
  return ret;
}

int benchmark::perf_init() {
  struct perf_event_attr pe;
  int fd;

  memset(&pe, 0, sizeof(struct perf_event_attr));
  pe.type = PERF_TYPE_HARDWARE;
  pe.size = sizeof(struct perf_event_attr);
  pe.config = PERF_COUNT_HW_INSTRUCTIONS;
  pe.disabled = 1;
  pe.exclude_kernel = 1;
  // Don't count hypervisor events.
  pe.exclude_hv = 1;

  fd = perf_event_open(&pe, 0, -1, -1, 0);
  if (fd == -1) {
    fprintf(stderr, "Error opening leader %llx %s\n", pe.config,
            std::strerror(errno));
    exit(EXIT_FAILURE);
  }
  return fd;
}

void benchmark::perf_start(int fd) {
  ioctl(fd, PERF_EVENT_IOC_RESET, 0);
  ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);
}

void benchmark::perf_stop(int fd) { ioctl(fd, PERF_EVENT_IOC_DISABLE, 0); }

long long benchmark::perf_read(int fd) {
  long long count;
  read(fd, &count, sizeof(long long));
  return count;
}

#endif