//-------------------------------------------------------------------------------------------------

#pragma once

//-------------------------------------------------------------------------------------------------

#include "types.h"
#include <string>
//-------------------------------------------------------------------------------------------------
//
// binary output stream
//
//-------------------------------------------------------------------------------------------------

class COutputStream
{
public:
	//-------------------------------------------------------------------------------------------------
	//
	// NULL = don't stream to a file ( just dynamically fill a buffer )
	//
	//-------------------------------------------------------------------------------------------------

	COutputStream  ();
	COutputStream  ( const std::string& filename ); 
	COutputStream  ( const COutputStream& other );

	//-------------------------------------------------------------------------------------------------

	~COutputStream ( );

	//-------------------------------------------------------------------------------------------------

	void Compress();
	//-------------------------------------------------------------------------------------------------

	u32 GetCurrentOffset( ) const 
	{ 
		return m_offset; 
	}

	//-------------------------------------------------------------------------------------------------

	void SetCurrentOffset( u32 new_offset )
	{ 
		CheckForResize( new_offset );

		m_offset = new_offset; 
	}

	//-------------------------------------------------------------------------------------------------

	static void SetEndianSwap( bool enable )
	{
		s_endianSwap = enable;
	}

	//-------------------------------------------------------------------------------------------------
	//
	// NB: don't cache this pointer
	//
	const char* GetBuffer( ) const 
	{ 
		return m_buffer; 
	}

	//-------------------------------------------------------------------------------------------------

	static const void* EndianSwap4( const void* v )
	{
		if ( !s_endianSwap )
		{
			return v;
		}

		static u8 b[4];

		u8* pValue = (u8*)v;
		
		b[ 0 ] = pValue[ 3 ];
		b[ 1 ] = pValue[ 2 ];
		b[ 2 ] = pValue[ 1 ];
		b[ 3 ] = pValue[ 0 ];

		return &b;
	}

	//-------------------------------------------------------------------------------------------------

	static const void* EndianSwap2( const void* v )
	{
		if ( !s_endianSwap )
		{
			return v;
		}

		static u8 b[2];

		u8* pValue = (u8*)v;

		b[ 0 ] = pValue[ 1 ];
		b[ 1 ] = pValue[ 0 ];

		return &b;
	}
	//-------------------------------------------------------------------------------------------------

	COutputStream&	Append			( const char* data, u32 size );

	//-------------------------------------------------------------------------------------------------

	COutputStream& operator << ( const char& t) 
	{ 
		return Append( &t, 1 ); 
	}

	//-------------------------------------------------------------------------------------------------

	COutputStream& operator << ( const u8& t) 
	{ 
		return Append( ( char* )&t, 1 ); 
	}
		
	//-------------------------------------------------------------------------------------------------

	COutputStream& COutputStream::operator << ( const u32& t )
	{ 
		return Append( ( char* )EndianSwap4(&t), sizeof( u32 ) ); 
	}
	//-------------------------------------------------------------------------------------------------

	COutputStream& COutputStream::operator << ( const unsigned int& t )
	{ 
		return Append( ( char* )EndianSwap4(&t), sizeof( unsigned int ) ); 
	}
	//-------------------------------------------------------------------------------------------------

	COutputStream& COutputStream::operator << ( const int& t )
	{ 
		return Append( ( char* )EndianSwap4(&t), sizeof( int ) ); 
	}
	//-------------------------------------------------------------------------------------------------

	COutputStream& COutputStream::operator << ( const s32& t )
	{ 
		return Append( ( char* )EndianSwap4(&t), sizeof( s32 ) ); 
	}
	//-------------------------------------------------------------------------------------------------

	COutputStream& COutputStream::operator << ( const float& t )
	{ 
		return Append( ( char* )EndianSwap4(&t), sizeof( float ) ); 
	}

		//-------------------------------------------------------------------------------------------------

	COutputStream& COutputStream::operator << ( const u16& t )
	{ 
		return Append( ( char* )EndianSwap2(&t), sizeof( u16 ) ); 
	}

	//-------------------------------------------------------------------------------------------------

	COutputStream& COutputStream::operator << ( const s16& t )
	{ 
		return Append( ( char* )EndianSwap2(&t), sizeof( s16 ) ); 
	}

	//-------------------------------------------------------------------------------------------------

	COutputStream& COutputStream::operator << ( const COutputStream& t)
	{
		COutputStream& os(const_cast<COutputStream&>(t));
		Append( t.m_buffer, t.m_offset );
		os.SetCurrentOffset( 0 );

		return *this;
	}

	//-------------------------------------------------------------------------------------------------

	COutputStream& COutputStream::operator << ( const std::string& t)
	{ 
		u32 length = (u32) t.size();
		*this << length; 
		return Append( t.c_str(), length ); 
	}

	//-------------------------------------------------------------------------------------------------
	
private:
	void			CheckForResize	( u32 new_size );
	
	//-------------------------------------------------------------------------------------------------
	
	char*						m_buffer;
	u32							m_maxsize;
	u32							m_offset;
	
	class CFile*						m_file;
	
	static bool					s_endianSwap;
	//-------------------------------------------------------------------------------------------------
	
	COutputStream& operator << ( const char* t)
	{
		u32 length = strlen( t );
		Append( (char*)EndianSwap4(&length), sizeof( u32 ) );
		return Append( t, length );
	}
	


};
	//-------------------------------------------------------------------------------------------------
