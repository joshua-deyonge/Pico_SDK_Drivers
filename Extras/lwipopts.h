#ifndef _LWIPOPTS_H
#define _LWIPOPTS_H

// Require these definitions
#define NO_SYS                      1
#define LWIP_SOCKET                 0
#define LWIP_NETCONN                0

// Memory settings - CRITICAL
#define MEM_LIBC_MALLOC             1
#define MEMP_MEM_MALLOC             0
#define MEM_ALIGNMENT               4
#define MEM_SIZE                    4096
#define MEMP_NUM_PBUF               16
#define MEMP_NUM_UDP_PCB            4
#define MEMP_NUM_TCP_PCB            4
#define MEMP_NUM_TCP_PCB_LISTEN     4
#define MEMP_NUM_TCP_SEG            16
#define MEMP_NUM_SYS_TIMEOUT        8
#define MEMP_NUM_NETBUF             0
#define MEMP_NUM_NETCONN            0

// PBUF settings - INCREASED
#define PBUF_POOL_SIZE              24      // Increased from 16
#define PBUF_POOL_BUFSIZE           512     // Increased from 256

// TCP settings - REDUCED to fit
#define LWIP_TCP                    1
#define TCP_MSS                     536     // Reduced from 1460
#define TCP_WND                     (4 * TCP_MSS)
#define TCP_SND_BUF                 (4 * TCP_MSS)
#define TCP_SND_QUEUELEN            ((4 * (TCP_SND_BUF) + (TCP_MSS - 1)) / (TCP_MSS))

// UDP settings  
#define LWIP_UDP                    1
#define UDP_TTL                     255

// DHCP
#define LWIP_DHCP                   1
#define DHCP_DOES_ARP_CHECK         0

// DNS
#define LWIP_DNS                    1
#define DNS_TABLE_SIZE              4
#define DNS_MAX_NAME_LENGTH         256

// ARP/Ethernet
#define LWIP_ARP                    1
#define LWIP_ETHERNET               1
#define LWIP_ICMP                   1
#define LWIP_RAW                    1
#define ETHARP_SUPPORT_VLAN         0

// Callbacks
#define LWIP_NETIF_STATUS_CALLBACK  1
#define LWIP_NETIF_LINK_CALLBACK    1
#define LWIP_NETIF_HOSTNAME         1

// Stats (disabled for now)
#define LWIP_STATS                  0
#define LWIP_STATS_DISPLAY          0

// Checksum
#define LWIP_CHKSUM_ALGORITHM       3

// Threading
#define SYS_LIGHTWEIGHT_PROT        1
#define LWIP_TCPIP_CORE_LOCKING     0

#endif /* _LWIPOPTS_H */