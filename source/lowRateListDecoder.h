
#ifndef LOWRATELISTDECODER_H
#define LOWRATELISTDECODER_H

#include <climits>
#include <iostream>

#include "FeedForwardTrellis.h"
#include "minHeap.h"
#include "tbcc_types.h"
#include "tbcc_namespace.h"
#include "consts.h"

class LowRateListDecoder{
public:
	LowRateListDecoder(FeedForwardTrellis FT, int listSize, int crcDegree, int crc, char stopping_rule);
	MessageInformation lowRateDecoding_MaxListsize(std::vector<float> receivedMessage, std::vector<int> punctured_indices);
	MessageInformation lowRateDecoding_MaxMetric(std::vector<float> receivedMessage, std::vector<int> punctured_indices);
	MessageInformation lowRateDecoding_MaxAngle(std::vector<float> receivedMessage, std::vector<int> punctured_indices);
	MessageInformation lowRateDecoding_MaxAngle_ProductMetric(std::vector<float> receivedMessage, std::vector<int> punctured_indices);
	MessageInformation lowRateDecoding_MaxAngle_ProductMetric_fixedp(llrvec receivedMessage, std::vector<int> punctured_indices);

	MessageInformation decode(std::vector<float> receivedMessage, std::vector<int> punctured_indices);

	/* - MLA - */
	MessageInformation lowRateDecoding_mla(std::vector<float> receivedMessage, std::vector<int> punctured_indices, std::vector<int> transmittedMessage);

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
		fp16_16 pathMetric = fp16_16((1 << 15) - 1);
		fp16_16 suboptimalPathMetric = fp16_16((1 << 15) - 1);
		bool init = false;
	};

  std::vector<int> pathToMessage(std::vector<int>); 
  std::vector<int> pathToCodeword(std::vector<int>); 
	std::vector<std::vector<cell>> constructLowRateTrellis(std::vector<float> receivedMessage);
  std::vector<std::vector<cell>> constructLowRateTrellis_Punctured(std::vector<float> receivedMessage, std::vector<int> punctured_indices);
	std::vector<std::vector<cell>> constructLowRateTrellis_Punctured_ProductMetric(std::vector<float> receivedMessage, std::vector<int> punctured_indices);
	std::vector<std::vector<cell_fixedp>> constructLowRateTrellis_Punctured_ProductMetric_fixedp(llrvec receivedMessage, std::vector<int> punctured_indices);
	std::vector<std::vector<std::vector<cell>>> constructLowRateMultiTrellis(std::vector<float> receivedMessage);
	std::vector<std::vector<cell>> constructMinimumLikelihoodLowRateTrellis(std::vector<float> receivedMessage);
};


#endif
