/*
 * =====================================================================================
 *
 *       Filename:  comm.h
 *
 *    Description:  This file defines the structures to setup communication between 
 *    nodes of the topology
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

#ifndef __COMM__
#define __COMM__

#include <stdint.h>

#define MAX_PACKET_BUFFER_SIZE   2048

typedef struct node_ node_t;
typedef struct interface_ interface_t;

typedef struct ev_dis_pkt_data_{

    node_t *recv_node;
    interface_t *recv_intf;
    char *pkt;
    uint32_t pkt_size;
} ev_dis_pkt_data_t;

int
send_pkt_to_self(char *pkt, uint32_t pkt_size, interface_t *interface);

/* API to send the packet out of the interface.
 * Nbr node must receieve the packet on other end
 * of the link*/
int
send_pkt_out(char *pkt, uint32_t pkt_size, interface_t *interface);

/*API to recv packet from interface*/
int
pkt_receive(node_t *node, interface_t *interface, 
            char *pkt, uint32_t pkt_size);

/* API to flood the packet out of all interfaces
 * of the node*/
int
send_pkt_flood(node_t *node, 
               interface_t *exempted_intf, 
               char *pkt, uint32_t pkt_size);

#endif /* __COMM__ */
