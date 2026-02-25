#include "temphumid.h"
#include "esp_log.h"

#define SHT3X_CMD_MEASURE_HIGHREP 0x2400

static const char *TAG = "TEMPHUMID";

static i2c_port_t s_i2c_port;
static uint8_t s_address;

esp_err_t temphumid_init(i2c_port_t i2c_port, uint8_t address)
{
    if (address != 0x44 && address != 0x45) {
        ESP_LOGE(TAG, "Invalid SHT3x address");
        return ESP_ERR_INVALID_ARG;
    }

    s_i2c_port = i2c_port;
    s_address = address;

    uint8_t cmd[2] = {0x30, 0xA2};
    esp_err_t ret = i2c_master_write_to_device(
        s_i2c_port,
        s_address,
        cmd,
        sizeof(cmd),
        pdMS_TO_TICKS(100)
    );


    return ret;
}
esp_err_t temphumid_read(temphumid_data_t *out_data)
{
    if (!out_data) {
        return ESP_ERR_INVALID_ARG;
    }

    uint8_t cmd[2] = {
        (SHT3X_CMD_MEASURE_HIGHREP >> 8) & 0xFF,
        SHT3X_CMD_MEASURE_HIGHREP & 0xFF
    };

    esp_err_t ret = i2c_master_write_to_device(
        s_i2c_port,
        s_address,
        cmd,
        sizeof(cmd),
        pdMS_TO_TICKS(100)
    );
    if (ret != ESP_OK) {
        return ret;
    }

    vTaskDelay(pdMS_TO_TICKS(20));

    uint8_t data[6];
    ret = i2c_master_read_from_device(
        s_i2c_port,
        s_address,
        data,
        6,
        pdMS_TO_TICKS(100)
    );
    if (ret != ESP_OK) {
        return ret;
    }

    uint16_t raw_temp = (data[0] << 8) | data[1];
    uint16_t raw_hum  = (data[3] << 8) | data[4];

    out_data->temperature = -45.0f + (175.0f * ((float)raw_temp / 65535.0f));
    out_data->humidity = 100.0f * ((float)raw_hum / 65535.0f);

    return ESP_OK;
}
