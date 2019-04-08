//==============================================================================
//
//  \file       CFile.h 
//              Created: 5/15/2006
//              Author: bsugden
//
//==============================================================================

#pragma once

//-------------------------------------------------------------------------------------------------

#define APP_ROOT ""

// ------------------------------------------------------------------------------

#include "stdio.h"

//------------------------------------------------------------------------------

	class CFileData
	{
	public:
		~CFileData();

		void 			Load( );

		u32				GetLength();

		void*			GetBuffer(); // stalls if not loaded
		u32				GetBufferLength();
		bool			IsLoaded();

		int 			Read ( void*       dest, u32 bytes );
		void 			Write( const void* src , u32 bytes );

		void 			SetPosition( u32 pos );
		void 			SetBuffer( char* buffer, u32 length );
		u32				GetPosition( );
		std::string		GetFilename( ) { return m_fileName; }

		void			CloseFile(	);
		//-------------------------------------------------------------------------------------------------
		CFileData(  const std::string& filename, void* data, u32 length );
		CFileData(  const std::string& filename, const char* access, u32 offset, u32 length );

		// -----------------------------------------------------------------------------
	private:


		FILE*						m_pFile;
		char*						m_buffer;
		u32							m_bufferLength;
		u32							m_bufferOffset;
		u32							m_bufferStartOffset;
		bool						m_ownsData;
		std::string					m_fileName;

		friend class CFile;
	};

// ------------------------------------------------------------------------------
