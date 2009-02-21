
#include "codeParser.h"

void Function::SetHeading( wxString heading )
{
	m_functionHeading = heading;
}

void Function::SetContents( wxString contents )
{
	m_functionContents = contents;
}

wxString Function::GetFunction()
{
	return m_documentation + wxT( "\n" ) + m_functionHeading + wxT( "\n{\n" ) + m_functionContents + wxT( "\n}" );
}


//---------------------------------------------------
//CodeParser
//---------------------------------------------------

void CodeParser::ParseFiles( wxString headerFileName, wxString sourceFileName, wxString className )
{
	m_className = className;
	wxTextFile headerFile( headerFileName );
	wxTextFile sourceFile( sourceFileName );

	wxString header;
	wxString source;

	//start opening files
	if ( headerFile.Open() )
	{
		wxString Str;

		Str = headerFile.GetFirstLine();
		while ( !( headerFile.Eof() ) )
		{
			header << Str;
			header << wxT( '\n' );
			Str = headerFile.GetNextLine();
		}
		headerFile.Close();
	}
	else
	{
		header = wxT( "" );
	}
	if ( sourceFile.Open() )
	{
		wxString Str;

		source = sourceFile.GetFirstLine();
		while ( !( sourceFile.Eof() ) )
		{
			source << Str;
			source << wxT( '\n' );
			Str = sourceFile.GetNextLine();
		}
		sourceFile.Close();
	}
	else
	{
		source = wxT( "" );
	}

	//parse the file contents
	ParseCode( header, source );
}

void CodeParser::ParseCode( wxString header, wxString source )
{
	ParseClass( ParseUserHInclude( header ) );

	ParseSourceFunctions( source );
}

wxString CodeParser::ParseUserHInclude( wxString code )
{
	int userIncludeEnd;

	//find the begining of the user include
	int userIncludeStart = code.Find( wxT( "//// end generated include" ) );
	if ( userIncludeStart != wxNOT_FOUND )
	{
		userIncludeStart = code.find( wxT( '\n' ), userIncludeStart );
		if ( userIncludeStart != wxNOT_FOUND )
		{
			//find the end of the user include
			userIncludeEnd = code.find( wxT( "\n/** Implementing " ), userIncludeStart );

			if ( userIncludeEnd != wxNOT_FOUND )
			{
				userIncludeStart++;
				m_userHInclude = code.substr( userIncludeStart, userIncludeEnd - userIncludeStart );

				// return the remaining code to be further parsed
				return code.substr( userIncludeEnd );
			}
		}

	}
	m_userHInclude = wxT( "" );
	return code;
}

wxString CodeParser::ParseClass( wxString code )
{
	int startClass = code.Find( m_className );
	if ( startClass != wxNOT_FOUND )
	{
		startClass = code.find( '{', startClass );
		if ( startClass != wxNOT_FOUND )
		{
			int stringSize = ParseBrackets( code.Mid( startClass ) );

			ParseUserMembers( code.Mid( startClass, stringSize ) );
			return code.Mid( startClass + stringSize );
		}
	}
	else
	{
		return wxT( "" );
	}
}

void CodeParser::ParseUserMembers( wxString code )
{
	int userMembersStart = code.Find( wxT( "//// end generated class members" ) );
	if ( userMembersStart != wxNOT_FOUND )
	{
		userMembersStart = code.find( '\n', userMembersStart );
		userMembersStart++;
		int stringSize = code.Len() - userMembersStart;
		m_userMemebers = code.Mid( userMembersStart, stringSize );
	}
	else
	{
		m_userMemebers = wxT( "" );
	}
}

wxString CodeParser::ParseSourceFunctions( wxString code )
{
		int loopcheck = 0;
	int functionStart = 0;
	int functionEnd = 0;
	int contentSize;
	wxString funcName;
	Function *func;
	wxString Str;

	while ( 1 )
	{
		//find the find the begining of the function name
		Str = m_className + wxT( "::" );
		functionStart = code.find( Str, functionEnd );
		if ( functionStart == wxNOT_FOUND )
		{
			return wxT( "" );
		}

		func = new Function();

		//find end of function name
		functionEnd = code.find_first_of( wxT( " (" ), functionStart );
		functionStart += m_className.Len() + 2;
		funcName = code.Mid( functionStart, functionEnd - functionStart );

		m_functions[funcName] = func;

		//find the begining of the line on which the function name resides
		functionStart = code.rfind( '\n', functionStart );
		func->SetDocumentation( code.Mid( functionEnd, functionEnd - functionStart ) );
		functionStart++;

		functionEnd = code.find( '\n', functionStart );
		func->SetHeading( code.Mid( functionEnd, functionEnd - functionStart ) );

		//find the opening brackets of the function
		functionStart = code.find( '{', functionStart );
		contentSize = ParseBrackets( code.Mid( functionStart ) ) - 3;
		if ( contentSize != wxNOT_FOUND )
		{
			functionStart += 2;
			functionEnd = functionStart + contentSize;
			func->SetContents( code.Mid( functionStart, functionEnd - functionStart ) );
			functionEnd += 2;
		}
		else
		{
			func->SetContents( wxT( "" ) );
			return wxT( "" );
		}

		loopcheck++;
		if ( loopcheck == 1000 )
		{
			return wxT( "" );
		}
	}
	return wxT( "" );
}

int CodeParser::ParseBrackets( wxString code )
{

		int loopcheck = 0;
	int openingBrackets = 1;
	int closingBrackets = 0;
	int index = 1;
	wxString Str;

	while ( openingBrackets > closingBrackets )
	{
		index = code.find_first_of( wxT( "{}" ), index );
		if ( index == wxNOT_FOUND )
		{
			return index;
		}
		if ( code.GetChar( index ) == '{' )
		{
			index++;
			openingBrackets++;
		}
		else
		{
			index++;
			closingBrackets++;
		}

		loopcheck++;
		if ( loopcheck == 100 )
		{
			return wxNOT_FOUND;
		}
	}
	index -= 1 ;
	return index;
}

wxString CodeParser::GetFunctionContents( wxString function )
{
	wxString contents = wxT( "" );
	Function *func;

	m_functionIter = m_functions.find( function );
	if ( m_functionIter != m_functions.end() )
	{
		func = m_functionIter->second;
		contents = func->GetContents();
		m_functions.erase( m_functionIter );
		delete func;
	}
	return contents;
}

wxString CodeParser::GetRemainingFunctions()
{
	wxString functions;
	m_functionIter = m_functions.begin();
	while ( m_functionIter != m_functions.end() )
	{
		functions +=  m_functionIter->second->GetFunction();
		m_functionIter++;
	}
	return functions;
}


