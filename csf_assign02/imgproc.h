// Header for image processing API functions (imgproc_mirror_h, etc.)
// as well as any helper functions they rely on.

#ifndef IMGPROC_H
#define IMGPROC_H

#include "image.h" // for struct Image and related functions

// Convert input pixels to grayscale.
// This transformation always succeeds.
//
// Parameters:
//   input_img  - pointer to the input Image
//   output_img - pointer to the output Image (in which the transformed
//                pixels should be stored)
void imgproc_grayscale( struct Image *input_img, struct Image *output_img );

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
void imgproc_rgb( struct Image *input_img, struct Image *output_img );

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
void imgproc_fade( struct Image *input_img, struct Image *output_img );

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
int imgproc_kaleidoscope( struct Image *input_img, struct Image *output_img );

// TODO: add prototypes for your helper functions

//functions to get color and combine them
uint8_t get_r(int32_t pixel);
uint8_t get_g(int32_t pixel);
uint8_t get_b(int32_t pixel);
uint8_t get_a(int32_t pixel);
uint32_t combineData(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

//functions for fade
int64_t gradient(int64_t num);
int64_t getRowTransform(int64_t rowIndex, int64_t height);
int64_t getColumnTransform(int64_t rowIndex, int64_t width);
int64_t getFadedComponentValue(int64_t rowIndex, int64_t colIndex, int64_t width, int64_t height, int64_t color);

//function to get adjusted index
int32_t getAdjustedIndex(int32_t index, int32_t indexingWidth, int32_t actualWidth);

void fillKaleidoscopeIndexArray(int32_t * indexArray, int32_t indexingWidth, int32_t indexingHeight, int32_t x, int32_t y);

#endif // IMGPROC_H
