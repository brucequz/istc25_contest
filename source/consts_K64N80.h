/* K64N128 PARAMETERS */
#ifndef K64N80_PARAM
#define K64N80_PARAM

#include <vector>

/* --- Convolutional Code Parameters --- */
inline constexpr int k = 1;                    /* Number of input bits */
inline constexpr int n = 1;                    /* Number of output bits */
inline constexpr int K = 64;                   /* Number of input bits */
inline constexpr int N = 80;                   /* Number of output bits */
inline constexpr int V = 8;                    /* Number of memory elements */
inline constexpr int M = 8;                    /* Degree of CRC - 1 */
inline constexpr unsigned int CRC = 0x18B;     /* CRC polynomial */
inline const std::vector<int> NUMERATORS = {205};  /* in octal */

inline const std::vector<int> PUNCTURING_INDICES = {};

/* --- List Decoder Parameters --- */
inline constexpr int MAX_LISTSIZE = 1e7;       /* Maximum list size */
inline constexpr double MAX_METRIC = 87.6;     /* Maximum decoding metric */
inline constexpr double MAX_ANGLE = 0.7498;    /* Maximum angle for the list decoder */
inline constexpr char ENCODING_RULE = 'Z';     /* Select code type: {Z: zero-terminated CC, T: tail-biting CC} */
inline constexpr char DECODING_RULE = 'N';     /* Decoding rule: {P: projected, N: non-projected}*/
inline constexpr char STOPPING_RULE = 'A';     /* Stopping rule: {M: metric, L: listsize, A: angle} */

/* --- Fixed Point Math --- */
constexpr int FIXED_POINT_FRACTIONAL_LENGTH = 12; /* Fixed point fractional length */

#endif