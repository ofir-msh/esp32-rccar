#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "host/ble_hs.h" // nimble/host - required to host peripheral
#include "movement.h"
#include "btdriver.h"
#include "sdkconfig.h"

static uint8_t command;
int device_rw(uint16_t con_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg);

struct ble_gatt_svc_def gatt_svcs[] = {
    {.type = BLE_GATT_SVC_TYPE_PRIMARY,
     .uuid = BLE_UUID16_DECLARE(0x1812), // define uuid for Generic Service device type
     .characteristics = (struct ble_gatt_chr_def[]){
         {.uuid = BLE_UUID16_DECLARE(0x2A68), // define uuid for Navigation characteristic
          .flags = BLE_GATT_CHR_F_WRITE,
          .access_cb = device_rw}, // to make count_cfg and add_svcs know where to stop (in initializeBLE)
         {0}}},                    // autofill with {0} to signify end of characteristics
    {0}};                          // autofill with {0} to signify end of services

int device_rw(uint16_t con_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR && ctxt->om->om_len == 1)
    {
        command = *(ctxt->om->om_data);
        printf("command: %d %c\n", command, command);
        switch (command)
        {
        case 'f':
        case 'b':
            engine(true);
            go(command > 'd');
            break;
        case 'l':
        case 'r':
            engine(true);
            rotate(command < 'o');
            break;
        default:
            return 0;
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        engine(false);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
    return 0;
}

void app_main(void)
{
    initializePins();
    initializeBLE(gatt_svcs);
}
