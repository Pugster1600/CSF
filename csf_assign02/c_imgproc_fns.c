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

void imgproc_grayscale( struct Image *input_img, struct Image *output_img ) {
  // TODO: implement
  
  for (int i = 0; i < input_img->height * input_img->width; i++) {
    int32_t value = input_img->data[i];
    int y = (79*get_r(value) + 128*get_g(value) + 49*get_b(value))/256;
    if (y > 255) {
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

  uint32_t * new_image = (uint32_t *)malloc(sizeof(uint32_t) * 4 * height * width);
  //int32_t new_image[4 * width * height];
  if (new_image == NULL){
    return;
  }

  free(output_img -> data);
  
  for (int y = 0; y < height; y++) { //y is rows
    for (int x = 0; x < width; x++) { //x is cols
        uint32_t p = input_img->data[(y * width) + x];
        
        // top left
        new_image[(y) * (2 * width) + (x)] = p; // ((current y + yshift/height) * (new width)) + (current x + xshift/width)

        // top right
        new_image[(y) * (2 * width) + (width + x)] = combineData(get_r(p), 0, 0, get_a(p)); 
        
        // bottom left
        new_image[(y + height) * (2 * width) + (x)] = combineData(0, get_g(p), 0, get_a(p)); ; 

        // bottom right
        new_image[(y + height) * (2 * width) + (x + width)] = combineData(0, 0, get_b(p), get_a(p)); 
    }
  }
  output_img->height = input_img->height * 2;
  output_img->width = input_img->width * 2;
  output_img->data = new_image;
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

int64_t getModifiedComponentValue(int64_t rowIndex, int64_t colIndex, int64_t width, int64_t height, int64_t  color){
  int64_t newColor = (color * getRowTransform(rowIndex, height) * getColumnTransform(colIndex, width)) / ONE_TRILLION;
  return newColor;
}

void imgproc_fade( struct Image *input_img, struct Image *output_img ) {
  // TODO: implement
  int width = input_img->width;
  int height = input_img->height;

  for (int y = 0; y < height; y++) { //y is rows
    for (int x = 0; x < width; x++) { //x is cols
      int32_t p = input_img->data[(y * width) + x];
      int newRed = getModifiedComponentValue(y, x, width, height, get_r(p));
      int newGreen = getModifiedComponentValue(y, x, width, height, get_g(p));
      int newBlue = getModifiedComponentValue(y, x, width, height, get_b(p));
      
      output_img -> data[(y * width) + x] = combineData(newRed, newGreen, newBlue, get_a(p));
      //printf("%lu %lu %lu\n", newRed, newGreen, newBlue);
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
  int32_t width = input_img -> width;
  int32_t height = input_img -> height;

  if (width != height){
    return 0;
  }

  int32_t halfWidth = width / 2;
  int32_t halfHeight = height / 2;

  for (int32_t y = 0; y < halfHeight; y++){ //row y
    for (int32_t x = 0; x < halfWidth; x++){ //col x
      //the line is y=x and we want y>x
      if (y > x){
        continue;
      }

      int32_t aTopLeftIndex = (y * width) + x;
      int32_t bTopLeftIndex = (x * width) + y;

      uint32_t p = input_img->data[aTopLeftIndex];
      uint32_t pixel = combineData(get_r(p), get_g(p), get_b(p), get_a(p));
      
      int32_t widthDistanceFromMiddle = halfWidth - x;
      int32_t heightDistanceFromMiddle = halfHeight - y;
      
      int32_t aTopRightIndex = aTopLeftIndex + (2 * widthDistanceFromMiddle) - 1;
      int32_t bTopRightIndex = bTopLeftIndex + (2 * heightDistanceFromMiddle) - 1;
      int32_t aBottomLeftIndex = aTopLeftIndex + ((2 * heightDistanceFromMiddle - 1) * width);
      int32_t bBottomLeftIndex = bTopLeftIndex + ((2 * widthDistanceFromMiddle - 1) * width);
      int32_t aBottomRightIndex = aBottomLeftIndex + (2 * widthDistanceFromMiddle) - 1;
      int32_t bBottomRightIndex = bBottomLeftIndex + (2 * heightDistanceFromMiddle) - 1;
      
      output_img -> data[aTopLeftIndex] = pixel;
      output_img -> data[bTopLeftIndex] = pixel;
      output_img -> data[aTopRightIndex] = pixel;
      output_img -> data[bTopRightIndex] = pixel;
      output_img -> data[aBottomLeftIndex] = pixel;
      output_img -> data[bBottomLeftIndex] = pixel;
      output_img -> data[aBottomRightIndex] = pixel;
      output_img -> data[bBottomRightIndex] = pixel;

      printf("0: %lu\n", output_img -> data[0]);

      //printf("%d %d %d %d %d %d\n", aTopRightIndex, bTopRightIndex, aBottomLeftIndex, bBottomLeftIndex, aBottomRightIndex, bBottomRightIndex);
    }
  }

  return 1;
}
//scp jshi61@ugradx.cs.jhu.edu:~/CSF/csf_assign02/solution.zip .