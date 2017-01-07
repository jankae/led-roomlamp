#ifndef CONVERSIONS_H_
#define CONVERSIONS_H_

/** converts a nibble (values from 0x00 to 0x0F) to the corresponding hex character */
#define NIBBLE_TO_HEXCHAR(nibble)	((nibble)>9 ? (nibble) + 55 : (nibble) + 48)

#endif
