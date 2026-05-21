#ifndef SRC_APPLICATION_CONTROLLERS_H_
#define SRC_APPLICATION_CONTROLLERS_H_

#include "appdefs.h"

namespace Controller
{
enum Src
{
	Off = 0,
	Expression,
	FswDown,
	FswConfirm,
	FswUp,
	CC1,
	NOTE = CC1 + 128,
	LAST_CONTROLLER = NOTE + 119, // From C-2 to B7
	External = 255
};

enum Dst
{
	PreampOnOff,
	AmpOnOff, AmpVolume, AmpSlave,
	CabSimOnOff,
	EqualOnOff,
	DelayOnOff, DelayMix, DelayFeedback, DelayTap,
	PhaserOnOff, PhaserMix, PhaserRate,
	FlangerOnOff, FlangerMix, FlangerRate,
	ChorusOnOff, ChorusMix, ChorusRate,
	ReverbOnOff, ReverbMix, ReverbTime,
	TremoloOnOff, TremoloIntensity, TremoloRate,
	PresetLevel,
	TremoloTap,
	CompressorOnOff, CompressorThreshold, CompressorVolume,
	RfOnOff, RfLFOrate, RfFreq,
	EROnOff, ERMix,
	RfLFOTAP,
	VolCtrlOnOff,
	Cab1Volume, Cab2Volume,
	GateOnOff, GateThresh,
	HPFfrequency, LPFfrequency, PresenceVal,
	PreampGain, PreampVolume, PreampLow, PreampMid, PreampHigh,
	EqBand1Lev, EqBand2Lev, EqBand3Lev, EqBand4Lev, EqBand5Lev,
	ReverbType
};

typedef struct
{
	uint8_t src;
	uint8_t dst;
	uint8_t minVal;
	uint8_t maxVal;
}TController;

const uint8_t controllersCount = 32; // Массив был 512 байт, те 128 контроллеров

void SetData(const TController& controller, uint8_t data);

extern const char* controllerCommandString;
extern const char* srcCommandString;
extern const char* dstCommandString;
extern const char* minCommandString;
extern const char* maxCommandString;

extern const char* pcoutCommandString;
extern const char* pcsetCommandString;

void setSrc(uint8_t controllerNum, uint8_t value);
void setDst(uint8_t controllerNum, uint8_t value);
void setMinVal(uint8_t controllerNum, uint8_t value);
void setMaxVal(uint8_t controllerNum, uint8_t value);
}



#endif /* SRC_APPLICATION_CONTROLLERS_H_ */
