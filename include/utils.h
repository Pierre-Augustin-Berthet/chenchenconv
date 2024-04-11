#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#define OUTPUT stdout

uint64_t rand64(void);
int32_t randmod(int32_t mod);

void print_binary_form(uint64_t in);

int32_t addq(int32_t ina, int32_t inb, int32_t mod);
int32_t subq(int32_t ina, int32_t inb, int32_t mod);
int32_t mulq(int32_t ina, int32_t inb, int32_t mod);

#endif