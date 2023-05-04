int ble_gap_event(struct ble_gap_event *event, void *arg);
void ble_on_sync();
void host_task(void *param);
void initializeBLE(struct ble_gatt_svc_def *gatt_svcs);