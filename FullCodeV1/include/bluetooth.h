#pragma once

#include <Arduino.h>
#include <functional>

// Initialize BluetoothSerial with an optional device name. Returns true on success.
bool btInit(const char* deviceName = "FlowerPower");

// Send text over Bluetooth
void btSend(const String& s);

// Data utilities
bool btAvailable();
String btReadString();
bool btConnected();

// Receive callback. User can register a handler to be called with incoming data.
void btOnReceive(std::function<void(const String&)> cb);

// Must be called regularly (e.g. in loop()) to dispatch incoming data to the callback.
void btPoll();

// --- NEW LOGGING FUNCTION ---
// Template allows 'T' to be any variable type (int, float, long, double, etc.)
template <typename T>
void btLog(const String& label, T value, const String& unit = "") {
    // Combine the label, the variable, and the unit into one string
    String message = label + String(value) + " " + unit + "\n";
    
    // Send it using your existing wrapper
    btSend(message);
}