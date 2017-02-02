////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Tomás Oliveira e Silva, December 2016
//
// ACA 2016/2017
//
// MD5 message digest for a single block of 512 bits (16 32-bit words)
//
// It is assumed that the input block has the salt, the password, and that it has been padded and terminated (see rfc 1321)
// A little-endian architecture is also assumed (the MD5 hash expects data in little-endian format)
//
// See md5.c, md5.cl, and md5.cu
//

#ifndef _MD5_H_
#define _MD5_H_

//
// macros used in the first round
//

#define MD5_F(x,y,z)  ((x & y) | (~x & z))
#define MD5_11  7
#define MD5_12 12
#define MD5_13 17
#define MD5_14 22

//
// macros used in the second round
//

#define MD5_G(x,y,z)  ((x & z) | (y & ~z))
#define MD5_21  5
#define MD5_22  9
#define MD5_23 14
#define MD5_24 20

//
// macros used in the third round
//

#define MD5_H(x,y,z)  (x ^ y ^ z)
#define MD5_31  4
#define MD5_32 11
#define MD5_33 16
#define MD5_34 23

//
// macros used in the fourth round
//

#define MD5_I(x,y,z)  (y ^ (x | ~z))
#define MD5_41  6
#define MD5_42 10
#define MD5_43 15
#define MD5_44 21

//
// state mixing macros
//

#define MD5_ROTL(x,n)                ((x << n) | (x >> (32 - n)))
#define MD5_ROUND(F,a,b,c,d,x,s,ac)  a += F(b,c,d) + x + ac; if(s != 0) a = MD5_ROTL(a,s); a += b

//
// MD5 message digest, adapted to process only one 512-bit block
//
// the input (16 32-bit words), is assumed to reside in data[0 * data_stride], ..., data[15 * data_stride]
// the output will be stored in hash[0 * hash_stride], ..., hash[3 * hash_stride]
// the keys T[i], will be initialized and accessed through the macro T(i)
//

#define MD5(data,data_stride,hash,hash_stride)                        \
  do                                                                  \
  {                                                                   \
    unsigned int a,b,c,d,X[16];                                       \
                                                                      \
    /*               */                                               \
    /* initial state */                                               \
    /*               */                                               \
    a = 0x67452301u;                                                  \
    b = 0xEFCDAB89u;                                                  \
    c = 0x98BADCFEu;                                                  \
    d = 0x10325476u;                                                  \
    /*                                     */                         \
    /* read input block to local registers */                         \
    /*                                     */                         \
    X[ 0] = data[ 0 * data_stride];                                   \
    X[ 1] = data[ 1 * data_stride];                                   \
    X[ 2] = data[ 2 * data_stride];                                   \
    X[ 3] = data[ 3 * data_stride];                                   \
    X[ 4] = data[ 4 * data_stride];                                   \
    X[ 5] = data[ 5 * data_stride];                                   \
    X[ 6] = data[ 6 * data_stride];                                   \
    X[ 7] = data[ 7 * data_stride];                                   \
    X[ 8] = data[ 8 * data_stride];                                   \
    X[ 9] = data[ 9 * data_stride];                                   \
    X[10] = data[10 * data_stride];                                   \
    X[11] = data[11 * data_stride];                                   \
    X[12] = data[12 * data_stride];                                   \
    X[13] = data[13 * data_stride];                                   \
    X[14] = data[14 * data_stride];                                   \
    X[15] = data[15 * data_stride];                                   \
    /*             */                                                 \
    /* first round */                                                 \
    /*             */                                                 \
    T( 0) = 0xD76AA478u; MD5_ROUND(MD5_F,a,b,c,d,X[ 0],MD5_11,T( 0)); \
    T( 1) = 0xE8C7B756u; MD5_ROUND(MD5_F,d,a,b,c,X[ 1],MD5_12,T( 1)); \
    T( 2) = 0x242070DBu; MD5_ROUND(MD5_F,c,d,a,b,X[ 2],MD5_13,T( 2)); \
    T( 3) = 0xC1BDCEEEu; MD5_ROUND(MD5_F,b,c,d,a,X[ 3],MD5_14,T( 3)); \
    T( 4) = 0xF57C0FAFu; MD5_ROUND(MD5_F,a,b,c,d,X[ 4],MD5_11,T( 4)); \
    T( 5) = 0x4787C62Au; MD5_ROUND(MD5_F,d,a,b,c,X[ 5],MD5_12,T( 5)); \
    T( 6) = 0xA8304613u; MD5_ROUND(MD5_F,c,d,a,b,X[ 6],MD5_13,T( 6)); \
    T( 7) = 0xFD469501u; MD5_ROUND(MD5_F,b,c,d,a,X[ 7],MD5_14,T( 7)); \
    T( 8) = 0x698098D8u; MD5_ROUND(MD5_F,a,b,c,d,X[ 8],MD5_11,T( 8)); \
    T( 9) = 0x8B44F7AFu; MD5_ROUND(MD5_F,d,a,b,c,X[ 9],MD5_12,T( 9)); \
    T(10) = 0xFFFF5BB1u; MD5_ROUND(MD5_F,c,d,a,b,X[10],MD5_13,T(10)); \
    T(11) = 0x895CD7BEu; MD5_ROUND(MD5_F,b,c,d,a,X[11],MD5_14,T(11)); \
    T(12) = 0x6B901122u; MD5_ROUND(MD5_F,a,b,c,d,X[12],MD5_11,T(12)); \
    T(13) = 0xFD987193u; MD5_ROUND(MD5_F,d,a,b,c,X[13],MD5_12,T(13)); \
    T(14) = 0xA679438Eu; MD5_ROUND(MD5_F,c,d,a,b,X[14],MD5_13,T(14)); \
    T(15) = 0x49B40821u; MD5_ROUND(MD5_F,b,c,d,a,X[15],MD5_14,T(15)); \
    /*              */                                                \
    /* second round */                                                \
    /*              */                                                \
    T(16) = 0xF61E2562u; MD5_ROUND(MD5_G,a,b,c,d,X[ 1],MD5_21,T(16)); \
    T(17) = 0xC040B340u; MD5_ROUND(MD5_G,d,a,b,c,X[ 6],MD5_22,T(17)); \
    T(18) = 0x265E5A51u; MD5_ROUND(MD5_G,c,d,a,b,X[11],MD5_23,T(18)); \
    T(19) = 0xE9B6C7AAu; MD5_ROUND(MD5_G,b,c,d,a,X[ 0],MD5_24,T(19)); \
    T(20) = 0xD62F105Du; MD5_ROUND(MD5_G,a,b,c,d,X[ 5],MD5_21,T(20)); \
    T(21) = 0x02441453u; MD5_ROUND(MD5_G,d,a,b,c,X[10],MD5_22,T(21)); \
    T(22) = 0xD8A1E681u; MD5_ROUND(MD5_G,c,d,a,b,X[15],MD5_23,T(22)); \
    T(23) = 0xE7D3FBC8u; MD5_ROUND(MD5_G,b,c,d,a,X[ 4],MD5_24,T(23)); \
    T(24) = 0x21E1CDE6u; MD5_ROUND(MD5_G,a,b,c,d,X[ 9],MD5_21,T(24)); \
    T(25) = 0xC33707D6u; MD5_ROUND(MD5_G,d,a,b,c,X[14],MD5_22,T(25)); \
    T(26) = 0xF4D50D87u; MD5_ROUND(MD5_G,c,d,a,b,X[ 3],MD5_23,T(26)); \
    T(27) = 0x455A14EDu; MD5_ROUND(MD5_G,b,c,d,a,X[ 8],MD5_24,T(27)); \
    T(28) = 0xA9E3E905u; MD5_ROUND(MD5_G,a,b,c,d,X[13],MD5_21,T(28)); \
    T(29) = 0xFCEFA3F8u; MD5_ROUND(MD5_G,d,a,b,c,X[ 2],MD5_22,T(29)); \
    T(30) = 0x676F02D9u; MD5_ROUND(MD5_G,c,d,a,b,X[ 7],MD5_23,T(30)); \
    T(31) = 0x8D2A4C8Au; MD5_ROUND(MD5_G,b,c,d,a,X[12],MD5_24,T(31)); \
    /*             */                                                 \
    /* third round */                                                 \
    /*             */                                                 \
    T(32) = 0xFFFA3942u; MD5_ROUND(MD5_H,a,b,c,d,X[ 5],MD5_31,T(32)); \
    T(33) = 0x8771F681u; MD5_ROUND(MD5_H,d,a,b,c,X[ 8],MD5_32,T(33)); \
    T(34) = 0x6D9D6122u; MD5_ROUND(MD5_H,c,d,a,b,X[11],MD5_33,T(34)); \
    T(35) = 0xFDE5380Cu; MD5_ROUND(MD5_H,b,c,d,a,X[14],MD5_34,T(35)); \
    T(36) = 0xA4BEEA44u; MD5_ROUND(MD5_H,a,b,c,d,X[ 1],MD5_31,T(36)); \
    T(37) = 0x4BDECFA9u; MD5_ROUND(MD5_H,d,a,b,c,X[ 4],MD5_32,T(37)); \
    T(38) = 0xF6BB4B60u; MD5_ROUND(MD5_H,c,d,a,b,X[ 7],MD5_33,T(38)); \
    T(39) = 0xBEBFBC70u; MD5_ROUND(MD5_H,b,c,d,a,X[10],MD5_34,T(39)); \
    T(40) = 0x289B7EC6u; MD5_ROUND(MD5_H,a,b,c,d,X[13],MD5_31,T(40)); \
    T(41) = 0xEAA127FAu; MD5_ROUND(MD5_H,d,a,b,c,X[ 0],MD5_32,T(41)); \
    T(42) = 0xD4EF3085u; MD5_ROUND(MD5_H,c,d,a,b,X[ 3],MD5_33,T(42)); \
    T(43) = 0x04881D05u; MD5_ROUND(MD5_H,b,c,d,a,X[ 6],MD5_34,T(43)); \
    T(44) = 0xD9D4D039u; MD5_ROUND(MD5_H,a,b,c,d,X[ 9],MD5_31,T(44)); \
    T(45) = 0xE6DB99E5u; MD5_ROUND(MD5_H,d,a,b,c,X[12],MD5_32,T(45)); \
    T(46) = 0x1FA27CF8u; MD5_ROUND(MD5_H,c,d,a,b,X[15],MD5_33,T(46)); \
    T(47) = 0xC4AC5665u; MD5_ROUND(MD5_H,b,c,d,a,X[ 2],MD5_34,T(47)); \
    /*              */                                                \
    /* fourth round */                                                \
    /*              */                                                \
    T(48) = 0xF4292244u; MD5_ROUND(MD5_I,a,b,c,d,X[ 0],MD5_41,T(48)); \
    T(49) = 0x432AFF97u; MD5_ROUND(MD5_I,d,a,b,c,X[ 7],MD5_42,T(49)); \
    T(50) = 0xAB9423A7u; MD5_ROUND(MD5_I,c,d,a,b,X[14],MD5_43,T(50)); \
    T(51) = 0xFC93A039u; MD5_ROUND(MD5_I,b,c,d,a,X[ 5],MD5_44,T(51)); \
    T(52) = 0x655B59C3u; MD5_ROUND(MD5_I,a,b,c,d,X[12],MD5_41,T(52)); \
    T(53) = 0x8F0CCC92u; MD5_ROUND(MD5_I,d,a,b,c,X[ 3],MD5_42,T(53)); \
    T(54) = 0xFFEFF47Du; MD5_ROUND(MD5_I,c,d,a,b,X[10],MD5_43,T(54)); \
    T(55) = 0x85845DD1u; MD5_ROUND(MD5_I,b,c,d,a,X[ 1],MD5_44,T(55)); \
    T(56) = 0x6FA87E4Fu; MD5_ROUND(MD5_I,a,b,c,d,X[ 8],MD5_41,T(56)); \
    T(57) = 0xFE2CE6E0u; MD5_ROUND(MD5_I,d,a,b,c,X[15],MD5_42,T(57)); \
    T(58) = 0xA3014314u; MD5_ROUND(MD5_I,c,d,a,b,X[ 6],MD5_43,T(58)); \
    T(59) = 0x4E0811A1u; MD5_ROUND(MD5_I,b,c,d,a,X[13],MD5_44,T(59)); \
    T(60) = 0xF7537E82u; MD5_ROUND(MD5_I,a,b,c,d,X[ 4],MD5_41,T(60)); \
    T(61) = 0xBD3AF235u; MD5_ROUND(MD5_I,d,a,b,c,X[11],MD5_42,T(61)); \
    T(62) = 0x2AD7D2BBu; MD5_ROUND(MD5_I,c,d,a,b,X[ 2],MD5_43,T(62)); \
    T(63) = 0xEB86D391u; MD5_ROUND(MD5_I,b,c,d,a,X[ 9],MD5_44,T(63)); \
    /*        */                                                      \
    /* finish */                                                      \
    /*        */                                                      \
    hash[0 * hash_stride] = a + 0x67452301u;                          \
    hash[1 * hash_stride] = b + 0xEFCDAB89u;                          \
    hash[2 * hash_stride] = c + 0x98BADCFEu;                          \
    hash[3 * hash_stride] = d + 0x10325476u;                          \
  }                                                                   \
  while(0)

#endif
