#include "ch32v003fun.h"
#include "uart.h"
#include "adxl345.h"

int main() {
    SystemInit();
        // Initialize UART with 115200 baud rate
    UART_Init(115200);
    UART_SendString("ADXL345 Test\r\n");

    // Initialize ADXL345
    if(!adxl345_init()) {
        UART_SendString("ADXL345 initialization failed!\r\n");
        while(1);
    }

    UART_SendString("ADXL345 initialized successfully\r\n");

    // Set range to ±4g
    adxl345_setRange(ADXL345_RANGE_4G);
   char buffer[32];

 while(1) {
        adxl345_data_t accel_data;

        if(adxl345_readAccel(&accel_data)) {
            UART_SendString("Acceleration:\r\n");
            int16_t x_int = (int16_t)(accel_data.x * 100);
            int16_t y_int = (int16_t)(accel_data.y * 100);
            int16_t z_int = (int16_t)(accel_data.z * 100);

            // Format and send X value
            sprintf(buffer, "X: %s%d.%02d g\r\n", (x_int < 0 ? "-" : ""),
                    abs(x_int) / 100, abs(x_int) % 100);
            UART_SendString(buffer);

            // Format and send Y value
            sprintf(buffer, "Y: %s%d.%02d g\r\n", (y_int < 0 ? "-" : ""),
                    abs(y_int) / 100, abs(y_int) % 100);
            UART_SendString(buffer);

            // Format and send Z value
            sprintf(buffer, "Z: %s%d.%02d g\r\n", (z_int < 0 ? "-" : ""),
                    abs(z_int) / 100, abs(z_int) % 100);
            UART_SendString(buffer);

            UART_SendString("\r\n");
        }

        Delay_Ms(1000);
    }
}
