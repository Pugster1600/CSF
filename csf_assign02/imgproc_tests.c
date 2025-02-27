#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tctest.h"
#include "imgproc.h"

// An expected color identified by a (non-zero) character code.
// Used in the "struct Picture" data type.
struct ExpectedColor {
  char c;
  uint32_t color;
};

// Type representing a "picture" of an expected image.
// Useful for creating a very simple Image to be accessed
// by test functions.
struct Picture {
  struct ExpectedColor colors[40];
  int width, height;
  const char *data;
};

// Some "basic" colors to use in test struct Pictures.
// Note that the ranges '1'-'5', 'A'-'E', and 'P'-'T'
// are (respectively) colors 'r','g','b','c', and 'm'
// with just the red, green, and blue color component values,
// which is useful for defining the expected image from the
// "rgb" transformation.
#define TEST_COLORS \
    { \
      { ' ', 0x000000FF }, \
      { 'r', 0xFF0000FF }, \
      { 'g', 0x00FF00FF }, \
      { 'b', 0x0000FFFF }, \
      { 'c', 0x00FFFFFF }, \
      { 'm', 0xFF00FFFF }, \
      { '1', 0xFF0000FF }, \
      { '2', 0x000000FF }, \
      { '3', 0x000000FF }, \
      { '4', 0x000000FF }, \
      { '5', 0xFF0000FF }, \
      { 'A', 0x000000FF }, \
      { 'B', 0x00FF00FF }, \
      { 'C', 0x000000FF }, \
      { 'D', 0x00FF00FF }, \
      { 'E', 0x000000FF }, \
      { 'P', 0x000000FF }, \
      { 'Q', 0x000000FF }, \
      { 'R', 0x0000FFFF }, \
      { 'S', 0x0000FFFF }, \
      { 'T', 0x0000FFFF }, \
    }

// Expected "basic" colors after grayscale transformation
#define TEST_COLORS_GRAYSCALE \
    { \
      { ' ', 0x000000FF }, \
      { 'r', 0x4E4E4EFF }, \
      { 'g', 0x7F7F7FFF }, \
      { 'b', 0x303030FF }, \
      { 'c', 0xB0B0B0FF }, \
      { 'm', 0x7F7F7FFF }, \
    }

// Data type for the test fixture object.
// This contains data (including Image objects) that
// can be accessed by test functions. This is useful
// because multiple test functions can access the same
// data (so you don't need to create/initialize that
// data multiple times in different test functions.)
typedef struct {
  // smiley-face picture
  struct Picture smiley_pic;

  // original smiley-face Image object
  struct Image *smiley;

  // empty Image object to use for output of
  // transformation on smiley-face image
  struct Image *smiley_out;

  // empty Image object to use for output of rgb
  // transformation on smiley-face image
  struct Image *smiley_rgb_out;

  // a square image (same width/height) to use as a test
  // for the kaleidoscope transformation
  struct Picture sq_test_pic;

  // original square Image object
  struct Image *sq_test;

  // empty image for output of kaleidoscope transformation
  struct Image *sq_test_out;

} TestObjs;

// Functions to create and clean up a test fixture object
TestObjs *setup( void );
void cleanup( TestObjs *objs );

// Helper functions used by the test code
struct Image *picture_to_img( const struct Picture *pic );
uint32_t lookup_color(char c, const struct ExpectedColor *colors);
bool images_equal( struct Image *a, struct Image *b );
void destroy_img( struct Image *img );

// Test functions
void test_rgb_basic( TestObjs *objs );
void test_grayscale_basic( TestObjs *objs );
void test_fade_basic( TestObjs *objs );
void test_kaleidoscope_basic( TestObjs *objs );

void testGetColor();
void testCombineData();
void testGetMappedPixel();
void testGradient();
void testGetFadedComponentValue();
void testGetAdjustedIndex();
void testFillKaleidoscopeIndexArray();

// TODO: add prototypes for additional test functions

void test_odd( TestObjs *objs ) {
  struct Picture sq_test_kaleidoscope_expected_pic = {
    TEST_COLORS,
    13, // width
    13, // height
    "rrrrrrrrrrrr"
    "rggggggggggr"
    "rgbbbbbbbbgr"
    "rgbmmmmmmbgr"
    "rgbmccccmbgr"
    "rgbmcrrcmbgr"
    "rgbmcrrcmbgr"
    "rgbmccccmbgr"
    "rgbmmmmmmbgr"
    "rgbbbbbbbbgr"
    "rggggggggggr"
    "rrrrrrrrrrrr"
  };

  imgproc_kaleidoscope( objs->sq_test, objs->sq_test_out );
}

int main( int argc, char **argv ) {
  // allow the specific test to execute to be specified as the
  // first command line argument
  if ( argc > 1 )
    tctest_testname_to_execute = argv[1];

  TEST_INIT();

  // Run tests.
  // Make sure you add additional TEST() macro invocations
  // for any additional test functions you add.
  TEST( test_rgb_basic );
  TEST( test_grayscale_basic );
  //TEST( test_fade_basic );
  //TEST( test_kaleidoscope_basic );
  TEST (test_odd);

  TEST (testGetColor);
  TEST (testCombineData);
  TEST (testGetMappedPixel);
  TEST (testGradient);
  TEST (testGetFadedComponentValue);
  TEST (testGetAdjustedIndex);
  TEST (testFillKaleidoscopeIndexArray);

  TEST_FINI();
}

void testGetColor(){
  //r g b a, 32 - 0 -> [0] = 7-0 little endian
  uint32_t value = 0xaabbccdd;
  uint32_t red = get_r(value); //sanity check: should be smallest value
  uint32_t green = get_g(value);
  uint32_t blue = get_b(value);
  uint32_t alpha = get_a(value);
  ASSERT (red == 0xaa);
  ASSERT (green == 0xbb);
  ASSERT (blue == 0xcc);
  ASSERT (alpha == 0xdd);

  //basic get color tests
  value = 0xdeadbeef;
  red = get_r(value); 
  green = get_g(value);
  blue = get_b(value); 
  alpha = get_a(value); 

  ASSERT (red == 0xde);
  ASSERT (green == 0xad);
  ASSERT (blue == 0xbe);
  ASSERT (alpha == 0xef);

  //basic get color tests
  value = 0x12345678;
  red = get_r(value);
  green = get_g(value);
  blue = get_b(value);
  alpha = get_a(value);

  ASSERT (red == 0x12);
  ASSERT (green == 0x34);
  ASSERT (blue == 0x56);
  ASSERT (alpha == 0x78);
}

void testCombineData(){
  //basic combine data test -> get_color then combining them
  uint32_t value = 0xaabbccdd;
  uint32_t red = get_r(value);
  uint32_t green = get_g(value);
  uint32_t blue = get_b(value);
  uint32_t alpha = get_a(value);
  uint32_t combined = combineData(red, green, blue, alpha);
  ASSERT(value == combined);

  value = 0xdeadbeef;
  red = get_r(value);
  green = get_g(value);
  blue = get_b(value);
  alpha = get_a(value);
  combined = combineData(red, green, blue, alpha);
  ASSERT(value == combined);

  value = 0x12345678;
  red = get_r(value);
  green = get_g(value);
  blue = get_b(value);
  alpha = get_a(value);
  combined = combineData(red, green, blue, alpha);
  ASSERT(value == combined);
}

void testGetMappedPixel(){
  //test first pixel in row
  int64_t rowIndex = 0;
  int64_t totalSize = 100;
  int64_t firstPixel = getMappedPixel(rowIndex, totalSize);
  ASSERT(firstPixel == 0); //if its the first one then no fade

  //test final pixel in row
  rowIndex = totalSize;
  int64_t finalPixel = getMappedPixel(rowIndex, totalSize);
  ASSERT(finalPixel == 2000); //if its final transform then 2k (0-2k)

  //test pixel in the middle of the row
  rowIndex = totalSize / 2;
  int64_t middlePixel = getMappedPixel(rowIndex, totalSize);
  ASSERT(middlePixel == 1000); //2000 * ratio

  //test pixel at the quarter location
  rowIndex = totalSize / 4;
  int64_t testPixel = getMappedPixel(rowIndex, totalSize);
  ASSERT(testPixel == 500); //0.25 * 2000
}

void testGradient(){
  //test gradient for pixel at the first row
  int64_t rowIndex = 0;
  int64_t totalSize = 100;
  int64_t firstPixel = getMappedPixel(rowIndex, totalSize);
  ASSERT(firstPixel == 0); //if its the first one then no fade
  int64_t firstGradient = gradient(firstPixel);
  ASSERT (firstGradient == 0); //first grad = 0

  //test gradient for pixel at the final row
  rowIndex = totalSize;
  int64_t finalPixel = getMappedPixel(rowIndex, totalSize);
  ASSERT(finalPixel == 2000); //if its final transform then 2k (0-2k)
  int64_t finalGradient = gradient(finalPixel);
  ASSERT(finalGradient == 0); //final grad = 0

  //test gradient for the row in the middle
  rowIndex = totalSize / 2;
  int64_t middlePixel = getMappedPixel(rowIndex, totalSize);
  ASSERT(middlePixel == 1000); //1000 at the middle
  int64_t middleGradient = gradient(middlePixel);
  ASSERT(middleGradient == 1000000); //max value at the middle

  //test gradient for the row in the qarter position
  rowIndex = totalSize / 4;
  int64_t testPixel = getMappedPixel(rowIndex, totalSize);
  ASSERT(testPixel == 500); 
  int64_t testPixelGradient = gradient(testPixel);
  ASSERT(testPixelGradient == (1000000 - 250000));
}

void testGetFadedComponentValue(){
  int64_t rowIndex = 0;
  int64_t colIndex = 0;
  int64_t width = 100;
  int64_t height = 100;
  int64_t color = 255;

  //test get fade for the first column and row
  int64_t firstRowFirstColumnFadedValue = getFadedComponentValue(rowIndex, colIndex, width, height, color);
  ASSERT(firstRowFirstColumnFadedValue == 0);

  //test fade for middle of row, last column
  rowIndex = height / 2;
  colIndex = width;
  int64_t middleRowFinalColumnFadedValue = getFadedComponentValue(rowIndex, colIndex, width, height, color);
  ASSERT(middleRowFinalColumnFadedValue == 0);

  //test fade for final row, middle column
  rowIndex = height;
  colIndex = width / 2;
  int64_t finalRowMiddleColumnFadedValue = getFadedComponentValue(rowIndex, colIndex, width, height, color);
  ASSERT(finalRowMiddleColumnFadedValue == 0);

  //test fade for final row and final column
  colIndex = width;
  int64_t finalRowFinalColumnFadedValue = getFadedComponentValue(rowIndex, colIndex, width, height, color);
  ASSERT(finalRowFinalColumnFadedValue == 0);

  //test fade for final row and final column
  colIndex = width/2;
  rowIndex = height/2;
  int64_t middleRowMiddleColumnFadedValue = getFadedComponentValue(rowIndex, colIndex, width, height, color);
  ASSERT(middleRowMiddleColumnFadedValue == color);

  //test fade for middle column and quarter row
  colIndex = width / 2;
  rowIndex = height / 4;
  int64_t testFadedValue = getFadedComponentValue(rowIndex, colIndex, width, height, color);
  ASSERT(testFadedValue == (uint64_t)(color * 0.75)); //750k / 1mil
}

//this test is to get the new index of the same x,y coordinates in 1d array from a different 1d array
void testGetAdjustedIndex(){
  int32_t index = 0;
  int32_t indexingWidth = 20;
  int32_t actualWidth = 10;

  //test getting the index = 0
  int32_t firstIndex = getAdjustedIndex(index, indexingWidth, actualWidth);
  ASSERT(firstIndex == 0);

  //row 10, col 10 aka the final index in a 10x10 grid
  int row = 10;
  int col = 10;
  index = (indexingWidth * (row - 1)) + (col - 1);
  int32_t finalIndex = getAdjustedIndex(index, indexingWidth, actualWidth);
  ASSERT(finalIndex == 99);

  //row 5, col 5 aka the middle of the grid
  row = 5;
  col = 5;
  index = (indexingWidth * (row - 1)) + (col - 1);
  int32_t middleIndex = getAdjustedIndex(index, indexingWidth, actualWidth);
  ASSERT(middleIndex == 44);

  //test getting the index at a random position, (3,4)
  row = 3; 
  col = 4;
  index = (indexingWidth * (row - 1)) + (col - 1);
  int32_t testIndex = getAdjustedIndex(index, indexingWidth, actualWidth);
  ASSERT(testIndex == 23); //row 3, col 4 of a 10x10 grid
}

//this fills the array with the 8 index locations of the pixel (each pixel gets copied 8 times)
void testFillKaleidoscopeIndexArray(){
  int32_t indexArray[8];
  int32_t width = 10; //and height
  int32_t x = 0; //x index
  int32_t y = 0; //y index

  //test with the first index 0,0
  fillKaleidoscopeIndexArray(indexArray, width, x, y);
  int32_t aTopLeftIndex = indexArray[0];
  int32_t bTopLeftIndex = indexArray[1];
  int32_t aTopRightIndex = indexArray[2];
  int32_t bTopRightIndex = indexArray[3];
  int32_t aBottomLeftIndex = indexArray[4];
  int32_t bBottomLeftIndex = indexArray[5];
  int32_t aBottomRightIndex = indexArray[6];
  int32_t bBottomRightIndex = indexArray[7];

  ASSERT (aTopLeftIndex == 0);
  ASSERT (bTopLeftIndex == 0);
  ASSERT (aTopRightIndex == 9);
  ASSERT (bTopRightIndex == 9);
  ASSERT (aBottomLeftIndex == 90);
  ASSERT (bBottomLeftIndex == 90);
  ASSERT (aBottomRightIndex == 99);
  ASSERT (bBottomRightIndex == 99);

  //test it at x=4, y=4
  x = 4; 
  y = 4; 
  fillKaleidoscopeIndexArray(indexArray, width, x, y);
  aTopLeftIndex = indexArray[0];
  bTopLeftIndex = indexArray[1];
  aTopRightIndex = indexArray[2];
  bTopRightIndex = indexArray[3];
  aBottomLeftIndex = indexArray[4];
  bBottomLeftIndex = indexArray[5];
  aBottomRightIndex = indexArray[6];
  bBottomRightIndex = indexArray[7];
  ASSERT (aTopLeftIndex == 44);
  ASSERT (bTopLeftIndex == 44);
  ASSERT (aTopRightIndex == 45);
  ASSERT (bTopRightIndex == 45);
  ASSERT (aBottomLeftIndex == 54);
  ASSERT (bBottomLeftIndex == 54);
  ASSERT (aBottomRightIndex == 55);
  ASSERT (bBottomRightIndex == 55);
  
  //test it at first row, middle column
  x = 4; 
  y = 0; 
  fillKaleidoscopeIndexArray(indexArray, width, x, y);
  aTopLeftIndex = indexArray[0];
  bTopLeftIndex = indexArray[1];
  aTopRightIndex = indexArray[2];
  bTopRightIndex = indexArray[3];
  aBottomLeftIndex = indexArray[4];
  bBottomLeftIndex = indexArray[5];
  aBottomRightIndex = indexArray[6];
  bBottomRightIndex = indexArray[7];
  ASSERT (aTopLeftIndex == 4);
  ASSERT (bTopLeftIndex == 40);
  ASSERT (aTopRightIndex == 5);
  ASSERT (bTopRightIndex == 49);
  ASSERT (aBottomLeftIndex == 94);
  ASSERT (bBottomLeftIndex == 50);
  ASSERT (aBottomRightIndex == 95);
  ASSERT (bBottomRightIndex == 59);

  //test fill with a more random coordinate (3,2)
  x = 3;
  y = 2; 
  fillKaleidoscopeIndexArray(indexArray, width, x, y);
  aTopLeftIndex = indexArray[0];
  bTopLeftIndex = indexArray[1];
  aTopRightIndex = indexArray[2];
  bTopRightIndex = indexArray[3];
  aBottomLeftIndex = indexArray[4];
  bBottomLeftIndex = indexArray[5];
  aBottomRightIndex = indexArray[6];
  bBottomRightIndex = indexArray[7];
  ASSERT (aTopLeftIndex == 23);
  ASSERT (bTopLeftIndex == 32);
  ASSERT (aTopRightIndex == 26);
  ASSERT (bTopRightIndex == 37);
  ASSERT (aBottomLeftIndex == 73);
  ASSERT (bBottomLeftIndex == 62);
  ASSERT (aBottomRightIndex == 76);
  ASSERT (bBottomRightIndex == 67);
}
////////////////////////////////////////////////////////////////////////
// Test fixture setup/cleanup functions
////////////////////////////////////////////////////////////////////////

TestObjs *setup( void ) {
  TestObjs *objs = (TestObjs *) malloc( sizeof(TestObjs) );

  struct Picture smiley_pic = {
    TEST_COLORS,
    16, // width
    10, // height
    "    mrrrggbc    "
    "   c        b   "
    "  r   r  b   c  "
    " b            b "
    " b            r "
    " g   b    c   r "
    "  c   ggrb   b  "
    "   m        c   "
    "    gggrrbmc    "
    "                "
  };
  objs->smiley_pic = smiley_pic;
  objs->smiley = picture_to_img( &smiley_pic );

  objs->smiley_out = (struct Image *) malloc( sizeof( struct Image ) );
  img_init( objs->smiley_out, objs->smiley->width, objs->smiley->height );

  // the rgb transformation requires an output Image with width
  // and height twice the original image
  objs->smiley_rgb_out = (struct Image *) malloc( sizeof( struct Image ) );
  img_init( objs->smiley_rgb_out, objs->smiley->width * 2, objs->smiley->height * 2 );

  struct Picture sq_test_pic = {
    TEST_COLORS,
    13, // width
    13, // height
    "rrrrrrr      "
    " gggggg      "
    "  bbbbb      "
    "   mmmm      "
    "    ccc      "
    "     rr      "
    "      r      "
    "             "
    "             "
    "             "
    "             "
    "             "
    "             "
  };
  objs->sq_test_pic = sq_test_pic;
  objs->sq_test = picture_to_img( &sq_test_pic );
  objs->sq_test_out = (struct Image *) malloc( sizeof( struct Image ) );
  img_init( objs->sq_test_out, objs->sq_test->width, objs->sq_test->height );

  return objs;
}

void cleanup( TestObjs *objs ) {
  destroy_img( objs->smiley );
  destroy_img( objs->smiley_out );
  destroy_img( objs->smiley_rgb_out );
  destroy_img( objs->sq_test );
  destroy_img( objs->sq_test_out );

  free( objs );
}

////////////////////////////////////////////////////////////////////////
// Test code helper functions
////////////////////////////////////////////////////////////////////////

struct Image *picture_to_img( const struct Picture *pic ) {
  struct Image *img;

  img = (struct Image *) malloc( sizeof(struct Image) );
  img_init( img, pic->width, pic->height );

  for ( int i = 0; i < pic->height; ++i ) {
    for ( int j = 0; j < pic->width; ++j ) {
      int index = i * img->width + j;
      uint32_t color = lookup_color( pic->data[index], pic->colors );
      img->data[index] = color;
    }
  }

  return img;
}

uint32_t lookup_color(char c, const struct ExpectedColor *colors) {
  for (int i = 0; ; i++) {
    assert(colors[i].c != 0);
    if (colors[i].c == c) {
      return colors[i].color;
    }
  }
}

// Returns true IFF both Image objects are identical
bool images_equal( struct Image *a, struct Image *b ) {
  if ( a->width != b->width || a->height != b->height )
    return false;

  for ( int i = 0; i < a->height; ++i )
    for ( int j = 0; j < a->width; ++j ) {
      int index = i*a->width + j;
      if ( a->data[index] != b->data[index] )
        return false;
    }

  return true;
}

void destroy_img( struct Image *img ) {
  if ( img != NULL )
    img_cleanup( img );
  free( img );
}

////////////////////////////////////////////////////////////////////////
// Test functions
////////////////////////////////////////////////////////////////////////

void test_rgb_basic( TestObjs *objs ) {
  struct Picture smiley_rgb_pic = {
    TEST_COLORS,
    32, // width
    20, // height
    "    mrrrggbc        51112234    "
    "   c        b      4        3   "
    "  r   r  b   c    1   1  3   4  "
    " b            b  3            3 "
    " b            r  3            1 "
    " g   b    c   r  2   3    4   1 "
    "  c   ggrb   b    4   2213   3  "
    "   m        c      5        4   "
    "    gggrrbmc        22211354    "
    "                                "
    "    EAAABBCD        TPPPQQRS    "
    "   D        C      S        R   "
    "  A   A  C   D    P   P  R   S  "
    " C            C  R            R "
    " C            A  R            P "
    " B   C    D   A  Q   R    S   P "
    "  D   BBAC   C    S   QQPR   R  "
    "   E        D      T        S   "
    "    BBBAACED        QQQPPRTS    "
    "                                "
  };

  struct Image *smiley_rgb_expected = picture_to_img( &smiley_rgb_pic );

  imgproc_rgb( objs->smiley, objs->smiley_rgb_out );

  ASSERT( images_equal( smiley_rgb_expected, objs->smiley_rgb_out ) );

  destroy_img( smiley_rgb_expected );
}

void test_grayscale_basic( TestObjs *objs ) {
  struct Picture smiley_grayscale_pic = {
    TEST_COLORS_GRAYSCALE,
    16, // width
    10, // height
    "    mrrrggbc    "
    "   c        b   "
    "  r   r  b   c  "
    " b            b "
    " b            r "
    " g   b    c   r "
    "  c   ggrb   b  "
    "   m        c   "
    "    gggrrbmc    "
    "                "
  };

  struct Image *smiley_grayscale_expected = picture_to_img( &smiley_grayscale_pic );

  imgproc_grayscale( objs->smiley, objs->smiley_out );

  ASSERT( images_equal( smiley_grayscale_expected, objs->smiley_out ) );

  destroy_img( smiley_grayscale_expected );
}

void test_fade_basic( TestObjs *objs ) {
  struct Picture smiley_fade_expected_pic = {
    {
      { ' ', 0x000000ff },
      { 'a', 0x000032ff },
      { 'b', 0x000039ff },
      { 'c', 0x000044ff },
      { 'd', 0x00005dff },
      { 'e', 0x000095ff },
      { 'f', 0x0000a0ff },
      { 'g', 0x0000dbff },
      { 'h', 0x0000f0ff },
      { 'i', 0x003737ff },
      { 'j', 0x003b00ff },
      { 'k', 0x006363ff },
      { 'l', 0x006b6bff },
      { 'm', 0x007a00ff },
      { 'n', 0x008c00ff },
      { 'o', 0x008c8cff },
      { 'p', 0x009900ff },
      { 'q', 0x00a0a0ff },
      { 'r', 0x00e500ff },
      { 's', 0x00efefff },
      { 't', 0x00f000ff },
      { 'u', 0x470000ff },
      { 'v', 0x6b0000ff },
      { 'w', 0x6f0000ff },
      { 'x', 0x820082ff },
      { 'y', 0x990000ff },
      { 'z', 0x990099ff },
      { 'A', 0xa00000ff },
      { 'B', 0xa30000ff },
      { 'C', 0xf40000ff },
    },
    16, 10, // width and height
    "                "
    "   i        c   "
    "  u   y  f   k  "
    " a            d "
    " b            v "
    " j   g    s   w "
    "  l   rtCh   e  "
    "   x        q   "
    "    mnpABfzo    "
    "                "
  };

  struct Image *smiley_fade_expected = picture_to_img( &smiley_fade_expected_pic );

  imgproc_fade( objs->smiley, objs->smiley_out );

  ASSERT( images_equal( smiley_fade_expected, objs->smiley_out ) );

  destroy_img( smiley_fade_expected );
}

void test_kaleidoscope_basic( TestObjs *objs ) {
  struct Picture sq_test_kaleidoscope_expected_pic = {
    TEST_COLORS,
    12, // width
    12, // height
    "rrrrrrrrrrrr"
    "rggggggggggr"
    "rgbbbbbbbbgr"
    "rgbmmmmmmbgr"
    "rgbmccccmbgr"
    "rgbmcrrcmbgr"
    "rgbmcrrcmbgr"
    "rgbmccccmbgr"
    "rgbmmmmmmbgr"
    "rgbbbbbbbbgr"
    "rggggggggggr"
    "rrrrrrrrrrrr"
  };

  struct Image *sq_test_kaleidoscope_expected = picture_to_img( &sq_test_kaleidoscope_expected_pic );

  imgproc_kaleidoscope( objs->sq_test, objs->sq_test_out );

  ASSERT( images_equal( sq_test_kaleidoscope_expected, objs->sq_test_out ) );

  destroy_img( sq_test_kaleidoscope_expected );
}