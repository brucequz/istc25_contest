#include <iostream>
#include <vector>
#include <random>
#include <cmath>
#include <chrono>
#include <algorithm>
#include "ldpc.h"

// Function declarations for tests
void test_no_error(ldpc &code, int verbose = 0);
void test_single_error(ldpc &code, float llr_mag, int verbose = 0);
int test_gaussian_noise(ldpc &code, float esno, int verbose = 0);
void test_alist_read_write(ldpc &code1, int verbose = 0);
void test_ldpc_encoder(ldpc &code, int verbose = 0);

void test_no_error(ldpc &code, int verbose) {
    bitvec info(code.n_cols, 0); // Initialize info bits to zero
    bitvec cw(code.n_cols);
    fltvec llr(code.n_cols);
    bitvec cw_est(code.n_cols, 0);
    fltvec llr_est(code.n_cols);

    if (verbose) {
        std::cout << "Running Test No Error..." << std::endl;
        std::cout << "Initial info bits: ";
        for (const auto &bit : info) std::cout << bit << " ";
        std::cout << std::endl;
        std::cout << "Encoding info bits..." << std::endl;
    }

    // Encode
    code.encode(info, cw);
    for (size_t i = 0; i < cw.size(); ++i) {
       llr[i] = (cw[i] == 0 ? 1.0f : -1.0f);
    }

    if (verbose) {
        std::cout << "Encoded codeword: ";
        for (const auto &bit : cw) std::cout << bit << " ";
        std::cout << std::endl;
    }

    // Decode
    int result = code.decode(llr, 20, llr_est);

    if (verbose) {
        std::cout << "Decoded LLRs: ";
        for (const auto &llr_value : llr_est) {
            if (llr_value <= 0) {
                std::cout << "XXX" << llr_value << " XXX ";
            } else
                std::cout << llr_value << " ";
        }
        std::cout << std::endl;
    }

    if (result == 1) {
       std::cout << "Test No Error: Passed" << std::endl;
    } else {
        std::cout << "Test No Error: Failed" << std::endl;
    }
}

void test_single_error(ldpc &code, float llr_mag, int verbose) {
    bitvec info(code.n_cols, 0); // Initialize info bits to zero
    bitvec cw(code.n_cols);
    fltvec llr(code.n_cols);
    fltvec llr_out(code.n_cols);
    bitvec cw_est(code.n_cols);

    code.encode(info, cw);

    if (verbose) {
        std::cout << "Running Test Single Error..." << std::endl;
        std::cout << "Initial info bits: ";
        for (const auto &bit : info) std::cout << bit << " ";
        std::cout << std::endl;

        std::cout << "Encoded codeword: ";
        for (const auto &bit : cw) std::cout << bit << " ";
        std::cout << std::endl;

        std::fill(llr.begin(), llr.end(), llr_mag);
        llr[0] =  -llr[0];
        std::cout << "Decoding..." << std::endl;
    }

    int result = code.decode(llr, 20, llr_out);

    if (verbose) {
        std::cout << "LLR output from decoder: ";
        for (const auto &llr_value : llr_out) std::cout << llr_value << " ";
        std::cout << std::endl;
    }

    if (result == 1) {
         std::cout << "Test Single Error: Passed" << std::endl;
    } else {
         std::cout << "Test Single Error: Failed" << std::endl;
    }
}

int test_gaussian_noise(ldpc &code, float esno, int verbose) {
    // Setup
    bitvec info(code.n_cols, 0); // Initialize info bits to zero
    bitvec cw(code.n_cols);
    fltvec llr(code.n_cols);
    fltvec llr_out(code.n_cols, 0.0f);
    bitvec cw_est(code.n_cols);

    // Setup binary RNG and encoding
    std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> rand_bit(0, 1);
    if (!code.parity_generator.empty()) {
        for (int i=0; i<code.n_cols - code.n_rows; ++i) info[i] = rand_bit(generator);
    }
    code.encode(info, cw);
 
    // Generate LLRs
    std::normal_distribution<float> distribution(4*esno, std::sqrt(8*esno));
    for (size_t i = 0; i < cw.size(); ++i) {
        llr[i] = (cw[i] == 0 ? 1.0f : -1.0f) * distribution(generator);
    }

    if (verbose) {
        std::cout << "Running Test Gaussian Noise..." << std::endl;
        std::cout << "Initial info bits: ";
        for (const auto &bit : info) std::cout << bit << " ";
        std::cout << std::endl;

        std::cout << "Encoded codeword: ";
        for (const auto &bit : cw) std::cout << bit << " ";
        std::cout << std::endl;

        std::cout << "LLRs with Gaussian noise: ";
        for (const auto &value : llr) std::cout << value << " ";
        std::cout << std::endl;
    }

    int result = code.decode(llr, 20, llr_out);

    if (verbose) {
        std::cout << "LLR output from decoder: ";
        for (const auto &value : llr_out) {
            if (value <= 0.0f) {
                std::cout << "XXX" << value << " XXX ";
            } else
                std::cout << value << " ";
        }
        std::cout << std::endl;
 
        if (result == 1) {
            std::cout << "Test Gaussian Noise: Passed" << std::endl;
        } else {
            std::cout << "Test Gaussian Noise: Failed" << std::endl;
        }
    }

    return result;
}

void test_alist_read_write(ldpc &code1, int verbose) {
    if (verbose) std::cout << "Running Test Alist Read/Write..." << std::endl;

    // Write the code to disk in alist format
    std::string filename = "test_code.alist";
    code1.write_alist(filename);

    // Instantiate another ldpc class and read the alist back in
    ldpc code2;
    code2.read_alist(filename);

    // Sort edges before comparing
    code1.sort_edges();
    code2.sort_edges();

    if (verbose) {
        std::cout << "Code 1 row indices: ";
        for (const auto &r : code1.row) std::cout << r << " ";
        std::cout << std::endl;

        std::cout << "Code 1 column indices: ";
        for (const auto &c : code1.col) std::cout << c << " ";
        std::cout << std::endl;

        std::cout << "Code 2 row indices: ";
        for (const auto &r : code2.row) std::cout << r << " ";
        std::cout << std::endl;

        std::cout << "Code 2 column indices: ";
        for (const auto &c : code2.col) std::cout << c << " ";
        std::cout << std::endl;
    }

    bool is_equal = (code1.n_rows == code2.n_rows) && (code1.n_cols == code2.n_cols) &&
                        (code1.row.size() == code2.row.size()) && (code1.col.size() == code2.col.size());
    if (is_equal) {
        for (size_t i = 0; i < code1.row.size(); ++i) {
            if (code1.row[i] != code2.row[i] || code1.col[i] != code2.col[i]) {
                is_equal = false;
                break;
            }
        }
    }

    if (is_equal) {
        std::cout << "Test Alist Read/Write: Passed" << std::endl;
    } else {
        std::cout << "Test Alist Read/Write: Failed" << std::endl;
    }
}

// Test ldpc::create_encoder and ldpc::encode
void test_ldpc_encode(ldpc &code, int verbose) {
    // Clear the parity generator for the given code
    code.parity_generator.clear();

    // Call ldpc::create_encoder to create the parity generator
    code.create_encoder(verbose);

    // Setup binary RNG
    std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> distribution(0, 1);

    // Generate random information bit string and call ldpc::encode
    bitvec info(code.n_cols - code.n_rows);
    std::generate(info.begin(), info.end(), [&]() { return distribution(generator); });
    bitvec cw(code.n_cols);
    code.encode(info, cw);

    // Compute and XOR parities into checks vector
    std::vector<int> checks(code.n_rows, 0);
    for (size_t i = 0; i < code.row.size(); ++i) {
        checks[code.row[i]] ^= cw[code.col[i]];
    }

    // Check that encoded codeword satisfies all the parity checks of the code
    bool parity_check_passed = std::all_of(checks.begin(), checks.end(), [](int parity) { return parity == 0; });

    if (parity_check_passed) {
        std::cout << "Test LDPC Encode: Passed" << std::endl;
    } else {
        std::cout << "Test LDPC Encode: Failed" << std::endl;
    }
}


int main(int argc, char* argv[])
{
    ldpc code;

    // Test encoder
    code.read_alist("hamm74.alist",1);
    test_ldpc_encode(code, 1);

    // Generate short ldpc code
    int r = 45; // Example number of rows
    int c = 90; // Example number of columns
    intvec row_degrees(r, 6); // Example row degrees
    intvec col_degrees(c, 3); // Example column degrees
    code.random(r, c, row_degrees, col_degrees);

    // Run test functions
    test_alist_read_write(code, 0);
    test_ldpc_encode(code, 0);
    test_no_error(code, 0); 
    test_single_error(code, 3.0f, 0);
    test_gaussian_noise(code, 0.8, 1); // Example ESNO value

    // Generate long ldpc code
    if (false) {
        r = 1800;
        c = 3600;
        row_degrees.resize(r);
        col_degrees.resize(c);
        std::fill(row_degrees.begin(), row_degrees.end(), 6); // Example row degrees
        std::fill(col_degrees.begin(), col_degrees.end(), 3); // Example column degrees
        code.random(r, c, row_degrees, col_degrees);
        std::cout << "Generate code n=" << c << " m=" << r << std::endl;
        //code.write_alist("long_code.txt");
    }
    //code.read_alist("CCSDS_ldpc_n256_k128.alist");

    // Test encoder
    test_ldpc_encode(code, 0);

    // Test single error
    test_single_error(code, 3.0f, 1);

    // Test Gaussian noise
    test_gaussian_noise(code, 0.72, 0);

    // Test gaussian noise
    int count = 0;
    for (int i=0; i<100; ++i) {
        if (!test_gaussian_noise(code, 0.72, 0))
            count++;
    }
    std::cout << count << " errors out of 100 trials.\n"; 
}

