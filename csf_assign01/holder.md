//UInt256 result;
  //strip away the leading 0s and whatnot
  UInt256 result = uint256_create_from_u32(0);
  char reversed[strlen(hex) + 1];
  memcpy(reversed, hex, strlen(hex) + 1);
  reverse(reversed, strlen(reversed) + 1);
  printf("str: %s\n", reversed);

  const int msbHexIndex = strlen(hex); //most right index
  const int maxSize = 64;
  const int lsbHexIndex = msbHexIndex - maxSize > 0 ? msbHexIndex - maxSize : 0; //most left index
  const int totalChars = msbHexIndex - lsbHexIndex;
  const int finalBucketChars = totalChars % 8; //basically remainder
  const int totalIterations = finalBucketChars == 0 ? (totalChars / 8) : (totalChars / 8) + 1;
  char newHex[totalChars];

  //going in reverse order to fill in the newHex 
  for (int i = msbHexIndex; i > lsbHexIndex; --i){
    int newHexOffset = msbHexIndex - i; //going from 0 - totalChars
    newHex[newHexOffset] = hex[i - 1];
  }

  //iterating 8 chars at a time
  for (int i = 0; i < totalIterations; i++){
    int charIterations = totalChars - (i * 8) >= 8 ? 8 : finalBucketChars; 
    char temp[charIterations + 1];
    temp[charIterations] = '\0';
    char * endptr;

    //filling in the 8 chars
    for (int j = 0; j < charIterations; j++){
      temp[j] = newHex[(i * 8) + j]; //new hex is reversed string
    }

    //convert the chars
    unsigned long val = strtoul(temp, &endptr, 16);
    result.data[i] = val;
  }

  -----------------------------------------
  UInt256 uint256_create_from_hex( const char *hex ) {
  //write psuedo code
  UInt256 result = uint256_create_from_u32(0);
  
  int inputHexLength = strlen(hex);
  int msbHexIndex = -1;
  int lsbHexIndex = 0; //all index + 1 to go to length
  int newHexLength = 0; //all length - 1 to go to index
  int maxChar = 64;
  int totalIterations = 0;
  int finalBucketChars = 0;

  //1. find leading zeros
  for (int i = inputHexLength - 1; i >= 0; --i){
    if (hex[i] != '0'){
      msbHexIndex = i;
      break;
    }
  }

  //2. if string is just 0
  if (msbHexIndex == -1){
    return result;
  }

  lsbHexIndex = msbHexIndex + 1 > maxChar ? msbHexIndex - (maxChar - 1) : 0;
  newHexLength = (msbHexIndex - lsbHexIndex) + 1; //no NULL!
  finalBucketChars = newHexLength % 8;
  totalIterations = (finalBucketChars == 0) ? (newHexLength / 8) : (newHexLength / 8) + 1;
  char newHex[newHexLength];

  //3. reversing string
  for (int i = msbHexIndex; i >= lsbHexIndex; --i){
    //newHex[msbHexIndex - i] = hex[i]; //reverse
    newHex[(newHexLength - 1) - (msbHexIndex - i)] = hex[i]; //not reverse
  }
  //printf("final: %d\n", finalBucketChars);
  for (int i = 0; i < totalIterations; ++i){
    int charIterations = newHexLength - (i * 8) >= 8 ? 8 : finalBucketChars; 
    char temp[9] = {'0', '0', '0', '0', '0', '0', '0', '0', '\0'};

    //filling in the 8 chars
    for (int j = 0; j < charIterations; ++j){
      temp[j] = newHex[(i * 8) + j]; //new hex is reversed string
      //charIterations - j not reversed
    }

    //convert the chars
    unsigned long val = strtoul(temp, NULL, 16);
    result.data[i] = val;
    printf("%s", temp);
  }

  return result;
}

UInt256 uint256_own_add( UInt256 left, UInt256 right ) {
  UInt256 sum;
  int carry = 0;
  // TODO: implement
  for (int i = 0; i < 8; i++){
    uint32_t addResult = left.data[i] + right.data[i] + carry;
    printf("result: %lu left: %lu right: %lu carry: %d\n", addResult, left.data[i], right.data[i], carry);
    carry = ((addResult < left.data[i]) || (addResult < right.data[i])) ? 1 : 0;
    sum.data[i] = addResult;
  }
  return sum;
}