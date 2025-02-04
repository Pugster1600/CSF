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
  for (unsigned long i = 1; i < 8; ++i) {
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
  for (int i = 0; i < 8; ++i){
    result.data[i] = data[i];
  }
  return result;
}

// Create a UInt256 value from a string of hexadecimal digits.
UInt256 uint256_create_from_hex( const char *hex ) {
  //write psuedo code
  UInt256 result = uint256_create_from_u32(0);
  
  const int lsbHexIndex = strlen(hex) - 1; //most right index
  const int maxSize = 64;
  const int msbHexIndex = lsbHexIndex - maxSize > 0 ? lsbHexIndex - (maxSize - 1): 0; //most left index
  const int totalChars = (lsbHexIndex - msbHexIndex) + 1;
  const int finalBucketChars = totalChars % 8; //basically remainder
  const int totalIterations = finalBucketChars == 0 ? (totalChars / 8) : (totalChars / 8) + 1;

  //iterating 8 chars at a time
  int index = lsbHexIndex;//totalChars - 1
  for (int i = 0; i < totalIterations; i++){
    int charIterations = totalChars - (i * 8) >= 8 ? 8 : finalBucketChars; 
    char temp[charIterations + 1];
    temp[charIterations] = '\0';

    //filling in the 8 chars
    for (int j = 0; j < charIterations; j++){
      temp[(charIterations - 1) - j] = hex[index];
      index--;
    }

    //convert the chars
    unsigned long val = strtoul(temp, NULL, 16);
    result.data[i] = val;
  }

  return result;
}

// Return a dynamically-allocated string of hex digits representing the
// given UInt256 value.
char * uint256_format_as_hex( UInt256 val ){
  char buffer[9];
  int buckets = 0;
  char * hex;

  //1. find first msb that is non 0
  for (int i = 7; i >= 0; i--){
    if (val.data[i] != 0){
      buckets = i + 1;
      break;
    } 
  }

  //2. if no buckets ie all 0 
  if (buckets == 0){
    hex = malloc(sizeof(char) * 2);
    hex[0] = '0';
    hex[1] = '\0';
    return hex;
  }

  //3. create the malloc hex buffer
  const int remainder = sprintf(buffer, "%x", val.data[buckets - 1]);
  const int totalChars = (buckets - 1) * 8 + remainder;
  hex = malloc((sizeof(char) * (totalChars)) + 1); //max of 64 + 1 for \0
  hex[sizeof(char) * totalChars] = '\0';
  
  //going through the array, starting from msb/finalGroup
  for (int i = 0; i < buckets; i++){
    int finalGroup = i == buckets - 1;
    int charIters;

    if (finalGroup){
      sprintf(buffer, "%x", val.data[i]);
      charIters = remainder;
    } 
    else{
      sprintf(buffer, "%08x", val.data[i]);
      charIters = 8;
    }
    //filling in the malloc buffer from msb to msb
    for (int charIndex = 0; charIndex < charIters; charIndex++){
      //total - previous iter offset - current iter offset + increment
      hex[totalChars - (i * 8) - charIters + charIndex] = buffer[charIndex];
    }
  }
  return hex;
}

// Get 32 bits of data from a UInt256 value.
// Index 0 is the least significant 32 bits, index 7 is the most
// significant 32 bits.
uint32_t uint256_get_bits( UInt256 val, unsigned index ) {
  // TODO: implement
  uint32_t bits;
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
  int carry = 0;
  // TODO: implement
  for (int i = 0; i < 8; i++){
    uint32_t addResult = left.data[i] + right.data[i] + carry;
    carry = ((addResult < left.data[i]) || (addResult < right.data[i])) ? 1 : 0;
    sum.data[i] = addResult;
  }
  return sum;
}

// Compute the difference of two UInt256 values.
UInt256 uint256_sub( UInt256 left, UInt256 right ) {
  UInt256 result;
  // TODO: implement
  UInt256 negatedRight = uint256_negate(right);
  result = uint256_add(left, negatedRight);

  return result;
}

// Return the two's-complement negation of the given UInt256 value.
UInt256 uint256_negate( UInt256 val ) {
  UInt256 result;
  // TODO: implement
  for (int i = 0; i < 8; i++){
    result.data[i] = ~val.data[i];
  }
  return uint256_add(result, uint256_create_from_u32(1));
}

// Compute the product of two UInt256 values.
UInt256 uint256_mul( UInt256 left, UInt256 right ) {
  UInt256 product = {0}; //need to initialize to 0 or breaks
  for (int i = 0; i < 256; i++) {
    //needs to check if bit position is a 1                                                                                                                                                                                                                          
    if (left.data[i / 32] & (1 << (i % 32))) {
      //left shift by bit position in A                                                                                                                                                                                                                              
      UInt256 term = uint256_lshift(right, i);
      // add result to running sum                                                                                                                                                                                                                                         //product holds running sum
      product = uint256_add(product, term);
    }
     
  }
  for (int i = 7; i>= 0; i--) {
    printf("product.data[%d] = %08x\n", i, product.data[i]); //Testing print
  }

  
  return product;
}

UInt256 uint256_lshift( UInt256 val, unsigned shift ) {
  assert( shift < 256 );

  UInt256 result = {0};
  //we need to compute both bucket and bit shift                                                                                                                                                                                                                      
  //moves bucket first then shifts bits inside                                                                                                                                                                                                                         //starts with MSB, shifts according to bucket/bit combination
  //then grabs the overflow bits from bucket next to it
  //then shifts the next bucket, which gets rid of most sig digits
  //we need to grab overflow before visiting buckets or else we'll lose info
  int bucket = shift / 32;
  int bitShift = shift % 32;

  for (int i = 7; i >= 0; i--) {
    if (i - bucket >= 0) {
      result.data[i] = val.data[i - bucket] << bitShift; //shifts here                                                                                                                                                                                
      if (i - bucket - 1 >= 0 && bitShift > 0) { //checks if there will overflow in following bucket (aka  bit shift exists)                                                                                                                                            
        result.data[i] |= val.data[i - bucket - 1] >> (32 - bitShift); //bitwise OR with current bucket to grab overflow                                                                                                                                                
      }
    }
    printf("result.data[%d] = %08x\n", i, result.data[i]); // Debug print
  }


  return result;
}
