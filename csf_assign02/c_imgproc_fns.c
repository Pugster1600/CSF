// C implementations of image processing functions

#include <stdlib.h>
#include <assert.h>
#include "imgproc.h"

// TODO: define your helper functions here

// Convert input pixels to grayscale.
// This transformation always succeeds.
//
// Parameters:
//   input_img  - pointer to the input Image
//   output_img - pointer to the output Image (in which the transformed
//                pixels should be stored)

uint8_t get_r(uint32_t pixel) {
  uint8_t * val = (uint8_t *)&pixel;
  return *(val);
}

uint8_t get_g(uint32_t pixel) {
  uint8_t * val = (uint8_t *)&pixel;
  return *(val+1);
}

uint8_t get_b(uint32_t pixel) {
  uint8_t * val = (uint8_t *)&pixel;
  return *(val+2);
}

uint8_t get_a(uint32_t pixel) {
  uint8_t * val = (uint8_t *)&pixel;
  return *(val+3);
}

uint32_t combineData(uint8_t r, uint8_t g, uint8_t b, uint8_t a){
  return (r<<24) | (g<<16) | (b<<8) | a;
}

void imgproc_grayscale( struct Image *input_img, struct Image *output_img ) {
  // TODO: implement
  
  for (int i = 0; i < input_img->height * input_img->width; i++) {
    uint32_t value = input_img->data[i];
    uint8_t y = (79*get_r(value) + 128*get_g(value) + 49*get_b(value))/256;
    output_img->data[i] =combineData(y,y,y,get_a(value));
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

  uint32_t * new_image = (uint32_t *)malloc(sizeof(uint32_t) * 4 * height * width);
  if (new_image == NULL){
    return;
  }
  
  //uint32_t new_image[4 * width * height];
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
        new_image[(y + height) * (2 * width) + (x + width)] = combineData(0, 0, get_b(p), get_a(p)); ;
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
void imgproc_fade( struct Image *input_img, struct Image *output_img ) {
  // TODO: implement
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
  return 0;
}
