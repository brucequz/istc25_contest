#ifndef MLA_TYPES_H
#define MLA_TYPES_H

#include <vector>
#include "../fpm/fixed.hpp"
#include "enc_dec.h"

struct CodeInformation {
  int k;              // numerator of the rate
  int n;              // denominator of the rate
  int v;              // number of memory elements
  int crcDeg;         // m+1, degree of CRC, # bits of CRC polynomial
  int crc;            // CRC polynomial
  int numInfoBits;    // number of information bits
  
  std::vector<int> numerators; // optimal code numerators
};

struct MessageInformation{
	MessageInformation() {
		message 					= std::vector<int>();
		path 							= std::vector<int>();
		listSize 					= -1;
    TBListSize        = -1;
		listSizeExceeded 	= false;
		metric 						= -1.0;
		angle_received_decoded_rad = 0.0;

		// History
		pathToTransmittedCodewordHistory = std::vector<double>();
		decodedCodewordSquaredNoiseMag 	= std::vector<double>();
	};
	std::vector<int> message;
	std::vector<int> path;
	int listSize;
  int TBListSize;
	bool listSizeExceeded;
	double metric;
	double angle_received_decoded_rad;
	std::vector<double> pathToTransmittedCodewordHistory;
	std::vector<double> decodedCodewordSquaredNoiseMag;
};

struct Fixed_p_MessageInformation{
	Fixed_p_MessageInformation() {
		message 					= std::vector<int>();
		path 							= std::vector<int>();
		listSize 					= -1;
    TBListSize        = -1;
		listSizeExceeded 	= false;
		metric 						= fixedp_type{-1};
		angle_received_decoded_rad = fixedp_type{-1};
	};
	std::vector<int> message;
	std::vector<int> path;
	int listSize;
  int TBListSize;
	bool listSizeExceeded;
	fixedp_type metric;
	fixedp_type angle_received_decoded_rad;
};

#endif