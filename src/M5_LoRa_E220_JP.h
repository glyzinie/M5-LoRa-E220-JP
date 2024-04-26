#ifndef __M5_LORA_E220_JP_LIB_H__
#define __M5_LORA_E220_JP_LIB_H__

#include <Arduino.h>
#include <SPIFFS.h>
#include "M5_LoRa_E220_JP_DEF.h"

#define CONFIG_MODE_BAUD 9600

// E220-900T22S(JP)の設定項目
struct LoRaConfigItem_t {
    uint16_t own_address;
    uint8_t baud_rate;
    uint8_t air_data_rate;
    uint8_t subpacket_size;
    uint8_t rssi_ambient_noise_flag;
    uint8_t transmitting_power;
    uint8_t own_channel;
    uint8_t rssi_byte_flag;
    uint8_t transmission_method_type;
    uint8_t lbt_flag;
    uint16_t wor_cycle;
    uint16_t encryption_key;
    uint16_t target_address;
#ifndef __M5_LORA_E220_JP_LIB_H__
#define __M5_LORA_E220_JP_LIB_H__

#include <Arduino.h>
#include "M5_LoRa_E220_JP_DEF.h" // Definitions for specific LoRa constants

#define CONFIG_MODE_BAUD 9600 // Default baud rate for serial communication

// LoRa configuration structure for E220-900T22S module settings
struct LoRaConfigItem_t {
    uint16_t own_address;               // Device's own address
    uint8_t baud_rate;                  // Baud rate for serial communication
    uint8_t air_data_rate;              // Air data rate for LoRa transmission
    uint8_t subpacket_size;             // Maximum size of each packet
    uint8_t rssi_ambient_noise_flag;    // Flag to enable RSSI ambient noise measurement
    uint8_t transmitting_power;         // Transmit power level
    uint8_t own_channel;                // Channel number for LoRa communication
    uint8_t rssi_byte_flag;             // Flag to enable RSSI byte in messages
    uint8_t transmission_method_type;   // Transmission method (e.g., point-to-point)
    uint8_t lbt_flag;                   // Listen Before Talk flag
    uint16_t wor_cycle;                 // Wake on Radio cycle duration
    uint16_t encryption_key;            // Encryption key for secure communication
    uint16_t target_address;            // Target device's address for directed communication
    uint8_t target_channel;             // Target channel for communication
};

// Structure to hold received data from LoRa module
struct RecvFrame_t {
    uint8_t recv_data[201]; // Buffer to store the received data
    uint8_t recv_data_len;  // Length of the received data
    int rssi;               // Received Signal Strength Indicator
};

// Class for controlling the LoRa E220 module
class LoRa_E220_JP {
public:
    // Constructor initializing the serial port reference
    LoRa_E220_JP(HardwareSerial& serial = Serial2);

    // Initialize serial communication with given parameters
    void init(unsigned long baud = CONFIG_MODE_BAUD,
              uint32_t config = SERIAL_8N1, uint8_t RX = 16, uint8_t TX = 17);

    // Configure LoRa settings based on the provided configuration structure
    int initLoRaSetting(const LoRaConfigItem_t &config);

    // Receive data from the LoRa module and store it in the provided buffer
    int receiveFrame(RecvFrame_t *recv_frame);

    // Send data to the LoRa module using the specified configuration
    int sendFrame(const LoRaConfigItem_t &config, const uint8_t *send_data, size_t size);

    // Set default values for the LoRa configuration
    void setDefaultConfigValue(LoRaConfigItem_t &config);

private:
    HardwareSerial& _serial; // Reference to the serial port used for communication
};

#endif // __M5_LORA_E220_JP_LIB_H__