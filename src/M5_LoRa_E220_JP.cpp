#include "M5_LoRa_E220_JP.h"

SemaphoreHandle_t xMutex;

template <typename T>
bool ConfRange(T target, T min, T max) {
    return target >= min && target <= max;
}

void LoRa_E220_JP::Init(HardwareSerial *serial, unsigned long baud,
                        uint32_t config, uint8_t RX, uint8_t TX) {
    _serial = serial;
    _serial->begin(baud, config, RX, TX);
}

int LoRa_E220_JP::InitLoRaSetting(LoRaConfigItem_t &config) {
    if (!ConfRange<int>(config.own_channel, 0, 30)) {
        return 1;
    }

    xMutex = xSemaphoreCreateMutex();
    xSemaphoreGive(xMutex);

    std::vector<uint8_t> command = {0xc0, 0x00, 0x08, 
                                    static_cast<uint8_t>(config.own_address >> 8),
                                    static_cast<uint8_t>(config.own_address & 0xff),
                                    static_cast<uint8_t>((config.baud_rate << 5) | config.air_data_rate),
                                    static_cast<uint8_t>((config.subpacket_size << 6) | (config.rssi_ambient_noise_flag << 5) | config.transmitting_power),
                                    config.own_channel,
                                    static_cast<uint8_t>((config.rssi_byte_flag << 7) | (config.transmission_method_type << 6) | (config.lbt_flag << 4) | config.wor_cycle),
                                    static_cast<uint8_t>(config.encryption_key >> 8),
                                    static_cast<uint8_t>(config.encryption_key & 0xff)};

    std::vector<uint8_t> response;

    if (xSemaphoreTake(xMutex, (TickType_t)100) == pdTRUE) {
        for (uint8_t i : command) {
            _serial->write(i);
        }
        _serial->flush();
        xSemaphoreGive(xMutex);
    }

    delay(100);

    while (_serial->available()) {
        if (xSemaphoreTake(xMutex, (TickType_t)100) == pdTRUE) {
            response.push_back(_serial->read());
            xSemaphoreGive(xMutex);
        }
    }

    return response.size() == command.size() ? 0 : 1;
}

int LoRa_E220_JP::ReceiveFrame(RecvFrame_t *recv_frame) {
    memset(recv_frame->recv_data, 0, sizeof(recv_frame->recv_data));
    int len = 0;
    uint8_t *start_p = recv_frame->recv_data;

    while (1) {
        if (_serial->available() > 0 && xSemaphoreTake(xMutex, (TickType_t)100) == pdTRUE) {
            uint8_t ch = _serial->read();
            if (len < sizeof(recv_frame->recv_data)) {
                start_p[len++] = ch;
            }
            xSemaphoreGive(xMutex);
        }
        if (!_serial->available() && len > 0) {
            delay(10); // Short delay to ensure all data is received
            if (!_serial->available()) {
                recv_frame->recv_data_len = len - 1;
                recv_frame->rssi = recv_frame->recv_data[len - 1] - 256;
                break;
            }
        }
        delay(100);
    }

    return 0;
}

int LoRa_E220_JP::SendFrame(LoRaConfigItem_t &config, uint8_t *send_data, int size) {
    if (size > config.subpacket_size) {
        return 1;
    }

    uint8_t frame[3 + size];
    frame[0] = config.target_address >> 8;
    frame[1] = config.target_address & 0xff;
    frame[2] = config.target_channel;

    memcpy(frame + 3, send_data, size);

    if (xSemaphoreTake(xMutex, (TickType_t)100) == pdTRUE) {
        _serial->write(frame, sizeof(frame));
        _serial->flush();
        xSemaphoreGive(xMutex);
    }

    return 0;
}

void LoRa_E220_JP::SetDefaultConfigValue(LoRaConfigItem_t &config) {
    config = {0, BAUD_9600, AIR_DATA_RATE_SF9_BW125, SUBPACKET_200_BYTE, RSSI_AMBIENT_NOISE_ENABLE, TX_POWER_13dBm, 0,
              RSSI_BYTE_ENABLE, UART_P2P_MODE, LBT_DISABLE, WOR_CYCLE_2000_MS, 0, 0, 0};
}
