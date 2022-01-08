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

#include <lwip/sys.h>
#include <lwip/etharp.h>
#include <lwip/netifapi.h>
#include <lwip/priv/api_msg.h>

#define NETIFAPI_VAR_REF(name)      API_VAR_REF(name)
#define NETIFAPI_VAR_DECLARE(name)  API_VAR_DECLARE(struct netifapi_msg, name)
#define NETIFAPI_VAR_ALLOC(name)    API_VAR_ALLOC(struct netifapi_msg, MEMP_NETIFAPI_MSG, name, ERR_MEM)
#define NETIFAPI_VAR_FREE(name)     API_VAR_FREE(MEMP_NETIFAPI_MSG, name)

static struct netif *netif_find_by_name(const char *name)
{
    struct netif *netif = NULL;
    LWIP_ASSERT_CORE_LOCKED();
    if (name == NULL) {
        return NULL;
    }
    NETIF_FOREACH(netif) {
        if (strcmp("lo", name) == 0 && (netif->name[0] == 'l' && netif->name[1] == 'o')) {
            LWIP_DEBUGF(NETIF_DEBUG, ("netif_find_by_name: found lo\n"));
            return netif;
        }

        if (strcmp(netif->full_name, name) == 0) {
            LWIP_DEBUGF(NETIF_DEBUG, ("netif_find_by_name: found %s\n", name));
            return netif;
        }
    }

    LWIP_DEBUGF(NETIF_DEBUG, ("netif_find_by_name: didn't find %s\n", name));
    return NULL;
}

static err_t netifapi_do_find_by_name(struct tcpip_api_call_data *m)
{
    /* cast through void* to silence alignment warnings.
     * We know it works because the structs have been instantiated as struct netifapi_msg */
    struct netifapi_msg *msg = (struct netifapi_msg *)(void *)m;
    msg->netif = netif_find_by_name(msg->msg.ifs.name);
    return ERR_OK;
}

struct netif *netifapi_netif_find_by_name(const char *name)
{
    struct netif *netif = NULL;
    NETIFAPI_VAR_DECLARE(msg);
    NETIFAPI_VAR_ALLOC(msg);
    NETIFAPI_VAR_REF(msg).netif = NULL;
#if LWIP_MPU_COMPATIBLE
    if (strncpy_s(NETIFAPI_VAR_REF(msg).msg.ifs.name, NETIF_NAMESIZE, name, NETIF_NAMESIZE - 1)) {
        NETIFAPI_VAR_FREE(msg);
        return netif;
    }
    NETIFAPI_VAR_REF(msg).msg.ifs.name[NETIF_NAMESIZE - 1] = '\0';
#else
    NETIFAPI_VAR_REF(msg).msg.ifs.name = (char *)name;
#endif /* LWIP_MPU_COMPATIBLE */

    (void)tcpip_api_call(netifapi_do_find_by_name, &API_VAR_REF(msg).call);
    netif = msg.netif;
    NETIFAPI_VAR_FREE(msg);
    return netif;
}

#if LWIP_IPV6
int ip6addr_aton(const char *cp, ip6_addr_t *addr)
{
    const int ipv6_blocks = 8;
    u16_t current_block_index = 0;
    u16_t current_block_value = 0;
    u16_t addr16[ipv6_blocks];
    u16_t *a16 = (u16_t *)addr->addr;
    int squash_pos = ipv6_blocks;
    int i;
    const char *sc = cp;
    const char *ss = cp-1;

    for (; ; sc++) {
        if (current_block_index >= ipv6_blocks) {
            return 0; // address too long
        }
        if (*sc == 0) {
            if (sc - ss == 1) {
                if (squash_pos != current_block_index) {
                    return 0; // empty address or address ends with a single ':'
                } // else address ends with one valid "::"
            } else {
                addr16[current_block_index++] = current_block_value;
            }
            break;
        } else if (*sc == ':') {
            if (sc - ss == 1) {
                if (sc != cp || sc[1] != ':') {
                    return 0; // address begins with a single ':' or contains ":::"
                } // else address begins with one valid "::"
            } else {
                addr16[current_block_index++] = current_block_value;
            }
            if (sc[1] == ':') {
                if (squash_pos != ipv6_blocks) {
                    return 0; // more than one "::"
                }
                squash_pos = current_block_index;
                sc++;
            }
            ss = sc; // ss points to the recent ':' position
            current_block_value = 0;
        } else if (lwip_isxdigit(*sc) && (sc - ss) < 5) { // 4 hex-digits at most
            current_block_value = (current_block_value << 4) +
                (*sc | ('a' - 'A')) - '0' - ('a' - '9' - 1) * (*sc >= 'A');
#if LWIP_IPV4
        } else if (*sc == '.' && current_block_index < ipv6_blocks - 1) {
            ip4_addr_t ip4;
            int ret = ip4addr_aton(ss+1, &ip4);
            if (!ret) {
                return 0;
            }
            ip4.addr = lwip_ntohl(ip4.addr);
            addr16[current_block_index++] = (u16_t)(ip4.addr >> 16);
            addr16[current_block_index++] = (u16_t)(ip4.addr);
            break;
#endif /* LWIP_IPV4 */
        } else {
            return 0; // unexpected char or too many digits
        }
    }

    if (squash_pos == ipv6_blocks && current_block_index != ipv6_blocks) {
        return 0; // address too short
    }
    if (squash_pos != ipv6_blocks && current_block_index == ipv6_blocks) {
        return 0; // unexpected "::" in address
    }

    for (i = 0; i < squash_pos; ++i) {
        a16[i] = lwip_htons(addr16[i]);
    }
    for (; i < ipv6_blocks - current_block_index + squash_pos; ++i) {
        a16[i] = 0;
    }
    for (; i < ipv6_blocks; ++i) {
        a16[i] = lwip_htons(addr16[i - ipv6_blocks + current_block_index]);
    }

    return 1;
}
#endif /* LWIP_IPV6 */
