#ifndef __I2C_SLAVE_H
#define __I2C_SLAVE_H

#include <stdint.h>

#define APB_CLOCK FUNCONF_SYSTEM_CORE_CLOCK

#endif

void init_i2c_master(void);
uint8_t i2c_send(uint8_t addr, uint8_t *data, uint8_t sz);
int i2c_receive(uint8_t addr, uint8_t *buf, uint8_t sz);
int i2c_slave_available(uint8_t addr);


