#ifndef BLUETOOTHIO_H
#define BLUETOOTHIO_H

#include "_AbstractIO.hpp"
#include "misc/helpers.hpp"

class BluetoothIO : public AbstractIO {
    public:
        BluetoothIO();
        // void print(const char *message);
        // void println(const char *message);
        size_t write(uint8_t c) override;
    protected:
        void handle() override;
        void printHeader() override;
    private:
};

inline BluetoothIO::BluetoothIO() = default;
inline size_t BluetoothIO::write(uint8_t c) { return 0; }
inline void BluetoothIO::handle() {}
inline void BluetoothIO::printHeader() {}

#endif //BLUETOOTHIO_H