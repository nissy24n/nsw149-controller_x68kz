// NSW-149 Controller Joystick for X68000Z
#include <Joystick.h>
#include <XBOXONE.h>

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

class NSW149 : public XBOXONE {
public:
  NSW149(USB *pUsb):XBOXONE(pUsb) {};
  virtual uint8_t Poll();
  virtual bool VIDPIDOK(uint16_t vid, uint16_t pid) {
    return (vid == 0x0F0D && pid == 0x00F1);
  };
  uint16_t getStick(ButtonEnum b);
  uint16_t getFunction(ButtonEnum b);
  uint16_t getButton(ButtonEnum b);

private:
  uint8_t readBuf[XBOX_ONE_EP_MAXPKTSIZE];
};

uint8_t NSW149::Poll() {
  uint8_t rcode = 0;

  if(!isReady())
    return 0;

  if((int32_t)((uint32_t)millis() - qNextPollTime) >= 0L) {
    qNextPollTime = (uint32_t)millis() + readPollInterval();
    uint16_t length =  (uint16_t)epInfo[ XBOX_ONE_INPUT_PIPE ].maxPktSize;
    uint8_t rcode = pUsb->inTransfer(bAddress, epInfo[ XBOX_ONE_INPUT_PIPE ].epAddr, &length, readBuf, readPollInterval());
    if(!rcode) {
    }
  }
  return rcode;
}

uint16_t NSW149::getStick(ButtonEnum b) {
  if (b == UP) {
    if (readBuf[2] == 0 || readBuf[2] == 1 || readBuf[2] == 7) return true;
  } else
  if (b == RIGHT) {
    if (readBuf[2] == 1 || readBuf[2] == 2 || readBuf[2] == 3) return true;
  } else
  if (b == DOWN) {
    if (readBuf[2] == 3 || readBuf[2] == 4 || readBuf[2] == 5) return true;
  } else
  if (b == LEFT) {
    if (readBuf[2] == 5 || readBuf[2] == 6 || readBuf[2] == 7) return true;
  }
  return false;
}

uint16_t NSW149::getFunction(ButtonEnum b) {
  if (b == CAPTURE) {
    if ((readBuf[1] & 0b00100000) == 0b00100000) return true;
  } else
  if (b == HOME) {
    if ((readBuf[1] & 0b00010000) == 0b00010000) return true;
  } else
  if (b == MINUS) {
    if ((readBuf[1] & 0b00000001) == 0b00000001) return true;
  } else
  if (b == PLUS) {
    if ((readBuf[1] & 0b00000010) == 0b00000010) return true;
  }
  return false;
}

uint16_t NSW149::getButton(ButtonEnum b) {
  if (b == Y) {
    if ((readBuf[0] & 0b00000001) == 0b00000001) return true;
  } else
  if (b == X) {
    if ((readBuf[0] & 0b00001000) == 0b00001000) return true;
  } else
  if (b == R) {
    if ((readBuf[0] & 0b00100000) == 0b00100000) return true;
  } else
  if (b == L) {
    if ((readBuf[0] & 0b00010000) == 0b00010000) return true;
  }
  if (b == B) {
    if ((readBuf[0] & 0b00000010) == 0b00000010) return true;
  } else
  if (b == A) {
    if ((readBuf[0] & 0b00000100) == 0b00000100) return true;
  } else
  if (b == ZR) {
    if ((readBuf[0] & 0b10000000) == 0b10000000) return true;
  } else
  if (b == ZL) {
    if ((readBuf[0] & 0b01000000) == 0b01000000) return true;
  }
  return false;
}

USB Usb;
NSW149 fs(&Usb);

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_GAMEPAD,
  8, 1,                  // Button Count, Hat Switch Count
  false, false, false,   // X and Y, but no Z Axis
  false, false, false,   // No Rx, Ry, or Rz
  false, false,          // No rudder or throttle
  false, false, false);  // No accelerator, brake, or steering

void setup() {
  if (Usb.Init() == -1) {
    while (1); //halt
  }
  Joystick.begin();
}
void loop() {
  Usb.Task();
  if (fs.XboxOneConnected) {
    bool up    = false;
    bool down  = false;
    bool left  = false;
    bool right = false;
    int16_t hat = JOYSTICK_HATSWITCH_RELEASE;
    if (fs.getStick(UP))    up    = true;
    if (fs.getStick(DOWN))  down  = true;
    if (fs.getStick(LEFT))  left  = true;
    if (fs.getStick(RIGHT)) right = true;
    if ( up && !down && !left && !right) {
      hat =   0;
    } else
    if ( up && !down && !left &&  right) {
      hat =  45;
    } else
    if (!up && !down && !left &&  right) {
      hat =  90;
    } else
    if (!up &&  down && !left &&  right) {
      hat = 135;
    } else
    if (!up &&  down && !left && !right) {
      hat = 180;
    } else
    if (!up &&  down &&  left && !right) {
      hat = 225;
    } else
    if (!up && !down &&  left && !right) {
      hat = 270;
    } else
    if ( up && !down &&  left && !right) {
      hat = 315;
    }
    Joystick.setHatSwitch(0, hat);

    if (fs.getButton(B)) {
      Joystick.setButton(1, true);
    } else {
      Joystick.setButton(1, false);
    }
    if (fs.getButton(A)) {
      Joystick.setButton(2, true);
    } else {
      Joystick.setButton(2, false);
    }
  }

  delay(1);
}
