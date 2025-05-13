#include <iostream>
#include <cmath>
#include <algorithm>
#include "enc_dec.h"
#include "tbcc.h"

tbcc code;

// Setup for [n,k] code
int enc_dec::init(int k, int n, bool opt_avg_latency) {
    // Contestants should replace this code
    //   This code should initialize the encoder-decoder

    // Setup encoder
    code.create_encoder();

    // Decoding iterations
    return 0;
}

int64_t enc_dec::llr2int(float float_llr) {
    // Contestants should replace this code
    //   This code should convert a single LLR to the integer representation used by decoder
    return fp16_16(float_llr).value;
    // return std::round((32768/25.0)*float_llr);
}

float enc_dec::fixed2float(fp16_16 fixed_llr) {
    return float(double(fixed_llr));
}

// Encode k info bits into n codeword bits
void enc_dec::encode(bitvec &info, bitvec &cw) {

    // add crc
	crc::crc_calculation(info, M+1, CRC);
    // Encode
    bitvec cw_unpunctured(152);
    code.encode(info, cw_unpunctured);
    
    // std::cout << "printing unpunctured encoded word: ";
    // utils::print_int_vector(cw_unpunctured);
    // std::cout << std::endl;
    // puncture
    bitvec cw_punctured = {};
    for (int i = 0; i < cw_unpunctured.size(); i++) {
        if (std::find(PUNCTURING_INDICES.begin(), PUNCTURING_INDICES.end(), i) == PUNCTURING_INDICES.end()) {
            cw_punctured.push_back(cw_unpunctured[i]);
        }
    }
    cw = cw_punctured;
}

// Decode n llrs into n codeword bits and k info bits, return -1 if detected error
int enc_dec::decode(fltvec &llr, bitvec &cw_est, bitvec &info_est) {
    
    fltvec unpunctured_symbols(152);
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
    // float received_word_energy = utils::compute_vector_energy(unpunctured_symbols);
    // float energy_normalize_factor = std::sqrt(128.0 / received_word_energy);  // normalizing received message
    // std::vector<float> projected_received_word(unpunctured_symbols.size(), 0.0);
    // for (size_t i = 0; i < unpunctured_symbols.size(); i++) {
    //   projected_received_word[i] = unpunctured_symbols[i] * energy_normalize_factor;
    // }

    // std::cout << "unpuncture the bits" << std::endl;
    // utils::print_double_vector(unpunctured_symbols);
    // std::cout << std::endl;

    MessageInformation mi_result = code.decode(unpunctured_symbols, 4, PUNCTURING_INDICES, 0);
    
    info_est = mi_result.message;
    int result = 1;
    return result;
}


// Decode n llrs into n codeword bits and k info bits, return -1 if detected error
int enc_dec::decode_fixedp(llrvec&llr, bitvec &cw_est, bitvec &info_est) {
    
    llrvec unpunctured_symbols(152);
    auto llr_ptr = llr.begin();
    for (int i = 0; i < unpunctured_symbols.size(); i++) {
        if (find(PUNCTURING_INDICES.begin(), PUNCTURING_INDICES.end(), i) == PUNCTURING_INDICES.end()) {
            // this position is not punctured
            unpunctured_symbols[i] = *llr_ptr;
            llr_ptr++;
        } else {
            unpunctured_symbols[i] = 0;
        }
    }    
    // projecting onto the codeword sphere
    // float received_word_energy = utils::compute_vector_energy(unpunctured_symbols);
    // float energy_normalize_factor = std::sqrt(128.0 / received_word_energy);  // normalizing received message
    // std::vector<float> projected_received_word(unpunctured_symbols.size(), 0.0);
    // for (size_t i = 0; i < unpunctured_symbols.size(); i++) {
    //   projected_received_word[i] = unpunctured_symbols[i] * energy_normalize_factor;
    // }

    // std::cout << "unpuncture the bits" << std::endl;
    // utils::print_double_vector(unpunctured_symbols);
    // std::cout << std::endl;

    MessageInformation mi_result = code.decode_fixedp(unpunctured_symbols, 4, PUNCTURING_INDICES, 0);
    
    info_est = mi_result.message;
    int result = 1;
    return result;
}

