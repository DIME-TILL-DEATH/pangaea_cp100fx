#include "console_helpers.h"

#include "preset.h"

#include "sharc_task.h"

static const char hexes[] = "0123456789abcdef";

void console_printf(const char *format, ...)
{
	if(TTaskUtilities::GetCurrentTaskHandle() != ConsoleTask->GetHandle())
		ConsoleTask->PrintF(&format);
	else
		ConsoleTask->UnsafePrintF(&format);
}

void i2hex(uint8_t val, char* dest)
{
	dest[0] = hexes[ val >> 4  ];
	dest[1] = hexes[ val & 0xf ];
	dest[2] = 0;
}

void default_param_handler(uint8_t *param_ptr, TTranslator *rl, TTranslator::const_symbol_type_ptr_t *args, const size_t count)
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
		msg_console("%s\r%s\n", args[0], hex);
	}
}

void default_param_setter(const TParamDescriptor& paramDescriptor, uint8_t value)
{
	char hex[3] = {0, 0, 0};
	*paramDescriptor.ptr = value;
	SharcTask->setParameter(paramDescriptor.dspAddress, paramDescriptor.dspPosition, *paramDescriptor.ptr);
	i2hex(*paramDescriptor.ptr, hex);
	console_printf("%s\r%s\n", paramDescriptor.handlerStr, hex);
}
