#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "uint256.h"

// Create a UInt256 value from a single uint32_t value.
// Only the least-significant 32 bits are initialized directly,
// all other bits are set to 0.
UInt256 uint256_create_from_u32( uint32_t val ) {
  UInt256 result;
  // TODO: implement
  result.data[0] = val;
  for (long unsigned int i = 1; i < sizeof(result.data)-1; ++i) {
    result.data[i] = 0;
  }
  
  return result;
}

// Create a UInt256 value from an array of N WORDS uint32_t values.
// The element at index 0 is the least significant, and the element
// at index 7 is the most significant.
UInt256 uint256_create( const uint32_t data[8] ) {
  UInt256 result;
  // TODO: implement
  for (long unsigned int i = 0; i < sizeof(result.data)-1; ++i){
    result.data[i] = data[i];
  }
  return result;
}

// Create a UInt256 value from a string of hexadecimal digits.
UInt256 uint256_create_from_hex( const char *hex ) {
  UInt256 result;
  // TODO: implement

  //0. attributes
  int length = strlen(hex);
  int maxSize = 64;

  //1. create the reversed string
  char copiedString[length + 1];
  strcpy(copiedString, hex);
  reverse(copiedString, length);

  
  int updatedHexLength = length > maxSize ? maxSize : length; 

  

  int sections;
  int remainder;

  if (updatedHexLength == length || updatedHexLength == maxSize){
    remainder = updatedHexLength % 8;
    if (remainder == 0){
      sections = updatedHexLength / 8;
    } else{
      sections = (updatedHexLength / 8) + 1;
    }

    for (int i = 0; i < sections; i++){
      result.data[i] = 0;
      //unsigned long int result = strtout(
    }
  }


    //substring 8 into string.strtoul
    //check if length - i less than 8
    
  return result;
}


void reverse(char *str, int length) {
  int start = 0;
  int end = length - 1;
  while (start < end) {
    char temp = str[start];
    str[start] = str[end];
    str[end] = temp;
    start++;
    end--;
  }
}

// Return a dynamically-allocated string of hex digits representing the
// given UInt256 value.
char *uint256_format_as_hex( UInt256 val ) {
  char *hex = NULL;
  // TODO: implement
  return hex;
}

// Get 32 bits of data from a UInt256 value.
// Index 0 is the least significant 32 bits, index 7 is the most
// significant 32 bits.
uint32_t uint256_get_bits( UInt256 val, unsigned index ) {
  uint32_t bits;
  //TODO
  bits = val.data[index];
  return bits;
}

// Return 1 if bit at given index is set, 0 otherwise.
int uint256_is_bit_set( UInt256 val, unsigned index) {
  // TODO: implement
  int arrayIndex = index / 32; //get the array index
  int shift = index % 32;
  uint32_t bitmask = 1 << shift;

  if (val.data[arrayIndex] & bitmask){
    return 1;
  }
  return 0;
}

// Compute the sum of two UInt256 values.
UInt256 uint256_add( UInt256 left, UInt256 right ) {
  UInt256 sum;
  // TODO: implement
  return sum;
}

// Compute the difference of two UInt256 values.
UInt256 uint256_sub( UInt256 left, UInt256 right ) {
  UInt256 result;
  // TODO: implement
  return result;
}

// Return the two's-complement negation of the given UInt256 value.
UInt256 uint256_negate( UInt256 val ) {
  UInt256 result;
  // TODO: implement
  //toggle each bit with xor then + 1
  return result;
}

// Compute the product of two UInt256 values.
UInt256 uint256_mul( UInt256 left, UInt256 right ) {
  UInt256 product;
  // TODO: implement
  return product;
}

UInt256 uint256_lshift( UInt256 val, unsigned shift ) {
  assert( shift < 256 );
  UInt256 result;
  // TODO: implement
  return result;
}
