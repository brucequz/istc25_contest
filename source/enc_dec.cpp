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

fixedp_type enc_dec::llr2int(float float_llr) {
    return fixedp_type(float_llr);
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
    fixedp_type esno_linear{esno};
    
    // unpuncture and convert llr to raw output value
    llrvec unpunctured_symbols(N + PUNCTURING_INDICES.size());
    auto llr_idx = 0;
    for (int i = 0; i < unpunctured_symbols.size(); i++) {
        if (find(PUNCTURING_INDICES.begin(), PUNCTURING_INDICES.end(), i) == PUNCTURING_INDICES.end()) {
            // this position is not punctured
            unpunctured_symbols[i] = llr[llr_idx] / (4 * esno_linear);
            llr_idx++;
        } else {
            unpunctured_symbols[i] = fixedp_type(0.0); // punctured positions are set to 0
        }
    }

    /// projecting onto the codeword sphere
    // 1. compute the energy of the received word
    fixedp_type received_word_energy{0};
    for (size_t i = 0; i < unpunctured_symbols.size(); i++) {
        received_word_energy += unpunctured_symbols[i] * unpunctured_symbols[i];
    }
    // 2. compute the energy normalize factor
    fixedp_type fixed_p_N{N};
    fixedp_type energy_normalize_factor = fpm::sqrt(fixed_p_N / received_word_energy);

    // 3. project the received word onto the codeword sphere
    llrvec projected_received_word(N + PUNCTURING_INDICES.size(), fixedp_type(0.0));
    for (size_t i = 0; i < projected_received_word.size(); i++) {
        projected_received_word[i] = unpunctured_symbols[i] * energy_normalize_factor;
    }
    // std::cout << "printing projected received word: " << std::endl;
    // utils::print_fixed_vector(projected_received_word);

    /// decoding
    Fixed_p_MessageInformation mi_result;
    mi_result = code.fixedp_decode(projected_received_word, PUNCTURING_INDICES);
    info_est = mi_result.message;
    int result = 1;
    return result;
}

int enc_dec::decode_floatp(fltvec &llr_flt, bitvec &cw_est, bitvec &info_est, float esno) {

    // std::cout << "printing received vector";
    // utils::print_double_vector(llr_flt);

    
    // unpuncture
    fltvec unpunctured_symbols(N + PUNCTURING_INDICES.size());
    auto llr_idx = llr_flt.begin();
    for (int i = 0; i < unpunctured_symbols.size(); i++) {
        if (find(PUNCTURING_INDICES.begin(), PUNCTURING_INDICES.end(), i) == PUNCTURING_INDICES.end()) {
            // this position is not punctured
            unpunctured_symbols[i] = *llr_idx;
            llr_idx++;
        } else {
            unpunctured_symbols[i] = 0.0; // punctured positions are set to 0
        }
    }

    /// projecting onto the codeword sphere
    float received_word_energy = utils::compute_vector_energy(unpunctured_symbols);
    float energy_normalize_factor = std::sqrt(N / received_word_energy);
    std::vector<float> projected_received_word(unpunctured_symbols.size(), 0.0);
    for (size_t i = 0; i < unpunctured_symbols.size(); i++) {
      projected_received_word[i] = unpunctured_symbols[i] * energy_normalize_factor;
    }

    // std::cout << "printing projected received word: " << std::endl;
    // utils::print_double_vector(projected_received_word);

    MessageInformation mi_result = code.decode(projected_received_word, PUNCTURING_INDICES, 0);
    info_est = mi_result.message;

    int result = 1;
    return result;
}

