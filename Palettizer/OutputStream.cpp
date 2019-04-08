//-------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "OutputStream.h"
#include <iostream>
#include <fstream>
#include "Types.h"
#include "File.h"

//-------------------------------------------------------------------------------------------------

bool COutputStream::s_endianSwap = false;

//-------------------------------------------------------------------------------------------------

COutputStream::COutputStream(  )
{
	m_file		= NULL;
	m_offset	= 0;
	m_maxsize	= 16384;
	m_buffer	= new char[ m_maxsize ];
}

//-------------------------------------------------------------------------------------------------

COutputStream::COutputStream( const std::string& filename )
{
	m_file		= new CFile( filename, "wb" );
	m_offset	= 0;
	m_maxsize	= 16384;
	m_buffer	= new char[ m_maxsize ];
}

//-------------------------------------------------------------------------------------------------

COutputStream::COutputStream( const COutputStream& other )
: m_maxsize( other.m_maxsize )
, m_offset( other.m_offset )
, m_file( NULL )
{
	m_buffer = new char[ m_maxsize ];
	memcpy( m_buffer, other.m_buffer, m_maxsize );
}

//-------------------------------------------------------------------------------------------------

void COutputStream::CheckForResize( u32 new_size )
{
	//
	// grow buffer
	//
	if ( new_size >= m_maxsize )
	{
		char* old = m_buffer;

		while ( new_size >= m_maxsize ) 
		{
			m_maxsize *= 2;
		}

		m_buffer = new char[ m_maxsize ];
		memcpy( m_buffer, old, m_offset );
		delete [] old;
	}
}

//-------------------------------------------------------------------------------------------------


COutputStream& COutputStream::Append( const char* data, u32 size )
{
	//
	// grow buffer if necessary
	//
	CheckForResize( m_offset + size );

	// 
	// append data
	// 
	memcpy( m_buffer + m_offset, data, size );

	m_offset += size;

	return *this;
}

//-------------------------------------------------------------------------------------------------

COutputStream::~COutputStream( )
{
	//
	// flush buffer
	//
	if ( m_file )
	{ 
		if ( m_offset != 0 )
		{
			m_file->Write( m_buffer, m_offset );
		}
		delete m_file ;
	} 

	delete [] m_buffer ;
}

//-------------------------------------------------------------------------------------------------

