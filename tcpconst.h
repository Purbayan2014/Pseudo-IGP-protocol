/*
 * =====================================================================================
 *
 *       Filename:  tcpconst.h
 *
 *    Description:  This file defines all standard Constants used by TCPIP stack
 *
 *        Version:  1.0

 *       Revision:  1.0
 *       Compiler:  gcc
 *
 *         Author: Purbayan Majumder

.
 *        This program is free software: you can redistribute it and/or modify
 *        it under the terms of the GNU General Public License as published by  
 *        the Free Software Foundation, version 3.
 *
 *        This program is distributed in the hope that it will be useful, but 
 *        WITHOUT ANY WARRANTY; without even the implied warranty of 
 *        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 *        General Public License for more details.
 *
 *        You should have received a copy of the GNU General Public License 
 *        along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * =====================================================================================
 */

#ifndef __TCPCONST__
#define __TCPCONST__

#include <stdint.h>

typedef enum{

    ETH_HDR,
    IP_HDR
} hdr_type_t;

/*Specified in ethernet_hdr->type*/
#define ARP_BROAD_REQ   1
#define ARP_REPLY       2
#define PROTO_ARP         806
#define BROADCAST_MAC   0xFFFFFFFFFFFF
#define ETH_IP          0x0800
#define ICMP_PRO        1
#define ICMP_ECHO_REQ   8
#define ICMP_ECHO_REP   0
#define PROTO_ISIS      0x83
#define MTCP            20
#define USERAPP1        21
#define VLAN_8021Q_PROTO    0x8100
#define IP_IN_IP        4
#define NMP_HELLO_MSG_CODE	13 /*Randomly chosen*/
#define INTF_MAX_METRIC     16777215 /*Choosen as per the standard = 2^24 -1*/
#define INTF_METRIC_DEFAULT 1
#define TCP_LOG_BUFFER_LEN	256
 /* Should be less than or equal to UT_PARSER_BUFF_MAX_SIZE */
#define NODE_PRINT_BUFF_LEN (2048 * 10)

/*Add DDCP Protocol Numbers*/
#define DDCP_MSG_TYPE_FLOOD_QUERY    1  /*Randomly chosen, should not exceed 2^16 -1*/
#define DDCP_MSG_TYPE_UCAST_REPLY    2  /*Randomly chosen, must not exceed 255*/
#define PKT_BUFFER_RIGHT_ROOM        128   
#define MAX_NXT_HOPS        4


/* Protocol IDs*/
#define PROTO_STATIC 1

static inline unsigned char *
proto_name_str (uint16_t proto) {

    switch(proto) {
        case PROTO_ISIS:
            return "isis";
        case PROTO_STATIC:
            return "static";
        case PROTO_ARP:
            return "arp";
        default: ;
    }
}

#endif /* __TCPCONST__ */

