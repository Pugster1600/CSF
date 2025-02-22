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
 * Used to get the 8-bit red value from pixel
 * Register used: 
 * %edi - uint32_t pixel
 *
 * Stack memroy:
 * none
 *
 * returns the red value to %al
 */
 .globl get_r
get_r:
	xor %rax, %rax 						 /*clear the entire thing*/
	shr $24, %edi               /*get bits 31-24 in little endian format*/
	movb %dil, %al              /*move the 8-bit value to 8-bit return reg*/
	ret

/*
 * Used to get the 8-bit green value from pixel
 * Register used: 
 * %edi - uint32_t pixel
 *
 * Stack memroy:
 * none
 *
 * returns the green value to %al
 */
 .globl get_g
get_g:
	xor %rax, %rax 						 /*clear the entire thing*/
	shr $16, %edi;             /*get bits 23-16 in %dil*/
	movb %dil, %al             /*move the 8-bit value to 8-bit return reg*/
	ret

/*
 * Used to get the 8-bit blue value from pixel
 * Register used: 
 * %edi - uint32_t pixel
 *
 * Stack memroy:
 * none
 *
 * returns the blue value to %al
 */
 .globl get_b
get_b:
	xor %rax, %rax 						 /*clear the entire thing*/
	shr $8, %edi               /*get bits 15-8 in %dil*/
	movb %dil, %al             /*move the 8-bit value to 8-bit return reg*/
	ret

/*
 * Used to get the 8-bit alpha value from pixel
 * Register used: 
 * %edi - uint32_t pixel
 *
 * Stack memory:
 * none
 *
 * returns the alpha value to %al
 */
 .globl get_a
get_a:
	xor %rax, %rax 						 /*clear the entire thing*/
	movb %dil, %al         /*move the 8-bit value to 8-bit return reg*/
	ret

/*
 * Used to get the combined 32-bit pixel value consistening of rgba
 * Register used: 
 * %dil (%rdi) - uint8_t red
 * %sil (%rsi) - uint8_t green
 * %dl (%rdx) - uint8_t blue
 * %cl (%rcx) - uint8_t alpha
 *
 * Stack memroy:
 * none
 *
 * returns the uint32_t combined value in to %eax
 */
	.global combineData
combineData:
	shl $24, %edi          /*move red to bits 31-24 b/c little endian*/
	shl $16, %esi          /*move green to bits 23-16*/
	shl $8, %edx           /*move green to bits 15-8*/

	or %esi, %edi          /*or with red*/
	or %edx, %edi 
	or %ecx, %edi
	movl %edi, %eax        /*move the final red into eax*/
	ret

/*
 * Used to get the pixel value mapping from 0-2000
 * Register used: 
 * %rdi - int64_t index
 * %rsi - int64_t totalSize
 *
 * Stack memroy:
 * none
 *
 * returns the pixel value mapping in %rax
 */
	.globl getMappedPixel
getMappedPixel:
	imulq $2000000000, %rdi  /*index * 2 billion -> numerator*/
	imulq $1000000, %rsi     /*totalSize * 1 million -> denominator*/

	movq $0, %rdx            /*get 64 bit result*/
	movq %rdi, %rax          /*move numerator to return reg*/
	divq %rsi                /*rax / rsi*/

	ret

/*
 * Used to get the gradient of an input number between 0-2000
 * Register used: 
 * %rdi - int64_t num
 * %rsi - int64_t totalSize
 *
 * Stack memroy:
 * none
 *
 * returns the gradient in %rax
 */
	.globl gradient
gradient:
	subq $1000, %rdi             /*num - 1000*/
	imulq %rdi, %rdi             /*(num - 1000) ** 2*/
	movq $1000000, %rax          /*move 1mil into rax*/
	subq %rdi, %rax              /*1million - (num - 1000) ** 2*/
	ret

/**
 * Returns the faded component value based on the index, amount in row/col and color value
 * 
 * Parameters: 
 * %rdi - int64_t rowIndex
 * %rsi - int64_t colIndex
 * %rdx - int64_t width
 * %rcx - int64_t height
 * %r8 - int8_t color
 *
 * Stack memory:
 * -8(%rbp) - %r12
 * -16(%rbp) - %r13 
 * -24(%rbp) - %r14
 * -32(%rbp) - %r15
 * -40(%rbp) - %rdi - rowIndex
 * -48(%rbp) - %rsi - colIndex
 * -56(%rbp) - %rdx - width
 * -64(%rbp) - %rcx - height
 * -72(%rbp) - %r8 - color
 */
	.globl getFadedComponentValue
getFadedComponentValue:
	pushq %r12                      /*Saving callee registers*/
	pushq %r13
	pushq %r14
	pushq %r15

	/*calling getMapped Pixel*/
	pushq %rdi
	pushq %rsi
	pushq %rdx
	pushq %rcx
	pushq %r8
	movq %rdi, %rdi                 /*moving rowIndex for arg1*/
	movq %rcx, %rsi                 /*moving height for arg2*/
	call getMappedPixel
	movq %rax, %r12                 /*r12 - mappedRow var*/
	popq %r8 
	popq %rcx
	popq %rdx
	popq %rsi
	popq %rdi 
	
	/*calling getMapped Pixel for col*/
	pushq %rdi
	pushq %rsi
	pushq %rdx
	pushq %rcx
	pushq %r8
	movq %rsi, %rdi                 /*moving colIndex for arg1*/
	movq %rdx, %rsi                 /*moving width for arg2*/
	call getMappedPixel
	movq %rax, %r13                 /*r13 - mappedCol reg*/
	popq %r8 
	popq %rcx
	popq %rdx
	popq %rsi
	popq %rdi 

	/*calling gradient for r12 - mappedRow*/
	pushq %rdi
	pushq %rsi
	pushq %rdx
	pushq %rcx
	pushq %r8
	movq %r12, %rdi                 /*moving mappedRow into arg1*/
	call gradient
	movq %rax, %r12                 /*moving ret to r12*/
	popq %r8 
	popq %rcx
	popq %rdx
	popq %rsi
	popq %rdi 

	/*calling gradient for r13 - mappedCol*/
	pushq %rdi
	pushq %rsi
	pushq %rdx
	pushq %rcx
	pushq %r8
	movq %r13, %rdi                /*moving mappedCol into arg1*/
	call gradient
	movq %rax, %r13                /*moving ret to r13*/
	popq %r8 
	popq %rcx
	popq %rdx
	popq %rsi
	popq %rdi 
	
	imulq %r8, %r12                /*color * gradient(mappedRow)*/
	imulq %r12, %r13               /*color * gradient(mappedRow) * gradient(mappedCol)*/
	movq %r13, %rax                /*move result to return */

	movq $0, %rdx                  /*64 bit dividend*/
	movq $1000000000000, %rsi      /*one trillion to %rsi*/
	divq %rsi                      /*color * gradient(mappedRow) * gradient(mappedCol) / trillion*/

	/*popping callee registers*/ 
	popq %r15
	popq %r14
	popq %r13
	popq %r12

	ret

/**
 * Returns the index from a new grid based on the same x and y position 
 *
 * Parameters
 * %edi (rdi) -> int32_t index
 * %esi (rsi) -> int32_t indexingWidth
 * %edx (rdx) -> int32_t actualWidth
 *
 * Stack memory:
 * %r12 
 * %rdx -> int32_t actualWidth (clobbered by division)
 */
	.globl getAdjustedIndex
getAdjustedIndex:
	pushq %r12              /*save callee*/
	pushq %rdx              /*edx gets clobbered by dividing*/
	
	movq $0, %rdx          /*64 bit value*/
	movl %edi, %eax        /*index into eax ie dividend*/
	divl %esi              /*eax = index / indexingWidth -> row*/

	movl %eax, %r12d       /*r12d = index % indexingWidth -> col*/
	movl %edx, %eax        /*eax = actualWidth*/

	popq %rdx               /*restore clobbered register*/
	
	imull %edx, %r12d      /*actualWidth * row*/
	addl %r12d, %eax       /*actualWidth * row + col*/

	popq %r12

	ret 


/**
 * Fills in the array that holds reflected and mirroed indicies in kaleidoscope
 * Parameters:
 * %rdi - int32_t * pointer to indexArray (address thus 64 bits)
 * %esi - int32_t indexingDimension (width and height)
 * %edx - int32_t current x
 * %ecx - int32_t current y
 *
 * Stack memory:
 * %r11 -> used to stored the index
 * %r12 -> used to store halfIndex (halfHeight and halfWidth)
 * %r13 -> used to store widthDistanceFromMiddle
 * %r14 -> used to store heightDistanceFromMiddle
 * %r15 -> used to store index calculations
 * %rdx -> x value (clobbered by division)
 */
	.globl fillKaleidoscopeIndexArray
fillKaleidoscopeIndexArray:
	/*save callee reigsters*/
	pushq %r11
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15

	pushq %rdx                            /*clobbered by division*/
	
	/*calculate half index*/
	
	movq $0, %rdx												 /*need this else arithmetic exception b/c its 127:0 rdx:rax for division*/
	movl %esi, %eax                      /*indexingDimension into eax*/
	movl $2, %r11d											 /*put diving factor into r11*/
	divl %r11d                           /*indexingDimension / 2*/
	movl %eax, %r12d                     /*r12 -> halfIndex = indexingDimension / 2*/
	popq %rdx														 /*restore rdx that was clobbered*/

	/*widthDistanceFromMiddle*/
	movl %r12d, %r13d										 
	subl %edx, %r13d										 /*r13 = widthDistanceFromMiddle*/

	/*heightDistanceFromMiddle*/
	movl %r12d, %r14d
	subl %ecx, %r14d										 /*r14 = heightDistanceFromMiddle*/

	/*aTopLeftIndex*/
	movl %esi, %r15d                     /*indexing dimenion to r13*/
	imull %ecx, %r15d                    /* y * indexingDimension */
	addl %edx, %r15d										 /* y * indexingDimension + x*/
	movq $0, %r11
	movl %r15d, (%rdi, %r11, 4)          /* save to array index`0 (the address is 64bits BUT the value we are moving is 32bits)*/


	/*bTopLeftIndex*/
	movl %esi, %r15d                    /*indexing dimenion to r13*/
	imull %edx, %r15d                   /* x * indexingDimension */
	addl %ecx, %r15d										/* x * indexingDimension + y*/
	movq $1, %r11
	movl %r15d, (%rdi, %r11, 4)         /* save to array index 1 (movl 32bit data, (64bit address, 64bit index, sizeof(data)))*/

	/*aTopRightIndex*/
	movl %r13d, %r15d									  /*widthDistanceFromMiddle*/
	imull $2, %r15d											/*widthDistanceFromMiddle * 2*/
	subl $1, %r15d											/*(widthDistanceFromMiddle * 2) - 1*/
	movq $0, %r11												
	movl (%rdi, %r11, 4), %eax					/*move aTopLeftIndex to eax*/
	addl %eax, %r15d										/*aTopLeftIndex + (widthDistanceFromMiddle * 2) - 1*/
	movq $2, %r11
	movl %r15d, (%rdi, %r11, 4)         /*save to index 2*/

	/*bTopRightIndex*/
	movl %r14d, %r15d										/*heightDistanceFromMiddle*/
	imull $2, %r15d											/*heightDistanceFromMiddle * 2*/
	subl $1, %r15d											/*heightDistanceFromMiddle * 2 - 1*/
	movq $1, %r11												
	movl (%rdi, %r11, 4), %eax					/*move bTopLeftIndex to eax*/
	addl %eax, %r15d										/*bTopLeftIndex + (heightDistanceFromMiddle * 2) - 1*/
	movq $3, %r11
	movl %r15d, (%rdi, %r11, 4)         /* save to index 3*/

	/*aBottomLeftIndex*/
	movl %r14d, %r15d										/*heightDistanceFromMiddle*/
	imull $2, %r15d											/*heightDistanceFromMiddle * 2*/
	subl $1, %r15d											/*heightDistanceFromMiddle * 2 - 1*/
	imull %esi, %r15d										/*((heightDistanceFromMiddle * 2) - 1) * indexingDimension*/
	movq $0, %r11
	movl (%rdi, %r11, 4), %eax					/*move aTopLeftIndex to eax*/
	addl %eax, %r15d										/*aTopLeftIndex + ((heightDistanceFromMiddle * 2) - 1) * indexingDimension*/
	movq $4, %r11
	movl %r15d, (%rdi, %r11, 4)         /* save to index 4*/

	/*bBottomLeftIndex*/
	movl %r13d, %r15d									  /*widthDistanceFromMiddle*/
	imull $2, %r15d											/*widthDistanceFromMiddle * 2*/
	subl $1, %r15d											/*widthDistanceFromMiddle * 2 - 1*/
	imull %esi, %r15d										/*((widthDistanceFromMiddle * 2) - 1) * indexingDimension*/
	movq $1, %r11
	movl (%rdi, %r11, 4), %eax					/*move bTopLeftIndex to eax*/
	addl %eax, %r15d										/*bTopLeftIndex + ((widthDistanceFromMiddle * 2) - 1) * indexingDimension*/
	movq $5, %r11
	movl %r15d, (%rdi, %r11, 4)         /* save to index 5*/

	/*aBottomRightIndex*/
	movl %r13d, %r15d									  /*widthDistanceFromMiddle*/
	imull $2, %r15d											/*widthDistanceFromMiddle * 2*/
	subl $1, %r15d											/*widthDistanceFromMiddle * 2 - 1*/
	movq $4, %r11
	movl (%rdi, %r11, 4), %eax					/*move aBottomLeftIndex to eax*/
	addl %eax, %r15d										/*aBottomLeftIndex + ((widthDistanceFromMiddle * 2) - 1)*/
	movq $6, %r11
	movl %r15d, (%rdi, %r11, 4)         /* save to index 6*/

	/*bBottomRightIndex*/
	movl %r14d, %r15d										/*heightDistanceFromMiddle*/
	imull $2, %r15d											/*heightDistanceFromMiddle * 2*/
	subl $1, %r15d											/*heightDistanceFromMiddle * 2 - 1*/
	movq $5, %r11
	movl (%rdi, %r11, 4), %eax					/*move bBottomLeftIndex to eax*/
	addl %eax, %r15d										/*bBottomLeftIndex + ((heightDistanceFromMiddle * 2) - 1)*/
	movq $7, %r11
	movl %r15d, (%rdi, %r11, 4)         /* save to index 7*/

	/*restore callee register states*/
	popq %r15
	popq %r14
	popq %r13
	popq %r12
	popq %r11
	
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

	pushq %rbp
	pushq %rbx
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15

	/* Load width and height from struct */
	movl (%rdi), %r15d      /* width in r15d */
	movl 4(%rdi), %r14d     /* height in r14d */
	movq 8(%rdi), %r8       /* input buffer address in r8 */
	movq 8(%rsi), %r9       /* output buffer address in r9 */

	xorl %r12d, %r12d      /* y index in r12d */
	xorl %r13d, %r13d      /* x index in r13d */

	/* Calculate 2*width to help w time complexity */
        movl %r15d, %r10d
        shll $1, %r10d

.LrgbOuterLoop:
    cmpl %r14d, %r12d      /* compare y with height */
    jge .LrgbExit

	
.LrgbInnerLoop:
	
	/* y index in r12, x index in r13 */
	/* height of image in r14, width of r15 */
	/* input image in rdi, output in rsi */
	cmpl %r15d, %r13d      /* compare x with width */
	jge .LrgbNextRow
	
	/* Calculate input pixel address: input[y * width + x] */
	movl %r12d, %eax       /* y */
	imull %r15d, %eax             /* y * width */
	addl %r13d, %eax       /* y * width + x */
	movl (%r8, %rax, 4), %ebx  /* Load pixel into ebx */

	
	/*Top Left */
	/* y * 2*width + x */
	movl %r12d, %eax
	imull %r10d, %eax      # y * 2*width
	addl %r13d, %eax      # y * 2*width + x
	movl %ebx, (%r9, %rax, 4)

	/* top right red */
	movl %ebx, %edi /*moving pixel to arg1*/
	call get_r
	movl %eax, %r11d /*red to arg4*/
	movl %ebx, %edi /*pixel to arg1*/
	call get_a

	movl %r11d, %edi /*red to arg1*/
	xorl %esi, %esi /* clear green */
	xorl %edx, %edx /* clear blue */
	movl %eax, %ecx /*alpha to arg4*/                      
        call combineData

	
	movl %r12d, %ecx    /*move y index*/
	imull %r10d, %ecx    /*r10 = 2 * width */
	addl %r13d, %ecx
	addl %r15d, %ecx
	movl %eax, (%r9, %rcx, 4) /* Calculate new index */
	
	
	/* Bottom Left: Green  */
	/* y * 2*width + (width + x) */
	movl %ebx, %edi        /* pixel to edi for get_g */
	call get_g
	movl %eax, %r11d       /* save green value */
	movl %ebx, %edi        /* pixel for get_a */
	call get_a
	xorl %edi, %edi /* clear red */
	movl %r11d, %esi /* green to arg2 */
	xorl %edx, %edx /* clear blue */
        movl %eax, %ecx /*alpha to arg4*/                                                                              
        call combineData
	
	movl %r12d, %ecx
	addl %r14d, %ecx       /* y + height */
	imull %r10d, %ecx       /* (y + height) * 2*width */
	addl %r13d, %ecx       /* (y + height) * 2*width + x */
	movl %eax, (%r9, %rcx, 4)

	
	/* Bottom Right: Blue */
	movl %ebx, %edi        /* pixel to edi for get_b */
	call get_b
	movl %eax, %r11d       /* save blue value */
	movl %ebx, %edi        /* pixel for get_a */
	call get_a
	xorl %edi, %edi /* clear red */
	xorl %esi, %esi /* clear green */
	movl %r11d, %edx /*blue to arg3*/
        movl %eax, %ecx /*alpha to arg4*/ 
        call combineData

	movl %r12d, %ecx
	addl %r14d, %ecx      /* y + height */
	imull %r10d, %ecx      /* (y + height) * 2*width */
	addl %r13d, %ecx
	addl %r15d, %ecx      /* (y + height) * 2*width + x + width */
	movl %eax, (%r9, %rcx, 4)
	
	/* Increment x */
	incl %r13d
	jmp .LrgbInnerLoop

.LrgbNextRow:
	xorl %r13d, %r13d      /* reset x */
        incl %r12d             /* increment y */
        jmp .LrgbOuterLoop
	
.LrgbExit:
	popq %r15
	popq %r14
	popq %r13
	popq %r12
	popq %rbx
	popq %rbp
	
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
 *
 * Stack memory:
 * %r10 -> used to store alpha
 * %r11 -> used to store green * 128
 * %r12 -> used to store red * 79
 * %r13 -> used to store pixel ie input_img -> data[index]
 * %r14 -> used to store the index for the loop
 * %r15 -> used to store width * height aka iteration bound
 * %rdi -> used to call get_rgba and combineData (need to save input img)
 * %rsi -> used to call combineData (need to save output img)
 * $rdx -> used to call combineData (saved just in case other functions clober it)
 * $rcx -> used to call combineData (saved just in case other functions clober it)
 */
	.globl imgproc_grayscale
imgproc_grayscale:
	/* TODO: implement */
	pushq %r10
	pushq %r11
	pushq %r12
	pushq %r13
	pushq %r14
	pushq %r15

	/*height*/
	movq $1, %r10
	movl (%rdi, %r10, 4), %r14d				/*height in r14*/

	/*width*/
	movq $0, %r10
	movl (%rdi, %r10, 4), %r15d       /*width in r15*/

	imull %r14d, %r15d									/*r15 = bound*/
	movq $0, %r14											/*r14 = index*/

	jmp .LgrayscaleConditionCheck

.LgrayscaleLoop:
	/*reset the registers*/
	movq $0, %r8										
	movq $0, %r9										
	movq $0, %r10										
	movq $0, %r11										
	movq $0, %r12										
	movq $0, %r13										
	movq $0, %rax										

	movq 8(%rdi), %r13										/*value located at rsi+8 is the address to the array (address of array in r8)*/
	movl (%r13,%r14,4), %r13d			    		/*(pixel) incrementing the address to the array to access element %r14*/
	
	pushq %rdi
	pushq %rsi
	pushq %rdx
	pushq %rcx
	movl %r13d, %edi											/*move pixel to arg 1*/
	call get_r
	imull $79, %eax		 										/*red * 79 -> rax * rdi*/
	movl %eax, %r12d											/*save to r12*/
	popq %rcx
	popq %rdx
	popq %rsi
	popq %rdi

	pushq %rdi
	pushq %rsi
	pushq %rdx
	pushq %rcx
	movl %r13d, %edi											/*move pixel to arg 1*/
	call get_g
	imull $128, %eax											/*green * 128*/
	movl %eax, %r11d											/*green to r11*/
	popq %rcx
	popq %rdx
	popq %rsi
	popq %rdi

	pushq %rdi
	pushq %rsi
	pushq %rdx
	pushq %rcx
	movl %r13d, %edi											/*move pixel to arg 1*/
	call get_a
	movl %eax, %r10d											/*move alpha to r10*/
	popq %rcx
	popq %rdx
	popq %rsi
	popq %rdi

	pushq %rdi
	pushq %rsi
	pushq %rdx
	pushq %rcx
	movl %r13d, %edi											/*move pixel to arg 1*/
	call get_b													
	imull $49, %eax											  /*blue * 49 -> leave in eax*/
	popq %rcx
	popq %rdx
	popq %rsi
	popq %rdi

	addl %r12d, %eax										/*blue * 49 + red * 79*/
	addl %r11d, %eax									  /*blue * 49 + red * 79 + green * 128*/

	pushq %rdi
	pushq %rsi
	pushq %rdx
	pushq %rcx
	movq $0, %rdx												/*64 bit division*/
	movl $256, %r9d											/*divisor = 256*/
	divl %r9d														/*blue * 49 + red * 79 + green * 128 / 256*/
	movl %eax, %edi											/* move grayscale pixel to arg 1*/
	movl %eax, %esi											/* move grayscale pixel to arg 2*/
	movl %eax, %edx											/* move grayscale pixel to arg 3*/
	movl %r10d, %ecx										/* move alpha to arg4*/
	call combineData
	popq %rcx
	popq %rdx
	popq %rsi
	popq %rdi

	movq 8(%rsi), %r8									/*value located at rsi+8 is the address to the array (address of array in r8)*/
	leaq (%r8,%r14,4), %r8			  		/*incrementing the address to the array to access element x*/
	movl %eax, (%r8)									/*this stores eax as the new pointer for some reason*/

	jmp .LgrayscaleIncrement

.LgrayscaleIncrement:
	addl $1, %r14d 										/*index++*/
.LgrayscaleConditionCheck:
	cmp %r15d, %r14d									/*bound vs index*/
	jl .LgrayscaleLoop 								/*jump to loop if bound>index*/
	nop 

.LgrayscaleExit:
	popq %r15
	popq %r14
	popq %r13
	popq %r12
	popq %r11
	popq %r10
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
