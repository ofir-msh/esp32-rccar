#include "nvs_flash.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"                // nimble/host - required to host peripheral
#include "services/gap/ble_svc_gap.h"   // nimble/host/services/gap -> gap = the connecting & advertising stage
#include "services/gatt/ble_svc_gatt.h" // nimble/host/services/gatt -> gatt = device profiler - details all services

uint8_t ble_addr_type;

int ble_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type)
    {
    case BLE_GAP_EVENT_CONNECT:
        printf("Connected\n");
        break;
    case BLE_GAP_EVENT_DISCONNECT:
        printf("Disconnected\n");
        break;
    default:
        break;
    }
    return 0;
}

// sync happens almost immediately after init
void ble_on_sync()
{
    int rc;
    ble_hs_id_infer_auto(0, &ble_addr_type);

    struct ble_hs_adv_fields adv_fields;
    memset(&adv_fields, 0, sizeof(adv_fields));
    const char *device_name = ble_svc_gap_device_name();
    adv_fields.name = (uint8_t *)device_name;
    adv_fields.name_len = strlen(device_name);
    adv_fields.name_is_complete = 1;
    ble_gap_adv_set_fields(&adv_fields);

    struct ble_gap_adv_params adv_params;
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;
    // advertising - advertise characteristics to scans made by other bt devices
    // discovery - scan bt devices around
    // ble_gap_event = handler when something happens in gap stage
    rc = ble_gap_adv_start(ble_addr_type, NULL, BLE_HS_FOREVER, &adv_params, ble_gap_event, NULL);
    assert(rc == 0);
}

void host_task(void *param)
{
    nimble_port_run();
    nimble_port_freertos_deinit();
}

void initializeBLE(struct ble_gatt_svc_def *gatt_svcs)
{
    esp_err_t rc = nvs_flash_init(); // initialize non volatile storage
    if (rc == ESP_ERR_NVS_NO_FREE_PAGES || rc == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        rc = nvs_flash_init();
    }
    ESP_ERROR_CHECK(rc);

    rc = nimble_port_init(); // initialize esp's port of nimBLE
    ESP_ERROR_CHECK(rc);
    ble_svc_gap_device_name_set("RCcar"); // set BT device name
    ble_svc_gap_init();                   // initialize GAP service - advertising/discovery
    ble_svc_gatt_init();                  // initialize GATT service - allows device to expose data to clients (ATT) as services
    ble_gatts_count_cfg(gatt_svcs);       // reconfigure gatts config according to the listed services in gatt_svcs
    ble_gatts_add_svcs(gatt_svcs);        // register services to bt device
    ble_hs_cfg.sync_cb = ble_on_sync;     // attach on_sync handler
    nimble_port_freertos_init(host_task); // start BLE in its own host task to not block thread
}