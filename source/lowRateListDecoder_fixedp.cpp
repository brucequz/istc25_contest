#include <algorithm>
#include "lowRateListDecoder.h"


Fixed_p_MessageInformation LowRateListDecoder::decode(llrvec receivedMessage, intvec punctured_indices) {
	/** Decode according to a policy passed into the constructor
	 * 
	 */
	if (STOPPING_RULE == 'A') {
    return lowRateDecoding_MaxAngle_ProductMetric_TB_fixedp(receivedMessage, punctured_indices);
  }
	throw std::invalid_argument("INVALID DECODING CHOICE!");
}

Fixed_p_MessageInformation LowRateListDecoder::lowRateDecoding_MaxAngle_ProductMetric_TB_fixedp(llrvec receivedMessage, intvec punctured_indices) {
	std::vector<std::vector<fixedp_cell>> trellisInfo;
	trellisInfo = constructLowRateTrellis_Punctured_ProductMetric_fixedp(receivedMessage, punctured_indices);

	// std::cout << "printing the first row of fixedp pathMetric." << std::endl;
	// for (size_t i = 0; i < trellisInfo.size(); i++) {
	// 	std::cout << trellisInfo[i].back().pathMetric << ", ";
	// }
	// std::cout << std::endl;
	// std::cout << "trellisInfo shape: " << trellisInfo.size() << ", " << trellisInfo[0].size() << std::endl;

	Fixed_p_MessageInformation output;
	MinHeap_fixedp detourTree;
	std::vector<std::vector<int>> previousPaths;

	// create nodes for each valid ending state with no detours
	for(int i = 0; i < lowrate_numStates; i++){
		DetourObject_fixedp detour;
		detour.startingState = i;
		detour.pathMetric = trellisInfo[i][lowrate_pathLength - 1].pathMetric;
		detourTree.insert(detour);
	}

	int numPathsSearched = 0;
	int TBPathsSearched = 0;
	fixedp_type currentAngleExplored{0};

	while(currentAngleExplored < fixedp_type{MAX_ANGLE}){
		DetourObject_fixedp detour = detourTree.pop();
		// std::cout << "floatp detour tree item: " << detour.pathMetric << std::endl;
		std::vector<int> path(lowrate_pathLength);

		int newTracebackStage = lowrate_pathLength - 1;
		fixedp_type forwardPartialPathMetric{0};
		int currentState = detour.startingState;

		// if we are taking a detour from a previous path, we skip backwards to the point where we take the
		// detour from the previous path
		if(detour.originalPathIndex != -1){
			forwardPartialPathMetric = detour.forwardPathMetric;
			newTracebackStage = detour.detourStage;

			// while we only need to copy the path from the detour to the end, this simplifies things,
			// and we'll write over the earlier data in any case
			path = previousPaths[detour.originalPathIndex];
			currentState = path[newTracebackStage];

			fixedp_type suboptimalPathMetric = trellisInfo[currentState][newTracebackStage].suboptimalPathMetric;

			currentState = trellisInfo[currentState][newTracebackStage].suboptimalFatherState;
			newTracebackStage--;
			
			fixedp_type prevPathMetric = trellisInfo[currentState][newTracebackStage].pathMetric;

			forwardPartialPathMetric += suboptimalPathMetric - prevPathMetric;
			
		}
		path[newTracebackStage] = currentState;

		// actually tracing back
		for(int stage = newTracebackStage; stage > 0; stage--){
			fixedp_type suboptimalPathMetric = trellisInfo[currentState][stage].suboptimalPathMetric;
			fixedp_type currPathMetric = trellisInfo[currentState][stage].pathMetric;

			// if there is a detour we add to the detourTree
			if(trellisInfo[currentState][stage].suboptimalFatherState != -1){
				DetourObject_fixedp localDetour;
				localDetour.detourStage = stage;
				localDetour.originalPathIndex = numPathsSearched;
				localDetour.pathMetric = suboptimalPathMetric + forwardPartialPathMetric;
				localDetour.forwardPathMetric = forwardPartialPathMetric;
				localDetour.startingState = detour.startingState;
				detourTree.insert(localDetour);
			}
			currentState = trellisInfo[currentState][stage].optimalFatherState;
			fixedp_type prevPathMetric = trellisInfo[currentState][stage - 1].pathMetric;
			forwardPartialPathMetric += currPathMetric - prevPathMetric;
			path[stage - 1] = currentState;
		} // for(int stage = newTracebackStage; stage > 0; stage--)
		
		previousPaths.push_back(path);

		std::vector<int> message = pathToMessage(path);
		std::vector<int> codeword = pathToCodeword(path);

		// std::cout << "fixedp forward partial path metric: " << forwardPartialPathMetric << std::endl;
		// std::cout << "printing codeword: " << std::endl;
		// utils::print_int_vector(codeword);

		// std::cout << "printing -forwardPartialPathMetric/N: " << -forwardPartialPathMetric/N << std::endl;
		currentAngleExplored = fpm::acos( std::max(fixedp_type{-1}, std::min(fixedp_type{1}, -forwardPartialPathMetric/fixedp_type{N})) );
		// std::cout << "fixed p current Angle: " << currentAngleExplored << std::endl;
		// std::cout << "max angle: " << MAX_ANGLE << std::endl;

		// one trellis decoding requires both a tb and crc check
		if(path[0] == path[lowrate_pathLength - 1] && crc::crc_check(message, crcDegree, crc) && currentAngleExplored <= fixedp_type{MAX_ANGLE}){
			output.message = message;
			output.path = path;
			output.listSize = numPathsSearched + 1;
			output.metric = forwardPartialPathMetric;
			output.TBListSize = TBPathsSearched + 1;
			output.angle_received_decoded_rad = currentAngleExplored;
			// std::cout << "list size: " << numPathsSearched << ", fixed p returning angle decoded = " << currentAngleExplored << ", metric = " << forwardPartialPathMetric << std::endl;
			return output;
		}

		numPathsSearched++;
		if(path[0] == path[lowrate_pathLength - 1])
			TBPathsSearched++;
	} // while(currentAngleExplored < MAX_ANGLE)

	return output;
}

std::vector<std::vector<LowRateListDecoder::fixedp_cell>> LowRateListDecoder::constructLowRateTrellis_Punctured_ProductMetric_fixedp(llrvec receivedMessage, intvec punctured_indices){
	/* Constructs a trellis for a low rate code, with puncturing
		Args:
			receivedMessage (std::vector<float>): the received message
			punctured_indices (std::vector<int>): the indices of the punctured bits

		Returns:
			std::vector<std::vector<cell>>: the trellis
	*/

	/* ---- Code Begins ---- */
	std::vector<std::vector<fixedp_cell>> trellisInfo;
	lowrate_pathLength = (receivedMessage.size() / lowrate_symbolLength) + 1;

	trellisInfo = std::vector<std::vector<fixedp_cell>>(lowrate_numStates, std::vector<fixedp_cell>(lowrate_pathLength));

	// initializes all the valid starting states
	for(int i = 0; i < lowrate_numStates; i++){
		trellisInfo[i][0].pathMetric = fixedp_type{0};
		trellisInfo[i][0].init = true;
	}
	
	// building the trellis
	for(int stage = 0; stage < lowrate_pathLength - 1; stage++){
		for(int currentState = 0; currentState < lowrate_numStates; currentState++){
			// if the state / stage is invalid, we move on
			if(!trellisInfo[currentState][stage].init)
				continue;

			// otherwise, we compute the relevent information
			for(int forwardPathIndex = 0; forwardPathIndex < numForwardPaths; forwardPathIndex++){
				// since our transitions correspond to symbols, the forwardPathIndex has no correlation 
				// beyond indexing the forward path

				int nextState = lowrate_nextStates[currentState][forwardPathIndex];
				
				// if the nextState is invalid, we move on
				if(nextState < 0)
					continue;
				
				fixedp_type branchMetric{0};
				std::vector<int> output_point = crc::get_point(lowrate_outputs[currentState][forwardPathIndex], lowrate_symbolLength);
				
				for(int i = 0; i < lowrate_symbolLength; i++){
					if (std::find(punctured_indices.begin(), punctured_indices.end(), lowrate_symbolLength * stage + i) != punctured_indices.end()){
						branchMetric += 0;
					} else {
						branchMetric += -receivedMessage[lowrate_symbolLength* stage + i] * fixedp_type{output_point[i]};
					}
				}
				
				fixedp_type totalPathMetric = branchMetric + trellisInfo[currentState][stage].pathMetric;
				
				// dealing with cases of uninitialized states, when the transition becomes the optimal father state, and suboptimal father state, in order
				if(!trellisInfo[nextState][stage + 1].init){
					trellisInfo[nextState][stage + 1].pathMetric = totalPathMetric;
					trellisInfo[nextState][stage + 1].optimalFatherState = currentState;
					trellisInfo[nextState][stage + 1].init = true;
				}
				else if(trellisInfo[nextState][stage + 1].pathMetric > totalPathMetric){
					trellisInfo[nextState][stage + 1].suboptimalPathMetric = trellisInfo[nextState][stage + 1].pathMetric;
					trellisInfo[nextState][stage + 1].suboptimalFatherState = trellisInfo[nextState][stage + 1].optimalFatherState;
					trellisInfo[nextState][stage + 1].pathMetric = totalPathMetric;
					trellisInfo[nextState][stage + 1].optimalFatherState = currentState;
				}
				else{
					trellisInfo[nextState][stage + 1].suboptimalPathMetric = totalPathMetric;
					trellisInfo[nextState][stage + 1].suboptimalFatherState = currentState;
				}
			}
		}
	}
	return trellisInfo;
}
