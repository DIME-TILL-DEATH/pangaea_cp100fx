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
	External = 255
};

enum Dst
{
	PreampOnOff,
	AmpOnOff, AmpVolume, AmpSlave,
	CabSimOnOff,
	EqualOnOff,
	DelayOnOff, DelayVolume, DelayFeedback, DelayTap,
	PhaserOnOff, PhaserVolume, PhaserRate,
	FlangerOnOff, FlangerVolume, FlangerRate,
	ChorusOnOff, ChorusVolume, ChorusRate,
	ReverbOnOff, ReverbVolume, ReverbTime,
	TremoloOnOff, TremoloIntensity, TremoloRate,
	PresetLevel,
	TremoloTap,
	CompressorOnOff, CompressorThreshold, CompressorVolume,
	RfOnOff, RfLFOrate, RfFreq,
	EROnOff, ERVolume,
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

}

const uint8_t controllersCount = 32; // Массив был 512 байт, те 128 контроллеров

#endif /* SRC_APPLICATION_CONTROLLERS_H_ */
