//==============================================================================
//
//  \file       CFileData.cpp 
//
//==============================================================================
//  \addtogroup Core
//  @{

#include "stdafx.h"
#include "File.h"
#include "stdio.h"
#include <string>
#define min( a, b ) a < b ? a : b
// ------------------------------------------------------------------------------
#ifdef _IOS
#include <CFNetwork/CFNetwork.h>
static void ConvertFilePath( const char* in_filename, char* out_filename )
{
	// Get a reference to the main bundle
	CFBundleRef mainBundle = CFBundleGetMainBundle();

	CFStringRef filenm = CFStringCreateWithCString( NULL, in_filename, kCFStringEncodingASCII )
		;	// Get a reference to the file's URL
	CFURLRef imageURL = CFBundleCopyResourceURL(mainBundle, filenm, NULL, NULL);

	// Convert the URL reference into a string reference
	CFStringRef imagePath = CFURLCopyFileSystemPath(imageURL, kCFURLPOSIXPathStyle);

	// Get the system encoding method
	CFStringEncoding encodingMethod = CFStringGetSystemEncoding();

	// Convert the string reference into a C string
	const char *path = CFStringGetCStringPtr(imagePath, encodingMethod);
	strcpy( out_filename, path );
}

#endif


// ------------------------------------------------------------------------------

CFileData::CFileData( const std::string& filename,const char* access, u32 offset, u32 length ) : m_pFile( NULL )
{
#ifdef WIN32
	int err = fopen_s( &m_pFile, ( std::string( APP_ROOT ) + filename ).c_str(), access );
	m_fileName = std::string( APP_ROOT ) + filename;
#else
	char temp_file_name[ 2048 ];
	ConvertFilePath( filename.c_str(), temp_file_name );
	m_pFile = fopen( temp_file_name, access );
	m_fileName = temp_file_name;
#endif

	m_bufferLength = length;		// length is non zero if loading from bank file
	m_bufferStartOffset = offset;	// offset is non zero if loading from bank file 

	m_bufferOffset = 0;
	m_buffer = NULL;
	if ( m_bufferStartOffset !=0 && access[0] == 'r' )
		SetPosition( m_bufferOffset );
	m_ownsData = true;

} 
//-------------------------------------------------------------------------------------------------

CFileData::CFileData(  const std::string& filename, void* data, u32 length )
{
	m_buffer = (char*)data;
	m_bufferLength = length;
	m_bufferOffset = 0;
	m_bufferStartOffset = 0;
	m_fileName = filename;
	m_pFile = nullptr;
	m_ownsData = false;
}

// ------------------------------------------------------------------------------
//
//  destructor
//

CFileData::~CFileData( )
{
	CloseFile();
	if ( m_ownsData )
	{
		delete [] m_buffer;
	}
}

// ------------------------------------------------------------------------------

void CFileData::CloseFile()
{
	if ( m_pFile )
	{
		fclose( m_pFile );
		m_pFile = nullptr;
	}
}

//-------------------------------------------------------------------------------------------------

void CFileData::SetBuffer( char* buffer, u32 length )
{
	if ( m_buffer )
		delete [] m_buffer;
	m_bufferLength = length;
	m_buffer = buffer;
}

// ------------------------------------------------------------------------------

u32 CFileData::GetLength()
{
	if (!m_pFile)
		return 0;

	if ( m_bufferLength != 0 )
		return m_bufferLength;

	fseek( m_pFile, 0, SEEK_END );
	u32 bufferLength = ftell(m_pFile); 
	fseek( m_pFile, 0, SEEK_SET );

	return bufferLength;
}

//-------------------------------------------------------------------------------------------------

void CFileData::Load( )
{
	//
	// make this asynchronous
	//
	m_bufferLength = GetLength();

	m_buffer = new char[ m_bufferLength + 1 ];
	fread( m_buffer, 1, m_bufferLength, m_pFile );
	m_buffer[ m_bufferLength ] = 0;
	m_bufferOffset = m_bufferLength;
}

// ------------------------------------------------------------------------------

void* CFileData::GetBuffer( )
{
	//
	// add : stall if async
	//
	if ( m_bufferLength == 0 )
	{
		Load();
	}

	return m_buffer;
}

// ------------------------------------------------------------------------------

u32 CFileData::GetBufferLength( )
{
	//
	// add : stall if async
	//
	if ( m_bufferLength == 0 )
	{
		Load();
	}

	return m_bufferLength;
}

// ------------------------------------------------------------------------------

void CFileData::SetPosition( u32 pos )
{
	if ( !IsLoaded( ) )
	{
		fseek( m_pFile, pos + m_bufferStartOffset, SEEK_SET );
	}
	m_bufferOffset = pos;
}

// ------------------------------------------------------------------------------

u32 CFileData::GetPosition( )
{
	if ( IsLoaded( ) )
		return m_bufferOffset;
	return ftell(m_pFile);
}

// ------------------------------------------------------------------------------

int CFileData::Read( void* dest, u32 bytes )
{
	if ( IsLoaded() )
	{
		bytes = min( bytes, GetBufferLength( ) - m_bufferOffset );
		memcpy( dest, (char*)GetBuffer() + m_bufferOffset , bytes );
		return bytes;
	}
	else
	{
		if ( GetLength() > 0 )
			bytes = min( bytes, GetLength( ) - m_bufferOffset );

		int read = fread( dest, 1, bytes, m_pFile );
		m_bufferOffset += read;
		return read;
	}
}

// ------------------------------------------------------------------------------

void CFileData::Write( const void* src, u32 bytes )
{
	fwrite( src, bytes, 1, m_pFile );
	m_bufferLength += bytes;
}

// ------------------------------------------------------------------------------

bool CFileData::IsLoaded( )
{
	return m_buffer != nullptr;
}

// ------------------------------------------------------------------------------
//  @} 
