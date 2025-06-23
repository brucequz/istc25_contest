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
void tbcc::create_encoder(int verbose) {}


// BALD decoding
MessageInformation tbcc::decode(fltvec &llr_in, intvec punctured_indices, int verbose = 0) {
    // first decode using product metric
    if (ENCODING_RULE == 'T') {
        return decoder.lowRateDecoding_MaxAngle_ProductMetric(llr_in, punctured_indices);
    }

    return decoder.lowRateDecoding_MaxAngle_ProductMetric_ZT(llr_in);
    // returns the correct result we get from squared distance metric
    // return decoder.decode(llr_in, punctured_indices);
}
    


// Encode info bitvec into codeword bitvec
void tbcc::encode(intvec &info, intvec &cw) {
    if (ENCODING_RULE == 'T') {
        cw = trellis.encode(info);
    } else if (ENCODING_RULE == 'Z') {
        intvec info_zt = info;
        for (int i=0; i<V; i++){
            info_zt.push_back(0);
        }
        cw = trellis.encode_zt(info_zt);
    }
}
