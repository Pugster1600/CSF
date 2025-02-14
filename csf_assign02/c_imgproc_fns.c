// C implementations of image processing functions

#include <stdlib.h>
#include <assert.h>
#include "imgproc.h"
#include <stdio.h>

// TODO: define your helper functions here

// Convert input pixels to grayscale.
// This transformation always succeeds.
//
// Parameters:
//   input_img  - pointer to the input Image
//   output_img - pointer to the output Image (in which the transformed
//                pixels should be stored)

//little endian in memory -> lsb first [a,b,g,r] [0-7, 8-15, 15-23, 24-31]
uint8_t get_r(int32_t pixel) { //
  uint8_t * val = (uint8_t *)&pixel;
  return *(val + 3);
}

uint8_t get_g(int32_t pixel) {
  uint8_t * val = (uint8_t *)&pixel;
  return *(val + 2);
}

uint8_t get_b(int32_t pixel) {
  uint8_t * val = (uint8_t *)&pixel;
  return *(val + 1);
}

uint8_t get_a(int32_t pixel) {
  uint8_t * val = (uint8_t *)&pixel;
  return *(val);
}

//value = r,g,b,a then it gets flipped in memory
uint32_t combineData(uint8_t r, uint8_t g, uint8_t b, uint8_t a){
  return (r<<24) | (g<<16) | (b<<8) | a;
}

#define ONE_MILLION 1000000L
#define TWO_BILLION 2000000000L
#define ONE_TRILLION 1000000000000L

int64_t gradient(int64_t num){
  num -= 1000;
  return ONE_MILLION - (num * num);
}

int64_t getRowTransform(int64_t rowIndex, int64_t height){
  int64_t num = (TWO_BILLION * rowIndex) / (ONE_MILLION * height);
  return gradient(num);
}

int64_t getColumnTransform(int64_t colIndex, int64_t width){
  int64_t num = (TWO_BILLION * colIndex) / (ONE_MILLION * width);
  return gradient(num);
}

int64_t getFadedComponentValue(int64_t rowIndex, int64_t colIndex, int64_t width, int64_t height, int64_t color){
  int64_t newColor = (color * getRowTransform(rowIndex, height) * getColumnTransform(colIndex, width)) / ONE_TRILLION;
  return newColor;
}

int32_t getAdjustedIndex(int32_t index, int32_t indexingWidth, int32_t actualWidth){
  int32_t row = index / indexingWidth;
  int32_t col = index % indexingWidth;
  int32_t updatedIndex = (row * actualWidth) + col;
  return updatedIndex;
}

void fillKaleidoscopeIndexArray(int32_t * indexArray, int32_t indexingWidth, int32_t indexingHeight, int32_t x, int32_t y){
  int32_t halfWidth =  indexingWidth / 2;
  int32_t halfHeight = indexingHeight / 2;
  int32_t widthDistanceFromMiddle = halfWidth - x;
  int32_t heightDistanceFromMiddle = halfHeight - y;

  //index in the imaginary grid
  int32_t aTopLeftIndex = (y * indexingWidth) + x;
  int32_t bTopLeftIndex = (x * indexingHeight) + y;
  int32_t aTopRightIndex = aTopLeftIndex + (2 * widthDistanceFromMiddle) - 1;
  int32_t bTopRightIndex = bTopLeftIndex + (2 * heightDistanceFromMiddle) - 1;
  int32_t aBottomLeftIndex = aTopLeftIndex + ((2 * heightDistanceFromMiddle - 1) * indexingWidth);
  int32_t bBottomLeftIndex = bTopLeftIndex + ((2 * widthDistanceFromMiddle - 1) * indexingWidth);
  int32_t aBottomRightIndex = aBottomLeftIndex + (2 * widthDistanceFromMiddle) - 1;
  int32_t bBottomRightIndex = bBottomLeftIndex + (2 * heightDistanceFromMiddle) - 1;

  indexArray[0] = aTopLeftIndex;
  indexArray[1] = bTopLeftIndex;
  indexArray[2] = aTopRightIndex;
  indexArray[3] = bTopRightIndex;
  indexArray[4] = aBottomLeftIndex;
  indexArray[5] = bBottomLeftIndex;
  indexArray[6] = aBottomRightIndex;
  indexArray[7] = bBottomRightIndex;
}

void imgproc_grayscale( struct Image *input_img, struct Image *output_img ) {
  // TODO: implement
  
  for (int i = 0; i < input_img->height * input_img->width; i++) {
    int32_t value = input_img->data[i];
    int y = (79 * get_r(value) + 128 * get_g(value) + 49 * get_b(value))/256;

    if (y > 255) { //clamping
      y = 255;
    } else if (y < 0) {
      y = 0;
    }

    output_img->data[i] = combineData(y,y,y,get_a(value));
  }
}


// Render an output image containing 4 replicas of the original image,
// refered to as A, B, C, and D in the following diagram:
//
//   +---+---+
//   | A | B |
//   +---+---+
//   | C | D |
//   +---+---+
//
// The width and height of the output image are (respectively) twice
// the width and height of the input image.
//
// A is an exact copy of the original input image. B has only the
// red color component values of the input image, C has only the
// green color component values, and D has only the blue color component
// values.
//
// Each of the copies (A-D) should use the same alpha values as the
// original image.
//
// This transformation always succeeds.
//
// Parameters:
//   input_img - pointer to the input Image
//   output_img - pointer to the output Image (which will have
//                width and height twice the width/height of the
//                input image)
void imgproc_rgb( struct Image *input_img, struct Image *output_img ) {
  // TODO: implement
  int width = input_img->width;
  int height = input_img->height;

  //i think we have to free the previous image first becuase it is dynamically allocated

  //uint32_t * new_image = (uint32_t *)malloc(sizeof(uint32_t) * 4 * height * width); //int32_t new_image[4 * width * height];

  //free(output_img -> data);
  
  for (int y = 0; y < height; y++) { //y is rows
    for (int x = 0; x < width; x++) { //x is cols
        uint32_t p = input_img->data[(y * width) + x];
        
        // top left
        output_img -> data[(y) * (2 * width) + (x)] = p; // ((current y + yshift/height) * (new width)) + (current x + xshift/width)

        // top right
        output_img -> data[(y) * (2 * width) + (width + x)] = combineData(get_r(p), 0, 0, get_a(p)); 
        
        // bottom left
        output_img -> data[(y + height) * (2 * width) + (x)] = combineData(0, get_g(p), 0, get_a(p)); ; 

        // bottom right
        output_img -> data[(y + height) * (2 * width) + (x + width)] = combineData(0, 0, get_b(p), get_a(p)); 
    }
  }
  //output_img->height = input_img->height * 2;
  //output_img->width = input_img->width * 2;
  //output_img->data = new_image;
}

// Render a "faded" version of the input image.
//
// See the assignment description for an explanation of how this transformation
// should work.
//
// This transformation always succeeds.
//
// Parameters:
//   input_img - pointer to the input Image
//   output_img - pointer to the output Image

void imgproc_fade( struct Image *input_img, struct Image *output_img ) {
  // TODO: implement
  int width = input_img->width;
  int height = input_img->height;

  for (int y = 0; y < height; y++) { //y is rows
    for (int x = 0; x < width; x++) { //x is cols
      int32_t p = input_img->data[(y * width) + x];
      int newRed = getFadedComponentValue(y, x, width, height, get_r(p));
      int newGreen = getFadedComponentValue(y, x, width, height, get_g(p));
      int newBlue = getFadedComponentValue(y, x, width, height, get_b(p));
      
      output_img -> data[(y * width) + x] = combineData(newRed, newGreen, newBlue, get_a(p));
    }
  }
}

// Render a "kaleidoscope" transformation of input_img in output_img.
// The input_img must be square, i.e., the width and height must be
// the same. Assume that the input image is divided into 8 "wedges"
// like this:
//
//    +----+----+
//    |\ A |   /|
//    | \  |  / |
//    |  \ | /  |
//    | B \|/   |
//    +----+----+
//    |   /|\   |
//    |  / | \  |
//    | /  |  \ |
//    |/   |   \|
//    +----+----+
//
// The wedge labeled "A" is the one that will be replicated 8 times
// in the output image. In the output image,
//
//    Wedge A will have exactly the same pixels as wedge A in
//    the input image, in the same positions.
//
//    Wedge B will have exactly the same pixels as wedge A, but the
//    column and row are transposed. (I.e., it is a reflection across
//    the diagonal.)
//
//    All other wedges are mirrored versions of A such that the overall
//    image is symmetrical across all four lines dividing the image
//    (vertical split, horizontal split, and the two diagonal splits.)
//
// Parameters:
//   input_img  - pointer to the input Image
//   output_img - pointer to the output Image (in which the transformed
//                pixels should be stored)
//
// Returns:
//   1 if successful, 0 if the transformation fails because the
//   width and height of input_img are not the same.

int imgproc_kaleidoscope( struct Image *input_img, struct Image *output_img ) {
  // TODO: implement
  if (input_img -> width != input_img -> height){
    return 0;
  }

  int32_t actualWidth = input_img -> width;
  int32_t actualHeight = input_img -> height;
  int32_t isOdd = actualWidth % 2;
  int32_t indexingWidth = actualWidth + isOdd;
  int32_t indexingHeight = actualHeight + isOdd;
  
  //if index + 1 is divisible by this number then exclude ie the extended right column
  //if index is greater than this then also no ie the extended bottom row
  int32_t extraRowStartIndex = (indexingWidth) * (indexingHeight - 1);

  for (int32_t y = 0; y < (indexingWidth/2) ; y++){ //row y
    for (int32_t x = 0; x < indexingHeight/2; x++){ //col x
      if (y > x){ //the line is y=x and we want above thus y>x
        continue;
      }
      
      uint32_t p = input_img->data[(y * actualWidth) + x];
      int32_t indexArray[8];
      fillKaleidoscopeIndexArray(indexArray, indexingWidth, indexingHeight, x, y);
      
      for (int i = 0; i < 8; i++){
        int32_t index = indexArray[i];

        if (isOdd){ 
          if ((index >= extraRowStartIndex) || ((index + 1) % indexingWidth == 0)){ //if that index is out of bounds ie in the extended border
            continue;
          } 

          else { //if it is within bounds, need to adjust index to original image
            index = getAdjustedIndex(index, indexingWidth, actualWidth);
          }
        }
        output_img -> data[index] = p;
      }
    }
  }
  return 1;
}