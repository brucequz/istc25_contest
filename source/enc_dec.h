#ifndef ENC_DEC_H
#define ENC_DEC_H

#include <vector>
#include "../fpm/fixed.hpp"
#include "../fpm/math.hpp"
#include "consts.h"

// Two types: bit and llr
using fixedp_type = fpm::fixed<std::int32_t, std::int64_t, FIXED_POINT_FRACTIONAL_LENGTH>;
typedef int bit_type;
using bitvec = std::vector<bit_type>;
using llrvec = std::vector<fixedp_type>;
using fltvec = std::vector<float>;
using intvec = std::vector<int>;

class enc_dec
{
  public:
    // Setup for [n,k] code
    int init(int k, int n, bool opt_avg_latency); 

    // float LLR to integer representation
    fixedp_type llr2int(float float_llr);

    // Encode k info bits into n codeword bits
    void encode(bitvec &info, bitvec &cw);

    // Decode n llrs into n codeword bits and k info bits, return -1 if detected error
    int decode(llrvec &llr, bitvec &cw_est, bitvec &info_est, float esno);

    int decode_floatp(fltvec &llr_flt, bitvec &cw_est, bitvec &info_est, float esno);
};

#endif // ENC_DEC_H

