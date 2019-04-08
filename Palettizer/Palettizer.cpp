// Palettizer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TGALoader.h"

//-------------------------------------------------------------------------------------------------

struct Image
{
	//-------------------------------------------------------------------------------------------------

	struct color
	{
		float r, g, b;
		float distance( const color& lb )
		{
			return ( SQR( r - lb.r ) + SQR( g - lb.g ) + SQR( b - lb.b ) );
		}
		color toLinear() const
		{
			color c;
			c.r = r*r;
			c.g = g*g;
			c.b = b*b;
			return c;
		}
		color toGamma() const
		{
			color c;
			c.r = sqrt(r);
			c.g = sqrt(g);
			c.b = sqrt(b);
			return c;
		}
		color operator - ( const color& o )
		{
			color c;
			c.r = r - o.r;
			c.g = g - o.g;
			c.b = b - o.b;
			return c;
		}
		color operator + ( const color& o )
		{
			color c;
			c.r = r + o.r;
			c.g = g + o.g;
			c.b = b + o.b;
			return c;
		}
		color operator * ( float f )
		{
			color c = *this;
			c.r *= f; 
			c.g *= f;
			c.b *= f;
			return c;
		}
		color operator / ( float f )
		{
			color c = *this;
			c.r /= f;
			c.g /= f;
			c.b /= f;
			return c;
		}

	};

	//-------------------------------------------------------------------------------------------------

	Image( char* filename )
	{
		CTGAImage* i = TGAload( filename );
		iWidth = i->iWidth;
		iHeight = i->iHeight;
		p_cImageData = new color[ iWidth * iHeight ];
		for ( int y = 0; y < iHeight; y++ )
		{
			for ( int x = 0; x < iWidth; x++ )
			{
				p_cImageData[ ( x + y * iWidth ) ].r = ((float)i->p_cImageData[ ( x + ( iHeight - y - 1 ) * iWidth ) * 3 + 2 ]) / 255.f;
				p_cImageData[ ( x + y * iWidth ) ].g = ((float)i->p_cImageData[ ( x + ( iHeight - y - 1 ) * iWidth ) * 3 + 1 ]) / 255.f;
				p_cImageData[ ( x + y * iWidth ) ].b = ((float)i->p_cImageData[ ( x + ( iHeight - y - 1 ) * iWidth ) * 3 + 0 ]) / 255.f;

				p_cImageData[ ( x + y * iWidth ) ] = p_cImageData[ ( x + y * iWidth ) ].toLinear();
			}
		}
		delete i;
	}
	void SaveTGA( char* filename )
	{
		u8* pData = new u8[ iWidth *iHeight * 3 ];
		for ( int y = 0; y < iHeight; y++ )
		{
			for ( int x = 0; x < iWidth; x++ )
			{
				p_cImageData[ ( x + y * iWidth ) ] = p_cImageData[ ( x + y * iWidth ) ].toGamma();

				pData[ ( x + y * iWidth ) * 3 + 2 ] = ( u8 ) ( p_cImageData[ ( x + y * iWidth ) ].r * 255.f );
				pData[ ( x + y * iWidth ) * 3 + 1 ] = ( u8 ) ( p_cImageData[ ( x + y * iWidth ) ].g * 255.f );
				pData[ ( x + y * iWidth ) * 3 + 0 ] = ( u8 ) ( p_cImageData[ ( x + y * iWidth ) ].b * 255.f );
			}
		}
		TGAsave( filename, pData, iWidth, iHeight, 3 );
		delete pData;
	}
	~Image()
	{
		delete p_cImageData;
	}

	color GetPixel( int x, int y )
	{
		color c; c.r = c.g = c.b = 0;
		if ( x < 0 )
			return c;
		if ( y < 0 )
			return c;
		if ( x >= iWidth )
			return c;
		if ( y >= iHeight )
			return c;
		return p_cImageData[ ( x + y * iWidth ) ];
	}
	void SetPixel( color c, int x, int y )
	{
		if ( x < 0 )
			return;
		if ( y < 0 )
			return;
		if ( x >= iWidth )
			return;
		if ( y >= iHeight )
			return;
		p_cImageData[ ( x + y * iWidth ) ] = c;
	}

	//-------------------------------------------------------------------------------------------------

	s32	iWidth;
	s32	iHeight;
	color*	p_cImageData;
};

//-------------------------------------------------------------------------------------------------

Image::color mode2_palette[ ] =
{
	{ 0,0,0 },
	{ 1,0,0 },
	{ 0,1,0 },
	{ 0,0,1 },
	{ 0,1,1 },
	{ 1,0,1 },
	{ 1,1,0 },
	{ 1,1,1 },
};

//-------------------------------------------------------------------------------------------------

Image::color mode1_palette[ ] =
{
	{ 0,0,0 },
	{ 1,0,0 },
	{ 1,1,0 },
	{ 1,1,1 },
};

//-------------------------------------------------------------------------------------------------

Image::color mode0_palette[ ] =
{
	{ 0,0,0 },
	{ 1,0,0 },
};
//-------------------------------------------------------------------------------------------------

#define palette mode2_palette

//-------------------------------------------------------------------------------------------------

int main( int argc, char **argv )
{
	if ( argc == 0 )
		return 0;

	Image image( argv[ 1 ] );

	
	for ( int y = 0; y < image.iHeight; y++ )
	{
		for ( int x = 0; x < image.iWidth; x++ )
		{
			Image::color oldpixel = image.GetPixel( x, y );

			int iBest = 0;
			float bestdist = FLT_MAX;
			for ( int i = 0; i < sizeof( palette ) / sizeof( Image::color ); i++ )
			{
				float dist = palette[ i ].distance( oldpixel );
				if ( dist < bestdist )
				{
					iBest = i; bestdist = dist;
				}
			}
			Image::color newpixel = palette[ iBest ];

			image.SetPixel( newpixel, x, y );

			Image::color quant_error = oldpixel - newpixel;
			image.SetPixel( image.GetPixel( x + 1, y ) + ( quant_error * 7.f ) / 16.f , x + 1, y );
			image.SetPixel( image.GetPixel( x - 1, y + 1 ) + ( quant_error * 3.f  ) / 16.f , x - 1, y + 1 );
			image.SetPixel( image.GetPixel( x, y + 1 ) + ( quant_error * 5.f  ) / 16.f , x, y + 1 );
			image.SetPixel( image.GetPixel( x + 1, y + 1 ) + ( quant_error * 1.f  ) / 16.f , x + 1, y + 1 );
		}
	}

	image.SaveTGA( argv[ 2 ] );
	return 0;
}

