///
// kpng.cpp -- implements kView PNG image handler.
// Sirtaj Kang, 1996.
//
// $Id$
///

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_LIBPNG
#include<stdio.h>
#include<qimage.h>
#include<assert.h>

extern "C"
{
#include<png.h>
}

#include"kpng.h"				

void png_read_data(png_structp, png_bytep, png_uint_32);

void read_png_file(QImageIO * imageio)
{
	FILE *fp;
	png_structp png_ptr;
	png_infop info_ptr;
	QImage image;
	bool paletted = FALSE;
	
	debug("png: handler called");

	// Alloc and check various png_structures

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
		 NULL, NULL, NULL);

	CHECK_PTR(png_ptr);

	if (!png_ptr) {
		warning("read_png_file: unable to alloc png_ptr. Out of mem?");
		return;
	}

	debug("png: Creating info_struct...");

	info_ptr = png_create_info_struct(png_ptr);
	CHECK_PTR(info_ptr);

	if (!info_ptr) {
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return;
	}

	// Define IO callbacks
	// *TODO*

	fp = fopen(imageio->fileName(), "rb");
	assert(fp);
	png_init_io(png_ptr, fp);

	// Error callback

	if (setjmp(png_ptr->jmpbuf)) {
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(fp);

		warning("png: error creating read_str... bailing.");
		return;
	}
	// Read header, filling image info into info_ptr
	int passes=0;

	debug("png: reading to info_ptr");
	png_read_info(png_ptr, info_ptr);

	debug("png: image is %lux%lu.", info_ptr->width, info_ptr->height);
	debug("png: depth %d, channels %d, bpp %d.",
		  info_ptr->bit_depth,
		  info_ptr->channels,
		  info_ptr->pixel_depth);

	debug("png: setting library transformations...");

	// Decide bit depth

	int defaultDepth = QPixmap::defaultDepth();

#if 0
	if (defaultDepth == 8) {
		if (info_ptr->color_type & PNG_COLOR_MASK_COLOR) {
			if (info_ptr->valid & PNG_INFO_PLTE)
				png_set_dither(png_ptr, info_ptr->palette, 
						info_ptr->num_palette,
					256, info_ptr->hist, TRUE);
			else {
				png_color std_color_cube[17] =
			              {};

				png_set_dither(png_ptr, std_color_cube, 
				MAX_SCREEN_COLORS, MAX_SCREEN_COLORS, NULL);
			}
		}
		paletted = TRUE;

	} else 
#endif

	if( defaultDepth == 32) {

		if( png_ptr->color_type == PNG_COLOR_TYPE_PALETTE )
			png_set_expand(png_ptr);

		paletted = FALSE;

	}

/////// Strip to 8 bits
	if (info_ptr->bit_depth == 16){
		debug("png: stripping to 16 to 8 bits per channel.");
		png_set_strip_16(png_ptr);
	}

/////// expand to 8 bit
	if (info_ptr->bit_depth < 8){
		debug("png: setting padding to 8 bit");
	        png_set_packing(png_ptr);
	}

/////// Alpha-channeling simulated by PNG library
	debug("png: alpha channel options");
	if (info_ptr->valid & PNG_INFO_bKGD) {
		debug("png: simulating alpha for transparent img.");
		png_set_background(png_ptr, &(info_ptr->background),
			PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
	}
	else {
		png_color_16 default_background = { 0,0,0,0 };

		debug("png: setting background to 0?");
		png_set_background(png_ptr, &default_background,
			PNG_BACKGROUND_GAMMA_SCREEN, 0, 1.0);
	}

//////// Let libpng handle interlacing as well.

	if (info_ptr->interlace_type) {
		debug("png: library will handle this interlaced image.");
		passes = png_set_interlace_handling(png_ptr);
	}


	png_set_filler(png_ptr, 0, PNG_FILLER_BEFORE);
	png_read_update_info(png_ptr, info_ptr);

	debug("png: image is %lux%lu.", info_ptr->width, info_ptr->height);
	debug("png: depth %d, channels %d, bpp %d.",
		  info_ptr->bit_depth,
		  info_ptr->channels,
		  info_ptr->pixel_depth);

	// new image
	image.create(info_ptr->width, info_ptr->height, 32);

	// read file to image
	debug("png: read start");
	png_start_read_image(png_ptr);

	while(passes--)
		for(unsigned rowCtr = 0; rowCtr < info_ptr->height; rowCtr++)
			png_read_row(png_ptr, image.scanLine(rowCtr), NULL);


	//cleanup
	debug("png: read end");
	png_read_end(png_ptr, info_ptr);
	debug("png: destroy");
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	imageio->setImage(image);
	imageio->setStatus(0);

}//read_png_file

#endif
