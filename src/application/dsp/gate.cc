#include "gate.h"

#include "vdt/vdt.h"
#include "dsp.h"

//------------------------------------------------Gate-----------------------------------------------------
#define ENV_TR 0.0001f
#define CLOSED  1
#define OPENING 2
#define OPEN    3
#define CLOSING 4

float env = 0.0f;
float t_level;
float gate = 0.0f;
float a_rate;
float fs = 46875.0f;
float d_rate = 6.85959239e-05f;
float cut;
int state;
int hold_count = 0.0f;
int Pthreshold;
int Prange;
int Pattack;
int Pdecay;
int Phold;
void Gate_Change (int np, int value)
{
  switch (np)
    {
    case 0:Pthreshold = value;t_level = dB2rap((float)Pthreshold);break;
    case 1:Pattack = value;a_rate = 1000.0f / ((float)Pattack * fs); break;
    case 2:Pdecay = value;d_rate = 1000.0f / ((float)Pdecay * fs);break;
    }
}

void GATE_ChangePreset (void)
{
	  env = 0.0f;
	  gate = 0.0f;
	  fs = samp_rate;
	  state = CLOSED;
	  hold_count = 0;
    int presets[3] = {-50, 1, 100};
    for (int n = 0; n < 3; n++) Gate_Change (n, presets[n]);
}

float Gate_out (float efxout)
{
  float sum;
      sum = fabsf (efxout);
      if (sum > env)env = sum;
      else env = 0.0f;//sum * ENV_TR + env * (1.0f - ENV_TR);
  //------------------------------------------------------------------
      if (state == CLOSED)
      {
    	  if (env >= t_level)state = OPENING;
      }
      else {
    	  if (state == OPENING)
      	  {
    	  	  gate += a_rate;
    	  	  if (gate >= 1.0f)
    	  	  {
    	  		  gate = 1.0f;
    	  		  state = OPEN;
    	  	  }
      	  }
      	  else {
      		  if (state == OPEN)
      	  	  {
      			  if (env < t_level) state = CLOSING;
      	  	  }
      	  	  else {
      	  		  if (state == CLOSING)
      	  		  {
      	  			  gate -= d_rate;
      	  			  if (env >= t_level)state = OPENING;
      	  			  else {
      	  				  if (gate <= 0.0f)
      	  				  {
      	  					  gate = 0.0f;
      	  					  state = CLOSED;
      	  				  }
      	  			  }
      	  		  }
      	  	  }
      	  }
      }
      return gate;
}
void GATE_SetParam(uint32_t val)
{
	uint32_t va = val >> 8;
	val &= 0xff;
	switch(val & 0xff){
	case 0:Gate_Change(0,-((127 - va) * (70.0f/127.0f) + 17.0f));break;
	case 1:Gate_Change(1, va + 1);break;
	case 2:Gate_Change(2, va * 10 + 10);break;
	}
}
