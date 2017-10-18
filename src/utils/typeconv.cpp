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

#include "typeconv.h"
#include <wx/tokenzr.h>
#include "utils/stringutils.h"
#include "utils/debug.h"
#include "rad/bitmaps.h"
#include <wx/filename.h>
#include <string>
#include <cstring>
#include "rad/appdata.h"
#include <clocale>

#if wxVERSION_NUMBER < 2900
	#include <wx/propgrid/propgrid.h>
	#include <wx/propgrid/propdev.h>
#endif

#include <wx/filesys.h>
#include <wx/artprov.h>
#include "rad/inspector/objinspect.h"

////////////////////////////////////

// Assuming that the locale is constant throughout one execution,
// store the locale so that numbers can be stored in the "C" locale,
// but the rest of the program works in the user's locale.
class LocaleFinder
{
private:
	char* m_locale;

public:
	LocaleFinder()
	{
		// get current locale
		char* localePtr = ::setlocale( LC_NUMERIC, 0 );
		size_t size = ::strlen( localePtr ) + 1;
		m_locale = new char[ size ];
		::strncpy( m_locale, localePtr, size );
	}

	~LocaleFinder()
	{
		delete [] m_locale;
	}

	const char* GetString()
	{
		return m_locale;
	}
};

// Unfortunately, the locale is "C" at the start of execution, and is changed to the correct locale
// sometime later. This means that the LocaleFinder object cannot be simple declared like this:
//  static LocaleFinder s_locale;
// Instead, it must be created the first time that it is used, stored for the duration of the program,
// and deleted on close.
class LocaleHolder
{
private:
	LocaleFinder* m_finder;

public:
	LocaleHolder()
	:
	m_finder( 0 )
	{
	}

	~LocaleHolder()
	{
		delete m_finder;
	}

	const char* GetString()
	{
		if ( 0 == m_finder )
		{
			m_finder = new LocaleFinder;
		}

		return m_finder->GetString();
	}
};

// Creating this object will determine the current locale (when needed) and store it for the duration of the program
static LocaleHolder s_locale;

// Utility class for switching to "C" locale and back
class LocaleSwitcher
{
private:
	const char* m_locale;

public:
	LocaleSwitcher()
	{
		m_locale = s_locale.GetString(); // Get the locale first, or it will be lost!
		::setlocale( LC_NUMERIC, "C" );
	}

	~LocaleSwitcher()
	{
		::setlocale( LC_NUMERIC, m_locale );
	}
};

////////////////////////////////////

using namespace TypeConv;

wxString TypeConv::_StringToWxString(const std::string &str)
{
	return _StringToWxString(str.c_str());
}

wxString TypeConv::_StringToWxString(const char *str)
{
	wxString newstr( str, wxConvUTF8 );
	return newstr;
}

std::string TypeConv::_WxStringToString(const wxString &str)
{
	std::string newstr( str.mb_str(wxConvUTF8) );
	return newstr;
}

std::string TypeConv::_WxStringToAnsiString(const wxString &str)
{
	std::string newstr( str.mb_str(wxConvISO8859_1) );
	return newstr;
//    setlocale(LC_ALL, "");
//    size_t len = wcstombs(NULL, str.char_str(), 0);
//    std::vector<char> buf(len + 1);
//    wcstombs(&buf[0], str.char_str(), len);
//    return std::string(&buf[0]);
}

bool TypeConv::StringToPoint(const wxString &val, wxPoint *point)
{
	wxPoint result;

	bool error = false;
	wxString str_x,str_y;
	long val_x = -1, val_y = -1;

	if (val != wxT(""))
	{
		wxStringTokenizer tkz(val, wxT(","));
		if (tkz.HasMoreTokens())
		{
			str_x = tkz.GetNextToken();
			str_x.Trim(true);
			str_x.Trim(false);
			if (tkz.HasMoreTokens())
			{
				str_y = tkz.GetNextToken();
				str_y.Trim(true);
				str_y.Trim(false);
			}
			else
				error = true;
		}
		else
			error = true;

		if (!error)
			error = !str_x.ToLong(&val_x);

		if (!error)
			error = !str_y.ToLong(&val_y);

		if (!error)
			result = wxPoint(val_x,val_y);
	}
	else
		result = wxDefaultPosition;

	if (error)
		result = wxDefaultPosition;

	point->x = result.x;
	point->y = result.y;

	return !error;
}

wxPoint TypeConv::StringToPoint (const wxString &val)
{
	wxPoint result;
	StringToPoint (val,&result);
	return result;
}

wxSize TypeConv::StringToSize (const wxString &val)
{
	wxPoint point = StringToPoint(val);
	return wxSize(point.x, point.y);
}

int TypeConv::BitlistToInt (const wxString &str)
{
	int result = 0;
	wxStringTokenizer tkz(str, wxT("|"));
	while (tkz.HasMoreTokens())
	{
		wxString token;
		token = tkz.GetNextToken();
		token.Trim(true);
		token.Trim(false);

		result |= GetMacroValue(token);
	}

	return result;
}

wxString TypeConv::PointToString(const wxPoint &point)
{
	wxString value = wxString::Format(wxT("%d,%d"),point.x,point.y);
	return value;
}

wxString TypeConv::SizeToString(const wxSize &size)
{
	wxString value = wxString::Format(wxT("%d,%d"),size.GetWidth(), size.GetHeight());
	return value;
}

int TypeConv::GetMacroValue(const wxString &str)
{
	int value = 0;

	PMacroDictionary dic = MacroDictionary::GetInstance();
	dic->SearchMacro( str, &value );

	return value;
}

int TypeConv::StringToInt(const wxString &str)
{
	long l = 0;
	str.ToLong(&l);

	return (int)l;
}

wxFontContainer TypeConv::StringToFont (const wxString &str)
{
	wxFontContainer font;

	// face name, style, weight, point size, family, underlined
	wxStringTokenizer tkz( str, wxT(",") );

	if ( tkz.HasMoreTokens() )
	{
		wxString faceName = tkz.GetNextToken();
		faceName.Trim( true );
		faceName.Trim( false );
		font.SetFaceName( faceName );
	}

	if ( tkz.HasMoreTokens() )
	{
		long l_style;
		wxString s_style = tkz.GetNextToken();
		if ( s_style.ToLong( &l_style ) )
		{
			font.SetStyle( (int)l_style );
		}
	}

	if ( tkz.HasMoreTokens() )
	{
		long l_weight;
		wxString s_weight = tkz.GetNextToken();
		if ( s_weight.ToLong( &l_weight ) )
		{
			font.SetWeight( (int)l_weight );
		}
	}

	if ( tkz.HasMoreTokens() )
	{
		long l_size;
		wxString s_size = tkz.GetNextToken();
		if ( s_size.ToLong( &l_size ) )
		{
			font.SetPointSize( (int)l_size );
		}
	}

	if ( tkz.HasMoreTokens() )
	{
		long l_family;
		wxString s_family = tkz.GetNextToken();
		if ( s_family.ToLong( &l_family ) )
		{
			font.SetFamily( (int)l_family );
		}
	}

	if ( tkz.HasMoreTokens() )
	{
		long l_underlined;
		wxString s_underlined = tkz.GetNextToken();
		if ( s_underlined.ToLong( &l_underlined ) )
		{
			font.SetUnderlined( l_underlined != 0 );
		}
	}

	return font;
}

wxString TypeConv::FontToString (const wxFontContainer &font)
{
	// face name, style, weight, point size, family, underlined
	return wxString::Format( wxT("%s,%d,%d,%d,%d,%d"), font.GetFaceName().c_str(), font.GetStyle(), font.GetWeight(), font.GetPointSize(), font.GetFamily(), font.GetUnderlined() ? 1 : 0 );
}

std::map< int, wxString > MakeFontFamilyMap()
{
#define MAKE_FAMILY_MAP_ITEM(X) families[X] = #X;
	std::map< int, wxString > families;
	MAKE_FAMILY_MAP_ITEM(wxFONTFAMILY_DEFAULT);
	MAKE_FAMILY_MAP_ITEM(wxFONTFAMILY_DECORATIVE);
	MAKE_FAMILY_MAP_ITEM(wxFONTFAMILY_ROMAN);
	MAKE_FAMILY_MAP_ITEM(wxFONTFAMILY_SCRIPT);
	MAKE_FAMILY_MAP_ITEM(wxFONTFAMILY_SWISS);
	MAKE_FAMILY_MAP_ITEM(wxFONTFAMILY_MODERN);
	MAKE_FAMILY_MAP_ITEM(wxFONTFAMILY_TELETYPE);
	return families;
}

wxString TypeConv::FontFamilyToString( const int family )
{
	static const std::map< int, wxString > s_families = MakeFontFamilyMap();
	std::map< int, wxString >::const_iterator item = s_families.find( family );
	if ( item != s_families.end() )
	{
		return item->second;
	}
	return "wxFONTFAMILY_UNKNOWN";
}

wxBitmap TypeConv::StringToBitmap( const wxString& filename )
{
	#ifndef __WXFB_DEBUG__
	wxLogNull stopLogging;
	#endif

	// Get bitmap from art provider
	if( filename.Contains( _("Load From Art Provider") ) )
	{
		wxString image = filename.AfterFirst( wxT(';') ).Trim( false );
		wxString rid = image.BeforeFirst( wxT(';') ).Trim( false );
		wxString cid = image.AfterFirst( wxT(';') ).Trim( false );

		if( rid.IsEmpty() || cid.IsEmpty() )
		{
			return AppBitmaps::GetBitmap( wxT("unknown") );
		}
		else
		{
			//return wxArtProvider::GetBitmap( rid, cid + wxT("_C") ){
			wxBitmap bmp = wxArtProvider::GetBitmap( rid, cid + wxT("_C") );

			if (!bmp.IsOk())
			{
				return AppBitmaps::GetBitmap( wxT("unknown") );

				/* // Create another bitmap of the appropriate size to show it's invalid.
				// We can get here if the user entered a custom wxArtID which, presumably,
				// they will have already installed in their app.
				bmp = wxArtProvider::GetBitmap( wxT("wxART_MISSING_IMAGE"), cid + wxT("_C") );

				if (bmp.IsOk())
				{
					wxMemoryDC dc;
					dc.SelectObject(bmp);
					dc.SetPen(wxPen(*wxRED, 3));
					dc.DrawLine(wxPoint(0, 0), wxPoint(bmp.GetWidth(), bmp.GetHeight()));
					dc.SelectObject(wxNullBitmap);
				} */
			}
			else
				return bmp;
		}
	}

	// Get path from bitmap property
	wxString path = filename.AfterFirst( wxT(';') ).Trim( false );

	// No value - default bitmap
	if ( !wxFileName( path ).IsOk() )
	{
		return AppBitmaps::GetBitmap( wxT("unknown") );
	}

	// Setup the working directory to the project path - paths should be saved in the .fbp file relative to the location
	// of the .fbp file
	wxFileSystem system;
	system.ChangePathTo( AppData()->GetProjectPath(), true );

	// The loader can get goofy on linux if it starts with file:, not sure why (wxGTK 2.8.7)
	wxFSFile *fsfile = NULL;
	wxString remainder;
	if ( path.StartsWith( wxT("file:"), &remainder ) )
	{
		fsfile = system.OpenFile( remainder, wxFS_READ | wxFS_SEEKABLE );
	}
	else
	{
		fsfile = system.OpenFile( path, wxFS_READ | wxFS_SEEKABLE );
	}

	// Unable to open the file
	if ( fsfile == NULL )
	{
		return AppBitmaps::GetBitmap( wxT("unknown") );
	}

	// Create a wxImage from the file stream
	wxImage img( *(fsfile->GetStream()) );
	delete fsfile;

	// The stream is not an image
	if ( !img.Ok() )
	{
		return AppBitmaps::GetBitmap( wxT("unknown") );
	}

	// Create a wxBitmap from the image
	return wxBitmap( img );
}

void TypeConv::ParseBitmapWithResource( const wxString& value, wxString* image, wxString* source, wxSize* icoSize )
{
	// Splitting bitmap resource property value - it is of the form "path; source [width; height]"

	*image = value;
	*source = _("Load From File");
	*icoSize = wxDefaultSize;

	wxArrayString children;
	wxStringTokenizer tkz( value, wxT("[];"), wxTOKEN_RET_EMPTY );
	while ( tkz.HasMoreTokens() )
	{
		wxString child = tkz.GetNextToken();
		child.Trim( false );
		child.Trim( true );
		children.Add( child );
	}

	if( children.Index( _("Load From Art Provider") ) == wxNOT_FOUND )
	{
		// "break;" was left out intentionally
		long temp;
		switch ( children.size() )
		{
			case 5:
			case 4:
				if( children.size() > 4 )
				{
					children[4].ToLong( &temp );
					icoSize->SetHeight( temp );
				}
			case 3:
				if( children.size() > 3 )
				{
					children[3].ToLong( &temp );
					icoSize->SetWidth( temp );
				}
			case 2:
				if( children.size() > 1 ) *image = children[1];
			case 1:
				if( children.size() > 0 ) *source = children[0];
			default:
				break;
		}
	}
	else
	{
		if( children.size() == 3 )
		{
			*image = children[1] + wxT(":") + children[2];
			*source = children[0];
		}
		else
		{
			*image = wxT("");
			*source = children[0];
		}
	}
	wxLogDebug( wxT("TypeConv:ParseBitmap: source:%s image:%s "), source->c_str(), image->c_str() );
}

wxString TypeConv::MakeAbsolutePath ( const wxString& filename, const wxString& basePath )
{
	wxFileName fnFile( filename );
	wxFileName noChanges = fnFile;
	if ( fnFile.IsRelative() )
	{
		// Es una ruta relativa, por tanto hemos de obtener la ruta completa
		// a partir de basePath
		wxFileName fnBasePath( basePath );
		if ( fnBasePath.IsAbsolute() )
		{
			if ( fnFile.MakeAbsolute(basePath) )
			{
				wxString path = fnFile.GetFullPath();
				return path;
			}
		}
	}

	// Either it is already absolute, or it could not be made absolute, so give it back - but change to '/' for separators
	wxString path = noChanges.GetFullPath();
	return path;
}

wxString TypeConv::MakeRelativePath( const wxString& filename, const wxString& basePath )
{
	wxFileName fnFile( filename );
	wxFileName noChanges = fnFile;
	if ( fnFile.IsAbsolute() )
	{
		wxFileName fnBasePath( basePath) ;
		if ( fnBasePath.IsAbsolute() )
		{
			if ( fnFile.MakeRelativeTo( basePath ) )
			{
				return fnFile.GetFullPath( wxPATH_UNIX );
			}
		}
	}

	// Either it is already relative, or it could not be made relative, so give it back - but change to '/' for separators
	if ( noChanges.IsAbsolute() )
	{
		wxString path = noChanges.GetFullPath();
		return path;
	}
	else
	{
		return noChanges.GetFullPath( wxPATH_UNIX );
	}
}

void TypeConv::SplitFileSystemURL( const wxString& url, wxString* protocol, wxString* path, wxString* anchor )
{
	wxString remainder;
	if ( url.StartsWith( wxT("file:"), &remainder ) )
	{
		*protocol = wxT("file:");
	}
	else
	{
		protocol->clear();
		remainder = url;
	}

	*path = remainder.BeforeFirst( wxT('#') );
	if ( remainder.size() > path->size() )
	{
		*anchor = remainder.substr( path->size() );
	}
	else
	{
		anchor->clear();
	}
}

wxString TypeConv::MakeAbsoluteURL( const wxString& url, const wxString& basePath )
{
	wxString protocol, path, anchor;
	SplitFileSystemURL( url, &protocol, &path, &anchor );
	return protocol + MakeAbsolutePath( path, basePath ) + anchor;
}

wxString TypeConv::MakeRelativeURL( const wxString& url, const wxString& basePath )
{
	wxString protocol, path, anchor;
	SplitFileSystemURL( url, &protocol, &path, &anchor );
	return protocol + MakeRelativePath( path, basePath ) + anchor;
}

#define ElseIfSystemColourConvert( NAME, value )	\
	else if ( value == wxT(#NAME) )					\
	{												\
		systemVal =	NAME;							\
	}

wxSystemColour TypeConv::StringToSystemColour( const wxString& str )
{
	wxSystemColour systemVal = wxSYS_COLOUR_BTNFACE;

	if( false )
	{}
	ElseIfSystemColourConvert( wxSYS_COLOUR_SCROLLBAR, str )
	ElseIfSystemColourConvert( wxSYS_COLOUR_BACKGROUND, str )
	ElseIfSystemColourConvert( wxSYS_COLOUR_ACTIVECAPTION, str )
	ElseIfSystemColourConvert( wxSYS_COLOUR_INACTIVECAPTION, str )
	ElseIfSystemColourConvert( wxSYS_COLOUR_MENU, str )
	ElseIfSystemColourConvert( wxSYS_COLOUR_WINDOW, str )
	ElseIfSystemColourConvert( wxSYS_COLOUR_WINDOWFRAME, str )
	ElseIfSystemColourConvert( wxSYS_COLOUR_MENUTEXT, str )
	ElseIfSystemColourConvert( wxSYS_COLOUR_WINDOWTEXT, str )
	ElseIfSystemColourConvert( wxSYS_COLOUR_CAPTIONTEXT, str )
	ElseIfSystemColourConvert( wxSYS_COLOUR_ACTIVEBORDER, str )
	ElseIfSystemColourConvert( wxSYS_COLOUR_INACTIVEBORDER, str )
	ElseIfSystemColourConvert( wxSYS_COLOUR_APPWORKSPACE, str )
	ElseIfSystemColourConvert( wxSYS_COLOUR_HIGHLIGHT, str )
	ElseIfSystemColourConvert( wxSYS_COLOUR_HIGHLIGHTTEXT, str )
	ElseIfSystemColourConvert( wxSYS_COLOUR_BTNFACE, str )
	ElseIfSystemColourConvert( wxSYS_COLOUR_BTNSHADOW, str )
	ElseIfSystemColourConvert( wxSYS_COLOUR_GRAYTEXT, str )
	ElseIfSystemColourConvert( wxSYS_COLOUR_BTNTEXT, str )
	ElseIfSystemColourConvert( wxSYS_COLOUR_INACTIVECAPTIONTEXT, str )
	ElseIfSystemColourConvert( wxSYS_COLOUR_BTNHIGHLIGHT, str )
	ElseIfSystemColourConvert( wxSYS_COLOUR_3DDKSHADOW, str )
	ElseIfSystemColourConvert( wxSYS_COLOUR_3DLIGHT, str )
	ElseIfSystemColourConvert( wxSYS_COLOUR_INFOTEXT, str )
	ElseIfSystemColourConvert( wxSYS_COLOUR_INFOBK, str )

	return systemVal;
}

wxColour TypeConv::StringToColour( const wxString& str )
{

	// check for system colour
	if ( str.find_first_of( wxT("wx") ) == 0 )
	{
		return wxSystemSettings::GetColour( StringToSystemColour( str ) );
	}
	else
	{
		wxStringTokenizer tkz(str,wxT(","));
		unsigned int red,green,blue;

		red = green = blue = 0;

		//  bool set_red, set_green, set_blue;

		//  set_red = set_green = set_blue = false;

		if (tkz.HasMoreTokens())
		{
			wxString s_red = tkz.GetNextToken();
			long l_red;

			if (s_red.ToLong(&l_red) && (l_red >= 0 && l_red <= 255))
			{
				red = (int)l_red;
				//      set_size = true;
			}
		}

		if (tkz.HasMoreTokens())
		{
			wxString s_green = tkz.GetNextToken();
			long l_green;

			if (s_green.ToLong(&l_green) && (l_green >= 0 && l_green <= 255))
			{
				green = (int)l_green;
				//      set_size = true;
			}
		}

		if (tkz.HasMoreTokens())
		{
			wxString s_blue = tkz.GetNextToken();
			long l_blue;

			if (s_blue.ToLong(&l_blue) && (l_blue >= 0 && l_blue <= 255))
			{
				blue = (int)l_blue;
				//      set_size = true;
			}
		}


		return wxColour(red,green,blue);
	}
}

wxString TypeConv::ColourToString( const wxColour& colour )
{
	return wxString::Format(wxT("%d,%d,%d"),colour.Red(),colour.Green(),colour.Blue());
}

#define SystemColourConvertCase( NAME )	\
	case NAME:							\
		s = wxT(#NAME);						\
		break;

wxString TypeConv::SystemColourToString( long colour )
{
	wxString s;

	switch ( colour )
	{
		SystemColourConvertCase( wxSYS_COLOUR_SCROLLBAR )
		SystemColourConvertCase( wxSYS_COLOUR_BACKGROUND )
		SystemColourConvertCase( wxSYS_COLOUR_ACTIVECAPTION )
		SystemColourConvertCase( wxSYS_COLOUR_INACTIVECAPTION )
		SystemColourConvertCase( wxSYS_COLOUR_MENU )
		SystemColourConvertCase( wxSYS_COLOUR_WINDOW )
		SystemColourConvertCase( wxSYS_COLOUR_WINDOWFRAME )
		SystemColourConvertCase( wxSYS_COLOUR_MENUTEXT )
		SystemColourConvertCase( wxSYS_COLOUR_WINDOWTEXT )
		SystemColourConvertCase( wxSYS_COLOUR_CAPTIONTEXT )
		SystemColourConvertCase( wxSYS_COLOUR_ACTIVEBORDER )
		SystemColourConvertCase( wxSYS_COLOUR_INACTIVEBORDER )
		SystemColourConvertCase( wxSYS_COLOUR_APPWORKSPACE )
		SystemColourConvertCase( wxSYS_COLOUR_HIGHLIGHT )
		SystemColourConvertCase( wxSYS_COLOUR_HIGHLIGHTTEXT )
		SystemColourConvertCase( wxSYS_COLOUR_BTNFACE )
		SystemColourConvertCase( wxSYS_COLOUR_BTNSHADOW )
		SystemColourConvertCase( wxSYS_COLOUR_GRAYTEXT )
		SystemColourConvertCase( wxSYS_COLOUR_BTNTEXT )
		SystemColourConvertCase( wxSYS_COLOUR_INACTIVECAPTIONTEXT )
		SystemColourConvertCase( wxSYS_COLOUR_BTNHIGHLIGHT )
		SystemColourConvertCase( wxSYS_COLOUR_3DDKSHADOW )
		SystemColourConvertCase( wxSYS_COLOUR_3DLIGHT )
		SystemColourConvertCase( wxSYS_COLOUR_INFOTEXT )
		SystemColourConvertCase( wxSYS_COLOUR_INFOBK )
	}

	return s;
}

bool TypeConv::FlagSet  (const wxString &flag, const wxString &currentValue)
{
	bool set = false;
	wxStringTokenizer tkz(currentValue, wxT("|"));
	while (!set && tkz.HasMoreTokens())
	{
		wxString token;
		token = tkz.GetNextToken();
		token.Trim(true);
		token.Trim(false);

		if (token == flag)
			set = true;
	}

	return set;
}

wxString TypeConv::ClearFlag(const wxString &flag, const wxString &currentValue)
{
	if (flag == wxT(""))
		return currentValue;

	wxString result;
	wxStringTokenizer tkz(currentValue, wxT("|"));
	while (tkz.HasMoreTokens())
	{
		wxString token;
		token = tkz.GetNextToken();
		token.Trim(true);
		token.Trim(false);

		if (token != flag)
		{
			if (result != wxT(""))
				result = result + wxT('|');

			result = result + token;
		}
	}

	return result;
}

wxString TypeConv::SetFlag  (const wxString &flag, const wxString &currentValue)
{
	if (flag == wxT(""))
		return currentValue;

	bool found = false;
	wxString result = currentValue;
	wxStringTokenizer tkz(currentValue, wxT("|"));
	while (tkz.HasMoreTokens())
	{
		wxString token;
		token = tkz.GetNextToken();
		token.Trim(true);
		token.Trim(false);

		if (token == flag)
			found = true;
	}

	if (!found)
	{
		if (result != wxT(""))
			result = result + wxT('|');

		result = result + flag;
	}
	return result;
}

// la representación de un array de cadenas será:
// 'string1' 'string2' 'string3'
// el caracter (') se representa dentro de una cadena como ('')
// 'wxString''1'''
wxArrayString TypeConv::OldStringToArrayString( const wxString& str )
{
	int i=0, size = (int)str.Length(), state = 0;
	wxArrayString result;
	wxString substr;
	while (i < size)
	{
		wxChar c = str[i];
		switch (state)
		{
		case 0: // esperando (') de comienzo de cadena
			if (c == wxT('\''))
				state = 1;
			break;
		case 1: // guardando cadena
			if (c == wxT('\''))
			{
				if (i+1 < size && str[i+1] == wxT('\''))
				{
					substr = substr + wxT('\'');  // sustitución ('') por (') y seguimos
					i++;
				}
				else
				{
					result.Add(substr); // fin de cadena
					substr.Clear();
					state = 0;
				}
			}
			else
				substr = substr + c; // seguimos guardado la cadena

			break;
		}
		i++;
	}
	return result;
}

wxArrayString TypeConv::StringToArrayString( const wxString& str )
{
	//wxArrayString result = wxStringTokenize( str, wxT(";") );
	wxArrayString result;

	WX_PG_TOKENIZER2_BEGIN( str, wxT('"') )

		result.Add ( token );

	WX_PG_TOKENIZER2_END()

	return result;
}

wxString TypeConv::ArrayStringToString(const wxArrayString &arrayStr)
{
	wxString result;

#if wxVERSION_NUMBER < 2900
	wxPropertyGrid::ArrayStringToString( result, arrayStr, wxT('"'), wxT('"'), 1 );
#else
	wxArrayStringProperty::ArrayStringToString( result, arrayStr, '"', 1 );
#endif

	return result;
}

wxString TypeConv::ReplaceSynonymous(const wxString &bitlist)
{
	wxMessageBox(wxT("Antes: ")+bitlist);
	wxString result;
	wxString translation;
	wxStringTokenizer tkz(bitlist, wxT("|"));
	while (tkz.HasMoreTokens())
	{
		wxString token;
		token = tkz.GetNextToken();
		token.Trim(true);
		token.Trim(false);

		if (result != wxT(""))
			result = result + wxChar('|');

		if (MacroDictionary::GetInstance()->SearchSynonymous( token, translation))
			result += translation;
		else
			result += token;

	}
	wxMessageBox(wxT("Despues: ")+result);
	return result;
}


wxString TypeConv::TextToString(const wxString &str)
{
	wxString result;

	for (unsigned int i=0 ; i < str.length() ; i++)
	{
		wxChar c = str[i];
		if ( c == wxT('\\') )
		{
			if (i < str.length() - 1)
			{
				wxChar next = str[i+1];

				switch (next)
				{
				case wxT('n'): result += wxT('\n');
					i++;
					break;

				case wxT('t'): result += wxT('\t');
					i++;
					break;

				case wxT('r'): result += wxT('\r');
					i++;
					break;

				case wxT('\\'): result += wxT('\\');
					i++;
					break;
				}
			}
		}
		else
			result += c;
	}

	return result;
}

wxString TypeConv::StringToText(const wxString &str)
{
	wxString result;

	for (unsigned int i=0 ; i < str.length() ; i++)
	{
		wxChar c = str[i];

		switch (c)
		{
		case wxT('\n'): result += wxT("\\n");
			break;

		case wxT('\t'): result += wxT("\\t");
			break;

		case wxT('\r'): result += wxT("\\r");
			break;

		case wxT('\\'): result += wxT("\\\\");
			break;

		default:
			result += c;
			break;
		}
	}
	return result;
}

double TypeConv::StringToFloat( const wxString& str )
{
	// Numbers are stored in "C" locale
	LocaleSwitcher switcher;

	double out;
	str.ToDouble( &out );
	return out;
}

wxString TypeConv::FloatToString( const double& val )
{
	// Numbers are stored in "C" locale
	LocaleSwitcher switcher;

	wxString convert;
	convert << val;
	return convert;
}

///////////////////////////////////////////////////////////////////////////////

PMacroDictionary MacroDictionary::s_instance = 0;

PMacroDictionary MacroDictionary::GetInstance()
{
	if ( 0 == s_instance )
	{
		s_instance = new MacroDictionary();
	}

	return s_instance;
}

void MacroDictionary::Destroy()
{
	delete s_instance;
	s_instance = 0;
}

bool MacroDictionary::SearchMacro(wxString name, int *result)
{
	bool found = false;
	MacroMap::iterator it = m_map.find(name);
	if (it != m_map.end())
	{
		found = true;
		*result = it->second;
	}

	return found;
}

bool MacroDictionary::SearchSynonymous(wxString synName, wxString& result)
{
	bool found = false;
	SynMap::iterator it = m_synMap.find(synName);
	if (it != m_synMap.end())
	{
		found = true;
		result = it->second;
	}

	return found;
}
/*
#define MACRO(x) m_map.insert(MacroMap::value_type(#x,x))
#define MACRO2(x,y) m_map.insert(MacroMap::value_type(#x,y))*/

void MacroDictionary::AddMacro(wxString name, int value)
{
	m_map.insert(MacroMap::value_type(name,value));
}

void MacroDictionary::AddSynonymous(wxString synName, wxString name)
{
	m_synMap.insert(SynMap::value_type(synName, name));
}

MacroDictionary::MacroDictionary()
{
	// Las macros serán incluidas en las bibliotecas de componentes...
	// Sizers macros
}


