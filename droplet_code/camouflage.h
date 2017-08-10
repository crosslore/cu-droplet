/*
* camouflage.h
* For Camouflage Project
* Created: 5/25/2016, re-factored on 6/30/2016
* Author : Yang Li
 */ 

#pragma once

#include "droplet_init.h"

#define NUM_NEIGHBOR_12		12
#define NUM_NEIGHBOR_8		8
#define NUM_NEIGHBOR_4		4

#define NEIGHBOR_ANGLE_THRESH 20

#define NEIGHBOR_MSG_FLAG	'N'
#define RGB_MSG_FLAG		'R'
#define PATTERN_MSG_FLAG	'P'
#define TURING_MSG_FLAG		'T'
#define NUM_PATTERNS		3
#define NUM_PREPARE			25 //25
#define NUM_GRADIENT		10 //10
#define NUM_CONSENSUS		10 //10
#define NUM_TURING			20 //20
#define NUM_WAITING			500	//500

#define SLOT_LENGTH_MS		271	//271
#define SLOTS_PER_FRAME		37	//37
#define FRAME_LENGTH_MS		(((uint32_t)SLOT_LENGTH_MS)*((uint32_t)SLOTS_PER_FRAME))
#define LOOP_DELAY_MS		17	//17

//Turing Pattern related
#define TURING_F_STRIPE		(0.75f)
#define TURING_F_SPOT		(0.55f)
#define TURING_RANDOM		(0.02f)		// A threshold for random pattern
#define PI_6				0.523598775598298873077  // pi/6
#define PI_12				0.261799387799149436538  // pi/12
#define TEST_PREPARE		1
#define TEST_GRADIENT		1
#define TEST_CONSENSUS		1
#define TEST_TURING			1


int threshold_mottled = 50;	// 45

const float rgb_weights[3] = {0.5, 0.5, 0.0};  // RGB to Gray

const uint8_t vIndex[NUM_NEIGHBOR_4] = {0, 2, 8, 10};
const uint8_t hIndex[NUM_NEIGHBOR_4] = {1, 3, 9, 11};

/*  */
typedef struct Droplet_struct{
	float myPattern_f[NUM_PATTERNS];
	id_t neighborIds[NUM_NEIGHBOR_12];
	int16_t rgb[3];
	int16_t colorDiff[2];
	id_t dropletId;
	uint8_t mySlot;
	uint8_t myDegree;
	uint8_t turing_color;
} Droplet;

typedef struct NeighborId_struct{
	id_t Ids[NUM_NEIGHBOR_4];
	id_t dropletId;
	uint8_t  gotMsg_flags[NUM_NEIGHBOR_4];
	char flag;
} neighborMsg;

typedef struct RGB_struct{
	int16_t rgb[3];
	uint16_t dropletId;
	char flag;
} rgbMsg;

typedef struct Pattern_struct{
	float pattern_f[NUM_PATTERNS];
	id_t dropletId;
	uint8_t degree;
	char flag;
} patternMsg;

typedef struct Turing_struct{
	id_t dropletId;
	uint8_t color;
	char flag;
} turingMsg;


Droplet me;

/*
* myFourDr stores the neighbors' IDs of my four neighbors
* so that I can extend the neighbor graph to a larger one as needed

From:
			0	
		3	X	1	
			2	
To:
			8
		7	0	4
    11	3	X	1	9
		6	2	5
			10	
*/
neighborMsg myFourDr;
/* DELETED
	uint8_t myFourDrConfs[NUM_NEIGHBOR_4];

* In the new version, it seems from John we don't need Conf value to 
* decide if the measurement is better than others.
*/
// store 4 neighbor's neighbor information
neighborMsg fourNeiInfo[NUM_NEIGHBOR_4];
// store 4 neighbor's RGB information
rgbMsg fourNeiRGB[NUM_NEIGHBOR_4];
// store 12 neighbor's Turing Color information
turingMsg twelveNeiTuring[NUM_NEIGHBOR_12];

/* Used in consensus phase */
typedef struct pattern_node_struct{
	float pattern_f[NUM_PATTERNS];
	uint8_t degree;
	struct pattern_node_struct* next;
} patternNode;
patternNode* nbrPatternRoot;
patternNode* lastPatternAdded;


/*       Print data        */ 
// RGB reading
rgbMsg allRGB[NUM_PREPARE];
patternMsg  allPattern[NUM_CONSENSUS];
uint8_t turingHistory[NUM_TURING][NUM_NEIGHBOR_12];
uint8_t turingHistoryCorrected[NUM_TURING][NUM_NEIGHBOR_12];

int16_t red_array[NUM_PREPARE];
int16_t green_array[NUM_PREPARE];
int16_t blue_array[NUM_PREPARE];

// Store information from neighboring Droplets
/*
Neighbor Index document:
//////////////////////////////
        8
    7	0	4
11	3	X	1	9
    6	2	5
       10
//////////////////////////////
*/
Droplet neighborDroplets[NUM_NEIGHBOR_12];

typedef enum{
	Prepare=0,
	Gradient=1,
	Consensus=2,
	Turing=3,
	Waiting=4
} Phase;

typedef struct rnb_node_struct{
	int16_t range;
	int16_t bearing;
	id_t id;
	struct rnb_node_struct* next;
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
void handle_neighbor_msg(neighborMsg* msg);
void handle_rgb_msg(rgbMsg* msg);
void handle_pattern_msg(patternMsg* msg);
void handle_turing_msg(turingMsg* msg);

void handleRNB(void);
void updateNeighbors(void);
void processMeasList(RnbNode* node);

void extendNeighbors(void);
uint8_t user_handle_command(char* command_word, char* command_args);

void prepareSlot(void);
void prepareEOP(void);
void gradientEOP(void);
void consensusEOP(void);
void turingEOP(void);
void waitingEOP(void);
void setColor(void);

void sendRGBMsg(void);
void sendPatternMsg(void);
void sendTuringMsg(void);
void sendNeiMsg(void);
void decidePattern(void);
void weightedAverage(void);
void changeColor(void);

void displayMenu(void);
void printNs(void);
void printRGBs(void);
void printRGBs_ordered(void);
void printRGB(void);
void printProb(void);
void printTuring(void);
void printTuringHistory(void);
void printTuringHistoryCorrected(void);


/*
in math.h
#define M_PI		3.14159265358979323846	pi 
#define M_PI_2		1.57079632679489661923	pi/2
#define M_PI_4		0.78539816339744830962	pi/4
#define M_1_PI		0.31830988618379067154	1/pi

*/