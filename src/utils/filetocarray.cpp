#include "filetocarray.h"

#include "../rad/appdata.h"
#include "../model/objectbase.h"
#include "../codegen/codewriter.h"
#include "../codegen/cppcg.h"
#include "typeconv.h"
#include "wxfbexception.h"

#include <wx/filename.h>

#include <fstream>

#define CASE_BITMAP_TYPE( x )	\
	case x:						\
		return wxT(#x);

wxString GetBitmapTypeName( long type )
{
	switch( type )
	{
		CASE_BITMAP_TYPE( wxBITMAP_TYPE_BMP )
		CASE_BITMAP_TYPE( wxBITMAP_TYPE_ICO )
		CASE_BITMAP_TYPE( wxBITMAP_TYPE_CUR )
		CASE_BITMAP_TYPE( wxBITMAP_TYPE_XBM )
		CASE_BITMAP_TYPE( wxBITMAP_TYPE_XBM_DATA )
		CASE_BITMAP_TYPE( wxBITMAP_TYPE_XPM )
		CASE_BITMAP_TYPE( wxBITMAP_TYPE_XPM_DATA )
		CASE_BITMAP_TYPE( wxBITMAP_TYPE_TIF )
		CASE_BITMAP_TYPE( wxBITMAP_TYPE_GIF )
		CASE_BITMAP_TYPE( wxBITMAP_TYPE_PNG )
		CASE_BITMAP_TYPE( wxBITMAP_TYPE_JPEG )
		CASE_BITMAP_TYPE( wxBITMAP_TYPE_PNM )
		CASE_BITMAP_TYPE( wxBITMAP_TYPE_PCX )
		CASE_BITMAP_TYPE( wxBITMAP_TYPE_PICT )
		CASE_BITMAP_TYPE( wxBITMAP_TYPE_ICON )
		CASE_BITMAP_TYPE( wxBITMAP_TYPE_ANI )
		CASE_BITMAP_TYPE( wxBITMAP_TYPE_IFF )
		CASE_BITMAP_TYPE( wxBITMAP_TYPE_TGA )
		CASE_BITMAP_TYPE( wxBITMAP_TYPE_MACCURSOR )
		default:
			return wxT("wxBITMAP_TYPE_ANY");
	};
}

wxString GetBitmapType( const wxFileName& sourceFileName )
{
	wxImageHandler* handler = wxImage::FindHandler( sourceFileName.GetExt(), wxBITMAP_TYPE_ANY );
	if ( handler)
	{
		return GetBitmapTypeName( handler->GetType() );
	}
	else
	{
		return wxT("wxBITMAP_TYPE_ANY");
	}
}

wxString FileToCArray::Generate( const wxString& sourcePath )
{
	wxFileName sourceFileName( sourcePath );

	const wxString& sourceFullName = sourceFileName.GetFullName();
	const wxString targetFullName = sourceFullName + wxT(".h");
	wxString arrayName = CppCodeGenerator::ConvertEmbeddedBitmapName( sourcePath );

	if ( !sourceFileName.FileExists() )
	{
		wxLogWarning( sourcePath + wxT(" does not exist") );
		return targetFullName;
	}

	PObjectBase project = AppData()->GetProjectData();

	// Get the output path
	wxString outputPath;
	wxString embeddedFilesOutputPath;
	try
	{
		outputPath = AppData()->GetOutputPath();
		embeddedFilesOutputPath = AppData()->GetEmbeddedFilesOutputPath();
	}
	catch ( wxFBException& ex )
	{
		wxLogWarning( ex.what() );
		return targetFullName;
	}

	// Determin if Microsoft BOM should be used
	bool useMicrosoftBOM = false;
	PProperty pUseMicrosoftBOM = project->GetProperty( wxT( "use_microsoft_bom" ) );
	if ( pUseMicrosoftBOM )
	{
		useMicrosoftBOM = ( pUseMicrosoftBOM->GetValueAsInteger() != 0 );
	}

	// Determine if Utf8 or Ansi is to be created
	bool useUtf8 = false;
	PProperty pUseUtf8 = project->GetProperty( _("encoding") );

	if ( pUseUtf8 )
	{
		useUtf8 = ( pUseUtf8->GetValueAsString() != wxT("ANSI") );
	}

	// setup output file
	PCodeWriter arrayCodeWriter( new FileCodeWriter( embeddedFilesOutputPath + targetFullName, useMicrosoftBOM, useUtf8 ) );

	const wxString headerGuardName = arrayName.Upper() + wxT("_H");
	arrayCodeWriter->WriteLn( wxT("#ifndef ") + headerGuardName );
	arrayCodeWriter->WriteLn( wxT("#define ") + headerGuardName );
	arrayCodeWriter->WriteLn();

	arrayCodeWriter->WriteLn( wxT("#include <wx/mstream.h>") );
	arrayCodeWriter->WriteLn( wxT("#include <wx/image.h>") );
	arrayCodeWriter->WriteLn( wxT("#include <wx/bitmap.h>") );
	arrayCodeWriter->WriteLn();

	arrayCodeWriter->WriteLn( wxT("static const unsigned char ") + arrayName + wxT("[] = ") );
	arrayCodeWriter->WriteLn( wxT("{") );
	arrayCodeWriter->Indent();

	unsigned int count = 1;
	const unsigned int bytesPerLine = 10;
	std::ifstream binFile( static_cast< const char* >( sourcePath.mb_str( wxConvFile ) ), std::ios::binary );
	for ( std::istreambuf_iterator< char > byte( binFile ), end; byte != end; ++byte, ++count )
	{
		arrayCodeWriter->Write( wxString::Format( wxT("0x%02X, "), static_cast< unsigned int >( static_cast< unsigned char >( *byte ) ) ) );
		if ( count >= bytesPerLine )
		{
			arrayCodeWriter->WriteLn();
			count = 0;
		}
	}
	if ( (count < bytesPerLine) && (count != 1) )
	{
		arrayCodeWriter->WriteLn();
	}
	arrayCodeWriter->Unindent();
	arrayCodeWriter->WriteLn( wxT("};") );
	arrayCodeWriter->WriteLn();

	arrayCodeWriter->WriteLn( wxT("wxBitmap& ") + arrayName + wxT("_to_wx_bitmap()") );
	arrayCodeWriter->WriteLn( wxT("{") );
	arrayCodeWriter->Indent();
	arrayCodeWriter->WriteLn( wxT("static wxMemoryInputStream memIStream( ") + arrayName + wxT(", sizeof( ") + arrayName + wxT(" ) );") );
	arrayCodeWriter->WriteLn( wxT("static wxImage image( memIStream, ") + GetBitmapType( sourceFileName ) + wxT(" );") );
	arrayCodeWriter->WriteLn( wxT("static wxBitmap bmp( image );") );
	arrayCodeWriter->WriteLn( wxT("return bmp;") );
	arrayCodeWriter->Unindent();
	arrayCodeWriter->WriteLn( wxT("};") );
	arrayCodeWriter->WriteLn();

	arrayCodeWriter->WriteLn();
	arrayCodeWriter->WriteLn( wxT("#endif //") + headerGuardName );

	return TypeConv::MakeRelativePath( embeddedFilesOutputPath + targetFullName, outputPath );
}

