#ifndef tbcc_H
#define tbcc_H

#include "FeedForwardTrellis.h"
#include "consts.h"
#include "tbcc_types.h"
#include "minHeap.h"
#include "tbcc_namespace.h"
#include "lowRateListDecoder.h"
#include <string>
#include <vector>

// using intvec = std::vector<int>;
// using fltvec = std::vector<float>;
// using dblvec = std::vector<double>;
// using llrvec = std::vector<int64_t>;

// Class for generating, encoding, and decoding binary tbcc codes
class tbcc
{
  private:
    // basic parameters
    int k_, n_, v_, theta_t_;
    std::vector<int> numerators;

    /* - Trellis setup - */
    FeedForwardTrellis trellis;

    /* - Decoder setup - */
    LowRateListDecoder decoder;

  public:

    // Sparse binary matrix given by list row nad col positions of non-zero elements
    intvec row, col;
    
    // Parity generator matrix
    std::vector<std::vector<int>> parity_generator;

    // Constructor
    tbcc() : k_(k), n_(n), v_(V), theta_t_(0), numerators({POLY1, POLY2}), 
              trellis(k, n, V, {POLY1, POLY2}), decoder(trellis, 1e7, M+1, CRC, 'A'){}

    // tbcc() : k(K), n(N), v(V), theta_t(0), numerators({POLY1, POLY2}) {}

    // Generate encoder
    void create_encoder(int verbose = 0);

    // BALD decoding
    MessageInformation decode(fltvec &llr_in, double theta_t, intvec punctured_indices, int verbose);

    MessageInformation decode_fixedp(llrvec &llr_in, double theta_t, intvec punctured_indices, int verbose = 0);
 
    // Encode k info bits into n codeword bits
    void encode(intvec &info, intvec &cw);
};

// MORE TEST FUNCTIONS
void test_no_error(tbcc &code);
// void test_single_error(tbcc &code);
// void test_gaussian_noise(tbcc &code, float esno);

#endif // tbcc_H

