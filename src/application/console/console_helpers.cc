#include "console_helpers.h"

#include "sharc_task.h"
#include "console_task.h"

void console_printf(const char *format, ...)
{
	if(TTaskUtilities::GetCurrentTaskHandle() != ConsoleTask->GetHandle())
		ConsoleTask->PrintF(&format);
	else
		ConsoleTask->UnsafePrintF(&format);
}

void default_param_handler(uint8_t *param_ptr, TTranslator::const_symbol_type_ptr_t *args, const size_t count)
{
	if (count > 0)
	{
		if (count == 2)
		{
			char *end;
			uint8_t val = kgp_sdk_libc::strtol(args[1], &end, 16);
			*param_ptr = val;
		}
		console_printf("%s\r%02x\n", args[0], *param_ptr);
	}
}

void default_param_setter(const TParamDescriptor& paramDescriptor, uint8_t value)
{
	uint8_t* param_ptr = (uint8_t*)paramDescriptor.ptr;
	*param_ptr = value;
	SharcTask->setParameter(paramDescriptor.dspAddress, paramDescriptor.dspPosition, *param_ptr);
	console_printf("%s\r%02x\n", paramDescriptor.handlerStr, *param_ptr);
}
