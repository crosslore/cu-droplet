/*
* camouflage.h
* For Camouflage Project
* Created: 5/25/2016, re-factored on 6/30/2016
* Updated: 8/12/2017 (with localization)
* Author : Yang Li, John Klingner
 */ 

#pragma once

#include "droplet_init.h"

#define PATTERN_MSG_FLAG	'P'
#define TURING_MSG_FLAG		'T'
#define BOT_POS_MSG_FLAG	'B'
#define NUM_LOCALIZE		20 // 20
#define NUM_PREPARE			20 // 20
#define NUM_GRADIENT		10 // 10
#define NUM_CONSENSUS		10 // 10
#define NUM_TURING			20 // 20
#define NUM_WAITING			500


#define SLOTS_PER_FRAME		23	// 37
#define LOOP_DELAY_MS		17

//Turing Pattern related
#define TURING_F			(0.75f)
#define TURING_RANDOM		(0.02f)		// A threshold for random pattern

#define TEST_PREPARE		1
#define TEST_GRADIENT		1
#define TEST_CONSENSUS		1
#define TEST_TURING			1

#define L_OF_G_SIGMA		0.5
#define L_OF_G_WIDTH		50.0 //mm

//All of the below should be in mm
#define ACTIVATOR_WIDTH 210
#define ACTIVATOR_HEIGHT 70
#define INHIBITOR_WIDTH 210
#define INHIBITOR_HEIGHT 210

const float rgb_weights[3] = {0.5, 0.5, 0.0};  // RGB to Gray

typedef struct pattern_struct{
	float x;
	float y;
}Pattern;

/*  */
typedef struct Droplet_struct{
	int16_t rgb[3];
	id_t dropletId;
	uint8_t slot;
	uint8_t degree;
	uint8_t turingColor;
	uint8_t nA; //num activators
	uint8_t nI; //num inhibitors
	Pattern p;
	Matrix patternTransformA;
	Matrix patternTransformI;
} Droplet;


/* Used in consensus phase */
typedef struct pattern_node_struct{
	Pattern p;
	uint8_t degree;
	struct pattern_node_struct* next;
} PatternNode;
PatternNode* nbrPatternRoot;
PatternNode* lastPatternAdded;

typedef struct pattern_msg_struct{ //12 bytes
	Pattern p;
	id_t dropletId;
	uint8_t degree;
	char flag;
} PatternMsg;

typedef struct turing_msg_struct{
	int16_t x;
	int16_t y;
	char flag;
} TuringMsg;

#define NUM_TRACKED_BOTS 32

#define POS_C_DEFINED(pos) ((((pos)->x)!=UNDF)&&(((pos)->y)!=UNDF))

typedef struct pos_and_color_struct{
	int16_t x;
	int16_t y;
	uint8_t col;
}PosColor;
PosColor otherBots[NUM_TRACKED_BOTS+1];
PosColor myPosColor;

#define NUM_TRANSMITTED_BOTS 5
#define NUM_CHOSEN_BOTS (NUM_TRANSMITTED_BOTS-1)

typedef struct botpos_msg_struct{
	PosColor bots[NUM_TRANSMITTED_BOTS];
	char flag;
}BotPosMsg;

Droplet me;

/*       Print data        */ 
int16_t allRGB[NUM_PREPARE][3]; // RGB reading
Pattern allPatterns[NUM_CONSENSUS];
uint8_t turingHistory[NUM_TURING][3];

int16_t red_array[NUM_LOCALIZE];
int16_t green_array[NUM_LOCALIZE];
int16_t blue_array[NUM_LOCALIZE];

#define NUM_PHASES 5

typedef enum{
	Localize,
	Prepare,
	Consensus,
	Turing,
	Waiting
} Phase;
uint32_t slotLength[NUM_PHASES] = {397, 271, 271, 271, 271};
uint32_t frameLength[NUM_PHASES];


typedef struct rnb_node_struct{
	int16_t range;
	int16_t bearing;
	id_t id;
	struct rnb_node_struct* next;
	uint8_t conf;
} RnbNode;
RnbNode* measRoot;
RnbNode* lastMeasAdded;


uint32_t frameStart;
uint32_t frameCount;
uint16_t loopID;
Phase phase;
uint8_t counter;			// to exit phases

void init(void);
void loop(void);
void handle_msg	(ir_msg* msg_struct);
void handleBotPosMsg(BotPosMsg* msg);
void handlePatternMsg(PatternMsg* msg);
void handleTuringMsg(TuringMsg* msg);

void handleRNB(void);

uint8_t user_handle_command(char* command_word, char* command_args);

void localizeSlot(void);

void localizeEOP(void);
void prepareEOP(void);
void gradientEOP(void);
void consensusEOP(void);
void turingEOP(void);
void waitingEOP(void);
void setColor(void);

void sendBotPosMsg(void);
void sendPatternMsg(void);
void sendTuringMsg(void);
void decidePattern(void);
void weightedAverage(void);
void updateTuringColor(void);

void chooseTransmittedBots(uint8_t (*indices)[NUM_CHOSEN_BOTS]);
uint8_t addBot(PosColor pos);

inline void LofGxy(int16_t deltaX, int16_t deltaY, float* LofGx, float* LofGy){
	float scalarTerm = 1.0/(M_2_PI*powf(L_OF_G_SIGMA,6.0));
	float scaledXsq = powf(deltaX/L_OF_G_WIDTH,2);
	float scaledYsq = powf(deltaY/L_OF_G_WIDTH,2);
	float expTerm = -((scaledXsq + scaledYsq)/(2*powf(L_OF_G_SIGMA,2)));
	float leftSide = scalarTerm*exp(expTerm);
	*LofGx = leftSide*(scaledXsq - powf(L_OF_G_SIGMA,2));
	*LofGy = leftSide*(scaledYsq - powf(L_OF_G_SIGMA,2));
}

inline uint8_t packColor(int16_t r, int16_t g, int16_t b, uint8_t turingColor){
	uint8_t packedVal = 0;
	if(turingColor){
		packedVal |= 0b11000000;
	}
	if(r>25 && r<=40){
		packedVal |= 0b00010000;
		}else if(r>40){
		packedVal |= 0b00100000;
	}
	if(g>25 && g<=40){
		packedVal |= 0b00000100;
		}else if(g>40){
		packedVal |= 0b00001000;
	}
	if(b>25 && b<=40){
		packedVal |= 0b00000001;
		}else if(b>40){
		packedVal |= 0b00000010;
	}
	return packedVal;
}

inline float unpackColorToGray(uint8_t col){
		float rgb[3];
		rgb[0] = (col>>4)*0.5;
		rgb[1] = (col>>2)*0.5;
		rgb[2] = (col)*0.5;
		float grayVal = 0.0;
		for(uint8_t i=0; i<3; i++){
			grayVal += rgb[i]*rgb_weights[i];
		}
		return grayVal;
}


//Comparison functions below are used for the calls to qsort.

static int distCmp(const void* a, const void* b){
	PosColor* aPos = (((PosColor*)a));
	PosColor* bPos = (((PosColor*)b));
	float aDist = hypot(aPos->y - myPos.y, aPos->x - myPos.x);
	float bDist = hypot(bPos->y - myPos.y, bPos->x - myPos.x);
	if(POS_C_DEFINED(aPos) && POS_C_DEFINED(bPos)){
		if(aDist < bDist){
			return -1;
		}else if(bDist < aDist){
			return 1;
		}else{
			return 0;
		}
	}else if(POS_C_DEFINED(aPos)){
		return -1;
	}else if(POS_C_DEFINED(bPos)){
		return 1;
	}else{
		return 0;
	}
}

static int randomizerCmp(const void* aR, const void* bR){
	uint16_t aAll = *(((uint16_t*)aR));
	uint16_t bAll = *(((uint16_t*)bR));
	uint16_t a = aAll&0xFF;
	uint16_t b = bAll&0xFF;
	if(a < b){
		return -1;
	}else if(b < a){
		return 1;
	}else{
		return 0;
	}
}


/*
in math.h
#define M_PI		3.14159265358979323846	pi 
#define M_PI_2		1.57079632679489661923	pi/2
#define M_PI_4		0.78539816339744830962	pi/4
#define M_1_PI		0.31830988618379067154	1/pi

*/