#pragma once
#ifndef GPIO_COMMON_H
#define GPIO_COMMON_H

//  Standard Libraries
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

int32_t gpio_export(uint16_t gpio_pin);
int32_t gpio_direction(uint16_t gpio_pin, const uint8_t *direction);
int32_t gpio_write(uint16_t gpio_pin, uint8_t value);
int32_t gpio_read(uint16_t gpio_pin);

#endif