//==============================================================================
//
//  \file       CFile.h 
//              Created: 5/15/2006
//              Author: bsugden
//
//==============================================================================

#pragma once

// ------------------------------------------------------------------------------

#include "stdio.h"
#include "filedata.h"


	class CFileData;
	class CFile
	{
	public:
		//-------------------------------------------------------------------------------------------------
		struct ITimeStamp
		{
			ITimeStamp( const std::string& filename ) {};
			virtual bool operator < ( const ITimeStamp& other ) = 0;
		};
		//-------------------------------------------------------------------------------------------------
		CFile( const std::string& filename, const char* access, class CFileCache* cache =  nullptr );
		CFile( struct CTOCEntry* toc_entry ) ;

		~CFile();

		void 						Load( )												{ m_fileData->Load(); }

		u32							GetLength()											{ return m_fileData->GetLength(); }

		void*						GetBuffer()											{ return m_fileData->GetBuffer(); } // stalls if not loaded
		u32							GetBufferLength()									{ return m_fileData->GetBufferLength(); } ;
		bool						IsLoaded()											{ return m_fileData->IsLoaded(); } ;

		int 						Read ( void*       dest, u32 bytes )				{ return m_fileData->Read( dest, bytes ); } ;
		void 						Write( const void* src , u32 bytes )				{ m_fileData->Write( src, bytes ); } ;

		void 						SetPosition( u32 pos )								{ m_fileData->SetPosition( pos ); } ;
		u32							GetPosition( )										{ return m_fileData->GetPosition( ); } ;

		std::string					GetFilename( )										{ return m_fileData->GetFilename(); }
		CFileData*					GetData()											{ return m_fileData; }


		//-------------------------------------------------------------------------------------------------

		static ITimeStamp*			GetTimeStamp										( const std::string& filename );
		static bool					FileExists											( const std::string& filename );			
		static bool					IsFileOlder											( const std::string& filenameIsThisOlder, const std::string& filenameThanThis );

		//-------------------------------------------------------------------------------------------------

	private:

		std::pair<struct CTOCEntry*,struct CTableOfContents*> CFile::GetFile( const std::string& filename );

		CFileData*					m_fileData;
	};

// ------------------------------------------------------------------------------
