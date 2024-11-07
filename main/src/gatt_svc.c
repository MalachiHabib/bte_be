/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
/* Includes */
#include "gatt_svc.h"
#include "common.h"
#include "heart_rate.h"
#include "sensor.h"

/* Private function declarations */
static int temp_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                           struct ble_gatt_access_ctxt *ctxt, void *arg);

/* Private variables */
/* Temp sensor service */
static const ble_uuid16_t temp_svc_uuid = BLE_UUID16_INIT(0x1809);

static uint16_t temp_char_val_handle;
static const ble_uuid16_t temp_chr_uuid = BLE_UUID16_INIT(0x2A1C);

static uint16_t temp_chr_conn_handle = 0;
static bool temp_chr_conn_handle_inited = false;
static bool temp_ind_status = false;

/* GATT services table */
static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &temp_svc_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]){
            {/* Temperature characteristic */
             .uuid = &temp_chr_uuid.u,
             .access_cb = temp_chr_access,
             .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_INDICATE,
             .val_handle = &temp_char_val_handle},
            {
                0, /* No more characteristics in this service */
            }},
    },
    {
        0, /* No more services */
    },
};

/* Function to convert float to IEEE-11073 FLOAT-Type format */
void float_to_ieee11073(float temperature, uint8_t *ieee_data)
{
    int32_t mantissa;
    int8_t exponent;
    uint32_t ieee_float;

    /* Special cases */
    if (temperature == 0.0f)
    {
        mantissa = 0;
        exponent = 0;
    }
    else
    {
        /* Scale the temperature to maintain precision */
        exponent = -2; // We will multiply mantissa by 10^-2
        mantissa = (int32_t)(temperature * 100);

        /* Ensure mantissa fits in 24 bits */
        if (mantissa > 0x7FFFFF)
            mantissa = 0x7FFFFF;
        else if (mantissa < -0x800000)
            mantissa = -0x800000;
    }

    /* Combine mantissa and exponent into IEEE-11073 FLOAT-Type */
    ieee_float = ((uint32_t)(uint8_t)exponent << 24) | (mantissa & 0xFFFFFF);

    /* Store in little-endian format */
    ieee_data[0] = (uint8_t)(ieee_float & 0xFF);
    ieee_data[1] = (uint8_t)((ieee_float >> 8) & 0xFF);
    ieee_data[2] = (uint8_t)((ieee_float >> 16) & 0xFF);
    ieee_data[3] = (uint8_t)((ieee_float >> 24) & 0xFF);
}

static int temp_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                           struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR)
    {
        uint8_t flags = 0x00;  // Flags: 0x00 indicates Celsius, no timestamp, no temperature type
        uint8_t temp_bytes[4]; // IEEE-11073 FLOAT-Type format data

        /* Get the current temperature */
        float current_temperature = get_temperature();

        /* Convert the temperature to IEEE-11073 format */
        float_to_ieee11073(current_temperature, temp_bytes);

        /* Prepare the data to send */
        uint8_t data[5];
        data[0] = flags;                                  // Flags byte
        memcpy(&data[1], temp_bytes, sizeof(temp_bytes)); // Temperature value

        /* Append data to the characteristic's response */
        int rc = os_mbuf_append(ctxt->om, data, sizeof(data));
        if (rc != 0)
        {
            ESP_LOGE("GATT_SVC", "Failed to append temperature data");
            return BLE_ATT_ERR_INSUFFICIENT_RES;
        }

        ESP_LOGI("GATT_SVC", "Temperature read: %.2f°C", current_temperature);
        return 0; // Indicate success
    }
    return BLE_ATT_ERR_UNLIKELY; // Operation not supported
}

void send_temperature_indication(void)
{
    if (temp_ind_status && temp_chr_conn_handle_inited)
    {
        ble_gatts_indicate(temp_chr_conn_handle,
                           temp_char_val_handle);
        ESP_LOGI(TAG, "temp indication sent!");
    }
}

/*
 *  Handle GATT attribute register events
 *      - Service register event
 *      - Characteristic register event
 *      - Descriptor register event
 */
void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg)
{
    char buf[BLE_UUID_STR_LEN];
    switch (ctxt->op)
    {
    case BLE_GATT_REGISTER_OP_SVC:
        ESP_LOGD("GATT_SVR", "Registered service %s with handle=%d",
                 ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf),
                 ctxt->svc.handle);
        break;
    case BLE_GATT_REGISTER_OP_CHR:
        ESP_LOGD("GATT_SVR", "Registered characteristic %s with val_handle=%d",
                 ble_uuid_to_str(ctxt->chr.chr_def->uuid, buf),
                 ctxt->chr.val_handle);
        break;
    case BLE_GATT_REGISTER_OP_DSC:
        ESP_LOGD("GATT_SVR", "Registered descriptor %s with handle=%d",
                 ble_uuid_to_str(ctxt->dsc.dsc_def->uuid, buf),
                 ctxt->dsc.handle);
        break;
    default:
        assert(0);
        break;
    }
}

/*
 *  GATT server subscribe event callback
 *      1. Update heart rate subscription status
 */

void gatt_svr_subscribe_cb(struct ble_gap_event *event)
{
    /* Check connection handle */
    if (event->subscribe.conn_handle != BLE_HS_CONN_HANDLE_NONE)
    {
        ESP_LOGI(TAG, "subscribe event; conn_handle=%d attr_handle=%d",
                 event->subscribe.conn_handle, event->subscribe.attr_handle);
    }
    else
    {
        ESP_LOGI(TAG, "subscribe by nimble stack; attr_handle=%d",
                 event->subscribe.attr_handle);
    }

    if (event->subscribe.attr_handle == temp_char_val_handle)
    {
        temp_chr_conn_handle = event->subscribe.conn_handle;
        temp_chr_conn_handle_inited = true;
        temp_ind_status = event->subscribe.cur_indicate;
    }
}

/*
 *  GATT server initialization
 *      1. Initialize GATT service
 *      2. Update NimBLE host GATT services counter
 *      3. Add GATT services to server
 */
int gatt_svc_init(void)
{
    /* Local variables */
    int rc;

    /* 1. GATT service initialization */
    ble_svc_gatt_init();

    /* 2. Update GATT services counter */
    rc = ble_gatts_count_cfg(gatt_svr_svcs);
    if (rc != 0)
    {
        return rc;
    }

    /* 3. Add GATT services */
    rc = ble_gatts_add_svcs(gatt_svr_svcs);
    if (rc != 0)
    {
        return rc;
    }

    return 0;
}
