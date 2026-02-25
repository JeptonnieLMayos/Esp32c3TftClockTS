#pragma once
#include "driver/i2c.h"

typedef struct {
    float temperature;
    float humidity;
} temphumid_data_t;

esp_err_t temphumid_init(i2c_port_t i2c_port, uint8_t address);
esp_err_t temphumid_read(temphumid_data_t *out_data);