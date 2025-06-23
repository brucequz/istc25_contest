#include <iostream>
#include <filesystem>
#include <cmath>
#include "enc_dec.h"
#include "tbcc.h"

tbcc code;
int max_iter;

// Setup for [n,k] code
int enc_dec::init(int k, int n, bool opt_avg_latency) {
    // Setup encoder
    code.create_encoder();
    return 0;
}

llr_type enc_dec::llr2int(float float_llr) {
    // Contestants should replace this code
    //   This code should convert a single LLR to the integer representation used by decoder
    return std::round((32768/25.0)*float_llr);
}

// Encode k info bits into n codeword bits
void enc_dec::encode(bitvec &info, bitvec &cw) {
    // encode block
    bitvec cw_unpunctured;
    code.encode(info, cw_unpunctured);
    // puncturing
    bitvec cw_punctured = {};
    for (int i = 0; i < cw_unpunctured.size(); i++) {
        if (std::find(PUNCTURING_INDICES.begin(), PUNCTURING_INDICES.end(), i) == PUNCTURING_INDICES.end()) {
            cw_punctured.push_back(cw_unpunctured[i]);
        }
    }
    cw = cw_punctured;
}

// Decode n llrs into n codeword bits and k info bits, return -1 if detected error
int enc_dec::decode(llrvec &llr, bitvec &cw_est, bitvec &info_est, float esno) {

    // convert llr to raw output value
    double snr_linear = pow(10.0, esno / 10.0);
    // for (size_t i = 0; i < llr.size) // TODO
    
    // unpuncture
    fltvec unpunctured_symbols(N + PUNCTURING_INDICES.size());
    auto llr_ptr = llr.begin();
    for (int i = 0; i < unpunctured_symbols.size(); i++) {
        if (find(PUNCTURING_INDICES.begin(), PUNCTURING_INDICES.end(), i) == PUNCTURING_INDICES.end()) {
            // this position is not punctured
            unpunctured_symbols[i] = *llr_ptr;
            llr_ptr++;
        } else {
            unpunctured_symbols[i] = 0.0f;
        }
    }

    // projecting onto the codeword sphere
    float received_word_energy = utils::compute_vector_energy(unpunctured_symbols);
    float energy_normalize_factor = std::sqrt(N / received_word_energy);
    std::vector<float> projected_received_word(unpunctured_symbols.size(), 0.0);
    for (size_t i = 0; i < unpunctured_symbols.size(); i++) {
      projected_received_word[i] = unpunctured_symbols[i] * energy_normalize_factor;
    }

    MessageInformation mi_result = code.decode(projected_received_word, PUNCTURING_INDICES, 0);
    info_est = mi_result.message;
    int result = 1;
    return result;
}

