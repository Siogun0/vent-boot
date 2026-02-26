#ifndef UTILS_H_
#define UTILS_H_

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define PI 3.141592653589793f

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define SQ(a)			 	((a) * (a))
#define ABS(a) 				( ((a) < 0) ? -(a) : (a) )
#define DBAND(a, l, h) 		( (((a) <= (h)) && ((a) >= (l))) ? 0 : (a) )
#define CLIP(a, l, h) 		( MAX((MIN((a), (h))), (l)) )
#define CLIPL(a, l) 		( ((a) < (l)) ? (l) : (a))
#define CLIPH(a, h) 		( ((a) > (h)) ? (h) : (a))
#define SIGN(a) 			( ((a) > 0) ? 1 : (((a) == 0) ? 0 : -1) )
#define DIRECTION(a) 		( ((a) >= 0) ? 1 : -1 )
#define INRANGE(a, l, h) 	( (((a) > (l)) && ((a) <= (h))) || ((a) == (l)) )
#define ROUND(a) 			( ABS((a) - ((int)(a))) >= 0.5 ? ((int)(a)) + SIGN(a) : (int)(a) )
#define DECAY(a, l, h)     	CLIP((((a) - (l)) / ((h) - (l))), 0.0, 1.0)
#define INV_DECAY(a, l, h) 	CLIP((((h) - (a)) / ((h) - (l))), 0.0, 1.0)
#define DEG_TO_RAD(a)		((a) * PI / 180.0f)
#define RAD_TO_DEG(a)		((a) * 180.0f / PI)
#define LOGIC_XOR(a, b)		(((a) || (b)) && !((a) && (b)))


/*
DECAY
           _______
  1       /
  0   ___/
        l  h
*/

/*
INV_DECAY
      ___
  1      \
  0       \_______
        l  h
 */


#define TIME_H(t)			(uint32_t)((t) / 3600.0)
#define TIME_M(t)			(uint32_t)((long long)(t) / 60 % 60)
#define TIME_S(t)			(uint32_t)((long long)(t) % 60)


float FitPhase(float theta);

float RotRate(float ang, float dt);

uint16_t ltb16(uint16_t val);

int16_t ltb16s(int16_t val);

uint32_t ltb32(uint32_t val);

uint64_t ltb64(uint64_t val);

void swapf(float *a, float *b);

void VectClip(float *d, float *q, float hi);

void TimersInit(volatile float *timers, int size);
void TimersUpdate(volatile float *timers, int size, float dt);
int32_t IsTimerExpired(volatile float *cntr, float tout);
void TimerReset(volatile float *timers, int n);

typedef volatile struct
{
	float tmax;
	float timer;
	uint32_t status;
} t_alive;

void AliveInit(t_alive *alive, float tmax);
void AliveUpdate(t_alive *alive, float dt);
void AliveReset(t_alive *alive);









#endif


