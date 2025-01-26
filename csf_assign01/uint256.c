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
  unsigned long iterations = sizeof(result.data) / sizeof(result.data[0]);

  // TODO: implement
  result.data[0] = val;
  for (unsigned long i = 1; i < iterations; ++i) {
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
  //UInt256 result;
  UInt256 result = uint256_create_from_u32(0);

  int firstIndex = strlen(hex); //most right index
  int maxSize = 64;
  int finalIndex = firstIndex - maxSize > 0 ? firstIndex - maxSize : 0; //most left index
  int totalChars = firstIndex - finalIndex;
  int extraChars = totalChars % 8;
  int totalIterations = extraChars == 0 ? (totalChars / 8) : (totalChars / 8) + 1;
  char newHex[totalChars];

  //going in reverse order to fill in the newHex 
  for (int i = firstIndex; i > finalIndex; --i){
    int newHexOffset = firstIndex - i;
    newHex[newHexOffset] = hex[i - 1]; //newHexMaxIndex - newHexOffset -> int newHexMaxIndex = sizeof(newHex) - 1;
  }

  //converting 8 chars at a time
  for (int i = 0; i < totalIterations; i++){
    int charIterations = totalChars - (i * 8) >= 8 ? 8 : extraChars;
    char temp[charIterations + 1];
    temp[charIterations] = '\0';
    char * endptr;

    for (int j = 0; j < charIterations; j++){
      temp[j] = newHex[(i * 8) + j];
    }

    unsigned long int val = strtoul(temp, &endptr, 16);
    result.data[i] = val;

    printf("val: %lu\n", result.data[i]);
    printf("str: %s\n", temp);
  }
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
char * uint256_format_as_hex( UInt256 val ) {
  //loop through val and cover with bit mask
  char buffer[9];
  int places;

  //find first group to not equal 0
  for (int i = 7; i >= 0; i--){
    if (val.data[i] != 0){
      places = i + 1;
      break;
    } 
  }

  //something is wrong with remainder!
  int remainder = sprintf(buffer, "%x", val.data[places - 1]);
  int totalChars = (places - 1) * 8 + remainder;
  char *hex = malloc((sizeof(char) * (totalChars + 1))); //max 64 + 1
  hex[sizeof(char) * totalChars] = '\0';

  for (int i = 0; i < places; i++){
    int finalGroup = i == places - 1;
    if (finalGroup){
      sprintf(buffer, "%x", val.data[i]);
    } 
    else{
      sprintf(buffer, "%08x", val.data[i]);
    }

    printf("buff %s\n", buffer);

    int bit = finalGroup ? remainder : 8;
    for (int j = 0; j < bit; j++){
      //going from lsb to msb
      int hexIndex = (places - 1 - i) * 8 + j;
      if (bit != 8){
        hex[hexIndex] = buffer[bit - 1 - j];
      }
      else{
        hex[hexIndex] = buffer[8 - 1 - j];
      }
      
      printf("val: %d\n", hexIndex);
    }

  }

  for (int i = 0; i < (sizeof(char) * (totalChars + 1)); i++){
    printf("%c", hex[i]);
    if (hex[i] == '\0'){
      printf("WETF", hex[i+1]);
    }
  }
  printf("\n");


  // TODO: implement
  return hex;
  //return hex;
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
