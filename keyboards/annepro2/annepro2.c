#include "ch.h"
#include "hal.h"
#include "annepro2.h"
#include "annepro2_ble.h"
#include "qmk_ap2_led.h"

/*
 * Serial configuration
 */
static const SerialConfig ledUartConfig = {
  .speed = 115200,
};

static const SerialConfig bleUartConfig = {
  .speed = 115200,
};

static uint8_t ledMcuWakeup[11] = {
    0x7b, 0x10, 0x43, 0x10, 0x03, 0x00, 0x00, 0x7d, 0x02, 0x01, 0x02
};


uint32_t annepro2LedMatrix[MATRIX_ROWS * MATRIX_COLS];

/*
 * Pre init
 * Start LED MCU
 * Start BT MCU
 */
void keyboard_pre_init_kb(void){
    keyboard_pre_init_user();
    
    sdStart(&SD0, &ledUartConfig);
    sdWrite(&SD0, ledMcuWakeup, 11);

    sdStart(&SD1, &bleUartConfig);
    annepro2_ble_startup();
}

/*
 * Post init
 */
void keyboard_post_init_kb(void){
    keyboard_post_init_user();
}

/*
 * Matrix init
 */
void matrix_init_kb(void){
    matrix_init_user();
}

/*
 * Send command to LED MCU
 */
void sendLedCommand(uint8_t cmd){
    sdPut(&SD0, cmd);
}

void annepro2LedUpdate(uint8_t row, uint8_t col){
    sdPut(&SD0, CMD_LED_SET);
    sdPut(&SD0, SET_KEY);
    sdPut(&SD0, row);
    sdPut(&SD0, col);
    sdWrite(&SD0, (uint8_t *)&annepro2LedMatrix[row * MATRIX_COLS + col], sizeof(uint32_t));
}

/*
 * Update leds based on kb status
 */
bool led_update_kb(led_t status){
    bool res = led_update_user(status);
    if(res){
        annepro2LedMatrix[2 * MATRIX_COLS] = status.caps_lock ? 0xFF3355 : 0;
        annepro2LedUpdate(2, 0);
    }
    return res;
}

/*!
 * Process keypresses
 * @returns false   process complete
 */
bool process_record_kb(uint16_t keycode, keyrecord_t *record){
    if (record->event.pressed){
        switch (keycode){
            case KC_AP2_BT1:
                annepro2_ble_broadcast(0);
                return false;
            case KC_AP2_BT2:
                annepro2_ble_broadcast(1);
                return false;
            case KC_AP2_BT3:
                annepro2_ble_broadcast(2);
                return false;
            case KC_AP2_BT4:
                annepro2_ble_broadcast(3);
                return false;
            case KC_AP2_USB:
                annepro2_ble_disconnect();
                return false;
            case KC_AP2_BT_UNPAIR:
                annepro2_ble_unpair();
                return false;
            default:
                break;
        }
    }
    return process_record_user(keycode, record);
}
