#ifndef SRC_APPLICATION_CONSOLE_CONSOLE_HELPERS_H_
#define SRC_APPLICATION_CONSOLE_CONSOLE_HELPERS_H_

#include "appdefs.h"

#include "gui_task.h"

#include "preset.h"

static const char hexes[] = "0123456789abcdef";

inline void i2hex(uint8_t val, char* dest)
{
	dest[0] = hexes[ val >> 4  ];
	dest[1] = hexes[ val & 0xf ];
	dest[2] = 0;
}

inline void default_param_handler(uint8_t *param_ptr, TReadLine *rl, TReadLine::const_symbol_type_ptr_t *args, const size_t count)
{
	char hex[3] = {0, 0, 0};
	if (count > 0)
	{
		if (count == 2)
		{
			char *end;
			uint8_t val = kgp_sdk_libc::strtol(args[1], &end, 16);
			*param_ptr = val;
		}

		i2hex(*param_ptr, hex);
		msg_console("%s\n", hex);
	}
}

#endif /* SRC_APPLICATION_CONSOLE_CONSOLE_HELPERS_H_ */
