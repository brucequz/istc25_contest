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
    // if (stage == 1) break;
		for(int currentState = 0; currentState < lowrate_numStates; currentState++){
      // if (currentState == 1) break;
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
				
        // std::cout << std::endl;
				for(int i = 0; i < lowrate_symbolLength; i++) {
					if (std::find(punctured_indices.begin(), punctured_indices.end(), lowrate_symbolLength * stage + i) != punctured_indices.end()){
						branchMetric += 0;
					} else {
						branchMetric += -(receivedMessage[2 * stage + i] * fp16_16(output_point[i]));
            // std::cout << "[DEBUG] received message 2 * stage + i: " << receivedMessage[2 * stage + i] << "; Output point: " << output_point[i];
            // std::cout << "; branch metric: " << branchMetric << ", ";
					}
				}

				
				int64_t totalPathMetric = branchMetric + trellisInfo[currentState][stage].pathMetric;
        
				
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

  // std::cout << "[DEBUG] fixedp: " << trellisInfo[lowrate_numStates-1][lowrate_pathLength-2].pathMetric << std::endl;
	return trellisInfo;
}

// MessageInformation_fixedp LowRateListDecoder::lowRateDecoding_MaxListsize_fixedp(llrvec receivedMessage, std::vector<int> punctured_indices){
// 	std::vector<std::vector<cell_fixedp>> trellisInfo;

//   // std::cout << std::endl;
//   // std::cout << "priting received message: ";
//   // for (size_t i_rv = 0; i_rv < receivedMessage.size(); i_rv++) {
//   //   std::cout << receivedMessage[i_rv] << ", ";
//   // }
// 	trellisInfo = constructLowRateTrellis_Punctured_ProductMetric_fixedp(receivedMessage, punctured_indices);

//   // int trellis_height = trellisInfo.size();
//   // int trellis_width  = trellisInfo[0].size();
//   // std::cout << "fixed point solution: ";
//   // for (size_t i_cell = 0; i_cell < trellis_height; i_cell++) {
//   //   std::cout << (double(trellisInfo[i_cell][trellis_width-2].pathMetric) / (1 << 16)) << ", ";
//   // }
//   // std::cout << std::endl;

//   // // projecting onto the codeword sphere
//   // int32_t received_word_energy = 0;
//   // // sum of squares to compute vector energy
//   // for (size_t i = 0; i < receivedMessage.size(); i++) {
//   //     int64_t squares = (receivedMessage[i] * receivedMessage[i]);
//   //     received_word_energy += (squares >> 16);
//   // }
//   // std::cout << "received word energy: " << received_word_energy << std::endl;
  
//   // fp16_16 numerator = sqrt(fp16_16(128.0));
//   // std::cout << "numerator: " << numerator.value << std::endl;

  

// 	// start search
// 	MessageInformation_fixedp output;
// 	//RBTree detourTree;
// 	MinHeap detourTree;
// 	std::vector<std::vector<int>> previousPaths;
	

// 	// create nodes for each valid ending state with no detours
// 	for(int i = 0; i < lowrate_numStates; i++){
// 		DetourObject detour;
// 		detour.startingState = i;
// 		detour.pathMetric = trellisInfo[i][lowrate_pathLength - 1].pathMetric;
// 		detourTree.insert(detour);
// 	}

// 	int numPathsSearched = 0;
// 	int TBPathsSearched = 0;
	
// 	while(numPathsSearched < MAX_LISTSIZE){
// 		DetourObject detour = detourTree.pop();
//     // std::cout << "fixedp detour tree item: " << detour.pathMetric << std::endl;
// 		std::vector<int> path(lowrate_pathLength);

// 		int newTracebackStage = lowrate_pathLength - 1;
// 		int32_t forwardPartialPathMetric = 0;
// 		int currentState = detour.startingState;

// 		// if we are taking a detour from a previous path, we skip backwards to the point where we take the
// 		// detour from the previous path
// 		if(detour.originalPathIndex != -1){
// 			forwardPartialPathMetric = detour.forwardPathMetric;
// 			newTracebackStage = detour.detourStage;

// 			// while we only need to copy the path from the detour to the end, this simplifies things,
// 			// and we'll write over the earlier data in any case
// 			path = previousPaths[detour.originalPathIndex];
// 			currentState = path[newTracebackStage];

// 			int32_t suboptimalPathMetric = trellisInfo[currentState][newTracebackStage].suboptimalPathMetric;

// 			currentState = trellisInfo[currentState][newTracebackStage].suboptimalFatherState;
// 			newTracebackStage--;
			
// 			int32_t prevPathMetric = trellisInfo[currentState][newTracebackStage].pathMetric;

// 			forwardPartialPathMetric += suboptimalPathMetric - prevPathMetric;
			
// 		}
// 		path[newTracebackStage] = currentState;

// 		// actually tracing back
// 		for(int stage = newTracebackStage; stage > 0; stage--){
// 			int32_t suboptimalPathMetric = trellisInfo[currentState][stage].suboptimalPathMetric;
// 			int32_t currPathMetric = trellisInfo[currentState][stage].pathMetric;

// 			// if there is a detour we add to the detourTree
// 			if(trellisInfo[currentState][stage].suboptimalFatherState != -1){
// 				DetourObject localDetour;
// 				localDetour.detourStage = stage;
// 				localDetour.originalPathIndex = numPathsSearched;
// 				localDetour.pathMetric = suboptimalPathMetric + forwardPartialPathMetric;
// 				localDetour.forwardPathMetric = forwardPartialPathMetric;
// 				localDetour.startingState = detour.startingState;
// 				detourTree.insert(localDetour);
// 			}
// 			currentState = trellisInfo[currentState][stage].optimalFatherState;
// 			int32_t prevPathMetric = trellisInfo[currentState][stage - 1].pathMetric;
// 			forwardPartialPathMetric += currPathMetric - prevPathMetric;
// 			path[stage - 1] = currentState;
// 		} // for(int stage = newTracebackStage; stage > 0; stage--)
		
// 		previousPaths.push_back(path);

// 		std::vector<int> message = pathToMessage(path);
// 		std::vector<int> codeword = pathToCodeword(path);

//     // std::cout << "fixedp forward partial path metric: " << forwardPartialPathMetric << std::endl;
// 		// std::cout << "fixedp printing codeword: " << std::endl;
// 		// utils::print_int_vector(codeword);
//     // TODO
// 		// currentAngleExplored = forwardPartialPathMetric / 
// 		// std::cout << "angle explored: " << currentAngleExplored << std::endl;
		
// 		// one trellis decoding requires both a tb and crc check
// 		if(path[0] == path[lowrate_pathLength - 1] && crc::crc_check(message, crcDegree, crc) && numPathsSearched <= MAX_LISTSIZE){
// 			output.message = message;
// 			output.path = path;
// 			output.listSize = numPathsSearched + 1;
// 			output.metric = forwardPartialPathMetric;
// 			output.TBListSize = TBPathsSearched + 1;
//       // std::cout << "fixed point returning metric = " << forwardPartialPathMetric << std::endl;
// 			return output;
// 		}

// 		numPathsSearched++;
// 		if(path[0] == path[lowrate_pathLength - 1])
// 			TBPathsSearched++;
// 	} // while(numPathsSearched < MAX_LISTSIZE)

// 	output.listSizeExceeded = true;
// 	output.listSize = numPathsSearched;
// 	// std::cerr << "[WARNING]: TC IS NOT FOUND!!! " << std::endl;
// 	return output;
// }

MessageInformation_fixedp LowRateListDecoder::lowRateDecoding_MaxListsize_fixedp(llrvec receivedMessage, std::vector<int> punctured_indices){
	std::vector<std::vector<cell_fixedp>> trellisInfo;

  // std::cout << std::endl;
  // std::cout << "priting received message: ";
  // for (size_t i_rv = 0; i_rv < receivedMessage.size(); i_rv++) {
  //   std::cout << receivedMessage[i_rv] << ", ";
  // }
	trellisInfo = constructLowRateTrellis_Punctured_ProductMetric_fixedp(receivedMessage, punctured_indices);

  // int trellis_height = trellisInfo.size();
  // int trellis_width  = trellisInfo[0].size();
  // std::cout << "fixed point solution: ";
  // for (size_t i_cell = 0; i_cell < trellis_height; i_cell++) {
  //   std::cout << trellisInfo[i_cell][trellis_width-2].pathMetric << ", ";
  // }
  // std::cout << std::endl;

  // // projecting onto the codeword sphere
  // int32_t received_word_energy = 0;
  // // sum of squares to compute vector energy
  // for (size_t i = 0; i < receivedMessage.size(); i++) {
  //     int64_t squares = (receivedMessage[i] * receivedMessage[i]);
  //     received_word_energy += (squares >> 16);
  // }
  // std::cout << "received word energy: " << received_word_energy << std::endl;
  
  // fp16_16 numerator = sqrt(fp16_16(128.0));
  // std::cout << "numerator: " << numerator.value << std::endl;

  

	// start search
	MessageInformation_fixedp output;
	//RBTree detourTree;
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
	int32_t currentAngleExplored = 0.0;
	
	while(numPathsSearched < MAX_LISTSIZE){
		DetourObject_fixedp detour = detourTree.pop();
		std::vector<int> path(lowrate_pathLength);

		int newTracebackStage = lowrate_pathLength - 1;
		int32_t forwardPartialPathMetric = 0;
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

			int32_t suboptimalPathMetric = trellisInfo[currentState][newTracebackStage].suboptimalPathMetric;

			currentState = trellisInfo[currentState][newTracebackStage].suboptimalFatherState;
			newTracebackStage--;
			
			int32_t prevPathMetric = trellisInfo[currentState][newTracebackStage].pathMetric;

			forwardPartialPathMetric += suboptimalPathMetric - prevPathMetric;
			
		}
		path[newTracebackStage] = currentState;

		// actually tracing back
		for(int stage = newTracebackStage; stage > 0; stage--){
			int32_t suboptimalPathMetric = trellisInfo[currentState][stage].suboptimalPathMetric;
			int32_t currPathMetric = trellisInfo[currentState][stage].pathMetric;

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
			int32_t prevPathMetric = trellisInfo[currentState][stage - 1].pathMetric;
			forwardPartialPathMetric += currPathMetric - prevPathMetric;
			path[stage - 1] = currentState;
		} // for(int stage = newTracebackStage; stage > 0; stage--)
		
		previousPaths.push_back(path);

		std::vector<int> message = pathToMessage(path);
		std::vector<int> codeword = pathToCodeword(path);

    
		// std::cout << "fixedp printing codeword: " << std::endl;
		// utils::print_int_vector(codeword);
    // TODO
		// currentAngleExplored = forwardPartialPathMetric / 
		// std::cout << "angle explored: " << currentAngleExplored << std::endl;
		
		// one trellis decoding requires both a tb and crc check
		if(path[0] == path[lowrate_pathLength - 1] && crc::crc_check(message, crcDegree, crc) && currentAngleExplored <= MAX_ANGLE){
			output.message = message;
			output.path = path;
			output.listSize = numPathsSearched + 1;
			output.metric = forwardPartialPathMetric;
			output.TBListSize = TBPathsSearched + 1;
			output.angle_received_decoded_rad = currentAngleExplored;
      // std::cout << "fixedp list size: " << output.listSize << std::endl;
      // std::cout << std::endl;
			return output;
		}

		numPathsSearched++;
		if(path[0] == path[lowrate_pathLength - 1])
			TBPathsSearched++;
	} // while(numPathsSearched < MAX_LISTSIZE)

	output.listSizeExceeded = true;
	output.listSize = numPathsSearched;
	std::cerr << "[WARNING]: FIXEDP TC IS NOT FOUND!!! " << std::endl;
	return output;
}