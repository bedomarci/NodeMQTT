#ifndef BLUETOOTHIO_H
#define BLUETOOTHIO_H

#include "_AbstractIO.hpp"
#include "misc/helpers.hpp"

class BluetoothIO : public AbstractIO {
    public:
        BluetoothIO();
        // void print(const char *message);
        // void println(const char *message);
        size_t write(uint8_t c);
    protected:
        void handle();
        void printHeader();
    private:
};

#endif //BLUETOOTHIO_H