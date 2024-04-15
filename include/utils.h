#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#define OUTPUT stdout

uint32_t rand64(void);
uint32_t randmod(uint32_t mod);

void print_binary_form(uint32_t in);

uint32_t addq(uint32_t ina, uint32_t inb, uint32_t mod);
uint32_t subq(uint32_t ina, uint32_t inb, uint32_t mod);
uint32_t mulq(uint32_t ina, uint32_t inb, uint32_t mod);

#endif