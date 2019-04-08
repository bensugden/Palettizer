//==============================================================================
//
//  \file       CFile.cpp 
//
//==============================================================================
//  \addtogroup Core
//  @{

#include "stdafx.h"
#include "File.h"
#include "FileData.h"
#include "stdio.h"

// ------------------------------------------------------------------------------

static std::list< CTableOfContents* > s_tablesOfContents;
// ------------------------------------------------------------------------------

CFile::CFile( const std::string& filename, const char* access, CFileCache* cache ) 
{
	u32 file_offset = 0;
	u32 file_length = 0;

	m_fileData = new CFileData( filename, access, file_offset, file_length ) ;
	if ( cache != nullptr )
	{
		m_fileData->Load();
		m_fileData->SetPosition(0);
	}
} 

// ------------------------------------------------------------------------------
//
//  destructor
//

CFile::~CFile( )
{
	m_fileData->CloseFile();
}


//-------------------------------------------------------------------------------------------------

bool CFile::IsFileOlder( const std::string& filenameIsThisOlder, const std::string& filenameThanThis )
{

	
	return true;
}

// ------------------------------------------------------------------------------

bool CFile::FileExists( const std::string& filename)
{
	return true;
}

// ------------------------------------------------------------------------------
//  @} 
