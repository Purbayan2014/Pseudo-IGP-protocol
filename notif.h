/*
 * =====================================================================================
 *
 *       Filename:  notif.h
 *
 *    Description: This file implements Generaic Notif Chain structures definitions 
 *
 *        Version:  1.0
 *
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Purbayan Majumder, purbayan2014@gmail.com

 *
 * =====================================================================================
 */

#ifndef __NOTIF_CHAIN_
#define __NOTIF_CHAIN_

#include <stddef.h>  /* for size_t */
#include <stdbool.h>
#include "utils.h"
#include "gluethread/glthread.h"

#define MAX_NOTIF_KEY_SIZE	1536

typedef void (*nfc_app_cb)(void *, size_t);
typedef bool (*nfc_pkt_trap) (char *, size_t);

typedef struct notif_chain_elem_{

    char key[MAX_NOTIF_KEY_SIZE];
    size_t key_size;
    bool is_key_set;
    nfc_app_cb app_cb;
	nfc_pkt_trap pkt_trap_cb;
    glthread_t glue;
} notif_chain_elem_t;
GLTHREAD_TO_STRUCT(glthread_glue_to_notif_chain_elem,
                   notif_chain_elem_t, glue);

typedef struct notif_chain_ {

    char nfc_name[64];
    glthread_t notif_chain_head;
} notif_chain_t;

void
nfc_register_notif_chain(notif_chain_t *nfc,
                     notif_chain_elem_t *nfce);

void
nfc_invoke_notif_chain(notif_chain_t *nfc,
                       void *arg, size_t arg_size,
                       char *key, size_t key_size);

void
nfc_de_register_notif_chain(notif_chain_t *nfc,
                    notif_chain_elem_t *nfce_template);

#endif /*  __NOTIF_CHAIN_  */
