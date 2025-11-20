#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/prctl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>

#define file_name	"/var/log/sysmond.log"
#define mem_info	"/proc/meminfo"
#define cpu_info	"/proc/stat"
#define netdev_info	"/proc/net/dev"
#define sock_path	"/tmp/sysmon.sock"

#define net	"net"
#define mem	"mem"
#define all	"all"
#define stop	"stop"

