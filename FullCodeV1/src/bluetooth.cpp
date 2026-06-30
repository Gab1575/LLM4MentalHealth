#include "../include/bluetooth.h"
#include <BluetoothSerial.h>

static BluetoothSerial SerialBT;
static std::function<void(const String&)> recvCallback = nullptr;

bool btInit(const char* deviceName) {
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
    return false;
#else
    if (!SerialBT.begin(deviceName)) {
        return false;
    }
    return true;
#endif
}

void btSend(const String& s) {
    SerialBT.print(s);
}

bool btAvailable() {
    return SerialBT.available() > 0;
}

String btReadString() {
    String s = "";
    while (SerialBT.available()) {
        s += (char)SerialBT.read();
    }
    return s;
}

bool btConnected() {
    return SerialBT.connected();
}

void btOnReceive(std::function<void(const String&)> cb) {
    recvCallback = cb;
}

void btPoll() {
    while (btAvailable()) {
        String s = btReadString();
        if (recvCallback) recvCallback(s);
    }
}