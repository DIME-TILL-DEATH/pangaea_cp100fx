#ifndef FONT_H_
#define FONT_H_

#include "appdefs.h"

#define FONT_BLINKING (Font::TFontState)(blinkFlag * Font::fnsHighlight)
#define FONT_BLINKING_INVISIBLE (Font::TFontState)(blinkFlag * Font::fnsInvisible)

namespace Font
{
typedef enum
{
	fnsNormal = 0,
	fnsUnderscore,
	fnsHighlight,
	fnsInvisible
} TFontState;

typedef enum
{
	fnt12x13 = 0,
	fnt33x30,
	fntSystem,
} TFontName;

typedef struct
{
	TFontName name;
	TFontState state;
} TFontStruct;

uint8_t symbolWidth(TFontName fontName);
};

#endif /* FONT_H_ */
