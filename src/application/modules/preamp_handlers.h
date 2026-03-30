#ifndef _CONSOLE_PREAMP_HANDLERS_H_
#define _CONSOLE_PREAMP_HANDLERS_H_

#include "translator.h"

#include "modules.h"

void set_preamp_handlers(TTranslator *rl);

void preamp_on_command_handler(uint32_t value);

extern TParamDescriptor preampOn;

#endif /* _CONSOLE_PREAMP_HANDLERS_H_ */
