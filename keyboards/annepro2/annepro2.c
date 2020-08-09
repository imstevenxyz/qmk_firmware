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

bool capsActive = false;

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
    sendLedCommand(CMD_POST_INIT);
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

/*
layer_state_t layer_state_set_kb(layer_state_t layer){
    layer_state_set_user(layer);
    return layer;
}*/

/*
 * Update leds based on kb status
 */
bool led_update_kb(led_t status){
    bool res = led_update_user(status);
    if(res){
        if(status.caps_lock){
            setKeyColorOverride(2, 0, 0xFF0000);
            capsActive = true;
        }else if(capsActive){
            capsActive = false;
            resetColorOverride();
        }
    }
    return res;
}

void resetColorOverride(){
    sendLedCommand(CMD_LED_SET);
    sendLedCommand(UNSET);
}

void setKeyColorOverride(uint8_t row, uint8_t col, uint32_t color){
    sendLedCommand(CMD_LED_SET);
    sendLedCommand(SET_KEY);
    sdPut(&SD0, row);
    sdPut(&SD0, col);
    sdWrite(&SD0, (uint8_t *)&color, sizeof(uint32_t));
}

void setFNLayerColorOverride(uint8_t layer, uint32_t color){
    sendLedCommand(CMD_LED_SET);
    sendLedCommand(SET_FN);
    sdPut(&SD0, layer);
    sdWrite(&SD0, (uint8_t *)&color, sizeof(uint32_t));
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
