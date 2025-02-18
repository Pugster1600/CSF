#include <stdio.h>
#include <stdlib.h>
#include "tctest.h"

#include "uint256.h"

typedef struct {
  UInt256 zero; // the value equal to 0
  UInt256 one;  // the value equal to 1
  UInt256 max;  // the value equal to (2^256)-1
  UInt256 msb_set; // the value equal to 2^255
} TestObjs;

// Helper functions for implementing tests
void set_all( UInt256 *val, uint32_t wordval );

#define ASSERT_SAME( expected, actual ) \
do { \
  ASSERT( expected.data[0] == actual.data[0] ); \
  ASSERT( expected.data[1] == actual.data[1] ); \
  ASSERT( expected.data[2] == actual.data[2] ); \
  ASSERT( expected.data[3] == actual.data[3] ); \
  ASSERT( expected.data[4] == actual.data[4] ); \
  ASSERT( expected.data[5] == actual.data[5] ); \
  ASSERT( expected.data[6] == actual.data[6] ); \
  ASSERT( expected.data[7] == actual.data[7] ); \
} while ( 0 )

#define INIT_FROM_ARR( val, arr ) \
do { \
  for ( int i = 0; i < 8; ++i ) \
    val.data[i] = arr[i]; \
} while ( 0 )

// Functions to create and cleanup the test fixture object
TestObjs *setup( void );
void cleanup( TestObjs *objs );

// Declarations of test functions
void test_get_bits( TestObjs *objs );
void test_is_bit_set( TestObjs *objs );
void test_create_from_u32( TestObjs *objs );
void test_create( TestObjs *objs );
void test_create_from_hex( TestObjs *objs );
void test_format_as_hex( TestObjs *objs );
void test_add( TestObjs *objs );
void test_sub( TestObjs *objs );
void test_negate( TestObjs *objs );
void test_neg_overflow( TestObjs *objs );
void test_mul( TestObjs *objs );
void test_lshift( TestObjs *objs );

void isBitSetTest(){
  UInt256 data = uint256_create_from_u32(0);
  data.data[0] = 0xAA; 
  data.data[1] = 0x55; 
  data.data[7] = 0x55; 

  int shift = 0;
  int bit0 = uint256_is_bit_set(data, (shift*32) + 0);
  int bit1 = uint256_is_bit_set(data, (shift*32) + 1);
  int bit2 = uint256_is_bit_set(data, (shift*32) + 2);
  int bit3 = uint256_is_bit_set(data, (shift*32) + 3);
  int bit4 = uint256_is_bit_set(data, (shift*32) + 4);
  int bit5 = uint256_is_bit_set(data, (shift*32) + 5);
  int bit6 = uint256_is_bit_set(data, (shift*32) + 6);
  int bit7 = uint256_is_bit_set(data, (shift*32) + 7);
  ASSERT(0 == bit0);
  ASSERT(1 == bit1);
  ASSERT(0 == bit2);
  ASSERT(1 == bit3);
  ASSERT(0 == bit4);
  ASSERT(1 == bit5);
  ASSERT(0 == bit6);
  ASSERT(1 == bit7);

  shift = 1;
  bit0 = uint256_is_bit_set(data, (shift*32) + 0);
  bit1 = uint256_is_bit_set(data, (shift*32) + 1);
  bit2 = uint256_is_bit_set(data, (shift*32) + 2);
  bit3 = uint256_is_bit_set(data, (shift*32) + 3);
  bit4 = uint256_is_bit_set(data, (shift*32) + 4);
  bit5 = uint256_is_bit_set(data, (shift*32) + 5);
  bit6 = uint256_is_bit_set(data, (shift*32) + 6);
  bit7 = uint256_is_bit_set(data, (shift*32) + 7);
  ASSERT(1 == bit0);
  ASSERT(0 == bit1);
  ASSERT(1 == bit2);
  ASSERT(0 == bit3);
  ASSERT(1 == bit4);
  ASSERT(0 == bit5);
  ASSERT(1 == bit6);
  ASSERT(0 == bit7);

  shift = 7;
  bit0 = uint256_is_bit_set(data, (shift*32) + 0);
  bit1 = uint256_is_bit_set(data, (shift*32) + 1);
  bit2 = uint256_is_bit_set(data, (shift*32) + 2);
  bit3 = uint256_is_bit_set(data, (shift*32) + 3);
  bit4 = uint256_is_bit_set(data, (shift*32) + 4);
  bit5 = uint256_is_bit_set(data, (shift*32) + 5);
  bit6 = uint256_is_bit_set(data, (shift*32) + 6);
  bit7 = uint256_is_bit_set(data, (shift*32) + 7);
  ASSERT(1 == bit0);
  ASSERT(0 == bit1);
  ASSERT(1 == bit2);
  ASSERT(0 == bit3);
  ASSERT(1 == bit4);
  ASSERT(0 == bit5);
  ASSERT(1 == bit6);
  ASSERT(0 == bit7);

  shift = 3;
  bit0 = uint256_is_bit_set(data, (shift*32) + 0);
  bit1 = uint256_is_bit_set(data, (shift*32) + 1);
  bit2 = uint256_is_bit_set(data, (shift*32) + 2);
  bit3 = uint256_is_bit_set(data, (shift*32) + 3);
  bit4 = uint256_is_bit_set(data, (shift*32) + 4);
  bit5 = uint256_is_bit_set(data, (shift*32) + 5);
  bit6 = uint256_is_bit_set(data, (shift*32) + 6);
  bit7 = uint256_is_bit_set(data, (shift*32) + 7);
  ASSERT(0 == bit0);
  ASSERT(0 == bit1);
  ASSERT(0 == bit2);
  ASSERT(0 == bit3);
  ASSERT(0 == bit4);
  ASSERT(0 == bit5);
  ASSERT(0 == bit6);
  ASSERT(0 == bit7);
}

void getBitTest(){
  UInt256 data = uint256_create_from_u32(0);
  data.data[0] = 0xAA; 
  data.data[1] = 0x55; 
  data.data[7] = 0x55; 

  int shift = 0;
  int bits = uint256_get_bits(data,shift);
  ASSERT (0xAA == bits);


  shift = 1;
  bits = uint256_get_bits(data,shift);
  ASSERT (0x55 == bits);

  shift = 7;
  bits = uint256_get_bits(data,shift);
  ASSERT (0x55 == bits);

  shift = 3;
  bits = uint256_get_bits(data,shift);
  ASSERT (0x0 == bits);
}


void fromHexTest(){
  UInt256 val1 = uint256_create_from_hex("f00df00ddeadbeeffeebdaeddeadbeefdeadc0dedeadf00ddeadbeef12345678");
  ASSERT (val1.data[0] == 0x12345678);
  ASSERT (val1.data[1] == 0xdeadbeef);
  ASSERT (val1.data[2] == 0xdeadf00d);
  ASSERT (val1.data[3] == 0xdeadc0de);
  ASSERT (val1.data[4] == 0xdeadbeef); 
  ASSERT (val1.data[5] == 0xfeebdaed);
  ASSERT (val1.data[6] == 0xdeadbeef);
  ASSERT (val1.data[7] == 0xf00df00d);

  UInt256 val2 = uint256_create_from_hex("00000000f00df00ddeadbeeffeebdaeddeadbeefdeadc0dedeadf00ddeadbeef12345678");
  ASSERT (val2.data[0] == 0x12345678);
  ASSERT (val2.data[1] == 0xdeadbeef);
  ASSERT (val2.data[2] == 0xdeadf00d);
  ASSERT (val2.data[3] == 0xdeadc0de);
  ASSERT (val2.data[4] == 0xdeadbeef); 
  ASSERT (val2.data[5] == 0xfeebdaed);
  ASSERT (val2.data[6] == 0xdeadbeef);
  ASSERT (val2.data[7] == 0xf00df00d);

  UInt256 val3 = uint256_create_from_hex("1deadc0dedeadf00ddeadbeef12345678");
  ASSERT (val3.data[0] == 0x12345678);
  ASSERT (val3.data[1] == 0xdeadbeef);
  ASSERT (val3.data[2] == 0xdeadf00d);
  ASSERT (val3.data[3] == 0xdeadc0de);
  ASSERT (val3.data[4] == 0x1); 
  ASSERT (val3.data[5] == 0x0);
  ASSERT (val3.data[6] == 0x0);
  ASSERT (val3.data[7] == 0x0);
}

void toHexTest(){
  UInt256 data1 = uint256_create_from_u32(0);

  data1.data[0] = 0x12345678;
  data1.data[1] = 0xdeadbeef;
  data1.data[2] = 0xdeadf00d;
  data1.data[3] = 0xdeadc0de;
  data1.data[4] = 0xdeadbeef; 
  data1.data[5] = 0xfeebdaed;
  data1.data[6] = 0xdeadbeef;
  data1.data[7] = 0xf00df00d;

  char * val1 = uint256_format_as_hex(data1);
  ASSERT(0 == strcmp("f00df00ddeadbeeffeebdaeddeadbeefdeadc0dedeadf00ddeadbeef12345678", val1));

  free(val1);

  UInt256 data2 = uint256_create_from_u32(0);

  data2.data[0] = 0x12345678;
  data2.data[1] = 0xdeadbeef;
  data2.data[2] = 0xdeadf00d;
  data2.data[3] = 0xdeadc0de;
  data2.data[4] = 0x1;

  char * val2 = uint256_format_as_hex(data2);
  ASSERT(0 == strcmp("1deadc0dedeadf00ddeadbeef12345678", val2));
  free(val2);
}

void addTest(){
  UInt256 left;
  UInt256 right;
  left.data[0] = 0xffffffff;
  left.data[1] = 0xffffffff;
  left.data[2] = 0x0;
  left.data[3] = 0x0;
  left.data[3] = 0x0;
  left.data[4] = 0x0;
  left.data[5] = 0x0;
  left.data[6] = 0x0;
  left.data[7] = 0x0;

  right.data[0] = 0x2;
  right.data[1] = 0x0;
  right.data[2] = 0x0;
  right.data[3] = 0x0;
  right.data[3] = 0x0;
  right.data[4] = 0x0;
  right.data[5] = 0x0;
  right.data[6] = 0x0;
  right.data[7] = 0x0;

  UInt256 sum = uint256_add(left, right);

  ASSERT (sum.data[0] == 0x1);
  ASSERT (sum.data[1] == 0x0);
  ASSERT (sum.data[2] == 0x1);
  ASSERT (sum.data[3] == 0x0);
  ASSERT (sum.data[4] == 0x0); 
  ASSERT (sum.data[5] == 0x0);
  ASSERT (sum.data[6] == 0x0);
  ASSERT (sum.data[7] == 0x0);

  left.data[0] = 0xffffffff;
  left.data[1] = 0xffffffff;
  left.data[2] = 0xffffffff;
  left.data[3] = 0xffffffff;
  left.data[3] = 0xffffffff;
  left.data[4] = 0xffffffff;
  left.data[5] = 0xffffffff;
  left.data[6] = 0xffffffff;
  left.data[7] = 0xffffffff;

  right.data[0] = 0x2;
  right.data[1] = 0x0;
  right.data[2] = 0x0;
  right.data[3] = 0x0;
  right.data[3] = 0x0;
  right.data[4] = 0x0;
  right.data[5] = 0x0;
  right.data[6] = 0x0;
  right.data[7] = 0x0;

  sum = uint256_add(left, right);
  ASSERT (sum.data[0] == 0x1);
  ASSERT (sum.data[1] == 0x0);
  ASSERT (sum.data[2] == 0x0);
  ASSERT (sum.data[3] == 0x0);
  ASSERT (sum.data[4] == 0x0); 
  ASSERT (sum.data[5] == 0x0);
  ASSERT (sum.data[6] == 0x0);
  ASSERT (sum.data[7] == 0x0);

  left.data[0] = 0x11111111;
  left.data[1] = 0xfffffffe;
  left.data[2] = 0x0;
  left.data[3] = 0x11111111;
  left.data[3] = 0x11111111;
  left.data[4] = 0x11111111;
  left.data[5] = 0x11111111;
  left.data[6] = 0x11111111;
  left.data[7] = 0x11111111;

  right.data[0] = 0x11111111;
  right.data[1] = 0x3;
  right.data[2] = 0x1;
  right.data[3] = 0x11111111;
  right.data[3] = 0x11111111;
  right.data[4] = 0x11111111;
  right.data[5] = 0x11111111;
  right.data[6] = 0x11111111;
  right.data[7] = 0x11111111;

  sum = uint256_add(left, right);
  ASSERT (sum.data[0] == 0x22222222);
  ASSERT (sum.data[1] == 0x1);
  ASSERT (sum.data[2] == 0x2);
  ASSERT (sum.data[3] == 0x22222222);
  ASSERT (sum.data[4] == 0x22222222); 
  ASSERT (sum.data[5] == 0x22222222);
  ASSERT (sum.data[6] == 0x22222222);
  ASSERT (sum.data[7] == 0x22222222);

  right = uint256_create_from_hex("40c742e49be35bbb0f6c16ddda98da3aa39714cc57448ebba2d66ad6c0c6ffc");
  left = uint256_create_from_hex("350aca5b5353d8d1d84e16008af60d465103192efc1fe6c1fc11a734dfb714a");
  UInt256 predicted = uint256_create_from_hex("75d20d3fef37348ce7ba2cde658ee780f49a2dfb5364757d9ee8120ba07e146");
  sum = uint256_add(right, left);
  for (int i = 0; i < 8; i++){
    ASSERT(predicted.data[i] == sum.data[i]);
  }
  left.data[0] = 0x6c0c6ffc;
  left.data[1] = 0xba2d66ad;
  left.data[2] = 0xc57448eb;
  left.data[3] = 0xaa39714c;
  left.data[4] = 0xdda98da3;
  left.data[5] = 0xb0f6c16d;
  left.data[6] = 0x49be35bb;
  left.data[7] = 0x40c742e;
  right.data[0] = 0x4dfb714a;
  right.data[1] = 0x1fc11a73;
  right.data[2] = 0xefc1fe6c;
  right.data[3] = 0x65103192;
  right.data[4] = 0x08af60d4;
  right.data[5] = 0x1d84e160;
  right.data[6] = 0xb5353d8d;
  right.data[7] = 0x350aca5;
  predicted.data[0] = 0xba07e146;
  predicted.data[1] = 0xd9ee8120;
  predicted.data[2] = 0xb5364757;
  predicted.data[3] = 0x0f49a2df;
  predicted.data[4] = 0xe658ee78;
  predicted.data[5] = 0xce7ba2cd;
  predicted.data[6] = 0xfef37348;
  predicted.data[7] = 0x75d20d3;
  sum = uint256_add(left, right);
  for (int i = 0; i < 8; i++){
    ASSERT(sum.data[i] == predicted.data[i]);
  }

  right = uint256_create_from_hex("8625da298d5c4a100cfc7b4f142394edeb17675d4d1c3be473718b56bd48e9f");
  left = uint256_create_from_hex("2750269ce0d0554bb3b77ac884ef52223d7f22a338f5dbcb46d75496db673bd");
  predicted = uint256_create_from_hex("ad7600c66e2c9f5bc0b3f6179912e71028968a00861217afba48dfed98b025c");
  sum = uint256_add(right, left);
  for (int i = 0; i < 8; i++){
    ASSERT(predicted.data[i] == sum.data[i]);
  }
  left.data[0] = 0x6bd48e9f;
  left.data[1] = 0x473718b5;
  left.data[2] = 0xd4d1c3be;
  left.data[3] = 0xdeb17675;
  left.data[4] = 0xf142394e;
  left.data[5] = 0x00cfc7b4;
  left.data[6] = 0x98d5c4a1;
  left.data[7] = 0x8625da2;
  right.data[0] = 0x6db673bd;
  right.data[1] = 0xb46d7549;
  right.data[2] = 0x338f5dbc;
  right.data[3] = 0x23d7f22a;
  right.data[4] = 0x884ef522;
  right.data[5] = 0xbb3b77ac;
  right.data[6] = 0xce0d0554;
  right.data[7] = 0x2750269;
  predicted.data[0] = 0xd98b025c;
  predicted.data[1] = 0xfba48dfe;
  predicted.data[2] = 0x0861217a;
  predicted.data[3] = 0x028968a0;
  predicted.data[4] = 0x79912e71;
  predicted.data[5] = 0xbc0b3f61;
  predicted.data[6] = 0x66e2c9f5;
  predicted.data[7] = 0xad7600c;
  sum = uint256_add(left, right);
  for (int i = 0; i < 8; i++){
    ASSERT(sum.data[i] == predicted.data[i]);
  }

  right = uint256_create_from_hex("be0627a95850a30ad5a01699c3a8aeb4188f6619efe87a0b3b962c1495eb30c");
  left = uint256_create_from_hex("672148db875fd5f897e1c016407a8820981daf725e1ba0b082f5becab751c62");
  predicted = uint256_create_from_hex("125277084dfb079036d81d6b0042336d4b0ad158c4e041abbbe8beadf4d3cf6e");
  sum = uint256_add(right, left);
  for (int i = 0; i < 8; i++){
    ASSERT(predicted.data[i] == sum.data[i]);
  }
  left.data[0] = 0x495eb30c;
  left.data[1] = 0xb3b962c1;
  left.data[2] = 0x9efe87a0;
  left.data[3] = 0x4188f661;
  left.data[4] = 0x9c3a8aeb;
  left.data[5] = 0xad5a0169;
  left.data[6] = 0x95850a30;
  left.data[7] = 0xbe0627a;
  right.data[0] = 0xab751c62;
  right.data[1] = 0x082f5bec;
  right.data[2] = 0x25e1ba0b;
  right.data[3] = 0x0981daf7;
  right.data[4] = 0x6407a882;
  right.data[5] = 0x897e1c01;
  right.data[6] = 0xb875fd5f;
  right.data[7] = 0x672148d;
  predicted.data[0] = 0xf4d3cf6e;
  predicted.data[1] = 0xbbe8bead;
  predicted.data[2] = 0xc4e041ab;
  predicted.data[3] = 0x4b0ad158;
  predicted.data[4] = 0x0042336d;
  predicted.data[5] = 0x36d81d6b;
  predicted.data[6] = 0x4dfb0790;
  predicted.data[7] = 0x12527708;
  sum = uint256_add(left, right);
  for (int i = 0; i < 8; i++){
    ASSERT(sum.data[i] == predicted.data[i]);
  }
}

void subTest(){
  //assumption is that these are in 2's compliment respresentation
  UInt256 left = uint256_create_from_u32(1);
  UInt256 right = uint256_create_from_u32(1);
  UInt256 val = uint256_sub(left, right);

  ASSERT(val.data[0] == 0);
  ASSERT(val.data[1] == 0);
  ASSERT(val.data[2] == 0);
  ASSERT(val.data[3] == 0);
  ASSERT(val.data[4] == 0);
  ASSERT(val.data[5] == 0);
  ASSERT(val.data[6] == 0);
  ASSERT(val.data[7] == 0);

  left = uint256_create_from_hex("eb44685d42baecdc80b5bccc74d21881688941fcfe50f52e316b85875c62c3d");
  right = uint256_create_from_hex("55e1ba7f1faf4c9c41897d3aca935be8a1ef59aa493ffeaa8865186183a066d");
  UInt256 predicted = uint256_create_from_hex("9562adde230ba0403f2c3f91aa3ebc98c699e852b510f683a9066d25d8c25d0");
  val = uint256_sub(left, right);
  for (int i = 0; i < 8; i++){
    ASSERT(val.data[i] == predicted.data[i]);
  }
  
  left.data[0] = 0x75c62c3d;
  left.data[1] = 0xe316b858;
  left.data[2] = 0xcfe50f52;
  left.data[3] = 0x1688941f;
  left.data[4] = 0xc74d2188;
  left.data[5] = 0xc80b5bcc;
  left.data[6] = 0xd42baecd;
  left.data[7] = 0xeb44685;
  right.data[0] = 0x183a066d;
  right.data[1] = 0xa8865186;
  right.data[2] = 0xa493ffea;
  right.data[3] = 0x8a1ef59a;
  right.data[4] = 0xaca935be;
  right.data[5] = 0xc41897d3;
  right.data[6] = 0xf1faf4c9;
  right.data[7] = 0x55e1ba7;
  predicted.data[0] = 0x5d8c25d0;
  predicted.data[1] = 0x3a9066d2;
  predicted.data[2] = 0x2b510f68;
  predicted.data[3] = 0x8c699e85;
  predicted.data[4] = 0x1aa3ebc9;
  predicted.data[5] = 0x03f2c3f9;
  predicted.data[6] = 0xe230ba04;
  predicted.data[7] = 0x9562add;
  val = uint256_sub(left, right);
  for (int i = 0; i < 8; i++){
    ASSERT(val.data[i] == predicted.data[i]);
  }
  

  left = uint256_create_from_hex("d26c81f6fecbece79c1c90b0b9973da834afc7378db2f530adfaec47060bd8f");
  right = uint256_create_from_hex("7b43e72ae89e44e77202ee62b7b4ec95cd0272d9fc132f467d8955f566b3d8e");
  predicted = uint256_create_from_hex("57289acc162da8002a19a24e01e2511267ad545d919fc5ea307196519f58001");
  val = uint256_sub(left, right);
  for (int i = 0; i < 8; i++){
    ASSERT(val.data[i] == predicted.data[i]);
  }
  left.data[0] = 0x7060bd8f;
  left.data[1] = 0x0adfaec4;
  left.data[2] = 0x78db2f53;
  left.data[3] = 0x834afc73;
  left.data[4] = 0x0b9973da;
  left.data[5] = 0x79c1c90b;
  left.data[6] = 0x6fecbece;
  left.data[7] = 0xd26c81f;
  right.data[0] = 0x566b3d8e;
  right.data[1] = 0x67d8955f;
  right.data[2] = 0x9fc132f4;
  right.data[3] = 0x5cd0272d;
  right.data[4] = 0x2b7b4ec9;
  right.data[5] = 0x77202ee6;
  right.data[6] = 0xae89e44e;
  right.data[7] = 0x7b43e72;
  predicted.data[0] = 0x19f58001;
  predicted.data[1] = 0xa3071965;
  predicted.data[2] = 0xd919fc5e;
  predicted.data[3] = 0x267ad545;
  predicted.data[4] = 0xe01e2511;
  predicted.data[5] = 0x02a19a24;
  predicted.data[6] = 0xc162da80;
  predicted.data[7] = 0x57289ac;
  val = uint256_sub(left, right);
  for (int i = 0; i < 8; i++){
    ASSERT(val.data[i] == predicted.data[i]);
  }

  left = uint256_create_from_hex("995ee0a652e5588eea89261a1e7f3877bc6e41f8443926ab3fb728ee3b13acd");
  right = uint256_create_from_hex("898a495e5401c4c770e45bb0f3b2bd4e55d49287f269ecf6ee60cbc9ca8bde7");
  predicted = uint256_create_from_hex("fd49747fee393c779a4ca692acc7b296699af7051cf39b451565d247087ce6");
  val = uint256_sub(left, right);
  for (int i = 0; i < 8; i++){
    ASSERT(val.data[i] == predicted.data[i]);
  }
  left.data[0] = 0xe3b13acd;
  left.data[1] = 0xb3fb728e;
  left.data[2] = 0x8443926a;
  left.data[3] = 0x7bc6e41f;
  left.data[4] = 0xa1e7f387;
  left.data[5] = 0xeea89261;
  left.data[6] = 0x652e5588;
  left.data[7] = 0x995ee0a;
  right.data[0] = 0x9ca8bde7;
  right.data[1] = 0x6ee60cbc;
  right.data[2] = 0x7f269ecf;
  right.data[3] = 0xe55d4928;
  right.data[4] = 0x0f3b2bd4;
  right.data[5] = 0x770e45bb;
  right.data[6] = 0xe5401c4c;
  right.data[7] = 0x898a495;
  predicted.data[0] = 0x47087ce6;
  predicted.data[1] = 0x451565d2;
  predicted.data[2] = 0x051cf39b;
  predicted.data[3] = 0x96699af7;
  predicted.data[4] = 0x92acc7b2;
  predicted.data[5] = 0x779a4ca6;
  predicted.data[6] = 0x7fee393c;
  predicted.data[7] = 0xfd4974;
  val = uint256_sub(left, right);
  for (int i = 0; i < 8; i++){
    ASSERT(val.data[i] == predicted.data[i]);
  }
}

void negateTest(){
  UInt256 val = uint256_negate(uint256_create_from_u32(0));
  ASSERT(0 == val.data[0]);
  ASSERT(0 == val.data[1]);
  ASSERT(0 == val.data[2]);
  ASSERT(0 == val.data[3]);
  ASSERT(0 == val.data[4]);
  ASSERT(0 == val.data[5]);
  ASSERT(0 == val.data[6]);
  ASSERT(0 == val.data[7]);

  val.data[0] = 0x1;
  val.data[1] = 0x0;
  val.data[2] = 0x0;
  val.data[3] = 0x0;
  val.data[4] = 0x0;
  val.data[5] = 0x0;
  val.data[6] = 0x0;
  val.data[7] = 0x0;
  val = uint256_negate(val);
  ASSERT(0xffffffff == val.data[0]);
  ASSERT(0xffffffff == val.data[1]);
  ASSERT(0xffffffff == val.data[2]);
  ASSERT(0xffffffff == val.data[3]);
  ASSERT(0xffffffff == val.data[4]);
  ASSERT(0xffffffff == val.data[5]);
  ASSERT(0xffffffff == val.data[6]);
  ASSERT(0xffffffff == val.data[7]);
  
}

void createFrom32Test(){
  UInt256 val = uint256_create_from_u32(0xdeadbeef);
  ASSERT (0xdeadbeef == val.data[0]);
  ASSERT (0x0 == val.data[1]);
  ASSERT (0x0 == val.data[2]);
  ASSERT (0x0 == val.data[3]);
  ASSERT (0x0 == val.data[4]);
  ASSERT (0x0 == val.data[5]);
  ASSERT (0x0 == val.data[6]);
  ASSERT (0x0 == val.data[7]);

  val = uint256_create_from_u32(0xffffffff);
  ASSERT (0xffffffff == val.data[0]);
  ASSERT (0x0 == val.data[1]);
  ASSERT (0x0 == val.data[2]);
  ASSERT (0x0 == val.data[3]);
  ASSERT (0x0 == val.data[4]);
  ASSERT (0x0 == val.data[5]);
  ASSERT (0x0 == val.data[6]);
  ASSERT (0x0 == val.data[7]);

  val = uint256_create_from_u32(0x0);
  ASSERT (0x0 == val.data[0]);
  ASSERT (0x0 == val.data[1]);
  ASSERT (0x0 == val.data[2]);
  ASSERT (0x0 == val.data[3]);
  ASSERT (0x0 == val.data[4]);
  ASSERT (0x0 == val.data[5]);
  ASSERT (0x0 == val.data[6]);
  ASSERT (0x0 == val.data[7]);
}

void createFromArrayTest(){
  uint32_t data[8];
  data[0] = 0xdeadbeef;
  data[1] = 0xffffffff;
  data[2] = 0x0;
  data[3] = 0xf00d;
  data[4] = 0xdeadf00d;
  data[5] = 0xf00df00d;
  data[6] = 0xdeadbeef;
  data[7] = 0x1;
  UInt256 val = uint256_create(data);
  ASSERT (0xdeadbeef == val.data[0]);
  ASSERT (0xffffffff == val.data[1]);
  ASSERT (0x0 == val.data[2]);
  ASSERT (0xf00d == val.data[3]);
  ASSERT (0xdeadf00d == val.data[4]);
  ASSERT (0xf00df00d == val.data[5]);
  ASSERT (0xdeadbeef == val.data[6]);
  ASSERT (0x1 == val.data[7]);
}

void mulTest(){
  UInt256 left = uint256_create_from_u32(0);
  UInt256 right = uint256_create_from_u32(0xdeadbeef);
  UInt256 val = uint256_mul(left, right);
  ASSERT (0x0 == val.data[0]);
  ASSERT (0x0 == val.data[1]);
  ASSERT (0x0 == val.data[2]);
  ASSERT (0x0 == val.data[3]);
  ASSERT (0x0 == val.data[4]);
  ASSERT (0x0 == val.data[5]);
  ASSERT (0x0 == val.data[6]);
  ASSERT (0x0 == val.data[7]);

  left.data[0] = 0xf00dc0de;
  left.data[1] = 0xdeaddead;
  left.data[2] = 0xdeadf00d;
  left.data[3] = 0xdeadbeef;
  left.data[4] = 0xf00df00d;
  left.data[5] = 0xf00ddead;
  left.data[6] = 0xf00d;
  left.data[7] = 0xc0de;
  right = uint256_create_from_u32(1);
  val = uint256_mul(left, right);
  for (int i = 0; i < 8; i++){
    ASSERT(val.data[i] == left.data[i]);
  }


  left = uint256_create_from_hex("b231e947a5dd34cbac5d3b78ee58569");
  right = uint256_create_from_hex("e47f3df9ee11123ccaff5b28ebcfcda");
  val = uint256_mul(left, right);
  UInt256 predicted = uint256_create_from_hex("9f0d05aa671c8ddb8da6efdd7d8f8a75fd0b2953e408dcf7fa7b89d5e2f76a");
  for (int i = 0; i < 8; i++){
    ASSERT(val.data[i] == predicted.data[i]);
  }
  

  left = uint256_create_from_hex("ee6d374d4590226dda68de3157defd1");
  right = uint256_create_from_hex("e5ca9fab4a5c0a1ddbf34df64d0a916");
  val = uint256_mul(left, right);
  predicted = uint256_create_from_hex("d604695b1fc10c01b4fa5a58f711467daaa3d73ce30b05a43b1dc379f394f6");
  for (int i = 0; i < 8; i++){
    ASSERT(val.data[i] == predicted.data[i]);
  }
}

void lShiftTest(){

}

int main( int argc, char **argv ) {
  if ( argc > 1 )
    tctest_testname_to_execute = argv[1];

  TEST_INIT();

  TEST( test_get_bits );
  TEST( test_is_bit_set );
  TEST( test_create_from_u32 );
  TEST( test_create );
  TEST( test_create_from_hex );
  TEST( test_format_as_hex );
  TEST( test_add );
  TEST( test_sub );
  TEST( test_negate );
  TEST( test_neg_overflow );
  TEST( test_mul );
  TEST( test_lshift );

  TEST (createFrom32Test);
  TEST (createFromArrayTest);
  TEST (fromHexTest);
  TEST (toHexTest);
  TEST (getBitTest);
  TEST (isBitSetTest);
  TEST (addTest);
  TEST (subTest);
  TEST (negateTest);
  TEST (mulTest);
  TEST (lShiftTest);

  TEST_FINI();
}

// Set all of the "words" of a UInt256 to a specific initial value
void set_all( UInt256 *val, uint32_t wordval ) {
  for ( unsigned i = 0; i < 8; ++i ) {
    val->data[i] = wordval;
  }
}

TestObjs *setup( void ) {
  TestObjs *objs = (TestObjs *) malloc( sizeof(TestObjs ) );

  // initialize several UInt256 values "manually"
  set_all( &objs->zero, 0 );
  set_all( &objs->one, 0 );
  objs->one.data[0] = 1U;
  set_all( &objs->max, 0xFFFFFFFFU );

  // create a value with only the most-significant bit set
  uint32_t msb_set_data[8] = { 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0x80000000U };
  INIT_FROM_ARR( objs->msb_set, msb_set_data );

  return objs;
}

void cleanup( TestObjs *objs ) {
  free( objs );
}

void test_get_bits( TestObjs *objs ) {
  ASSERT( 0U == uint256_get_bits( objs->zero, 0 ) );
  ASSERT( 0U == uint256_get_bits( objs->zero, 1 ) );
  ASSERT( 0U == uint256_get_bits( objs->zero, 2 ) );
  ASSERT( 0U == uint256_get_bits( objs->zero, 3 ) );
  ASSERT( 0U == uint256_get_bits( objs->zero, 4 ) );
  ASSERT( 0U == uint256_get_bits( objs->zero, 5 ) );
  ASSERT( 0U == uint256_get_bits( objs->zero, 6 ) );
  ASSERT( 0U == uint256_get_bits( objs->zero, 7 ) );

  ASSERT( 1U == uint256_get_bits( objs->one, 0 ) );
  ASSERT( 0U == uint256_get_bits( objs->one, 1 ) );
  ASSERT( 0U == uint256_get_bits( objs->one, 2 ) );
  ASSERT( 0U == uint256_get_bits( objs->one, 3 ) );
  ASSERT( 0U == uint256_get_bits( objs->one, 4 ) );
  ASSERT( 0U == uint256_get_bits( objs->one, 5 ) );
  ASSERT( 0U == uint256_get_bits( objs->one, 6 ) );
  ASSERT( 0U == uint256_get_bits( objs->one, 7 ) );

  ASSERT( 0xFFFFFFFFU == uint256_get_bits( objs->max, 0 ) );
  ASSERT( 0xFFFFFFFFU == uint256_get_bits( objs->max, 1 ) );
  ASSERT( 0xFFFFFFFFU == uint256_get_bits( objs->max, 2 ) );
  ASSERT( 0xFFFFFFFFU == uint256_get_bits( objs->max, 3 ) );
  ASSERT( 0xFFFFFFFFU == uint256_get_bits( objs->max, 4 ) );
  ASSERT( 0xFFFFFFFFU == uint256_get_bits( objs->max, 5 ) );
  ASSERT( 0xFFFFFFFFU == uint256_get_bits( objs->max, 6 ) );
  ASSERT( 0xFFFFFFFFU == uint256_get_bits( objs->max, 7 ) );
}

void test_is_bit_set( TestObjs *objs ) {
  // no bits should be set in the zero value
  for ( int i = 0; i < 256; ++i )
    ASSERT( !uint256_is_bit_set( objs->zero, i ) );

  // only bit 0 should be set in the one value
  ASSERT( uint256_is_bit_set( objs->one, 0 ) );
  for ( int i = 1; i < 256; ++i )
    ASSERT( !uint256_is_bit_set( objs->one, i ) );

  // only bit 255 should be set in the msb_set value
  for ( int i = 0; i < 255; ++i )
    ASSERT( !uint256_is_bit_set( objs->msb_set, i ) );
  ASSERT( uint256_is_bit_set( objs->msb_set, 255 ) );

  // all bits should be set in the max value
  for ( int i = 0; i < 256; ++i )
    ASSERT( uint256_is_bit_set( objs->max, i ) );
}

void test_create_from_u32( TestObjs *objs ) {
  UInt256 zero = uint256_create_from_u32( 0U );
  UInt256 one = uint256_create_from_u32( 1U );

  ASSERT_SAME( objs->zero, zero );
  ASSERT_SAME( objs->one, one );
}

void test_create( TestObjs *objs ) {
  (void) objs;

  uint32_t data1[8] = { 1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U };
  UInt256 val1 = uint256_create( data1 );
  ASSERT( 1U == val1.data[0] );
  ASSERT( 2U == val1.data[1] );
  ASSERT( 3U == val1.data[2] );
  ASSERT( 4U == val1.data[3] );
  ASSERT( 5U == val1.data[4] );
  ASSERT( 6U == val1.data[5] );
  ASSERT( 7U == val1.data[6] );
  ASSERT( 8U == val1.data[7] );
}

void test_create_from_hex( TestObjs *objs ) {
  UInt256 zero = uint256_create_from_hex( "0" );
  ASSERT_SAME( objs->zero, zero );

  UInt256 one = uint256_create_from_hex( "1" );
  ASSERT_SAME( objs->one, one );

  UInt256 max = uint256_create_from_hex( "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff" );
  ASSERT_SAME( objs->max, max );
}

void test_format_as_hex( TestObjs *objs ) {
  char *s;

  s = uint256_format_as_hex( objs->zero );
  ASSERT( 0 == strcmp( "0", s ) );
  free( s );

  s = uint256_format_as_hex( objs->one );
  ASSERT( 0 == strcmp( "1", s ) );
  free( s );

  s = uint256_format_as_hex( objs->max );
  ASSERT( 0 == strcmp( "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", s ) );
  free( s );
}

void test_add( TestObjs *objs ) {
  UInt256 result;

  result = uint256_add( objs->zero, objs->zero );
  ASSERT_SAME( objs->zero, result );

  result = uint256_add( objs->zero, objs->one );
  ASSERT_SAME( objs->one, result );

  uint32_t two_data[8] = { 2U };
  UInt256 two;
  INIT_FROM_ARR( two, two_data );
  result = uint256_add( objs->one, objs->one );
  ASSERT_SAME( two, result );

  result = uint256_add( objs->max, objs->one );
  ASSERT_SAME( objs->zero, result );
}

void test_sub( TestObjs *objs ) {
  UInt256 result;

  result = uint256_sub( objs->zero, objs->zero );
  ASSERT_SAME( objs->zero, result );

  result = uint256_sub( objs->one, objs->one );
  ASSERT_SAME( objs->zero, result );

  result = uint256_sub( objs->zero, objs->one );
  ASSERT_SAME( objs->max, result );
}

void test_negate( TestObjs *objs ) {
  UInt256 result;

  result = uint256_negate( objs->zero );
  ASSERT_SAME( objs->zero, result );

  result = uint256_negate( objs->one );
  ASSERT_SAME( objs->max, result );

  result = uint256_negate( objs->max );
  ASSERT_SAME( objs->one, result );
}

void test_neg_overflow( TestObjs *objs ) {
  UInt256 result;

  uint32_t two_data[8] = { 2U };
  UInt256 two;
  INIT_FROM_ARR( two, two_data );

  result = uint256_sub( objs->one, objs->max );
  ASSERT_SAME( two, result );
}

void test_mul( TestObjs *objs ) {
  UInt256 left, right, result;

  // some very basic tests

  result = uint256_mul( objs->one, objs->one );
  ASSERT_SAME( objs->one, result );

  result = uint256_mul( objs->one, objs->zero );
  ASSERT_SAME( objs->zero, result );

  result = uint256_mul( objs->zero, objs->one );
  ASSERT_SAME( objs->zero, result );

  UInt256 two = { { 2,0,0,0,0,0,0,0 } }, four = { { 4,0,0,0,0,0,0,0 } };
  result = uint256_mul( two, two );
  ASSERT_SAME( four, result );

  // a more complicated test

  // fc42c691d6284761fb49dd54f3a13eb * 4cfd2c7d8790c50c280ff0ff77617a8e = 4bdd4cc8b6067f7617c05917f828d17a26046ba5f436cb7df595f6c68c00a5a
  left.data[0] = 0x4f3a13ebU;
  left.data[1] = 0x1fb49dd5U;
  left.data[2] = 0x1d628476U;
  left.data[3] = 0xfc42c69U;
  left.data[4] = 0x0U;
  left.data[5] = 0x0U;
  left.data[6] = 0x0U;
  left.data[7] = 0x0U;
  right.data[0] = 0x77617a8eU;
  right.data[1] = 0x280ff0ffU;
  right.data[2] = 0x8790c50cU;
  right.data[3] = 0x4cfd2c7dU;
  right.data[4] = 0x0U;
  right.data[5] = 0x0U;
  right.data[6] = 0x0U;
  right.data[7] = 0x0U;
  result = uint256_mul(left, right);
  ASSERT(0x68c00a5aU == result.data[0]);
  ASSERT(0xdf595f6cU == result.data[1]);
  ASSERT(0x5f436cb7U == result.data[2]);
  ASSERT(0xa26046baU == result.data[3]);
  ASSERT(0x7f828d17U == result.data[4]);
  ASSERT(0x617c0591U == result.data[5]);
  ASSERT(0x8b6067f7U == result.data[6]);
  ASSERT(0x4bdd4ccU == result.data[7]);
}

void test_lshift( TestObjs *objs ) {
  UInt256 result;

  UInt256 two = { { 2,0,0,0,0,0,0,0 } }, four = { { 4,0,0,0,0,0,0,0 } };

  // some very basic tests

  result = uint256_lshift( objs->one, 0 );
  ASSERT_SAME( objs->one, result );

  result = uint256_lshift( objs->one, 1 );
  ASSERT_SAME( two, result );

  result = uint256_lshift( objs->one, 2 );
  ASSERT_SAME( four, result );

  // a more complicated test
  {
    // Test shifting 727767d07ccff5fe25cd125b4523e8c7db1b8d1a2c8a2830284d72bb872c33a5 left by 50 bit(s)
    uint32_t arr[8] = {0x872c33a5U, 0x284d72bbU, 0x2c8a2830U, 0xdb1b8d1aU, 0x4523e8c7U, 0x25cd125bU, 0x7ccff5feU, 0x727767d0U};
    UInt256 val;
    INIT_FROM_ARR( val, arr );
    uint32_t expected_arr[8] = {0x00000000U, 0xce940000U, 0xcaee1cb0U, 0xa0c0a135U, 0x3468b228U, 0xa31f6c6eU, 0x496d148fU, 0xd7f89734U};
    UInt256 expected;
    INIT_FROM_ARR( expected, expected_arr );
    result = uint256_lshift( val, 50U );
    ASSERT_SAME( expected, result );
  }
}
