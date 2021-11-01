///////////////////////////////////////////////////////////////////////////////
//
// wxFormBuilder - A Visual Dialog Editor for wxWidgets.
// Copyright (C) 2005 José Antonio Hurtado
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// Written by
//   José Antonio Hurtado - joseantonio.hurtado@gmail.com
//   Juan Antonio Ortega  - jortegalalmolda@gmail.com
//
///////////////////////////////////////////////////////////////////////////////

#include "stringutils.h"

#include "typeconv.h"
#include "wxfbexception.h"

#include <ticpp.h>
#include <wx/ffile.h>
#include <wx/fontmap.h>

wxString StringUtils::IntToStr(const int num) {
	wxString result;
	result.Printf(wxT("%d"), num);
	return result;
}

wxString StringUtils::GetSupportedEncodings( bool columnateWithTab, wxArrayString* array )
{
	wxString result = wxEmptyString;
	size_t count = wxFontMapper::GetSupportedEncodingsCount();
	size_t max = 40;
	for ( size_t i = 0; i < count; ++i )
	{
		wxFontEncoding encoding = wxFontMapper::GetEncoding( i );
		wxString name = wxFontMapper::GetEncodingName( encoding );
		size_t length = name.length();
		if ( length > max )
		{
			max = length + 10;
		}
		if ( columnateWithTab )
		{
			name = name.Pad( (size_t)((max - length)/8 + 1), wxT('\t') );
		}
		else
		{
			name = name.Pad( max - length );
		}
		name += wxFontMapper::GetEncodingDescription( encoding );
		if ( NULL != array )
		{
			array->Add( name );
		}
		result += name;
		result += wxT("\n");
	}

	return result;
}

wxFontEncoding StringUtils::GetEncodingFromUser( const wxString& message )
{
	wxArrayString array;
	GetSupportedEncodings( false, &array );
	int selection = ::wxGetSingleChoiceIndex( message, _("Choose an Encoding"), array, wxTheApp->GetTopWindow() );
	if ( -1 == selection )
	{
		return wxFONTENCODING_MAX;
	}
	return wxFontMapper::GetEncoding( selection );
}


namespace XMLUtils
{
	template < class T, class U >
		void LoadXMLFileImp( T& doc, bool condenseWhiteSpace, const wxString& path, U* declaration )
	{
		if ( NULL == declaration )
		{
			// Ask user to all wxFB to convert the file to UTF-8 and add the XML declaration
			wxString msg = _("This xml file has no declaration.\n");
			msg 		+= _("Would you like wxFormBuilder to backup the file and convert it to UTF-8\?\n");
			msg			+= _("You will be prompted for an encoding.\n\n");
			msg			+= _("Path: ");
			msg			+= path;
			int result = wxMessageBox( msg, _("Missing Declaration"), wxICON_QUESTION | wxYES_NO | wxYES_DEFAULT, wxTheApp->GetTopWindow() );
			if ( wxNO == result )
			{
				// User declined, give up
				THROW_WXFBEX( _("Missing Declaration on XML File: ") << path );
			}

			// User accepted, convert the file
			wxFontEncoding chosenEncoding = StringUtils::GetEncodingFromUser( _("Please choose the original encoding.") );
			if ( wxFONTENCODING_MAX == chosenEncoding )
			{
				THROW_WXFBEX( _("Missing Declaration on XML File: ") << path );
			}

			ConvertAndAddDeclaration( path, chosenEncoding );

			// Reload
			LoadXMLFile( doc, condenseWhiteSpace, path );
			return;
		}

		// The file will have a declaration at this point
		wxString version = _WXSTR( declaration->Version() );
		if ( version.empty() )
		{
			version = wxT("1.0");
		}

		wxString standalone = _WXSTR( declaration->Standalone() );
		if ( standalone.empty() )
		{
			standalone = wxT("yes");
		}

		wxString encodingName = _WXSTR( declaration->Encoding() );
		if ( encodingName.empty() )
		{
			// Ask user to all wxFB to convert the file to UTF-8 and add the XML declaration
			wxString msg = _("This xml file has no encoding specified.\n");
			msg 		+= _("Would you like wxFormBuilder to backup the file and convert it to UTF-8\?\n");
			msg			+= _("You will be prompted for an encoding.\n\n");
			msg			+= _("Path: ");
			msg			+= path;
			if ( wxNO == wxMessageBox( msg, _("Unknown Encoding"), wxICON_QUESTION | wxYES_NO | wxYES_DEFAULT, wxTheApp->GetTopWindow() ) )
			{
				// User declined, give up
				THROW_WXFBEX( _("Unknown Encoding for XML File: ") << path );
			}

			// User accepted, convert the file
			wxFontEncoding chosenEncoding = StringUtils::GetEncodingFromUser( _("Please choose the original encoding.") );
			if ( wxFONTENCODING_MAX == chosenEncoding )
			{
				THROW_WXFBEX( _("Unknown Encoding for XML File: ") << path );
			}
			ConvertAndChangeDeclaration( path, version, standalone, chosenEncoding );

			// Reload
			LoadXMLFile( doc, condenseWhiteSpace, path );
			return;
		}

		// The file will have an encoding at this point
		wxFontEncoding encoding = wxFontMapperBase::GetEncodingFromName( encodingName.MakeLower() );
		if ( wxFONTENCODING_UTF8 == encoding )
		{
			// This is what we want
			return;
		}
		else if ( wxFONTENCODING_MAX == encoding )
		{
			wxString msg = wxString::Format( _("The encoding of this xml file is not supported.\n\nFile: %s\nEncoding: %s\nSupported Encodings:\n\n%s"),
							path,
							encodingName,
							StringUtils::GetSupportedEncodings() );
			wxMessageBox( msg, wxString::Format( _("Unsupported Encoding: %s"), encodingName ) );
			THROW_WXFBEX( _("Unsupported encoding for XML File: ") << path );
		}
		else
		{
			// Ask user to all wxFB to convert the file to UTF-8 and add the XML declaration
			wxString msg = wxString::Format( _("This xml file has specified encoding %s. wxFormBuilder only works with UTF-8.\n"),
							wxFontMapper::GetEncodingDescription( encoding ) );
			msg 		+= _("Would you like wxFormBuilder to backup the file and convert it to UTF-8\?\n\n");
			msg			+= _("Path: ");
			msg			+= path;
			if ( wxNO == wxMessageBox( msg, _("Not UTF-8"), wxICON_QUESTION | wxYES_NO | wxYES_DEFAULT, wxTheApp->GetTopWindow() ) )
			{
				// User declined, give up
				THROW_WXFBEX( _("Wrong Encoding for XML File: ") << path );
			}

			// User accepted, convert the file
			ConvertAndChangeDeclaration( path, version, standalone, encoding );

			// Reload
			LoadXMLFile( doc, condenseWhiteSpace, path );
			return;
		}
	}
}

void XMLUtils::LoadXMLFile( ticpp::Document& doc, bool condenseWhiteSpace, const wxString& path )
{
	try
	{
		if ( path.empty() )
		{
			THROW_WXFBEX( _("LoadXMLFile needs a path") )
		}

		if ( !::wxFileExists( path ) )
		{
			THROW_WXFBEX( _("The file does not exist.\nFile: ") << path )
		}
		TiXmlBase::SetCondenseWhiteSpace( condenseWhiteSpace );
		doc.SetValue( std::string( path.mb_str( wxConvFile ) ) );
		doc.LoadFile();
	}
	catch ( ticpp::Exception& )
	{
		// Ask user to all wxFB to convert the file to UTF-8 and add the XML declaration
		wxString msg = _("This xml file could not be loaded. This could be the result of an unsupported encoding.\n");
		msg 		+= _("Would you like wxFormBuilder to backup the file and convert it to UTF-8\?\n");
		msg			+= _("You will be prompted for the original encoding.\n\n");
		msg			+= _("Path: ");
		msg			+= path;
		if ( wxNO == wxMessageBox( msg, _("Unable to load file"), wxICON_QUESTION | wxYES_NO | wxYES_DEFAULT, wxTheApp->GetTopWindow() ) )
		{
			// User declined, give up
			THROW_WXFBEX( _("Unable to load file: ") << path );
		}

		// User accepted, convert the file
		wxFontEncoding chosenEncoding = StringUtils::GetEncodingFromUser( _("Please choose the original encoding.") );
		if ( wxFONTENCODING_MAX == chosenEncoding )
		{
			THROW_WXFBEX( _("Unable to load file: ") << path );
		}

		ConvertAndAddDeclaration( path, chosenEncoding );

		LoadXMLFile( doc, condenseWhiteSpace, path );
	}

	ticpp::Declaration* declaration;
	try
	{
		ticpp::Node* firstChild = doc.FirstChild();
		declaration = firstChild->ToDeclaration();
	}
	catch( ticpp::Exception& )
	{
		declaration = NULL;
	}

	LoadXMLFileImp( doc, condenseWhiteSpace, path, declaration );
}

void XMLUtils::LoadXMLFile( TiXmlDocument& doc, bool condenseWhiteSpace, const wxString& path )
{
	if ( path.empty() )
	{
		THROW_WXFBEX( _("LoadXMLFile needs a path") )
	}

	if ( !::wxFileExists( path ) )
	{
		THROW_WXFBEX( _("The file does not exist.\nFile: ") << path )
	}

	TiXmlBase::SetCondenseWhiteSpace( condenseWhiteSpace );
	doc.SetValue( std::string( path.mb_str( wxConvFile ) ) );
	if ( !doc.LoadFile() )
	{
		// Ask user to all wxFB to convert the file to UTF-8 and add the XML declaration
		wxString msg = _("This xml file could not be loaded. This could be the result of an unsupported encoding.\n");
		msg 		+= _("Would you like wxFormBuilder to backup the file and convert it to UTF-8\?\n");
		msg			+= _("You will be prompted for the original encoding.\n\n");
		msg			+= _("Path: ");
		msg			+= path;
		if ( wxNO == wxMessageBox( msg, _("Unable to load file"), wxICON_QUESTION | wxYES_NO | wxYES_DEFAULT, wxTheApp->GetTopWindow() ) )
		{
			// User declined, give up
			THROW_WXFBEX( _("Unable to load file: ") << path );
		}

		// User accepted, convert the file
		wxFontEncoding chosenEncoding = StringUtils::GetEncodingFromUser( _("Please choose the original encoding.") );
		if ( wxFONTENCODING_MAX == chosenEncoding )
		{
			THROW_WXFBEX( _("Unable to load file: ") << path );
		}

		ConvertAndAddDeclaration( path, chosenEncoding );

		LoadXMLFile( doc, condenseWhiteSpace, path );
	}

	TiXmlDeclaration* declaration = NULL;
	TiXmlNode* firstChild = doc.FirstChild();
	if ( firstChild )
	{
		declaration = firstChild->ToDeclaration();
	}

	LoadXMLFileImp( doc, condenseWhiteSpace, path, declaration );
}


void XMLUtils::ConvertAndAddDeclaration( const wxString& path, wxFontEncoding encoding, bool backup )
{
	ConvertAndChangeDeclaration( path, wxT("1.0"), wxT("yes"), encoding, backup );
}

void XMLUtils::ConvertAndChangeDeclaration( const wxString& path, const wxString& version, const wxString& standalone, wxFontEncoding encoding, bool backup )
{
	// Backup the file
	if ( backup )
	{
		if ( !::wxCopyFile( path, path + wxT(".bak") ) )
		{
			wxString msg = wxString::Format( _("Unable to backup file.\nFile: %s\nBackup: %s.bak"), path, path );
			THROW_WXFBEX( msg )
		}
	}

	// Read the entire contents into a string
	wxFFile oldEncoding( path, wxT("r") );
	wxString contents;
	wxCSConv encodingConv( encoding );
	if ( !oldEncoding.ReadAll( &contents, encodingConv ) )
	{
		wxString msg = wxString::Format( _("Unable to read the file in the specified encoding.\nFile: %s\nEncoding: %s"), path, wxFontMapper::GetEncodingDescription( encoding ) );
		THROW_WXFBEX( msg );
	}

	if ( contents.empty() )
	{
		wxString msg = wxString::Format( _("The file is either empty or read with the wrong encoding.\nFile: %s\nEncoding: %s"), path, wxFontMapper::GetEncodingDescription( encoding ) );
		THROW_WXFBEX( msg );
	}

	if ( !oldEncoding.Close() )
	{
		wxString msg = wxString::Format( _("Unable to close original file.\nFile: %s"), path );
		THROW_WXFBEX( msg );
	}

	// Modify the declaration, so TinyXML correctly determines the new encoding
	int declStart = contents.Find( wxT("<\?") );
	int declEnd = contents.Find( wxT("\?>") );
	if ( wxNOT_FOUND == declStart && wxNOT_FOUND == declEnd )
	{
		int firstElement = contents.Find( wxT("<") );
		if ( wxNOT_FOUND == firstElement )
		{
			firstElement = 0;
		}
		contents.insert( firstElement, wxString::Format( wxT("<\?xml version=\"%s\" encoding=\"UTF-8\" standalone=\"%s\" \?>\n"), version, standalone ) );
	}
	else
	{
		if ( wxNOT_FOUND == declStart )
		{
			wxString msg = wxString::Format( _("Found a declaration end tag \"\?>\" but could not find the start \"<\?\".\nFile: %s"), path );
			THROW_WXFBEX( msg );
		}

		if ( wxNOT_FOUND == declEnd )
		{
			wxString msg = wxString::Format( _("Found a declaration start tag \"<\?\" but could not find the end \"\?>\".\nFile: %s"), path );
			THROW_WXFBEX( msg );
		}

		// declStart and declEnd are both valid, replace that section with a new declaration
		contents.replace(
		    declStart, declEnd - declStart + 2,
		    wxString::Format(wxT("<\?xml version=\"%s\" encoding=\"UTF-8\" standalone=\"%s\" \?>"),
		                     version, standalone));
	}

	// Remove the old file
	if ( !::wxRemoveFile( path ) )
	{
		wxString msg = wxString::Format( _("Unable to delete original file.\nFile: %s"), path );
		THROW_WXFBEX( msg );
	}

	// Write the new file
	wxFFile newEncoding( path, wxT("w") );
	if ( !newEncoding.Write( contents, wxConvUTF8 ) )
	{
		wxString msg = wxString::Format( _("Unable to write file in its new encoding.\nFile: %s\nEncoding: %s"), path, wxFontMapper::GetEncodingDescription( wxFONTENCODING_UTF8 ) );
		THROW_WXFBEX( msg );
	}

	if ( !newEncoding.Close() )
	{
		wxString msg = wxString::Format( _("Unable to close file after converting the encoding.\nFile: %s\nOld Encoding: %s\nNew Encoding: %s"),
											path,
											wxFontMapper::GetEncodingDescription( encoding ),
											wxFontMapper::GetEncodingDescription( wxFONTENCODING_UTF8 ) );
		THROW_WXFBEX( msg );
	}
}
