/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "lwip_test.h"
#include <lwip/netif.h>
#include <lwip/netifapi.h>
#include <lwip/pbuf.h>
#include <lwip/ip4_addr.h>
#include <lwip/ip4.h>
#include <lwip/ip.h>
#include <lwip/def.h>
#include <lwip/udp.h>
#include <lwip/inet_chksum.h>
#include "lwip/prot/iana.h"

#define MSG "Hi, I am testing BT netif."
#define STACK_IP_BT "192.168.167.1"
#define PEER_IP_BT "100.100.100.100"
#define GATEWAY_IP_BT "192.168.167.239"
#define NETIF_MAC "121212"
#define PEER_MAC "454545"
#define NETIF_NAME_BT "bt1000"
#define TEST_CASE 210
static char g_buf[BUF_SIZE + 1] = { 0 };
struct netif *btProxyNf = NULL;

static void ReplayArpTask();
static void ArpPackageProc(struct netif *netif, struct pbuf *p);
extern void driverif_input(struct netif *netif, struct pbuf *p);
static void ReplayArpEncodeEthernet(struct netif *netif, struct pbuf *p);

static void ReplayUdpEncodeEthernet(struct netif *netif, struct pbuf *p)
{
    int ret;
    ret = pbuf_add_header(p, SIZEOF_ETH_HDR);
    ICUNIT_ASSERT_EQUAL(ret, 0, 1);

    struct eth_hdr *ethhdr;
    ethhdr = (struct eth_hdr *)p->payload;
    ethhdr->type = lwip_htons(ETHTYPE_IP);
    SMEMCPY(&ethhdr->dest, NETIF_MAC, ETH_HWADDR_LEN);
    SMEMCPY(&ethhdr->src,  PEER_MAC, ETH_HWADDR_LEN);

    driverif_input(netif, p);
}

static void ReplayUdpEncodeIp(struct pbuf *p, ip4_addr_t *src, ip4_addr_t *dest)
{
    struct ip_hdr *iphdr;
    int ret;
#if CHECKSUM_GEN_IP_INLINE
    u32_t chk_sum = 0;
#endif /* CHECKSUM_GEN_IP_INLINE */

   // ip header
    ret = pbuf_add_header(p, IP_HLEN);
    ICUNIT_ASSERT_EQUAL(ret, 0, 2);
    iphdr = (struct ip_hdr *)p->payload;
    IPH_TTL_SET(iphdr, UDP_TTL);
    IPH_PROTO_SET(iphdr, IP_PROTO_UDP);

#if CHECKSUM_GEN_IP_INLINE
    chk_sum += PP_NTOHS(IP_PROTO_UDP | (ttl << 8));
#endif /* CHECKSUM_GEN_IP_INLINE */

    /* dest cannot be NULL here */
    ip4_addr_copy(iphdr->dest, *dest);
#if CHECKSUM_GEN_IP_INLINE
    chk_sum += ip4_addr_get_u32(&iphdr->dest) & 0xFFFF;
    chk_sum += ip4_addr_get_u32(&iphdr->dest) >> 16;
#endif /* CHECKSUM_GEN_IP_INLINE */

    u16_t ip_hlen = IP_HLEN;
    IPH_VHL_SET(iphdr, 4, ip_hlen / 4);
    IPH_TOS_SET(iphdr, 0);
#if CHECKSUM_GEN_IP_INLINE
    chk_sum += PP_NTOHS(tos | (iphdr->_v_hl << 8));
#endif /* CHECKSUM_GEN_IP_INLINE */
    IPH_LEN_SET(iphdr, lwip_htons(p->tot_len));
#if CHECKSUM_GEN_IP_INLINE
    chk_sum += iphdr->_len;
#endif /* CHECKSUM_GEN_IP_INLINE */
    IPH_OFFSET_SET(iphdr, 0);
    IPH_ID_SET(iphdr, lwip_htons(0));
#if CHECKSUM_GEN_IP_INLINE
    chk_sum += iphdr->_id;
#endif /* CHECKSUM_GEN_IP_INLINE */
    ip4_addr_copy(iphdr->src, *src);

#if CHECKSUM_GEN_IP_INLINE
    chk_sum += ip4_addr_get_u32(&iphdr->src) & 0xFFFF;
    chk_sum += ip4_addr_get_u32(&iphdr->src) >> 16;
    chk_sum = (chk_sum >> 16) + (chk_sum & 0xFFFF);
    chk_sum = (chk_sum >> 16) + chk_sum;
    chk_sum = ~chk_sum;
    IF__NETIF_CHECKSUM_ENABLED(netif, NETIF_CHECKSUM_GEN_IP) {
        iphdr->_chksum = (u16_t)chk_sum; /* network order */
    }
#if LWIP_CHECKSUM_CTRL_PER_NETIF
    else {
        IPH_CHKSUM_SET(iphdr, 0);
    }
#endif /* LWIP_CHECKSUM_CTRL_PER_NETIF */
#else /* CHECKSUM_GEN_IP_INLINE */
    IPH_CHKSUM_SET(iphdr, 0);
#if CHECKSUM_GEN_IP
    IF__NETIF_CHECKSUM_ENABLED(netif, NETIF_CHECKSUM_GEN_IP) {
        IPH_CHKSUM_SET(iphdr, inet_chksum(iphdr, ip_hlen));
    }
#endif /* CHECKSUM_GEN_IP */
#endif /* CHECKSUM_GEN_IP_INLINE */

    ReplayUdpEncodeEthernet(btProxyNf, p);
}

static void ReplayUdp(void *ptemp)
{
    struct udp_hdr *udphdr;
    ip4_addr_t sipaddr, dipaddr;
    int size = strlen(MSG);
    int ret;
    (void)ptemp;

    LogPrintln("encode udp replay packet");
    inet_pton(AF_INET, PEER_IP_BT, &sipaddr);
    inet_pton(AF_INET, STACK_IP_BT, &dipaddr);

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, size, PBUF_RAM);
    u16_t chksum = LWIP_CHKSUM_COPY(p->payload, MSG, size);

    // udp header
    ret = pbuf_add_header(p, UDP_HLEN);
    ICUNIT_ASSERT_EQUAL(ret, 0, 1);
    udphdr = (struct udp_hdr *)p->payload;
    udphdr->src = lwip_htons(STACK_PORT);
    udphdr->dest = lwip_htons(STACK_PORT);
    udphdr->len = lwip_htons(p->tot_len);
    /* in UDP, 0 checksum means 'no checksum' */
    u16_t udpchksum = ip_chksum_pseudo_partial(p, IP_PROTO_UDP, p->tot_len, UDP_HLEN, &sipaddr, &dipaddr);
    u32_t acc = udpchksum + (u16_t)~(chksum);
    udpchksum = FOLD_U32T(acc);
    if (udpchksum == 0x0000) {
        udpchksum = 0xffff;
    }
    udphdr->chksum = udpchksum;
    udphdr->chksum = 0;

    ReplayUdpEncodeIp(p, &sipaddr, &dipaddr);
}

static void ReplayUdpTask()
{
    int ret;
    ret = sys_thread_new("replay_udp", ReplayUdp, NULL,
        STACK_TEST_SIZE, TCPIP_THREAD_PRIO);
    ICUNIT_ASSERT_NOT_EQUAL(ret, -1, 3);
}

static void ParsePackageIpUdp(struct netif *netif, struct pbuf *btBuf)
{
    // get scr_addr and dest_addr from ip head
    const struct ip_hdr *iphdr;
    iphdr = (const struct ip_hdr *)btBuf->payload;
    ip4_addr_p_t ip4_addr_scr = iphdr->src;
    ip4_addr_p_t ip4_addr_dest = iphdr->dest;
    char buf[32];
    int dataLen;

    // get scr_port and dest port from tcp head
    u16_t scr_port, dest_port;
    void *data;
    struct udp_hdr *udphdr;
    udphdr = (struct udp_hdr *)((u8_t *)iphdr + IPH_HL_BYTES(iphdr));
    scr_port = udphdr->src;
    dest_port = udphdr->dest;
    data = (void *)((u8_t *)udphdr + UDP_HLEN);

    LogPrintln("======Bt netif send package======");
    LogPrintln("netif full name %s", netif->full_name);
    (void)inet_ntop(AF_INET, &ip4_addr_scr.addr, buf, sizeof(buf));
    LogPrintln("package src ip %s", buf);
    (void)inet_ntop(AF_INET, &ip4_addr_dest.addr, buf, sizeof(buf));
    LogPrintln("package dest ip %s", buf);
    LogPrintln("package src port %d", lwip_ntohs(scr_port));
    LogPrintln("package dest port %d", lwip_ntohs(dest_port));
    LogPrintln("send data %s", (char *)data);
    dataLen = lwip_ntohs(udphdr->len) - UDP_HLEN;
    LogPrintln("send data length %d", dataLen);
    ICUNIT_ASSERT_EQUAL(dataLen, strlen(MSG), 4);
    LogPrintln("=================================");

    // 回应udp报文
    ReplayUdpTask();
}

static void ParsePackageEthernet(struct netif *netif, struct pbuf *p)
{
    u16_t next_hdr_offset = SIZEOF_ETH_HDR;
    // get src mac and dest mac
    struct eth_hdr *ethhdr;
    ethhdr = (struct eth_hdr *)p->payload;
    u16_t type = ethhdr->type;

    LogPrintln("ParsePackageEthernet type is %d", type);
    switch (type) {
#if LWIP_IPV4 && LWIP_ARP
        /* IP packet? */
        case PP_HTONS(ETHTYPE_IP): // 0x0008
            if (!(netif->flags & NETIF_FLAG_ETHARP)) {
                return;
            }
            /* skip Ethernet header (min. size checked above) */
            if (pbuf_remove_header(p, next_hdr_offset)) {
                return;
            } else {
                /* pass to IP layer */
                ParsePackageIpUdp(netif, p);
            }
            break;

        case PP_HTONS(ETHTYPE_ARP): // 0x0608
            if (!(netif->flags & NETIF_FLAG_ETHARP)) {
                return;
            }
            /* skip Ethernet header (min. size checked above) */
            if (pbuf_remove_header(p, next_hdr_offset)) {
                return;
            } else {
                /* pass p to ARP module */
                LogPrintln("recv arp packet");
                ArpPackageProc(netif, p);
            }
            break;
#endif /* LWIP_IPV4 && LWIP_ARP */

        default:
            LogPrintln("type is other %d", type);
            break;
    }
}

static void BtProxySend(struct netif *netif, struct pbuf *p)
{
    if (netif == NULL || p == NULL) {
        LogPrintln("%s : netif = NUll or p = NULL!", __func__);
        return;
    }
#if ETH_PAD_SIZE
    (void)pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif
    ParsePackageEthernet(netif, p);
}

static struct netif *CreateBtNetIf()
{
    struct netif *btNetif = NULL;
    btNetif = (struct netif *)malloc(sizeof(struct netif));
    if (btNetif == NULL) {
        LogPrintln("%s fail : netif malloc fail!", __func__);
        LWIP_ASSERT("btNetif malloc fail.", 0);
        return NULL;
    }
    (void)memset_s(btNetif, sizeof(struct netif), 0, sizeof(struct netif));
    btNetif->drv_send = BtProxySend;
    btNetif->link_layer_type = BT_PROXY_IF;
    btNetif->hwaddr_len = ETH_HWADDR_LEN;
    (void)memcpy_s(&btNetif->hwaddr, sizeof(btNetif->hwaddr), NETIF_MAC, ETH_HWADDR_LEN);
    (void)memcpy_s(&btNetif->full_name, sizeof(btNetif->full_name), NETIF_NAME_BT, sizeof(NETIF_NAME_BT));
    ip4_addr_t gw, ipaddr, netmask;
    IP4_ADDR(&gw, 192, 168, 167, 239);
    IP4_ADDR(&ipaddr, 192, 168, 167, 1);
    IP4_ADDR(&netmask, 255, 255, 255, 255);
    int ret = 0;
    if ((ret = netifapi_netif_add(btNetif, &ipaddr, &netmask, &gw, btNetif, driverif_init, tcpip_input)) != ERR_OK) {
        LogPrintln("%s : netifapi_netif_add fail!,ret=%d", __func__, ret);
        LWIP_ASSERT("btNetif add fail.", 0);
        return NULL;
    }
    LogPrintln("netifapi_netif_add success!");
    netif_set_link_up(btNetif);
    netifapi_netif_set_up(btNetif);
    return btNetif;
}

static void UdpTestNetifTask(void *p)
{
    (void)p;
    LogPrintln("net_socket_test_011.c enter");
    g_testCase = TEST_CASE;
    int sfd;
    struct sockaddr_in srvAddr = { 0 };
    struct sockaddr_in clnAddr = { 0 };
    socklen_t clnAddrLen = sizeof(clnAddr);
    struct ifreq nif;
    int ret;

    // 注册网卡
    btProxyNf = CreateBtNetIf();

    /* socket creation */
    sfd = socket(AF_INET, SOCK_DGRAM, 0);
    LWIP_ASSERT("socket invalid param.", sfd != -1);

    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = inet_addr(STACK_IP_BT);
    srvAddr.sin_port = htons(STACK_PORT);
    ret = bind(sfd, (struct sockaddr*)&srvAddr, sizeof(srvAddr));
    LWIP_ASSERT("socket invalid param.", ret == 0);

    /* 指定网卡接口 */
    char *inface = NETIF_NAME_BT;
    (void)strcpy_s(nif.ifr_name, sizeof(nif.ifr_name), inface);
    if (setsockopt(sfd, SOL_SOCKET, SO_BINDTODEVICE, (char *)&nif, sizeof(nif)) < 0) {
        LogPrintln("set intaface fail");
        LWIP_ASSERT("set intaface fail.", 0);
    }

    /* send */
    clnAddr.sin_family = AF_INET;
    clnAddr.sin_addr.s_addr = inet_addr(PEER_IP_BT);
    clnAddr.sin_port = htons(PEER_PORT);
    (void)memset_s(g_buf, sizeof(g_buf), 0, sizeof(g_buf));
    (void)strcpy_s(g_buf, sizeof(g_buf), MSG);
    ret = sendto(sfd, g_buf, strlen(MSG), 0, (struct sockaddr*)&clnAddr,
        (socklen_t)sizeof(clnAddr));
    LogPrintln("client send success: %d", sfd);
    LWIP_ASSERT("socket invalid param.", ret != -1);

    /* recv */
    (void)memset_s(g_buf, sizeof(g_buf), 0, sizeof(g_buf));
    ret = recvfrom(sfd, g_buf, sizeof(g_buf), 0, (struct sockaddr*)&clnAddr,
        &clnAddrLen);
    LWIP_ASSERT("socket invalid param.", ret == strlen(MSG));
    LogPrintln("cli recv: %s", g_buf);

    /* close socket */
    ret = closesocket(sfd);
    LWIP_ASSERT("socket invalid param.", ret != -1);
    return;
}

int UdpTestNetif()
{
    int ret = sys_thread_new("udp_test_netif", UdpTestNetifTask, NULL,
        STACK_TEST_SIZE, TCPIP_THREAD_PRIO);
    ICUNIT_ASSERT_NOT_EQUAL(ret, -1, 23);
    return ret;
}

static void ArpPackageProc(struct netif *netif, struct pbuf *p)
{
    struct etharp_hdr *hdr;
    ip4_addr_t sipaddr, dipaddr;
    hdr = (struct etharp_hdr *)p->payload;

    if (hdr->opcode != PP_HTONS(ARP_REQUEST)) {
        LogPrintln("opcode %d is not arp request", hdr->opcode);
        return;
    }

    inet_pton(AF_INET, GATEWAY_IP_BT, &sipaddr);
    inet_pton(AF_INET, STACK_IP_BT, &dipaddr);
    if (memcmp(&hdr->dipaddr, &sipaddr, sizeof(ip4_addr_t)) != EOK) {
        LogPrintln("hdr->dipaddr %u is invalid", hdr->dipaddr);
        return;
    }

    // 回应arp报文
    ReplayArpTask();
}

static void ReplayArpEncodeEthernet(struct netif *netif, struct pbuf *p)
{
    int ret;
    ret = pbuf_add_header(p, SIZEOF_ETH_HDR);
    ICUNIT_ASSERT_EQUAL(ret, 0, 1);

    struct eth_hdr *ethhdr;
    ethhdr = (struct eth_hdr *)p->payload;
    ethhdr->type = lwip_htons(ETHTYPE_ARP);
    SMEMCPY(&ethhdr->dest, NETIF_MAC, ETH_HWADDR_LEN);
    SMEMCPY(&ethhdr->src,  PEER_MAC, ETH_HWADDR_LEN);

    driverif_input(netif, p);
}

static void ReplayArp(void *ptemp)
{
    struct etharp_hdr *hdr;
    ip4_addr_t sipaddr, dipaddr;

    (void)ptemp;
    LogPrintln("encode arp replay packet");
    inet_pton(AF_INET, GATEWAY_IP_BT, &sipaddr);
    inet_pton(AF_INET, STACK_IP_BT, &dipaddr);
    struct pbuf *p = pbuf_alloc(PBUF_LINK, SIZEOF_ETHARP_HDR, PBUF_RAM);
    hdr = (struct etharp_hdr *)p->payload;
    hdr->opcode = PP_HTONS(ARP_REPLY);
    /* Write the ARP MAC-Addresses */
    SMEMCPY(&hdr->shwaddr, PEER_MAC, ETH_HWADDR_LEN);
    SMEMCPY(&hdr->dhwaddr, NETIF_MAC, ETH_HWADDR_LEN);
    /* Copy struct ip4_addr_wordaligned to aligned ip4_addr, to support compilers without
        * structure packing. */
    IPADDR_WORDALIGNED_COPY_FROM_IP4_ADDR_T(&hdr->sipaddr, &sipaddr);
    IPADDR_WORDALIGNED_COPY_FROM_IP4_ADDR_T(&hdr->dipaddr, &dipaddr);
    hdr->hwtype = PP_HTONS(LWIP_IANA_HWTYPE_ETHERNET);
    hdr->proto = PP_HTONS(ETHTYPE_IP);
    /* set hwlen and protolen */
    hdr->hwlen = ETH_HWADDR_LEN;
    hdr->protolen = sizeof(ip4_addr_t);

    ReplayArpEncodeEthernet(btProxyNf, p);
}

static void ReplayArpTask()
{
    int ret;
    ret = sys_thread_new("replay_arp", ReplayArp, NULL,
        STACK_TEST_SIZE, TCPIP_THREAD_PRIO);
    ICUNIT_ASSERT_NOT_EQUAL(ret, -1, 23);
}