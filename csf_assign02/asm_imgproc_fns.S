/*
 * x86-64 assembly language implementations of functions
 */

	.section .text

/* Offsets of struct Image fields */
#define IMAGE_WIDTH_OFFSET   0
#define IMAGE_HEIGHT_OFFSET  4
#define IMAGE_DATA_OFFSET    8

/*
 * TODO: define your helper functions here.
 * Don't forget to use the .globl directive to make
 * them visible to the test code.
 */

/*
 * %edi (from %rdi) holds the uint32_t pixel value
 * return the value to %rax's %al because its a byte
 */
 .globl get_r
get_r:
	shr $24, %edi //shifts eax right 24 -> r is now in al
	movb %dil, %al 
	ret

/*
 * %edi (from %rdi) holds the uint32_t pixel value
 * return the value to %rax's %al because its a byte
 */
 .globl get_g
get_g:
	shr $16, %edi; //shift eax by 16 -> g is now in al
	movb %dil, %al
	ret

/*
 * %edi (from %rdi) holds the uint32_t pixel value
 * return the value to %rax's %al because its a byte
 */
 .globl get_b
get_b:
	shr $8, %edi //shift eax right by 8 -> b is now in al
	movb %dil, %al
	ret

/*
 * %edi (from %rdi) holds the uint32_t pixel value
 * return the value to %rax's %al because its a byte
 */
 .globl get_a
get_a:
	movb %dil, %al
	ret

/**
 * r passed into %dil -> %rdi
 * g passed into %sil -> %rsi
 * b passed into %dl-> %rdx
 * a passed into %cl -> %rcx
 */
	.global combineData
combineData:
	shl $24, %edi //shift red << 24 -> little endian
	shl $16, %esi //shift green << 16
	shl $8, %edx //shift blue << 8

	or %esi, %edi //or green and red, storing in edi
	or %edx, %edi //or blue and edi, storing in edi
	or %ecx, %edi //or alpha and edi, storing in edi
	movl %edi, %eax //move edi to return
	ret

/**
 * rdi -> index
 * rsi -> totalSize
 */
	.globl getMappedPixel
getMappedPixel:
	imulq $2000000000, %rdi 
	imulq $1000000, %rsi

	movq $0, %rdx //64 bit dividend
	movq %rdi, %rax 
	divq %rsi // rax / rsi -> stored in rax

	ret

/**
 * rdi -> stores num
 */
	.globl gradient
gradient:
	subq $1000, %rdi
	imulq %rdi, %rdi
	movq $1000000, %rax
	subq %rdi, %rax
	ret

/*
 * Implementations of API functions
 */

/*
 * Render an output image containing 4 copies of the original image,
 * refered to as A, B, C, and D in the following diagram:
 *
 *   +---+---+
 *   | A | B |
 *   +---+---+
 *   | C | D |
 *   +---+---+
 *
 * The width and height of the output image are (respectively) twice
 * the width and height of the input image.
 *
 * A is an exact copy of the original input image. B has only the
 * red color component values of the input image, C has only the
 * green color component values, and D has only the blue color component
 * values.
 *
 * Each of the copies (A-D) should use the same alpha values as the
 * original image.
 *
 * This transformation always succeeds.
 *
 * Parameters:
 *   %rdi - pointer to the input Image
 *   %rsi - pointer to the output Image (which will have
 *          width and height twice the width/height of the
 *          input image)
 */
	.globl imgproc_rgb
imgproc_rgb:
	/* TODO: implement */
	ret

/*
 * void imgproc_grayscale( struct Image *input_img, struct Image *output_img );
 *
 * Transform image by converting each pixel to grayscale.
 * This transformation always succeeds.
 *
 * Parameters:
 *   %rdi - pointer to original struct Image
 *   %rsi - pointer to output struct Image
 */
	.globl imgproc_grayscale
imgproc_grayscale:
	/* TODO: implement */
	ret

/*
 * Render a "faded" version of the input image.
 *
 * See the assignment description for an explanation of how this transformation
 * should work.
 *
 * This transformation always succeeds.
 *
 * Parameters:
 *   %rdi - pointer to the input Image
 *   %rsi - pointer to the output Image
 */
	.globl imgproc_fade
imgproc_fade:
	/* TODO: implement */
	ret

/*
 * Render a "kaleidoscope" transformation of input_img in output_img.
 * The input_img must be square, i.e., the width and height must be
 * the same. Assume that the input image is divided into 8 "wedges"
 * like this:
 *
 *    +----+----+
 *    |\ A |   /|
 *    | \  |  / |
 *    |  \ | /  |
 *    | B \|/   |
 *    +----+----+
 *    |   /|\   |
 *    |  / | \  |
 *    | /  |  \ |
 *    |/   |   \|
 *    +----+----+
 *
 * The wedge labeled "A" is the one that will be replicated 8 times
 * in the output image. In the output image,
 *
 *    Wedge A will have exactly the same pixels as wedge A in
 *    the input image, in the same positions.
 *
 *    Wedge B will have exactly the same pixels as wedge A, but the
 *    column and row are transposed. (I.e., it is a reflection across
 *    the diagonal.)
 *
 *    All other wedges are mirrored versions of A such that the overall
 *    image is symmetrical across all four lines dividing the image
 *    (vertical split, horizontal split, and the two diagonal splits.)
 *
 * Parameters:
 *   input_img  - pointer to the input Image
 *   output_img - pointer to the output Image (in which the transformed
 *                pixels should be stored)
 *
 * Returns:
 *   1 if successful, 0 if the transformation fails because the
 *   width and height of input_img are not the same.
 */
	.globl imgproc_kaleidoscope
imgproc_kaleidoscope:
	/* TODO: implement */
	ret

	/* This avoids linker warning about executable stack */
.section .note.GNU-stack,"",@progbits

/*
vim:ft=gas:
*/
