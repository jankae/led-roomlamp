#include "uart.h"

void uart_Init() {
	/* set baud rate */
#include <util/setbaud.h>
	UART_BAUDH = UBRRH_VALUE;
	UART_BAUDL = UBRRL_VALUE;
#if USE_2X
	UART_CTRL_A |= (1<<UART_2X);
#endif
	/* enable TX and RX, enable receive interrupt */
	UART_CTRL_B |= (1 << RX_ENABLE) | (1 << TX_ENABLE) | (1 << RXCI_ENABLE);
}

uint8_t uart_getByte(uint8_t *b) {
	if (uart.rxReadPtr != uart.rxWritePtr) {
		/* there is some data in the buffer */
		*b = uart.RXBuffer[uart.rxReadPtr];
		uart.rxReadPtr = (uart.rxReadPtr + 1) & UART_RX_SIZE_MASK;
		return 1;
	} else {
		/* no new data available */
		return 0;
	}
}

void uart_sendByte(uint8_t b) {
	uint8_t freespace;
	do {
		/* calculate space in RX buffer */
		freespace =
				(uart.txWritePtr - uart.txReadPtr - 1) % UART_TX_BUFFER_SIZE;
	} while (!freespace);
	/* add byte to buffer */
	uart.TXBuffer[uart.txWritePtr] = b;
	uart.txWritePtr = (uart.txWritePtr + 1) & UART_TX_SIZE_MASK;
	/* there is data in the buffer -> enable send interrupt */
	UART_CTRL_B |= (1 << UDRI_ENABLE);
}

void uart_sendString(const char *s){
	while(*s){
		uart_sendByte(*s++);
	}
}

void uart_sendString_P(const char *s) {
	uint8_t c = pgm_read_byte(s++);
	while (c) {
		uart_sendByte(c);
		c = pgm_read_byte(s++);
	}
}

ISR(RXC_ISR) {
	uint8_t data = UART_DATA;
	/* calculate space in RX buffer */
	uint8_t freespace = (uart.rxWritePtr - uart.rxReadPtr - 1)
			% UART_RX_BUFFER_SIZE;
	/* add received byte to buffer if space available */
	if (freespace) {
		uart.RXBuffer[uart.rxWritePtr] = data;
		uart.rxWritePtr = (uart.rxWritePtr + 1) & UART_RX_SIZE_MASK;
	}
}

ISR(UDRE_ISR) {
	/* UART is ready to receive next byte */
	UART_DATA = uart.TXBuffer[uart.txReadPtr];
	uart.txReadPtr = (uart.txReadPtr + 1) & UART_TX_SIZE_MASK;

	if (uart.txReadPtr == uart.txWritePtr) {
		/* this was the last byte, disable interrupt */
		UART_CTRL_B &= ~(1 << UDRI_ENABLE);
	}
}

void uart_sendValue(int16_t val, uint8_t base) {
	char buffer[7];
	itoa(val, buffer, base);
	uart_sendString(buffer);
}

void uart_sendUnsignedValue(uint16_t val, uint8_t base) {
	char buffer[7];
	utoa(val, buffer, base);
	uart_sendString(buffer);
}
