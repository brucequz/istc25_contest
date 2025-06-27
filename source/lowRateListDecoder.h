
#ifndef LOWRATELISTDECODER_H
#define LOWRATELISTDECODER_H

#include <climits>
#include <iostream>
#include <algorithm>

#include "FeedForwardTrellis.h"
#include "minHeap.h"
#include "tbcc_types.h"
#include "tbcc_namespace.h"

class LowRateListDecoder{
public:
	LowRateListDecoder(FeedForwardTrellis FT, int listSize, int crcDegree, int crc, char stopping_rule);

	/* - Floating Point - */
	MessageInformation decode(std::vector<float> receivedMessage, std::vector<int> punctured_indices);
	MessageInformation lowRateDecoding_MaxListsize(std::vector<float> receivedMessage, std::vector<int> punctured_indices);
	MessageInformation lowRateDecoding_MaxMetric(std::vector<float> receivedMessage, std::vector<int> punctured_indices);
	MessageInformation lowRateDecoding_MaxAngle(std::vector<float> receivedMessage, std::vector<int> punctured_indices);
	MessageInformation lowRateDecoding_MaxAngle_ProductMetric(std::vector<float> receivedMessage, std::vector<int> punctured_indices);
	MessageInformation lowRateDecoding_MaxAngle_ProductMetric_ZT(std::vector<float> receivedMessage);

	/* - Fixed Point - */
	Fixed_p_MessageInformation decode(llrvec receivedMessage, intvec punctured_indices);
	Fixed_p_MessageInformation lowRateDecoding_MaxAngle_ProductMetric_TB_fixedp(llrvec receivedMessage, intvec punctured_indices);
	Fixed_p_MessageInformation lowRateDecoding_MaxAngle_ProductMetric_ZT_fixedp(llrvec receivedMessage);


private:
	int numForwardPaths;
	int listSize;
	int crcDegree;
	int crc;
	int n;
	char stopping_rule;

	std::vector<std::vector<int>> lowrate_nextStates;
	std::vector<std::vector<int>> lowrate_outputs;
	std::vector<std::vector<int>> neighboring_cwds; // ${listSize} x 516 matrix
	std::vector<std::vector<int>> neighboring_msgs;  // ${listSize} x 43 matrix
	std::vector<std::vector<int>> path_ie_state;
	int lowrate_numStates;
	int lowrate_symbolLength;
	int lowrate_pathLength;

	struct cell {
		int optimalFatherState = -1;
		int suboptimalFatherState = -1;
		float pathMetric = INT_MAX;
		float suboptimalPathMetric = INT_MAX;
		bool init = false;
	};

	struct fixedp_cell {
		int optimalFatherState = -1;
		int suboptimalFatherState = -1;
		fixedp_type pathMetric{3000};
		fixedp_type suboptimalPathMetric{3000};
		bool init = false;
	};

	std::vector<int> pathToMessage(std::vector<int>); 
  std::vector<int> pathToCodeword(std::vector<int>); 
	std::vector<int> pathToMessage_ZT(std::vector<int> path);

	/* - Floating Point - */
	std::vector<std::vector<cell>> constructLowRateTrellis(std::vector<float> receivedMessage);
	std::vector<std::vector<cell>> constructLowRateTrellis_ZT(std::vector<float> receivedMessage);

	/* - Fixed Point - */
	std::vector<std::vector<fixedp_cell>> constructLowRateTrellis_Punctured_ProductMetric_fixedp(llrvec receivedMessage, intvec punctured_indices);
	std::vector<std::vector<fixedp_cell>> constructLowRateTrellis_ZT_fixedp(llrvec receivedMessage);

	// Punctured
  std::vector<std::vector<cell>> constructLowRateTrellis_Punctured(std::vector<float> receivedMessage, std::vector<int> punctured_indices);
	std::vector<std::vector<cell>> constructLowRateTrellis_Punctured_ProductMetric(std::vector<float> receivedMessage, std::vector<int> punctured_indices);
};


#endif