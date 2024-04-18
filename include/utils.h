#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#define OUTPUT stdout

uint64_t rand64(void);
uint64_t randmod(uint64_t mod);

void print_binary_form(uint64_t in);

uint64_t addq(uint64_t ina, uint64_t inb, uint64_t mod);
uint64_t subq(uint64_t ina, uint64_t inb, uint64_t mod);
uint64_t mulq(uint64_t ina, uint64_t inb, uint64_t mod);

#endif