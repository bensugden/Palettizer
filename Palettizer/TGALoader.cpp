#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TGALoader.h"
#include "OutputStream.h"
#include "file.h"

static char CTGA_desc[256] = "\0";


u32 TGAsave(char *tganame, unsigned char *buffer, u32 w, u32 h, u32 d)
{
	CTGAstruct TGA_dest;

	memset(&TGA_dest, 0, sizeof(TGA_dest));

	if(!buffer || !d)
		TGA_dest.imagetype = 0;
	else if (d == 1)
		TGA_dest.imagetype = 3;
	else
		TGA_dest.imagetype = 2;

	TGA_dest.cmap_type  = 0;
	TGA_dest.cmap_size  = 0;
	TGA_dest.cmap_bits  = 24;
	TGA_dest.width      = (u16) w;
	TGA_dest.height     = (u16) h;
	TGA_dest.bitdepth   = (u16) d << 3;
	TGA_dest.descriptor = 0x20;					// start top left

	TGA_dest.idsize     = (unsigned char)strlen(CTGA_desc);

//    of = fopen( p_cTGAFullname, "wb" );
	FILE* stream = fopen( tganame, "wb" );
	if (!stream)
		return 1;

	fwrite( &TGA_dest, sizeof(TGA_dest), 1, stream );

   	if (TGA_dest.idsize > 0)
   		fwrite( &CTGA_desc, TGA_dest.idsize, 1, stream );

	if (d==3)
		fwrite( buffer, w*h*d,1,stream);
	else
	{
		//C_assert( d==4 ); // need 4 bytes per pixel for 24bit representation.
		for (s32 i=0;i<(s32)(w*h*4);i+=4)
		{
			fwrite( buffer+i, 3,1, stream );
		}
	}

	fclose( stream );
	return 0;
}


bool TGAisCompressed( CTGAstruct *t )
{
	switch(t->imagetype)
	{
		case  9:
			return true;

		case 10:
			return true;

		case 11:
			return true;

		default:
			return false;
	}
}



void TGArleUncompress( unsigned char *dest, unsigned char *source, CTGAstruct *tga, u32 dest_depth )
{
	s32	 i,j,n,depth,pdepth,x,y;
	s32	 vstep,hstep,sstep;
	bool	comp;
	;

	//source = ((unsigned char *)tga) + sizeof(CTGAstruct) ;

	depth = tga->bitdepth >> 3;

	if(tga->cmap_type)
	{
		pdepth = tga->cmap_bits>>3;
		source+=tga->cmap_size*pdepth;
	}

	if(tga->descriptor & (1<<5))
		vstep = 0;
	else
	{
		vstep = -2*tga->width*dest_depth;
		dest+=tga->width*(tga->height-1)*dest_depth;
	}

	if(tga->descriptor & (1<<4))
	{
		hstep = -(s32) dest_depth;
		dest+=(tga->width-1)*dest_depth;
	}
	else
		hstep = dest_depth;

	comp = TGAisCompressed(tga);

	sstep = depth;
	n = tga->width;
	    
	for(y=0;y<tga->height;y++)
	{
		for(x=0;x<tga->width;x+=n)
		{
			if(comp)
			{
				sstep	= (source[0] & 0x80) ? 0 : depth;
				n		= (source[0] & 0x7f) + 1;
				if(x+n>tga->width)
					return;				// SR_FILE_ERROR_CORRUPT
				source++;
			}
			
			for(i=0;i<n;i++)
			{
				for(j=0;j<depth;j++)
				{
					// READING OUT OF MEMORY!
					dest[j] = source[j];
				}

				#ifdef SN_TARGET_PS2
				char temp = dest[j-4];
				dest[j-4] = dest[j-2];
				dest[j-2] = temp;
				#endif

				source += sstep;
				dest+=hstep;
			}

			if(!sstep)
				source += depth;
		}
		dest+=vstep;
	}
}

void tgaDescSet(CTextureDesc *imagetype, u32 amask, u32 rmask, u32 gmask, u32 bmask)
{
	u32 i, m;
	s32 mask;

	imagetype->bmAmask.mask = amask;
	imagetype->bmRmask.mask = rmask;
	imagetype->bmGmask.mask = gmask;
	imagetype->bmBmask.mask = bmask;

	mask = amask | rmask | gmask | bmask;
	if (mask & 0xffff0000)
		imagetype->bytedepth = 4;
	else if (mask & 0x0000ff00)
		imagetype->bytedepth = 2;
	else
		imagetype->bytedepth = 1;

	for (m = 0; m < 4; m++)
	{
		i = imagetype->bmMasks[m].mask;
		for (imagetype->bmMasks[m].lshift = 0; !(i & 1) && i > 0; imagetype->bmMasks[m].lshift++, i >>= 1);
		for (imagetype->bmMasks[m].rshift = 8; i > 0;             imagetype->bmMasks[m].rshift--, i >>= 1);
	}
}

//-----------------------------------------------------------------------------

void		TGASaveAs8BitRaw( CTGAImage * tga, char* outname )
{
	u32 depth = tga->iBitDepth >> 3;

	u8* data = tga->p_cImageData;

	COutputStream o( outname );
	for(int y=0;y<tga->iHeight;y++)
	{
		for(int x=0;x<tga->iWidth;x++)
		{
			o << *data;
			data+= depth;
		}
	}
}
//-----------------------------------------------------------------------------

CTGAImage *TGAload( const char *tganame )
{
	u32 size;
	CTGAImage *p_image;
	unsigned char *p_ucDataUC;

	FILE *stream;
	stream = fopen(tganame, "rb");

	if (!stream)
		return NULL;

	fseek( stream, 0, SEEK_END );
	size = ftell(stream); 
	fseek( stream, 0, SEEK_SET );

	CTGAstruct tga;

	p_image = (CTGAImage*)malloc(sizeof( CTGAImage) );

	fread( &tga, sizeof( CTGAstruct ),1, stream);

	p_ucDataUC = (unsigned char *)malloc( size-sizeof( CTGAstruct ) );
	fread( p_ucDataUC, size-sizeof( CTGAstruct ), 1, stream);

	p_image->iHeight = tga.height;
	p_image->iWidth = tga.width;
	p_image->iBitDepth = tga.bitdepth;
	p_image->p_cImageData = (u8 *)p_ucDataUC;

	fclose( stream );
	return p_image;
}

//-----------------------------------------------------------------------------

bool TGAGetSize( const char *tganame, s32 *p_iWidth, s32 *p_iHeight )
{
	CTGAImage *p_image = TGAload( tganame );

	if(!p_image)
		return false;

	*p_iWidth = p_image->iWidth;
	*p_iHeight = p_image->iHeight;

	free( p_image->p_cImageData );
	free( p_image );

	return true;
}

//-------------------------------------------------------------------------------------------------
