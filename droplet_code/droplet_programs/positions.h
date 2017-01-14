#pragma once

#include "droplet_init.h"

extern Matrix identity_matrix;

#define POS_DEBUG_MODE
#define GEN_DEBUG_MODE
#define P_SAMPLE_DEBUG_MODE
#define P_L_DEBUG_MODE
//#define OCC_DEBUG_MODE
//#define RNB_DEBUG_MODE
//#define NB_DEBUG_MODE
//#define BALL_DEBUG_MODE

#define MIN_PACKED_X -1024
#define MIN_PACKED_Y -1024
#define MIN_PACKED_O -512

/*
 * Constant term for process noise. 
 * We don't actually have a process, but adding some process noise
 * keeps our pK from getting too (unrealistically) small over time,
 * which it would otherwise do as a result of aspects of system not
 * modelled by Kalman filter.
 */
Matrix qK = {{100,0,0},{0,100,0},{0,0,100}}; 

//uint8_t useNewInfo;
uint8_t useBlacklist;
uint8_t stdDevThreshold;
uint8_t useMeasAveraging;
uint8_t addedPosStdDev;
uint8_t addedMeasStdDev;

#ifdef OCC_DEBUG_MODE
#define OCC_DEBUG_PRINT(format, ...) printf_P(PSTR(format), ##__VA_ARGS__)
#else
#define OCC_DEBUG_PRINT(format, ...)
#endif

#ifdef POS_DEBUG_MODE
#define POS_DEBUG_PRINT(format, ...) printf_P(PSTR(format), ##__VA_ARGS__)
#else
#define POS_DEBUG_PRINT(format, ...)
#endif

#ifdef P_L_DEBUG_MODE
#define P_L_DEBUG_PRINT(format, ...) printf_P(PSTR(format), ##__VA_ARGS__)
#else
#define P_L_DEBUG_PRINT(format, ...)
#endif

#ifdef P_SAMPLE_DEBUG_MODE
#define P_SAMPLE_DEBUG_PRINT(format, ...) printf_P(PSTR(format), ##__VA_ARGS__)
#else
#define P_SAMPLE_DEBUG_PRINT(format, ...)
#endif


#ifdef BALL_DEBUG_MODE
#define BALL_DEBUG_PRINT(format, ...) printf_P(PSTR(format), ##__VA_ARGS__)
#else
#define BALL_DEBUG_PRINT(format, ...)
#endif

#ifdef GEN_DEBUG_MODE
#define GEN_DEBUG_PRINT(format, ...) printf_P(PSTR(format), ##__VA_ARGS__)
#else
#define GEN_DEBUG_PRINT(format, ...)
#endif

#ifdef NB_DEBUG_MODE
#define NB_DEBUG_PRINT(format, ...) printf_P(PSTR(format), ##__VA_ARGS__)
#else
#define NB_DEBUG_PRINT(format, ...)
#endif

#ifdef RNB_DEBUG_MODE
#define RNB_DEBUG_PRINT(format, ...) printf_P(PSTR(format), ##__VA_ARGS__)
#else
#define RNB_DEBUG_PRINT(format, ...)
#endif

#ifdef NEW_INFO_DEBUG_MODE
#define NEW_INFO_DEBUG_PRINT(format, ...) printf_P(PSTR(format), ##__VA_ARGS__)
#else
#define NEW_INFO_DEBUG_PRINT(format, ...)
#endif

/*
 * rnb takes 142 ms
 * messages take 2.5ms per byte. 
 * paddleMsg is 3 bytes. header is 8 bytes, so PaddleMsg should take 27.5
 * neighbMsg is 30 bytes. header is 8 bytes, so NeighbMsg should take 95ms
 * ballMsg is 7 bytes. header is 8 bytes, so ballMsg should take 37.5ms 
 */
#define RNB_DUR		100 //80 ms should be enough.
#define PADDLE_MSG_DUR		40 //padding. Probably excessive.
#define NEIGHB_MSG_DUR		100
#define BALL_MSG_DUR		40 //padding. Probably excessive.

#define UNDF	((int16_t)0x8000)

#define SLOT_LENGTH_MS			307
#define SLOTS_PER_FRAME			37
#define FRAME_LENGTH_MS			(((uint32_t)SLOT_LENGTH_MS)*((uint32_t)SLOTS_PER_FRAME))
#define LOOP_DELAY_MS			17

#define DROPLET_DIAMETER_MM		44.4
#define BALL_MSG_FLAG			'B'
#define NEAR_BOTS_MSG_FLAG		'N'
//#define N_PADDLE_MSG_FLAG		'P'
//#define S_PADDLE_MSG_FLAG		'S'

//TODO: Make paddle_width dynamically calculated!
//#define PADDLE_WIDTH		(FLOOR_WIDTH/3)
#define PADDLE_VEL				0.1
#define NUM_SEEDS 4
#define NUM_SHARED_BOTS 6
#define NUM_USED_BOTS 5
#define NUM_TRACKED_BOTS 12

const id_t	   SEED_IDS[NUM_SEEDS]	   = {0x6B6F, 0xCB64, 0xB41B, 0xDF64};
const int16_t  SEED_X[NUM_SEEDS]   = {100, 900, 100, 900};
const int16_t  SEED_Y[NUM_SEEDS]   = {900, 900, 100, 100};

#define MIN_X 0
#define MIN_Y 0
#define MAX_X 1000
#define MAX_Y 1000

#define STATE_PIXEL		0x1
#define STATE_NORTH		0x2
#define STATE_SOUTH		0x4

#define NORTH_PIXEL(state)		((state&STATE_PIXEL)&&(state&STATE_NORTH))
#define SOUTH_PIXEL(state)		((state&STATE_PIXEL)&&(state&STATE_SOUTH))
#define NS_PIXEL(state)			((state&STATE_PIXEL)&&((state&STATE_NORTH) || (state&STATE_SOUTH)))

#define OTHERBOT_BLACKLIST_FRAME_COUNT 4

typedef enum {
	POS,
	SYNC_TEST,
	OFF
} ColorMode;
ColorMode	colorMode;

typedef enum {
	PONG,
	BOUNCE
} GameMode;
GameMode gameMode;

typedef struct ball_msg_struct{
	char flag;
	uint8_t xPos;
	uint8_t yPos;
	uint8_t extraBits; //bits 7-5 are three high bits for xPos; bits 4-2 are three high bits for yPos; bits 0-1 are two low bits for id.
	int8_t xVel;
	int8_t yVel;
	uint8_t radius; //bits 0-1 are two high bits for id. rest is radius (which must be divisible by 4)
}BallMsg;

typedef struct packed_bot_meas_struct{
	id_t id;
	uint8_t range;
	int8_t b;
}PackedBotMeas;

typedef struct packed_bot_pos_struct{
	uint8_t xLow;
	uint8_t yLow;
	uint8_t oLow;
	uint8_t bits;
	uint8_t stdDev;
}PackedBotPos;

typedef struct near_bots_msg_struct{
	PackedBotMeas shared[NUM_SHARED_BOTS]; //4 bytes each
	PackedBotPos pos; //5 bytes.
	//id_t used[NUM_USED_BOTS];
	char flag;	
}NearBotsMsg;

typedef struct ball_dat_struct{
	uint32_t lastUpdate;	
	int16_t xPos;
	int16_t yPos;
	int8_t xVel;
	int8_t yVel;
	uint8_t id;
	uint8_t radius;
}BallDat;
BallDat theBall;

typedef struct bot_meas_struct
{
	id_t id;
	uint16_t r;
	int16_t b;
	int16_t h;
	uint8_t stdDev;	
} BotMeas;

typedef struct bot_pos_struct
{
	int16_t x;
	int16_t y;
	int16_t o;
	uint8_t stdDev;
} BotPos;

typedef struct other_bot_rnb_struct{
	BotMeas myMeas;
	BotMeas theirMeas;
	BotPos pos;
	uint32_t lastUsed;
	uint8_t occluded;
	//uint8_t hasNewInfo;
} OtherBot;
OtherBot nearBots[NUM_TRACKED_BOTS+1];

typedef struct hard_bot_struct{
	id_t id;
	struct hard_bot_struct* next;
} HardBot;
HardBot* hardBotsList;

id_t lastUsedBots[NUM_USED_BOTS];

uint32_t time_before;


uint8_t		lastBallID;
uint8_t		seedFlag;
uint8_t		myState;
uint32_t	frameCount;
uint32_t	frameStart;
uint32_t	lastBallMsg;
//uint32_t	lastPaddleMsg;
uint32_t	lastLightCheck;
uint16_t	mySlot;
uint16_t	loopID;
int16_t xRange;
int16_t yRange;
int16_t maxRange;


//float		paddleChange;
//int16_t		paddleStart;
//int16_t		paddleEnd;
uint8_t		isCovered;

BotPos myPos;
Matrix pK;
uint16_t myDist;
uint16_t otherDist;

void		init();
void		loop();
void		handleMySlot();
void		calcPosFromMeas(BotPos* calcPos, BotPos* pos, BotMeas* meas);
void		printPosFromMeas(BotPos* pos, BotMeas* meas);
uint8_t		countAvailableMeasurements();
uint8_t     nearBotUseabilityCheck(uint8_t i);

void		extrapolateCovarFromScalar(Matrix* A, float stdDev);
float		distillCovarIntoScalar();

void		prepExpectedPositions(BotPos* expPosArr);
void		combineExpectedPositions(float (*pos)[3], float* stdDev, uint8_t numMeas, BotPos* expPosArr);
void		handleFrameEnd(); 
void		updateHardBots();
void		increaseStdDev();


void		updatePosition();
void		updateNearBotOcclusions();
void		useNewRnbMeas();
void		updateBall();
//check_bounce is a helper function for updateBall.
void		check_bounce(int8_t xVel, int8_t yVel, int8_t* newXvel, int8_t* newYvel);
void		updateColor();
//Coverage functions below are helper functions for 'updateColor'.
float		getBallCoverage();
//float getPaddleCoverage();

void		checkLightLevel();

void		sendBallMsg();
void		handleBallMsg(BallMsg* msg, uint32_t arrivalTime);
void		sendNearBotsMsg();
void		handleNearBotsMsg(NearBotsMsg* msg, id_t senderID);
void		handle_msg			(ir_msg* msg_struct);

void		frameEndPrintout();

//These four functions are for interacting with the OtherBot data structure.
OtherBot*	getOtherBot(id_t id);
void		findAndRemoveOtherBot(id_t id);
void		removeOtherBot(uint8_t idx);
OtherBot*	addOtherBot(id_t id);
void		cleanOtherBot(OtherBot* other);
void		printNearBots();
void		printOtherBot(OtherBot* bot);

void		addHardBot(id_t id);
void		cleanHardBots();

//uint8_t checkNearBotForNewInfo(id_t usedBots[NUM_USED_BOTS]);
//void getUsedBots(id_t dest[NUM_USED_BOTS]);
//void addUsedBot(id_t bot);
//void initUsedBots();

/*
 * BEGIN INLINE HELPER FUNCTIONS
 */
 /* This function takes a bearing and heading measurement (b, h) and converts 
  * them to the bearing and heading (newB, newH) from the measured bot to the
  * measuring bot implied by that measurement (b, h).
  */
inline uint8_t validNearBotIdx(uint8_t i){
	return (nearBots[i].pos.x != UNDF && nearBots[i].pos.y!=UNDF && 
			nearBots[i].pos.o!=UNDF);
}


static uint8_t getStdDevFromRange(uint16_t range){
	return (range>=250) ? 200 : ( (range>=125) ? 60 : 30 );
}

inline static void packPos(PackedBotPos* pos){
	int16_t x, y, o;
	x = myPos.x;
	x = (x==UNDF) ? MIN_PACKED_X : x;
	y = myPos.y;
	y = (y==UNDF) ? MIN_PACKED_Y : y;
	o = myPos.o;
	o = (o==UNDF) ? MIN_PACKED_O : o;
	pos->stdDev  = myPos.stdDev;
	pos->xLow  = (uint8_t)(x&0xFF);
	pos->yLow  = (uint8_t)(y&0xFF);
	pos->oLow  = (uint8_t)(o&0xFF);
	pos->bits  = (uint8_t)((x>>8) & 0b00000111);
	pos->bits |= (uint8_t)((y>>5) & 0b00111000);
	pos->bits |= (uint8_t)((o>>2) & 0b11000000);
}

inline static void unpackPos(PackedBotPos* pPos, BotPos* otherPos){
	otherPos->stdDev = pPos->stdDev;
	otherPos->x = pPos->xLow;
	otherPos->x |= ((uint16_t)(pPos->bits & 0b00000111))<<8;
	otherPos->x = (otherPos->x)<<5; //shifting value left (and then right again) in case value is negative.
	otherPos->x = (otherPos->x)/32; //avr-gcc doesn't do arithmetic right shifts, so we're using integer division to get it.
	otherPos->x = (otherPos->x==MIN_PACKED_X) ? UNDF : otherPos->x;
	otherPos->y = pPos->yLow;
	otherPos->y |= ((uint16_t)(pPos->bits & 0b00111000))<<5;
	otherPos->y = (otherPos->y)<<5; //shifting value left (and then right again) in case value is negative.
	otherPos->y = (otherPos->y)/32; //avr-gcc doesn't do arithmetic right shifts, so we're using integer division to get it.
	otherPos->y = (otherPos->y==MIN_PACKED_Y) ? UNDF : otherPos->y;
	otherPos->o = pPos->oLow;
	otherPos->o |= ((uint16_t)(pPos->bits & 0b11000000))<<2;
	otherPos->o = (otherPos->o)<<6; //shifting value left (and then right again) in case value is negative.
	otherPos->o = (otherPos->o)/64; //avr-gcc doesn't do arithmetic right shifts, so we're using integer division to get it.
	otherPos->o = (otherPos->o==MIN_PACKED_O) ? UNDF : otherPos->o;
}

inline static int8_t packAngleMeas(int16_t angle){
	if(angle==UNDF){
		return 0x7E;
	}else{
		return (int8_t)(angle>>1);
	}
}

inline static int16_t unpackAngleMeas(int8_t angle){
	if(angle==0x7E){
		return UNDF;
	}else{
		return (((int16_t)angle)<<1);
	}
}

inline static uint8_t packRange(uint16_t range){
	range = (range>510) ? 510 : range;
	return ((uint8_t)((range+1)>>1)); //dividing by two, rounding towards closest.
}

inline static uint16_t unpackRange(uint8_t packedRange){
	return (((uint16_t)packedRange)<<1); 
}

inline static float getCoverageRatioA(uint8_t rad, uint16_t dist){ //when ball radius less than bot radius.
	const float intermediate = (((float)rad)/(2*DROPLET_RADIUS*DROPLET_RADIUS));
	return intermediate*(rad+DROPLET_RADIUS-dist);
}

inline static float getCoverageRatioB(uint8_t rad, uint16_t dist){ //when bot radius less than ball radius.
	const float intermediate = 1.0/(2.0*DROPLET_RADIUS);
	return intermediate*(rad+DROPLET_RADIUS-dist);
}

inline static uint8_t dirFromAngle(int16_t angle){
	return abs((angle - (angle>0 ? 360 : 0))/60);
}

inline static int8_t checkBallCrossedMe(){
	return sgn(((theBall.yVel*(theBall.yPos-myPos.y-theBall.xVel) + theBall.xVel*(theBall.xPos-myPos.x+theBall.yVel))));
}

inline static int8_t checkBounceHard(int16_t Bx, int16_t By, int32_t timePassed){
	int16_t Ax = myPos.x;
	int16_t Ay = myPos.y;
	int16_t x = theBall.xPos;
	int16_t y = theBall.yPos;
	int8_t signBefore = sgn((Bx-Ax)*(y-Ay) - (By-Ay)*(x-Ax));
	int16_t xAfter = x + (int16_t)((((int32_t)(theBall.xVel))*timePassed)/1000.0);
	int16_t yAfter = y + (int16_t)((((int32_t)(theBall.yVel))*timePassed)/1000.0);
	int8_t signAfter = sgn((Bx-Ax)*(yAfter-Ay) - (By-Ay)*(xAfter-Ax));
	BALL_DEBUG_PRINT("(%4d, %4d) [%hd] -> (%4d, %4d) [%hd]\r\n", x, y, signBefore, xAfter, yAfter, signAfter);
	if(signBefore!=signAfter){
		return 1;
	}else{
		return 0;
	}
}

/*Code below from http://stackoverflow.com/questions/573084/how-to-calculate-bounce-angle */
inline static void calculateBounce(int16_t Bx, int16_t By){
	int16_t vX = theBall.xVel;
	int16_t vY = theBall.yVel;
	int16_t normX = -(By-myPos.y);
	int16_t normY = (Bx-myPos.x);
	int16_t nDotN = normX*normX + normY*normY;
	int16_t vDotN = vX*normX + vY*normY;
	int16_t uX = normX*vDotN/nDotN;
	int16_t uY = normY*vDotN/nDotN;
	theBall.xVel = vX - 2*uX;
	theBall.yVel = vY - 2*uY;
}

////This function assumes that bP->x and bP->y are not UNDF.
//static void inline getOtherXY(BotMeas* bM, BotPos* bP, int16_t* oX, int16_t* oY){
	//int16_t xOffset = 
	//
//}

static int nearBotsCmpFunc(const void* a, const void* b){
	OtherBot* aN = (OtherBot*)a;
	OtherBot* bN = (OtherBot*)b;
	uint8_t aStdDev = (aN->myMeas).stdDev;
	uint8_t bStdDev = (bN->myMeas).stdDev;
	uint16_t aRange = (aN->myMeas).r;
	uint16_t bRange = (bN->myMeas).r;
	uint8_t aID = ((aN->theirMeas).id == get_droplet_id());
	uint8_t bID = ((bN->theirMeas).id == get_droplet_id());
	if(aRange < bRange){
		return -1;
	}else if(bRange < aRange){
		return 1;
	}else{
		if(aID && !bID){
			return -1;
		}else if(!aID && bID){
			return  1;
		}
		if(aStdDev < bStdDev){
			return -1;
		}else if(bStdDev < aStdDev){
			return 1;
		}
	}
	return 0;
}

static int nearBotMeasBearingCmpFunc(const void* a, const void* b){
	BotMeas* aN = (BotMeas*)a;
	BotMeas* bN = (BotMeas*)b;
	int16_t aC = aN->b;
	int16_t bC = bN->b;
	if(aN->id==0){
		return 1;
	}
	if(bN->id==0){
		return -1;
	}
	if(aC < bC){
		return 1;
	}else if(bC < aC){
		return -1;
	}else{
		return 0;
	}
}

inline static void killBall(){
	set_rgb(255,0,0);
	theBall.id = 0x0F;
}

inline static void initPositions(){
	myPos.x = UNDF;
	myPos.y = UNDF;
	myPos.o = UNDF;
	myPos.stdDev = 255;	
	myDist = UNDF;

	seedFlag = 0;	
	for(uint8_t i=0;i<NUM_SEEDS;i++){
		if(get_droplet_id()==SEED_IDS[i]){
			seedFlag = 1;
			myPos.x = SEED_X[i];
			myPos.y = SEED_Y[i];
			myPos.o = 0;
			myPos.stdDev = 1;
			break;
		}
	}
	//paddleChange = 0.0;
	theBall.lastUpdate = 0;
	theBall.xPos = UNDF;
	theBall.yPos = UNDF;
	theBall.xVel = 0;
	theBall.yVel = 0;
	theBall.id = 0;
	theBall.radius = 0;
	//paddleStart	    = MIN_DIM + (FLOOR_WIDTH>>1) - (PADDLE_WIDTH>>1);
	//paddleEnd		= MIN_DIM + (FLOOR_WIDTH>>1) + (PADDLE_WIDTH>>1);
}