#include <Arduino.h>
#include "hardware/pio.h"
#include "hardware/uart.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

#define UART0_TX_PIN 0
#define UART0_RX_PIN 1
#define UART1_TX_PIN 4
#define UART1_RX_PIN 5

// ------- //
// uart_rx //
// ------- //

#define uart_rx_wrap_target 0
#define uart_rx_wrap 2

static const uint16_t uart_rx_program_instructions[] = {
            //     .wrap_target
    0x2021, //  0: wait   0 pin, 1                   
    0x4008, //  1: in     pins, 8                    
    0x8020, //  2: push   block                      
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program uart_rx_program = {
    .instructions = uart_rx_program_instructions,
    .length = 3,
    .origin = -1,
};

static inline pio_sm_config uart_rx_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + uart_rx_wrap_target, offset + uart_rx_wrap);
    return c;
}
#endif

// ------- //
// uart_tx //
// ------- //

#define uart_tx_wrap_target 0
#define uart_tx_wrap 3

static const uint16_t uart_tx_program_instructions[] = {
            //     .wrap_target
    0x80a0, //  0: pull   block                      
    0xe001, //  1: set    pins, 1                    
    0x6008, //  2: out    pins, 8                    
    0xe000, //  3: set    pins, 0                    
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program uart_tx_program = {
    .instructions = uart_tx_program_instructions,
    .length = 4,
    .origin = -1,
};

static inline pio_sm_config uart_tx_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + uart_tx_wrap_target, offset + uart_tx_wrap);
    return c;
}
#endif

void setup()
{
    // Initialize USB serial
    Serial.begin(115200);

    

    // Initialize PIO
    PIO pio = pio0;

    // Initialize SM0 for RX
    uint rx_offset = pio_add_program(pio, &uart_rx_program);
    uint sm0 = pio_claim_unused_sm(pio, true);
    pio_sm_config c0 = uart_rx_program_get_default_config(rx_offset);
    sm_config_set_in_pins(&c0, UART0_RX_PIN);
    sm_config_set_clkdiv(&c0, 1085.069f); // Adjust as necessary
    pio_sm_set_consecutive_pindirs(pio, sm0, UART0_RX_PIN, 1, false);
    pio_sm_init(pio, sm0, rx_offset, &c0);
    pio_sm_set_enabled(pio, sm0, true);

    // Initialize SM1 for TX
    uint tx_offset = pio_add_program(pio, &uart_tx_program);
    uint sm1 = pio_claim_unused_sm(pio, true);
    pio_sm_config c1 = uart_tx_program_get_default_config(tx_offset);
    sm_config_set_out_pins(&c1, UART1_TX_PIN, 1);
    sm_config_set_clkdiv(&c1, 1085.069f); // Adjust as necessary
    pio_gpio_init(pio, UART1_TX_PIN);
    pio_sm_set_consecutive_pindirs(pio, sm1, UART1_TX_PIN, 1, true);
    pio_sm_init(pio, sm1, tx_offset, &c1);
    pio_sm_set_enabled(pio, sm1, true);

/*
    // Initialize UART0
    uart_init(uart0, 115200);
    gpio_set_function(UART0_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART0_RX_PIN, GPIO_FUNC_UART);

    // Initialize UART1
    uart_init(uart1, 115200);
    gpio_set_function(UART1_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART1_RX_PIN, GPIO_FUNC_UART);
*/
    delay(2000);

    Serial1.println("Serial1: UICC OK!");
    Serial2.println("Serial2: Host OK!");

    Serial.println("Serial: PICO OK!");
}

void loop()
{
   /*
    if (uart_is_readable(uart0))
    {
        char c = uart_getc(uart0);
        Serial.print("UART0: ");
        Serial.println(c);
    }

    if (uart_is_readable(uart1))
    {
        char c = uart_getc(uart1);
        Serial.print("UART1: ");
        Serial.println(c);
    }
*/
    delay(10);
}