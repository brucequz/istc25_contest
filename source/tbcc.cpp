#include <cmath>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <set>
#include <vector>
#include <random>
#include <fstream>
#include "tbcc.h"


// Generate tbcc encoder
void tbcc::create_encoder(int verbose) {
    // std::cout << "K = " << K << std::endl;
}


// BALD decoding
MessageInformation tbcc::decode(fltvec &llr_in, double theta_t, intvec punctured_indices, int verbose = 0) {
    // first decode using product metric
    return decoder.lowRateDecoding_MaxAngle_ProductMetric(llr_in, punctured_indices);
    // returns the correct result we get from squared distance metric
    // return decoder.decode(llr_in, punctured_indices);
}

MessageInformation_fixedp tbcc::decode_fixedp(llrvec &llr_in, double theta_t, intvec punctured_indices, int verbose) {
    // first decode using product metric
    return decoder.lowRateDecoding_MaxListsize_fixedp(llr_in, punctured_indices);
}
    


// Encode info bitvec into codeword bitvec
void tbcc::encode(intvec &info, intvec &cw) {
    cw = trellis.encode(info);
}

