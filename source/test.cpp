#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <chrono>
#include <algorithm>
#include "tbcc.h"
#include "fixed_point.h"

using fp16_16 = fixed<int32_t, int64_t, 16>;
using fp8_8 = fixed<int16_t, int32_t, 8>;

// Function declarations for tests
// void test_no_error(tbcc &code, int verbose = 0);
// void test_single_error(tbcc &code, float llr_mag, int verbose = 0);
// int test_gaussian_noise(tbcc &code, float esno, int verbose = 0);
// void test_tbcc_encoder(tbcc &code, int verbose = 0);

// void test_no_error(tbcc &code, int verbose) {
//     intvec info(N, 0); // Initialize info bits to zero
//     intvec cw(N);
//     fltvec llr(N);
//     intvec cw_est(N, 0);
//     fltvec llr_est(N);

//     if (verbose) {
//         std::cout << "Running Test No Error..." << std::endl;
//         std::cout << "Initial info bits: ";
//         for (const auto &bit : info) std::cout << bit << " ";
//         std::cout << std::endl;
//         std::cout << "Encoding info bits..." << std::endl;
//     }

//     // Encode
//     code.encode(info, cw);
//     for (size_t i = 0; i < cw.size(); ++i) {
//        llr[i] = (cw[i] == 0 ? 1.0f : -1.0f);
//     }

//     if (verbose) {
//         std::cout << "Encoded codeword: ";
//         for (const auto &bit : cw) std::cout << bit << " ";
//         std::cout << std::endl;
//     }

//     // Decode
//     MessageInformation result = code.decode(llr, 0.87, PUNCTURING_INDICES, 1);


//     if (result == 1) {
//        std::cout << "Test No Error: Passed" << std::endl;
//     } else {
//         std::cout << "Test No Error: Failed" << std::endl;
//     }
// }

// void test_single_error(tbcc &code, float llr_mag, int verbose) {
//     intvec info(code.n_cols, 0); // Initialize info bits to zero
//     intvec cw(code.n_cols);
//     fltvec llr(code.n_cols);
//     fltvec llr_out(code.n_cols);
//     intvec cw_est(code.n_cols);

//     code.encode(info, cw);

//     if (verbose) {
//         std::cout << "Running Test Single Error..." << std::endl;
//         std::cout << "Initial info bits: ";
//         for (const auto &bit : info) std::cout << bit << " ";
//         std::cout << std::endl;

//         std::cout << "Encoded codeword: ";
//         for (const auto &bit : cw) std::cout << bit << " ";
//         std::cout << std::endl;

//         std::fill(llr.begin(), llr.end(), llr_mag);
//         llr[0] =  -llr[0];
//         std::cout << "Decoding..." << std::endl;
//     }

//     int result = code.decode(llr, 20, llr_out, verbose);

//     if (verbose) {
//         std::cout << "LLR output from decoder: ";
//         for (const auto &llr_value : llr_out) std::cout << llr_value << " ";
//         std::cout << std::endl;
//     }

//     if (result == 1) {
//          std::cout << "Test Single Error: Passed" << std::endl;
//     } else {
//          std::cout << "Test Single Error: Failed" << std::endl;
//     }
// }

// int test_gaussian_noise(tbcc &code, float esno, int verbose) {
//     // Setup
//     intvec info(N, 0); // Initialize info bits to zero
//     intvec cw;
//     fltvec llr(N);
//     fltvec llr_out(N, 0.0f);
//     intvec cw_est(N);

//     // Setup binary RNG and encoding
//     std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
//     std::uniform_int_distribution<int> rand_bit(0, 1);
//     code.encode(info, cw);
//     std::cout << "cw size" << cw.size() << std::endl;
 
//     // Generate LLRs
//     std::normal_distribution<float> distribution(4*esno, std::sqrt(8*esno));
//     for (size_t i = 0; i < cw.size(); ++i) {
//         llr[i] = (cw[i] == 0 ? 1.0f : -1.0f) * distribution(generator);
//     }

//     if (verbose) {
//         std::cout << "Running Test Gaussian Noise..." << std::endl;
//         std::cout << "Initial info bits: ";
//         for (const auto &bit : info) std::cout << bit << " ";
//         std::cout << std::endl;

//         std::cout << "Encoded codeword: ";
//         for (const auto &bit : cw) std::cout << bit << " ";
//         std::cout << std::endl;

//         std::cout << "LLRs with Gaussian noise: ";
//         for (const auto &value : llr) std::cout << value << " ";
//         std::cout << std::endl;
//     }

//     int result = code.decode(llr, 0.87, PUNCTURING_INDICES, verbose);

//     if (verbose) {
//         std::cout << "LLR output from decoder: ";
//         for (const auto &value : llr_out) {
//             if (value <= 0.0f) {
//                 std::cout << "XXX" << value << " XXX ";
//             } else
//                 std::cout << value << " ";
//         }
//         std::cout << std::endl;
 
//         if (result == 1) {
//             std::cout << "Test Gaussian Noise: Passed" << std::endl;
//         } else {
//             std::cout << "Test Gaussian Noise: Failed" << std::endl;
//         }
//     }

//     return result;
//     // return 0;
// }


// // Test tbcc::create_encoder and tbcc::encode
// void test_tbcc_encode(tbcc &code, int verbose) {

//     // Call tbcc::create_encoder to create the parity generator
//     code.create_encoder(verbose);


//     // Setup binary RNG
//     std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
//     std::uniform_int_distribution<int> distribution(0, 1);

//     // Generate random information bit string and call tbcc::encode
//     intvec info(K);
//     std::generate(info.begin(), info.end(), [&]() { return distribution(generator); });
//     intvec cw(N);
//     code.encode(info, cw);
// }

using fp16_16 = fixed<int32_t, int64_t, 16>;
using fp8_8 = fixed<int16_t, int32_t, 8>;


int main(int argc, char* argv[])
{
    
    constexpr fp16_16 a(3.83494);
    constexpr fp16_16 b(-1);

    std::cout << "a: " << a.value << std::endl;

    constexpr double z = double(a * b);

    
    return 0;
}

