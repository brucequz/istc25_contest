#include "lowRateListDecoder.h"

std::vector<std::vector<LowRateListDecoder::cell_fixedp>> LowRateListDecoder::constructLowRateTrellis_Punctured_ProductMetric_fixedp(llrvec receivedMessage, std::vector<int> punctured_indices){
	/* Constructs a trellis for a low rate code, with puncturing
		Args:
			receivedMessage (std::vector<float>): the received message
			punctured_indices (std::vector<int>): the indices of the punctured bits

		Returns:
			std::vector<std::vector<cell_fixedp>>: the trellis
	*/

	/* ---- Code Begins ---- */
	std::vector<std::vector<cell_fixedp>> trellisInfo;
	lowrate_pathLength = (receivedMessage.size() / lowrate_symbolLength) + 1;

	trellisInfo = std::vector<std::vector<cell_fixedp>>(lowrate_numStates, std::vector<cell_fixedp>(lowrate_pathLength));

	// initializes all the valid starting states
	for(int i = 0; i < lowrate_numStates; i++){
		trellisInfo[i][0].pathMetric = 0;
		trellisInfo[i][0].init = true;
	}
	
	// building the trellis
	for(int stage = 0; stage < lowrate_pathLength - 1; stage++){
    if (stage == 1) break;
		for(int currentState = 0; currentState < lowrate_numStates; currentState++){
      if (currentState == 1) break;
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
				
				int64_t branchMetric = 0;
				std::vector<int> output_point = crc::get_point(lowrate_outputs[currentState][forwardPathIndex], lowrate_symbolLength);
				
        std::cout << std::endl;
				for(int i = 0; i < lowrate_symbolLength; i++) {
					if (std::find(punctured_indices.begin(), punctured_indices.end(), lowrate_symbolLength * stage + i) != punctured_indices.end()){
						branchMetric += 0;
					} else {
						branchMetric += -(receivedMessage[2 * stage + i] * fp16_16(output_point[i]));
            std::cout << "received message 2 * stage + i: " << receivedMessage[2 * stage + i] << "; Output point: " << output_point[i];
    
            std::cout << "; branch metric: " << branchMetric << ", ";
					}
				}

				
				fp16_16 totalPathMetric = branchMetric + trellisInfo[currentState][stage].pathMetric;
				
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

MessageInformation LowRateListDecoder::lowRateDecoding_MaxAngle_ProductMetric_fixedp(llrvec receivedMessage, std::vector<int> punctured_indices){
	std::vector<std::vector<cell_fixedp>> trellisInfo;

  // std::cout << std::endl;
  // std::cout << "priting received message: ";
  // for (size_t i_rv = 0; i_rv < receivedMessage.size(); i_rv++) {
  //   std::cout << receivedMessage[i_rv] << ", ";
  // }
	trellisInfo = constructLowRateTrellis_Punctured_ProductMetric_fixedp(receivedMessage, punctured_indices);

  int trellis_height = trellisInfo.size();
  int trellis_width  = trellisInfo[0].size();
  // std::cout << "fixed point solution: ";
  // for (size_t i_cell = 0; i_cell < trellis_height; i_cell++) {
  //   std::cout << trellisInfo[i_cell][trellis_width-1].pathMetric << ", ";
  // }

  
  std::cout << std::endl;
	// // start search
	MessageInformation output;
	// //RBTree detourTree;
	// MinHeap detourTree;
	// std::vector<std::vector<int>> previousPaths;
	

	// // create nodes for each valid ending state with no detours
	// for(int i = 0; i < lowrate_numStates; i++){
	// 	DetourObject detour;
	// 	detour.startingState = i;
	// 	detour.pathMetric = trellisInfo[i][lowrate_pathLength - 1].pathMetric;
	// 	detourTree.insert(detour);
	// }

	// int numPathsSearched = 0;
	// int TBPathsSearched = 0;
	// float currentAngleExplored = 0.0;
	
	// while(currentAngleExplored < MAX_ANGLE){
	// 	DetourObject detour = detourTree.pop();
	// 	std::vector<int> path(lowrate_pathLength);

	// 	int newTracebackStage = lowrate_pathLength - 1;
	// 	float forwardPartialPathMetric = 0;
	// 	int currentState = detour.startingState;

	// 	// if we are taking a detour from a previous path, we skip backwards to the point where we take the
	// 	// detour from the previous path
	// 	if(detour.originalPathIndex != -1){
	// 		forwardPartialPathMetric = detour.forwardPathMetric;
	// 		newTracebackStage = detour.detourStage;

	// 		// while we only need to copy the path from the detour to the end, this simplifies things,
	// 		// and we'll write over the earlier data in any case
	// 		path = previousPaths[detour.originalPathIndex];
	// 		currentState = path[newTracebackStage];

	// 		float suboptimalPathMetric = trellisInfo[currentState][newTracebackStage].suboptimalPathMetric;

	// 		currentState = trellisInfo[currentState][newTracebackStage].suboptimalFatherState;
	// 		newTracebackStage--;
			
	// 		float prevPathMetric = trellisInfo[currentState][newTracebackStage].pathMetric;

	// 		forwardPartialPathMetric += suboptimalPathMetric - prevPathMetric;
			
	// 	}
	// 	path[newTracebackStage] = currentState;

	// 	// actually tracing back
	// 	for(int stage = newTracebackStage; stage > 0; stage--){
	// 		float suboptimalPathMetric = trellisInfo[currentState][stage].suboptimalPathMetric;
	// 		float currPathMetric = trellisInfo[currentState][stage].pathMetric;

	// 		// if there is a detour we add to the detourTree
	// 		if(trellisInfo[currentState][stage].suboptimalFatherState != -1){
	// 			DetourObject localDetour;
	// 			localDetour.detourStage = stage;
	// 			localDetour.originalPathIndex = numPathsSearched;
	// 			localDetour.pathMetric = suboptimalPathMetric + forwardPartialPathMetric;
	// 			localDetour.forwardPathMetric = forwardPartialPathMetric;
	// 			localDetour.startingState = detour.startingState;
	// 			detourTree.insert(localDetour);
	// 		}
	// 		currentState = trellisInfo[currentState][stage].optimalFatherState;
	// 		float prevPathMetric = trellisInfo[currentState][stage - 1].pathMetric;
	// 		forwardPartialPathMetric += currPathMetric - prevPathMetric;
	// 		path[stage - 1] = currentState;
	// 	} // for(int stage = newTracebackStage; stage > 0; stage--)
		
	// 	previousPaths.push_back(path);

	// 	std::vector<int> message = pathToMessage(path);
	// 	std::vector<int> codeword = pathToCodeword(path);
	// 	// std::cout << "printing codeword: " << std::endl;
	// 	// utils::print_int_vector(codeword);
	// 	currentAngleExplored = utils::compute_angle_between_vectors_rad(receivedMessage, codeword);
	// 	// std::cout << "angle explored: " << currentAngleExplored << std::endl;
		
	// 	// one trellis decoding requires both a tb and crc check
	// 	if(path[0] == path[lowrate_pathLength - 1] && crc::crc_check(message, crcDegree, crc) && currentAngleExplored <= MAX_ANGLE){
	// 		output.message = message;
	// 		output.path = path;
	// 		output.listSize = numPathsSearched + 1;
	// 		output.metric = forwardPartialPathMetric;
	// 		output.TBListSize = TBPathsSearched + 1;
	// 		output.angle_received_decoded_rad = currentAngleExplored;
	// 		return output;
	// 	}

	// 	numPathsSearched++;
	// 	if(path[0] == path[lowrate_pathLength - 1])
	// 		TBPathsSearched++;
	// } // while(currentAngleExplored < MAX_ANGLE)

	// output.listSizeExceeded = true;
	// output.listSize = numPathsSearched;
	// // std::cerr << "[WARNING]: TC IS NOT FOUND!!! " << std::endl;
	return output;
}