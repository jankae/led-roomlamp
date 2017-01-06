#ifndef _USI_H_
#define _USI_H_

#include <avr/io.h>
#include <avr/interrupt.h>

#define DDR_USI             DDRA
#define PORT_USI            PORTA
#define PIN_USI             PINA
#define PORT_USI_SDA        PA6
#define PORT_USI_SCL        PA4
#define USI_START_COND_INT 	USISIF

#define SET_USI_TO_SEND_ACK()                                                                                          \
	{                                                                                                                  \
		USIDR = 0;                      /* Prepare ACK                         */                                      \
		DDR_USI |= (1 << PORT_USI_SDA); /* Set SDA as output                   */                                      \
		USISR = (0 << USI_START_COND_INT) | (1 << USIOIF) | (1 << USIPF) | (1 << USIDC)                                \
		        |                  /* Clear all flags, except Start Cond  */                                           \
		        (0x0E << USICNT0); /* set USI counter to shift 1 bit. */                                               \
	}

#define SET_USI_TO_READ_ACK()                                                                                          \
	{                                                                                                                  \
		DDR_USI &= ~(1 << PORT_USI_SDA); /* Set SDA as intput */                                                       \
		USIDR = 0;                       /* Prepare ACK        */                                                      \
		USISR = (0 << USI_START_COND_INT) | (1 << USIOIF) | (1 << USIPF) | (1 << USIDC)                                \
		        |                  /* Clear all flags, except Start Cond  */                                           \
		        (0x0E << USICNT0); /* set USI counter to shift 1 bit. */                                               \
	}

#define SET_USI_TO_TWI_START_CONDITION_MODE()                                                                          \
	{                                                                                                                  \
		USICR = (1 << USISIE) | (0 << USIOIE) | /* Enable Start Condition Interrupt. Disable Overflow Interrupt.*/     \
		        (1 << USIWM1) | (0 << USIWM0) | /* Set USI in Two-wire mode. No USI Counter overflow hold.      */     \
		        (1 << USICS1) | (0 << USICS0) | (0 << USICLK)                                                          \
		        | /* Shift Register Clock Source = External, positive edge        */                                   \
		        (0 << USITC);                                                                                          \
		USISR = (0 << USI_START_COND_INT) | (1 << USIOIF) | (1 << USIPF) | (1 << USIDC)                                \
		        | /* Clear all flags, except Start Cond                            */                                  \
		        (0x0 << USICNT0);                                                                                      \
	}

#define SET_USI_TO_SEND_DATA()                                                                                         \
	{                                                                                                                  \
		DDR_USI |= (1 << PORT_USI_SDA); /* Set SDA as output                  */                                       \
		USISR = (0 << USI_START_COND_INT) | (1 << USIOIF) | (1 << USIPF) | (1 << USIDC)                                \
		        |                 /* Clear all flags, except Start Cond */                                             \
		        (0x0 << USICNT0); /* set USI to shift out 8 bits        */                                             \
	}

#define SET_USI_TO_READ_DATA()                                                                                         \
	{                                                                                                                  \
		DDR_USI &= ~(1 << PORT_USI_SDA); /* Set SDA as input                   */                                      \
		USISR = (0 << USI_START_COND_INT) | (1 << USIOIF) | (1 << USIPF) | (1 << USIDC)                                \
		        |                 /* Clear all flags, except Start Cond */                                             \
		        (0x0 << USICNT0); /* set USI to shift out 8 bits        */                                             \
	}

/** Size if I2C buffer. Has to be a power of 2 (e.g. 2,4,8,16...) */
#define USI_DATA_SIZE 			16
#define USI_INDEX_MASK			(USI_DATA_SIZE - 1)

#define USI_REG_W_CTRL			0
#define USI_REG_R_VERSION		1
#define USI_REG_W_CURRENT_LOW	2
#define USI_REG_W_CURRENT_HIGH	3
#define USI_REG_W_VOLTAGE_LOW	4
#define USI_REG_W_VOLTAGE_HIGH	5
#define USI_REG_W_TEMP_LIMIT	6
#define USI_REG_R_TEMP			7
#define USI_REG_R_CURRENT_LOW	8
#define USI_REG_R_CURRENT_HIGH	9
#define USI_REG_R_VOLTAGE_LOW	10
#define USI_REG_R_VOLTAGE_HIGH	11
#define USI_REG_R_DUTY_IDX_LOW	12
#define USI_REG_R_DUTY_IDX_HIGH	13
#define USI_REG_R_COMPARE_VALUE	14
#define USI_REG_R_TOP_VALUE		15

#define USI_CTRL_UPDATE			0x01

typedef enum {
	USI_SLAVE_CHECK_ADDRESS,
	USI_SLAVE_SEND_DATA,
	USI_SLAVE_REQUEST_REPLY_FROM_SEND_DATA,
	USI_SLAVE_CHECK_REPLY_FROM_SEND_DATA,
	USI_SLAVE_REQUEST_DATA,
	USI_SLAVE_GET_DATA_AND_SEND_ACK,
	USI_SLAVE_IDLE
} usi_I2CState_t;

struct {
	uint8_t address;
	usi_I2CState_t state;
	uint8_t data[USI_DATA_SIZE];
	uint8_t index;
} usi;

void usi_InitI2C(uint8_t ownAddress);

void usi_CheckForStop(void);

#endif
