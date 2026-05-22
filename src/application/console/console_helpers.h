#ifndef _CONSOLE_HELPERS_H_
#define _CONSOLE_HELPERS_H_

#include "module_parameter.h"
#include "console_task.h"

void console_printf(const char *format, ...);
void default_param_handler(uint8_t *param_ptr, TTranslator::const_symbol_type_ptr_t *args, const size_t count);
void default_param_setter(const TParamDescriptor& paramDescriptor, uint8_t value);

#endif /* _CONSOLE_HELPERS_H_ */
