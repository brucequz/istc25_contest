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

Fixed_p_MessageInformation tbcc::fixedp_decode(llrvec &llr_in, intvec punctured_indices) {
    // maximum angle decoding
    if (ENCODING_RULE == 'T' && DECODING_RULE == 'P' && STOPPING_RULE == 'A') {
        return decoder.lowRateDecoding_MaxAngle_ProductMetric_TB_fixedp(llr_in, punctured_indices);
    }
}
    


// Encode info bitvec into codeword bitvec
void tbcc::encode(intvec &info, intvec &cw) {
    if (ENCODING_RULE == 'T') {
        int crc_length = M + 1;
        intvec info_crc = info;
        crc::crc_calculation(info_crc, crc_length, CRC);
        cw = trellis.encode(info_crc);
    } else if (ENCODING_RULE == 'Z') {
        intvec info_zt = info;
        for (int i=0; i<V; i++){
            info_zt.push_back(0);
        }
        cw = trellis.encode_zt(info_zt);
    }
    // convert (1,-1) back to (0,1)
    for (size_t i = 0; i < cw.size(); i++) {cw[i]=(1-cw[i])/2;}
}
