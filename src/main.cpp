#include <Arduino.h>
#include <ESP32CAN.h>
#include <CAN_config.h>
#include "slcan.h"

#define CAN_TX_PIN GPIO_NUM_5
#define CAN_RX_PIN GPIO_NUM_4

typedef enum prog_mode { MODE_NORMAL, MODE_SLCAN } PROGMODE;
PROGMODE g_mode = MODE_NORMAL;//SLCAN;

CAN_device_t CAN_cfg;               // CAN Config
SLCAN g_slCan(&CAN_cfg);
const int rx_queue_size = 10;       // Receive Queue size


void setup() {
//  Serial.begin(115200);
  Serial.begin(921600);
  Serial.println("Lincomatic ESP32 CANSpy v0.0.1");
  CAN_cfg.speed = CAN_SPEED_500KBPS;
  CAN_cfg.tx_pin_id = CAN_TX_PIN;
  CAN_cfg.rx_pin_id = CAN_RX_PIN;
  CAN_cfg.rx_queue = xQueueCreate(rx_queue_size, sizeof(CAN_frame_t));
  // Init CAN Module
  Serial.print("Init CAN...");
  ESP32Can.CANInit();
  if (g_mode == MODE_SLCAN) {
    g_slCan.begin();
  }
  Serial.println("done.");
}


#ifdef junk
void mainloop()
{

  CAN_frame_t rx_frame;

  // Receive next CAN frame from queue
  if (xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 3 * portTICK_PERIOD_MS) == pdTRUE) {

    if (rx_frame.FIR.B.FF == CAN_frame_std) {
      printf("STD: ");
    }
    else {
      printf("EXT: ");
    }

    if (rx_frame.FIR.B.RTR == CAN_RTR) {
      printf(" RTR from 0x%08X, DLC %d\r\n", rx_frame.MsgID,  rx_frame.FIR.B.DLC);
    }
    else {
      printf(" from 0x%08X, DLC %d, Data ", rx_frame.MsgID,  rx_frame.FIR.B.DLC);
      for (int i = 0; i < rx_frame.FIR.B.DLC; i++) {
        printf("0x%02X ", rx_frame.data.u8[i]);
      }
      printf("\n");
    }
  }
  /*
  // Send CAN Message
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    CAN_frame_t tx_frame;
    tx_frame.FIR.B.FF = CAN_frame_std;
    tx_frame.MsgID = 0x001;
    tx_frame.FIR.B.DLC = 8;
    tx_frame.data.u8[0] = 0x00;
    tx_frame.data.u8[1] = 0x01;
    tx_frame.data.u8[2] = 0x02;
    tx_frame.data.u8[3] = 0x03;
    tx_frame.data.u8[4] = 0x04;
    tx_frame.data.u8[5] = 0x05;
    tx_frame.data.u8[6] = 0x06;
    tx_frame.data.u8[7] = 0x07;
    ESP32Can.CANWriteFrame(&tx_frame);
  }
  */
}
#endif // junk

void mainloop()
{

  CAN_frame_t rx_frame;

  // Receive next CAN frame from queue
  if (xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 3 * portTICK_PERIOD_MS) == pdTRUE) {

    printf((rx_frame.FIR.B.FF == CAN_frame_std) ? "S" : "E");
    printf((rx_frame.FIR.B.RTR == CAN_RTR) ? "R" : " ");
    printf((rx_frame.FIR.B.FF == CAN_frame_std) ? " %03x" : " %04x",rx_frame.MsgID);
    for (int i = 0; i < rx_frame.FIR.B.DLC; i++) {
      printf(" %02X", rx_frame.data.u8[i]);
    }
    printf("\n");
  }
}


void loop()
{
  if (g_mode == MODE_SLCAN) {
    g_slCan.loop();
  }
  else {
    mainloop();
    //Serial.println("hello");    delay(200);
  }
}
