#ifndef VT100_H_
#define VT100_H_

/* Cursor movement */
#define VT100_CURSOR_HOME				"\x1b[H"
#define VT100_CURSOR_UP					"\x1b[A"
#define VT100_CURSOR_DOWN				"\x1b[B"
#define VT100_CURSOR_RIGHT				"\x1b[C"
#define VT100_CURSOR_LEFT				"\x1b[D"

/* Erasing */
#define VT100_ERASE_AFTER_CSR			"\x1b[K"
#define VT100_ERASE_BEFORE_CSR			"\x1b[1K"
#define VT100_ERASE_LINE				"\x1b[2K"
#define VT100_ERASE_DOWN				"\x1b[J"
#define VT100_ERASE_UP					"\x1b[1J"
#define VT100_ERASE_SCREEN				"\x1b[2J"

/* Attributes */
#define VT100_RESET_ATTRIBUTES			"\x1b[0m"
#define VT100_ATTRIBUTE_BRIGHT			"\x1b[1m"
#define VT100_ATTRIBUTE_DIM				"\x1b[2m"
#define VT100_ATTRIBUTE_UNDERSCORE		"\x1b[4m"
#define VT100_ATTRIBUTE_BLINK			"\x1b[5m"
#define VT100_ATTRIBUTE_REVERSE			"\x1b[7m"
#define VT100_ATTRIBUTE_HIDDEN			"\x1b[8m"

#define VT100_FOREGROUND_BLACK			"\x1b[30m"
#define VT100_FOREGROUND_RED			"\x1b[31m"
#define VT100_FOREGROUND_GREEN			"\x1b[32m"
#define VT100_FOREGROUND_YELLOW			"\x1b[33m"
#define VT100_FOREGROUND_BLUE			"\x1b[34m"
#define VT100_FOREGROUND_MAGENTA		"\x1b[35m"
#define VT100_FOREGROUND_CYAN			"\x1b[36m"
#define VT100_FOREGROUND_WHITE			"\x1b[37m"

#define VT100_BACKGROUND_BLACK			"\x1b[40m"
#define VT100_BACKGROUND_RED			"\x1b[41m"
#define VT100_BACKGROUND_GREEN			"\x1b[42m"
#define VT100_BACKGROUND_YELLOW			"\x1b[43m"
#define VT100_BACKGROUND_BLUE			"\x1b[44m"
#define VT100_BACKGROUND_MAGENTA		"\x1b[45m"
#define VT100_BACKGROUND_CYAN			"\x1b[46m"
#define VT100_BACKGROUND_WHITE			"\x1b[47m"

#endif
