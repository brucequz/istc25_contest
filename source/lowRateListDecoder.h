
#ifndef LOWRATELISTDECODER_H
#define LOWRATELISTDECODER_H

#include <climits>
#include <iostream>
#include <algorithm>

#include "FeedForwardTrellis.h"
#include "minHeap.h"
#include "tbcc_types.h"
#include "tbcc_namespace.h"
#include "consts.h"

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
	MessageInformation_fixedp lowRateDecoding_MaxListsize_fixedp(llrvec receivedMessage, std::vector<int> punctured_indices);
	// MessageInformation_fixedp lowRateDecoding_MaxListsize_fixedp(llrvec receivedMessage, std::vector<int> punctured_indices);

	

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

	struct cell_fixedp {
		int optimalFatherState = -1;
		int suboptimalFatherState = -1;
		int32_t pathMetric = (1 << 31) - 1;
		int32_t suboptimalPathMetric = (1 << 31) - 1;
		bool init = false;
	};

	std::vector<int> pathToMessage(std::vector<int>); 
  std::vector<int> pathToCodeword(std::vector<int>); 
	std::vector<int> pathToMessage_ZT(std::vector<int> path);

	/* - Floating Point - */
	std::vector<std::vector<cell>> constructLowRateTrellis(std::vector<float> receivedMessage);
	std::vector<std::vector<cell>> constructLowRateTrellis_ZT(std::vector<float> receivedMessage);

	// Punctured
  std::vector<std::vector<cell>> constructLowRateTrellis_Punctured(std::vector<float> receivedMessage, std::vector<int> punctured_indices);
	std::vector<std::vector<cell>> constructLowRateTrellis_Punctured_ProductMetric(std::vector<float> receivedMessage, std::vector<int> punctured_indices);
	

	/* - Fixed Point - */
	std::vector<std::vector<cell_fixedp>> constructLowRateTrellis_Punctured_ProductMetric_fixedp(llrvec receivedMessage, std::vector<int> punctured_indices);
};


#endif
