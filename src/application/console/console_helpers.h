#ifndef _CONSOLE_CONSOLE_HELPERS_H_
#define _CONSOLE_CONSOLE_HELPERS_H_

#include "appdefs.h"

#include "modules.h"
#include "preset.h"
#include "sharc.h"

#include "console_task.h"

void console_printf(const char *format, ...);
void i2hex(uint8_t val, char* dest);
void default_param_handler(uint8_t *param_ptr, TTranslator *rl, TTranslator::const_symbol_type_ptr_t *args, const size_t count);
void default_param_setter(const TParamDescriptor& paramDescriptor, uint8_t value);

#endif /* SRC_APPLICATION_CONSOLE_CONSOLE_HELPERS_H_ */
