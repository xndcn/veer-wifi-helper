#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <linux/ethtool.h>
#include <linux/if.h>
#include <linux/sockios.h>

#define DHD_IOCTL_MAGIC         0x00444944

#define IOCTL_ERROR    -2       /* Error code for ioctl failure */

#define DHD_GET_MAGIC                           0

#define WLC_UP                                  2
#define WLC_GET_UP                              162

typedef unsigned int    uint;
typedef  unsigned char  bool;

typedef struct dhd_ioctl {
        uint cmd;       /* common ioctl definition */
        void *buf;      /* pointer to user buffer */
        uint len;       /* length of user buffer */
        bool set;       /* get or set request (optional) */
        uint used;      /* bytes read or written (optional) */
        uint needed;    /* bytes needed (optional) */
        uint driver;    /* to identify target driver */
} dhd_ioctl_t;

static void
dhd_ioctl(struct ifreq *dhd, int cmd, void *buf, int len, bool set)
{
    struct ifreq *ifr = dhd;
    dhd_ioctl_t ioc;
    int ret = 0;
    int s;

    /* open socket to kernel */
    if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
        printf("socket error!\n");

    /* do it */
    ioc.cmd = cmd;
    ioc.buf = buf;
    ioc.len = len;
    ioc.set = set;
    ioc.driver = 0;
    ifr->ifr_data = (caddr_t) &ioc;
    if ((ret = ioctl(s, SIOCDEVPRIVATE, ifr)) < 0) {
        if (cmd != DHD_GET_MAGIC) {
            printf("IOCTL_ERROR %d\n", ret);
        }
    }
    /* cleanup */
    close(s);
}

int main()
{
	int fd;
	int ret;
	struct ethtool_drvinfo info;
	struct ifreq ifr;
	char buf[128];
	
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
		printf("socket error!\n");
	
	memset(&info, 0, sizeof(info));
	info.cmd = ETHTOOL_GDRVINFO;
	strcpy(info.driver, "?dhd");
	ifr.ifr_data = (caddr_t)&info;
    strcpy(ifr.ifr_name, "eth0");
    if ((ret = ioctl(fd, SIOCETHTOOL, &ifr)) < 0) {
    	printf("ioctl error!\n");    
    } else {
    	strcpy(buf, info.driver);
    }
    close(fd);
    
    memset(&info, 0, sizeof(info));
    strcpy(ifr.ifr_name, "eth0");
    dhd_ioctl(&ifr, WLC_UP, NULL, 0, 1);
    
    return 0;
}
