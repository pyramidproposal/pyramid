#ifndef PYR_MMT_STATE_H
#define PYR_MMT_STATE_H

#include <stdint.h>

/*
  Jasmin testing export function definitions.
*/
unsigned char MMT_th_get_fin_jazz(const unsigned char *statep, uint64_t index);

uint64_t MMT_th_get_idx_jazz(const unsigned char *statep, uint64_t index);

void MMT_th_get_sda_jazz(unsigned char *sdap, const unsigned char *statep,
                         uint64_t index);

void MMT_th_set_fin_0_jazz(unsigned char *statep, uint64_t index);

void MMT_th_set_fin_1_jazz(unsigned char *statep, uint64_t index);

void MMT_th_set_idx_jazz(unsigned char *statep, uint64_t idx, uint64_t index);

void MMT_th_set_sda_jazz(unsigned char *statep, const unsigned char *sdap,
                         uint64_t index);

unsigned char MMT_st_get_size_jazz(const unsigned char *stack_p);

unsigned char MMT_st_get_height_jazz(const unsigned char *stack_p,
                                     uint64_t index);

void MMT_st_set_size_jazz(unsigned char *stack_p, unsigned char size);

void MMT_st_set_height_jazz(unsigned char *stack_p, unsigned char height,
                            uint64_t index);

void MMT_stack_pop_jazz(unsigned char *pop_nodep, unsigned char *stack_p);

void MMT_stack_push_jazz(unsigned char *stack_p, unsigned char *push_nodep,
                         unsigned char push_nodeheight);

void MMT_get_auth_node_jazz(unsigned char *nodep, const unsigned char *statep,
                            uint64_t index);

void MMT_set_auth_node_jazz(unsigned char *statep, const unsigned char *nodep,
                            uint64_t index);

#endif
