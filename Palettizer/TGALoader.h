#ifndef _CTGA_H_
#define _CTGA_H_

#include "types.h"

#pragma pack (push, 1)

#define SQR( a ) ( ( a ) * ( a ) )
struct CTGAImage
{
	
	s32	iWidth;
	s32	iHeight;
	s32	iBitDepth;
	u8*	p_cImageData;
	char*	p_cClutData;
	

};

typedef struct CBitMask
{
	u32	mask;
	u32	lshift;
	u32	rshift;
} CBitMask;


typedef struct CTextureDesc
{
	u32	bytedepth;

	union
	{
		struct
		{
			CBitMask	bmRmask;			// red bit mask
			CBitMask	bmGmask;			// green bit mask
			CBitMask	bmBmask;			// blue bit mask
			CBitMask	bmAmask;			// alpha bit mask
		};

		CBitMask		bmMasks[ 4 ];
	};
} CTextureDesc;


typedef struct CTGAstruct
{
	unsigned char	idsize;				// size of ID string
	unsigned char	cmap_type;			// 1 == color map included
	unsigned char	imagetype;			// 1 == uncompressed, color mapped image

	u16	cmap_first;			// start from color 0
	u16	cmap_size;			// 256 colors in color map
	unsigned char	cmap_bits;			// 24 bits per color value

	u16	xorigin;
	u16	yorigin;

	u16	width;				// image width
	u16	height;				// image height

	unsigned char	bitdepth;
	unsigned char	descriptor;
} CTGAstruct;

#pragma pack ( pop )

u32			TGAsave( char *tganame, unsigned char *buffer, u32 w, u32 h, u32 d );
CTGAImage*	TGAload( const char *fname);
bool		TGAGetSize( const char *tganame, s32 *p_iWidth, s32 *p_iHeight );
void		TGArleUncompress( unsigned char *dest, unsigned char *source, CTGAstruct *tga, u32 dest_depth );
void		TGASaveAs8BitRaw( CTGAImage* img, char* outname );

#endif
