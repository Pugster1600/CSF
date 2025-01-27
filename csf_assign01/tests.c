#include <stdio.h>
#include "uint256.h"
#include <stdlib.h>
#include <string.h>

void isBitSetTest(){
  UInt256 data;
  data.data[0] = 0xAA;  // 0b10101010
  data.data[1] = 0x55;  // 0b01010101
  data.data[7] = 0x55;  // 0b01010101

  int shift = 0;
  int bit0 = uint256_is_bit_set(data, (shift*32) + 0);
  int bit1 = uint256_is_bit_set(data, (shift*32) + 1);
  int bit2 = uint256_is_bit_set(data, (shift*32) + 2);
  int bit3 = uint256_is_bit_set(data, (shift*32) + 3);
  int bit4 = uint256_is_bit_set(data, (shift*32) + 4);
  int bit5 = uint256_is_bit_set(data, (shift*32) + 5);
  int bit6 = uint256_is_bit_set(data, (shift*32) + 6);
  int bit7 = uint256_is_bit_set(data, (shift*32) + 7);
  printf("predicted: 0b10101010. Actual: 0b%d%d%d%d%d%d%d%d\n", bit7, bit6, bit5, bit4, bit3, bit2, bit1, bit0);

  shift = 1;
  bit0 = uint256_is_bit_set(data, (shift*32) + 0);
  bit1 = uint256_is_bit_set(data, (shift*32) + 1);
  bit2 = uint256_is_bit_set(data, (shift*32) + 2);
  bit3 = uint256_is_bit_set(data, (shift*32) + 3);
  bit4 = uint256_is_bit_set(data, (shift*32) + 4);
  bit5 = uint256_is_bit_set(data, (shift*32) + 5);
  bit6 = uint256_is_bit_set(data, (shift*32) + 6);
  bit7 = uint256_is_bit_set(data, (shift*32) + 7);
  printf("predicted: 0b01010101. Actual: 0b%d%d%d%d%d%d%d%d\n", bit7, bit6, bit5, bit4, bit3, bit2, bit1, bit0);

  shift = 7;
  bit0 = uint256_is_bit_set(data, (shift*32) + 0);
  bit1 = uint256_is_bit_set(data, (shift*32) + 1);
  bit2 = uint256_is_bit_set(data, (shift*32) + 2);
  bit3 = uint256_is_bit_set(data, (shift*32) + 3);
  bit4 = uint256_is_bit_set(data, (shift*32) + 4);
  bit5 = uint256_is_bit_set(data, (shift*32) + 5);
  bit6 = uint256_is_bit_set(data, (shift*32) + 6);
  bit7 = uint256_is_bit_set(data, (shift*32) + 7);
  printf("predicted: 0b01010101. Actual: 0b%d%d%d%d%d%d%d%d\n", bit7, bit6, bit5, bit4, bit3, bit2, bit1, bit0);
}

void fromHexTest(){
  //char string[100] = "ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff";
  char string[100] = "deadbeeff";
  //uint256_create_from_hex("fdeadbeef");
  UInt256 val = uint256_create_from_hex("afeebdaed000");
  //uint256_create_from_hex("deadbeefdeadbeefdeadbeefdeadbeefdeadbeefdeadbeefdeadbeefdeadbeef");
  //int x = strlen(string);
  for (int i = 0; i < 8; i++){
    printf("%lu", val.data[i]);
  }
}

void toHexTest(){
  UInt256 data = uint256_create_from_u32(0);
  //data.data[0] = 0xdeadbeef;  // 0b10101010
  //data.data[1] = 0xdeadbeef;  // 0b01010101
  //data.data[2] = 0x1111beef;
  //data.data[3] = 0xbeefa000;
  //data.data[3] = 0x11111111;
  //data.data[4] = 0x12345678; 
  //data.data[5] = 0x1111beef;
  //data.data[6] = 0x11111111;
  //data.data[7] = 0x111bef;

  data.data[0] = 0x1;  // 0b10101010
  data.data[1] = 0x0;  // 0b01010101
  data.data[2] = 0x0;
  data.data[3] = 0x0;
  data.data[3] = 0x0;
  data.data[4] = 0x0; 
  data.data[5] = 0x0;
  data.data[6] = 0x0;
  data.data[7] = 0xdeadbeef;

  data.data[0] = 0x1;  // 0b10101010
  data.data[1] = 0x0;  // 0b01010101
  data.data[2] = 0x0;
  data.data[3] = 0x0;
  data.data[3] = 0x0;
  data.data[4] = 0x0; 
  data.data[5] = 0x0;
  data.data[6] = 0x0;
  data.data[7] = 3735928559;

  //data.data[0] = 0xfeebdaed;  // 0b10101010
  //data.data[1] = 0x5b00b;  // 0b01010101
  //data.data[2] = 0xfeebdaed;
  //data.data[3] = 0x5b00b;
  //data.data[3] = 0xfeebdaed;
  //data.data[4] = 0x5b00b; 
  //data.data[5] = 0xfeebdaed;
  //data.data[6] = 0x5b00b;
  //data.data[7] = 0x5b00b;

  char * val = uint256_format_as_hex(data);
  printf("str: %s\n", val);

  UInt256 newVal = uint256_create_from_hex(val);
  for (int i = 0; i < 8; i++){
    printf("%d: %lu\n", i, newVal.data[i]);
  }
  free(val);
}
int main(){
  //isBitSetTest();
  fromHexTest();
  //toHexTest();
  return 0;
}