#ifndef _SLCAN_H_
#define _SLCAN_H_

class SLCAN {
  boolean _working;
  boolean _bluetooth;
  boolean _timestamp;
  boolean _cr;
  int _canSpeed;
  //  int _serSpeed;
  CAN_device_t *_canCfg;
  
  void ack();
  void nack();
  void parseCmd();
  void transferTty2Can();
  void transferCan2Tty();
  void sendCanMsg(char *buf, boolean rtr, boolean ext);
  void parseCmd(char *buf);
public:
  SLCAN(CAN_device_t *can_cfg);
  void begin();
  void loop();
};

#endif // _SLCAN_H_
