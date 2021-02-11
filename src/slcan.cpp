#include <ESP32CAN.h>           // v1.0.0     from https://github.com/nhatuan84/arduino-esp32-can-demo
#include <CAN_config.h>         // as above
#include <BluetoothSerial.h>    // v1.0
#include "slcan.h"

// CURRENTLY ESP32 Dev Module Board Definition
// PIN 4  CANTX 
// PIN 5  CANRX 
// PIN 12 _BLUETOOTH SWITCH
// PIN 14 NOT IN USE
// PIN 15 10k to ground to remove boot messages
// 3.3v 
// GND to SWITCH CENTER

BluetoothSerial SerialBT;


const int SWITCH_PIN_A    = 12;
//const int SWITCH_PIN_B  = 14;

static uint8_t hexval[17] = "0123456789ABCDEF";


SLCAN::SLCAN(CAN_device_t *can_cfg) :
  _working(false),
  _bluetooth(false),
  _timestamp(false),
  _cr(false),
  _canSpeed(500),
  _canCfg(can_cfg)
  //  _serSpeed(500000)
{
}

//----------------------------------------------------------------

void SLCAN::ack()
{
  if (_bluetooth) SerialBT.write('\r');
  else Serial.write('\r');
} // ack()

//----------------------------------------------------------------

void SLCAN::nack()
{
  if (_bluetooth) SerialBT.write('\a');
  else Serial.write('\a');
} // nack()

//----------------------------------------------------------------

void SLCAN::parseCmd(char *buf)
{                           // LAWICEL PROTOCOL
  switch (buf[0]) {
    case 'O':               // OPEN CAN
      _working=true;
      ESP32Can.CANInit();
      ack();
      break;
    case 'C':               // CLOSE CAN
      _working=false;
      ESP32Can.CANStop();
      ack();
      break;
    case 't':               // SEND STD FRAME
      sendCanMsg(buf,false,false);
      ack();
      break;
    case 'T':               // SEND EXT FRAME
      sendCanMsg(buf,false,true);
      ack();
      break;
    case 'r':               // SEND STD RTR FRAME
      sendCanMsg(buf,true,false);
      ack();
      break;
    case 'R':               // SEND EXT RTR FRAME
      sendCanMsg(buf,true,true);
      ack();
      break;
    case 'Z':               // ENABLE TIMESTAMPS
      switch (buf[1]) {
        case '0':           // TIMESTAMP OFF  
          _timestamp = false;
          ack();
          break;
        case '1':           // TIMESTAMP ON
          _timestamp = true;
          ack();
          break;
        default:
          break;
      }
      break;
    case 'M':               ///set ACCEPTANCE CODE ACn REG
      ack();
      break;
    case 'm':               // set ACCEPTANCE CODE AMn REG
      ack();
      break;
    case 's':               // CUSTOM CAN bit-rate
      nack();
      break;
    case 'S':               // CAN bit-rate
      switch (buf[1]) {
        case '0':           // 10k  
          nack();
          break;
        case '1':           // 20k
          nack();
          break;
        case '2':           // 50k
          nack();
          break;
        case '3':           // 100k
          _canCfg->speed=CAN_SPEED_100KBPS;
          _canSpeed = 100;
          ack();
          break;
        case '4':           // 125k
          _canCfg->speed=CAN_SPEED_125KBPS;
          _canSpeed = 125;
          ack();
          break;
        case '5':           // 250k
          _canCfg->speed=CAN_SPEED_250KBPS;
          _canSpeed = 250;
         ack();
          break;
        case '6':           // 500k
          _canCfg->speed=CAN_SPEED_500KBPS;
          _canSpeed = 500;
          ack();
          break;
        case '7': // 800k
          _canCfg->speed=CAN_SPEED_800KBPS;
          _canSpeed = 800;
          ack();
          break;
        case '8':           // 1000k
          _canCfg->speed=CAN_SPEED_1000KBPS;
          _canSpeed = 1000;
          ack();
          break;
        default:
          nack();
          break;
      }
      break;
    case 'F':               // STATUS FLAGS
      if (_bluetooth) SerialBT.print("F00");
      else Serial.print("F00");
      ack();
      break;
    case 'V':               // VERSION NUMBER
      if (_bluetooth) SerialBT.print("V1234");
      else Serial.print("V1234");
      ack();
      break;
    case 'N':               // SERIAL NUMBER
      if (_bluetooth) SerialBT.print("N2208");
      else Serial.print("N2208");
      ack();
      break;
    case 'l':               // (NOT SPEC) TOGGLE LINE FEED ON SERIAL
      _cr = !_cr;
      nack();
      break;
    case 'h':               // (NOT SPEC) HELP SERIAL
      Serial.println();
      Serial.println("mintynet.com - slcan esp32");
      Serial.println();
      Serial.println("O\t=\tStart slcan");
      Serial.println("C\t=\tStop slcan");
      Serial.println("t\t=\tSend std frame");
      Serial.println("r\t=\tSend std rtr frame");
      Serial.println("T\t=\tSend ext frame");
      Serial.println("R\t=\tSend ext rtr frame");
      Serial.println("Z0\t=\tTimestamp Off");
      Serial.println("Z1\t=\tTimestamp On");
      Serial.println("snn\t=\tSpeed 0xnnk N/A");
      Serial.println("S0\t=\tSpeed 10k N/A");
      Serial.println("S1\t=\tSpeed 20k N/A");
      Serial.println("S2\t=\tSpeed 50k N/A");
      Serial.println("S3\t=\tSpeed 100k");
      Serial.println("S4\t=\tSpeed 125k");
      Serial.println("S5\t=\tSpeed 250k");
      Serial.println("S6\t=\tSpeed 500k");
      Serial.println("S7\t=\tSpeed 800k");
      Serial.println("S8\t=\tSpeed 1000k");
      Serial.println("F\t=\tFlags N/A");
      Serial.println("N\t=\tSerial No");
      Serial.println("V\t=\tVersion");
      Serial.println("-----NOT SPEC-----");
      Serial.println("h\t=\tHelp");
      Serial.print("l\t=\tToggle CR ");
      if (_cr) {
        Serial.println("ON");
      } else {
        Serial.println("OFF");
      }
      Serial.print("CAN_SPEED:\t");
      switch(_canSpeed) {
        case 100:
          Serial.print("100");
          break;
        case 125:
          Serial.print("125");
          break;
        case 250:
          Serial.print("250");
          break;
        case 500:
          Serial.print("500");
          break;
        case 800:
          Serial.print("800");
          break;
        case 1000:
          Serial.print("1000");
          break;
        default:
          break;
      }
      Serial.print("kbps");
      if (_timestamp) {
        Serial.print("\tT");
      }
      if (_working) {
        Serial.print("\tON");
      } else {
        Serial.print("\tOFF");
      }
      Serial.println();
      nack();
      break;
    default:
      nack();
      break;
  }
} // parseCmd()

//----------------------------------------------------------------

void SLCAN::transferTty2Can()
{
  int ser_length;
  static char cmdbuf[32];
  static int cmdidx = 0;
  if (_bluetooth) {
    if ((ser_length = SerialBT.available()) > 0) {
      for (int i = 0; i < ser_length; i++) {
        char val = SerialBT.read();
        cmdbuf[cmdidx++] = val;
        if (cmdidx == 32)
        {
          nack();
          cmdidx = 0;
        } else if (val == '\r')
        {
          cmdbuf[cmdidx] = '\0';
          parseCmd(cmdbuf);
          cmdidx = 0;
        }
      }
    }
  } else {
    if ((ser_length = Serial.available()) > 0) {
      for (int i = 0; i < ser_length; i++) {
        char val = Serial.read();
        cmdbuf[cmdidx++] = val;
        if (cmdidx == 32)
        {
          nack();
          cmdidx = 0;
        } else if (val == '\r')
        {
          cmdbuf[cmdidx] = '\0';
          parseCmd(cmdbuf);
          cmdidx = 0;
        }
      }
    }
  }
} // transferTty2Can()

//----------------------------------------------------------------

void SLCAN::transferCan2Tty()
{
  CAN_frame_t rx_frame;
  String command = "";
  long time_now = 0;
  //receive next CAN frame from queue
  if(xQueueReceive(_canCfg->rx_queue,&rx_frame, 3*portTICK_PERIOD_MS)==pdTRUE) {
    //do stuff!
    if(_working) {
      if(rx_frame.FIR.B.FF==CAN_frame_ext) {
        if (rx_frame.FIR.B.RTR==CAN_RTR) {
          command = command + "R";
        } else {
          command = command + "T";
        }
        command = command + char(hexval[ (rx_frame.MsgID>>28)&15]);
        command = command + char(hexval[ (rx_frame.MsgID>>24)&15]);
        command = command + char(hexval[ (rx_frame.MsgID>>20)&15]);
        command = command + char(hexval[ (rx_frame.MsgID>>16)&15]);
        command = command + char(hexval[ (rx_frame.MsgID>>12)&15]);
        command = command + char(hexval[ (rx_frame.MsgID>>8)&15]);
        command = command + char(hexval[ (rx_frame.MsgID>>4)&15]);
        command = command + char(hexval[ rx_frame.MsgID&15]);
        command = command + char(hexval[ rx_frame.FIR.B.DLC ]);
      } else {
        if (rx_frame.FIR.B.RTR==CAN_RTR) {
          command = command + "r";
        } else {
          command = command + "t";
        }
        command = command + char(hexval[ (rx_frame.MsgID>>8)&15]);
        command = command + char(hexval[ (rx_frame.MsgID>>4)&15]);
        command = command + char(hexval[ rx_frame.MsgID&15]);
        command = command + char(hexval[ rx_frame.FIR.B.DLC ]);
      }
      for(int i = 0; i < rx_frame.FIR.B.DLC; i++){
        command = command + char(hexval[ rx_frame.data.u8[i]>>4 ]);
        command = command + char(hexval[ rx_frame.data.u8[i]&15 ]);
        //printf("%c\t", (char)rx_frame.data.u8[i]);
      }
    if (_timestamp) {
      time_now = millis() % 60000;
      command = command + char(hexval[ (time_now>>12)&15 ]);
      command = command + char(hexval[ (time_now>>8)&15 ]);
      command = command + char(hexval[ (time_now>>4)&15 ]);
      command = command + char(hexval[ time_now&15 ]);
    }
    command = command + '\r';
    if (_bluetooth) SerialBT.print(command);
    else Serial.print(command);
    if (_cr) Serial.println("");
    }
  }
} // transferCan2tty()

//----------------------------------------------------------------

void SLCAN::sendCanMsg(char *buf, boolean rtr, boolean ext) {
  if (!_working) {

  } else {
    CAN_frame_t tx_frame;
    int msg_id = 0;
    int msg_ide = 0;
    if (rtr) {
      if (ext) {
        sscanf(&buf[1], "%04x%04x", &msg_ide, &msg_id);
        tx_frame.FIR.B.RTR = CAN_RTR;
        tx_frame.FIR.B.FF = CAN_frame_ext;
      } else {
        sscanf(&buf[1], "%03x", &msg_id);
        tx_frame.FIR.B.RTR = CAN_RTR;
        tx_frame.FIR.B.FF = CAN_frame_std;
      }
    } else {
      if (ext) {
        sscanf(&buf[1], "%04x%04x", &msg_ide, &msg_id);
        tx_frame.FIR.B.RTR = CAN_no_RTR;
        tx_frame.FIR.B.FF = CAN_frame_ext;
      } else {
        sscanf(&buf[1], "%03x", &msg_id);
        tx_frame.FIR.B.RTR = CAN_no_RTR;
        tx_frame.FIR.B.FF = CAN_frame_std;
      }
    }
    tx_frame.MsgID = msg_ide*65536 + msg_id;
    int msg_len = 0;
    if (ext) {
      sscanf(&buf[9], "%01x", &msg_len);
    } else {
      sscanf(&buf[4], "%01x", &msg_len);
    }
    tx_frame.FIR.B.DLC = msg_len;
    int candata = 0;
    if (ext) {
      for (int i = 0; i < msg_len; i++) {
        sscanf(&buf[10 + (i*2)], "%02x", &candata);
        tx_frame.data.u8[i] = candata;
      }
    } else {
      for (int i = 0; i < msg_len; i++) {
        sscanf(&buf[5 + (i*2)], "%02x", &candata);
        tx_frame.data.u8[i] = candata;
      }
    }
    ESP32Can.CANWriteFrame(&tx_frame);
  }
} // sendCanMsg()

//----------------------------------------------------------------

void SLCAN::begin()
{
  //Wire.begin(21,22);
  pinMode(SWITCH_PIN_A,INPUT_PULLUP);
  //pinMode(SWITCH_PIN_B,INPUT_PULLUP);
  //  Serial.begin(_serSpeed);
  //  delay(100);
  //Serial.println("CAN demo");
  //  _canCfg->speed=CAN_SPEED_500KBPS;
  //  _canCfg->tx_pin_id = GPIO_NUM_4;
  //  _canCfg->rx_pin_id = GPIO_NUM_5;
  //  _canCfg->rx_queue = xQueueCreate(10,sizeof(CAN_frame_t));
  /*
  delay(2000);
  boolean switchA = digitalRead(SWITCH_PIN_A);
  if (!switchA) {
    SerialBT.begin("SLCAN");
    _bluetooth = true;
    Serial.println("BT Switch ON");
  } else {
    _bluetooth = false;
    Serial.println("BT Switch OFF");
  }
  if (_bluetooth) Serial.println("BLUETOOTH ON");
  */
} // setup()

//----------------------------------------------------------------

void SLCAN::loop()
{
  transferCan2Tty();
  transferTty2Can();
} // loop();
