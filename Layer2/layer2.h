/*
 * =====================================================================================
 *
 *       Filename:  layer2.h
 *
 *    Description: This file defines the structures required for Layer 2 functionality 
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

#ifndef __LAYER2__
#define __LAYER2__

#include <stdlib.h>  /*for calloc*/
#include <stdint.h>
#include "../net.h"
#include "../gluethread/glthread.h"
#include "../tcpconst.h"

#pragma pack (push,1)
typedef struct ethernet_hdr_{

    mac_add_t dst_mac;
    mac_add_t src_mac;
    unsigned short type;
    char payload[248];  /*Max allowed 1500*/
    uint32_t FCS;
} ethernet_hdr_t;
#pragma pack(pop)

#define ETH_FCS_SIZE    (sizeof(((ethernet_hdr_t *)0)->FCS))

#define ETH_HDR_SIZE_EXCL_PAYLOAD   \
    (sizeof(ethernet_hdr_t) - sizeof(((ethernet_hdr_t *)0)->payload))

#define ETH_FCS(eth_hdr_ptr, payload_size)  \
    (*(uint32_t *)(((char *)(((ethernet_hdr_t *)eth_hdr_ptr)->payload) + payload_size)))

/*APIs to be used to create topologies*/
void
node_set_intf_l2_mode(node_t *node, char *intf_name, intf_l2_mode_t intf_l2_mode);

void
node_set_intf_vlan_membership(node_t *node, char *intf_name, uint32_t vlan_id);


/*VLAN support*/

#pragma pack (push,1)
/*Vlan 802.1q 4 byte hdr*/
typedef struct vlan_8021q_hdr_{

    unsigned short tpid; /* = 0x8100*/
    short tci_pcp : 3 ;  /* inital 4 bits not used in this course*/
    short tci_dei : 1;   /*Not used*/
    short tci_vid : 12 ; /*Tagged vlan id*/
} vlan_8021q_hdr_t;

typedef struct vlan_ethernet_hdr_{

    mac_add_t dst_mac;
    mac_add_t src_mac;
    vlan_8021q_hdr_t vlan_8021q_hdr;
    unsigned short type;
    char payload[248];  /*Max allowed 1500*/
    uint32_t FCS;
} vlan_ethernet_hdr_t;
#pragma pack(pop)

static inline uint32_t
GET_802_1Q_VLAN_ID(vlan_8021q_hdr_t *vlan_8021q_hdr){

    return (uint32_t)vlan_8021q_hdr->tci_vid;
}

#define VLAN_ETH_FCS(vlan_eth_hdr_ptr, payload_size)  \
    (*(uint32_t *)(((char *)(((vlan_ethernet_hdr_t *)vlan_eth_hdr_ptr)->payload) + payload_size)))

#define VLAN_ETH_HDR_SIZE_EXCL_PAYLOAD  \
   (sizeof(vlan_ethernet_hdr_t) - sizeof(((vlan_ethernet_hdr_t *)0)->payload))

/* Return 0 if not vlan tagged, else return pointer to 801.1q vlan hdr
 * present in ethernet hdr*/
static inline vlan_8021q_hdr_t *
is_pkt_vlan_tagged(ethernet_hdr_t *ethernet_hdr){

    /*Check the 13th and 14th byte of the ethernet hdr,
     *      * if is value is 0x8100 then it is vlan tagged*/

    vlan_8021q_hdr_t *vlan_8021q_hdr =
        (vlan_8021q_hdr_t *)((char *)ethernet_hdr + (sizeof(mac_add_t) * 2));

    if(vlan_8021q_hdr->tpid == VLAN_8021Q_PROTO)
        return vlan_8021q_hdr;

    return NULL;
}

/*fn to get access to ethernet payload address*/
static inline char *
GET_ETHERNET_HDR_PAYLOAD(ethernet_hdr_t *ethernet_hdr){

   if(is_pkt_vlan_tagged(ethernet_hdr)){
        return ((vlan_ethernet_hdr_t *)(ethernet_hdr))->payload;
   }
   else
       return ethernet_hdr->payload;
}

#define GET_COMMON_ETH_FCS(eth_hdr_ptr, payload_size)   \
        (is_pkt_vlan_tagged(eth_hdr_ptr) ? VLAN_ETH_FCS(eth_hdr_ptr, payload_size) : \
            ETH_FCS(eth_hdr_ptr, payload_size))

static inline void
SET_COMMON_ETH_FCS(ethernet_hdr_t *ethernet_hdr, 
                   uint32_t payload_size,
                   uint32_t new_fcs){

    if(is_pkt_vlan_tagged(ethernet_hdr)){
        VLAN_ETH_FCS(ethernet_hdr, payload_size) = new_fcs;
    }
    else{
        ETH_FCS(ethernet_hdr, payload_size) = new_fcs;
    }
}

static inline ethernet_hdr_t *
TCP_IP_EXPAND_BUFFER_ETH_HDR(char *pkt, uint32_t pkt_size){

    char *temp = calloc(1, pkt_size);   
    memcpy(temp, pkt, pkt_size);    

    ethernet_hdr_t *eth_hdr = (ethernet_hdr_t *)(pkt - ETH_HDR_SIZE_EXCL_PAYLOAD);
    memset((char *)eth_hdr, 0, ETH_HDR_SIZE_EXCL_PAYLOAD);
    memcpy(eth_hdr->payload, temp, pkt_size);   
    SET_COMMON_ETH_FCS(eth_hdr, pkt_size, 0);
    free(temp); 
    return eth_hdr;
}

static inline uint32_t 
GET_ETH_HDR_SIZE_EXCL_PAYLOAD(ethernet_hdr_t *ethernet_hdr){

    if(is_pkt_vlan_tagged(ethernet_hdr)){
        return VLAN_ETH_HDR_SIZE_EXCL_PAYLOAD;        
    }
    else{
        return ETH_HDR_SIZE_EXCL_PAYLOAD; 
    }
}

static inline bool 
l2_frame_recv_qualify_on_interface(interface_t *interface, 
                                    ethernet_hdr_t *ethernet_hdr,
                                    uint32_t *output_vlan_id){

    *output_vlan_id = 0;

    vlan_8021q_hdr_t *vlan_8021q_hdr = 
                        is_pkt_vlan_tagged(ethernet_hdr);

    /* Presence of IP address on interface makes it work in L3 mode,
     * while absence of IP-address automatically make it work in
     * L2 mode provided that it is operational either in ACCESS mode or TRUNK mode.*/

    /* case 10 : If receiving interface is neither working in L3 mode
     * nor in L2 mode, then reject the packet*/
    if(!IS_INTF_L3_MODE(interface) &&
        IF_L2_MODE(interface) == L2_MODE_UNKNOWN){

        return false;
    }

    /* If interface is working in ACCESS mode but at the
     * same time not operating within a vlan, then it must
     * accept untagged packet only*/

    if(IF_L2_MODE(interface) == ACCESS &&
        get_access_intf_operating_vlan_id(interface) == 0){

        if(!vlan_8021q_hdr)
            return true;    /*case 3*/
        else
            return false;   /*case 4*/
    }

    /* if interface is working in ACCESS mode and operating with in
     * vlan, then :
     * 1. it must accept untagged frame and tag it with a vlan-id of an interface
     * 2. Or  it must accept tagged frame but tagged with same vlan-id as interface's vlan operation*/

    uint32_t intf_vlan_id = 0,
                 pkt_vlan_id = 0;

    if(IF_L2_MODE(interface) == ACCESS){
        
        intf_vlan_id = get_access_intf_operating_vlan_id(interface);
            
        if(!vlan_8021q_hdr && intf_vlan_id){
            *output_vlan_id = intf_vlan_id;
            return true; /*case 6*/
        }

        if(!vlan_8021q_hdr && !intf_vlan_id){
            /*case 3*/
            return true;
        }

        pkt_vlan_id = GET_802_1Q_VLAN_ID(vlan_8021q_hdr);
        if(pkt_vlan_id == intf_vlan_id){
            return true;    /*case 5*/
        }
        else{
            return false;   /*case 5*/
        }
    }

    /* if interface is operating in a TRUNK mode, then it must discard all untagged
     * frames*/
    
    if(IF_L2_MODE(interface) == TRUNK){
       
        if(!vlan_8021q_hdr){
            /*case 7 & 8*/
            return false;
        }
    }

    /* if interface is operating in a TRUNK mode, then it must accept the frame
     * which are tagged with any vlan-id in which interface is operating.*/

    if(IF_L2_MODE(interface) == TRUNK && 
            vlan_8021q_hdr){
        
        pkt_vlan_id = GET_802_1Q_VLAN_ID(vlan_8021q_hdr);
        if(is_trunk_interface_vlan_enabled(interface, pkt_vlan_id)){
            return true;    /*case 9*/
        }
        else{
            return false;   /*case 9*/
        }
    }
    
    /*If the interface is operating in L3 mode, and recv vlan tagged frame, drop it*/
    if(IS_INTF_L3_MODE(interface) && vlan_8021q_hdr){
        /*case 2*/
        return false;
    }

    /* If interface is working in L3 mode, then accept the frame only when
     * its dst mac matches with receiving interface MAC*/
    if(IS_INTF_L3_MODE(interface) &&
        memcmp(IF_MAC(interface), 
        ethernet_hdr->dst_mac.mac, 
        sizeof(mac_add_t)) == 0){
        /*case 1*/
        return true;
    }

    /*If interface is working in L3 mode, then accept the frame with
     * broadcast MAC*/
    if(IS_INTF_L3_MODE(interface) &&
        IS_MAC_BROADCAST_ADDR(ethernet_hdr->dst_mac.mac)){
        /*case 1*/
        return true;
    }

    return false;
}

ethernet_hdr_t *
untag_pkt_with_vlan_id(ethernet_hdr_t *ethernet_hdr,
                     uint32_t total_pkt_size,
                     uint32_t *new_pkt_size);

ethernet_hdr_t *
tag_pkt_with_vlan_id(ethernet_hdr_t *ethernet_hdr,
                     uint32_t total_pkt_size,
                     int vlan_id,
                     uint32_t *new_pkt_size);


/* L2 Switching */


/*L2 Switch Owns Mac Table*/

typedef struct mac_table_entry_{

    mac_add_t mac;
    char oif_name[IF_NAME_SIZE];
    glthread_t mac_entry_glue;
} mac_table_entry_t;
GLTHREAD_TO_STRUCT(mac_entry_glue_to_mac_entry, mac_table_entry_t, mac_entry_glue);

typedef struct mac_table_{

    glthread_t mac_entries;
} mac_table_t;


#endif /* __LAYER2__ */
