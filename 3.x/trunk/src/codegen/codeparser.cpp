
#include "codeParser.h"

void Function::SetHeading(wxString heading)
{
	m_functionHeading = heading;
}

void Function::SetContents(wxString contents)
{
	m_functionContents = contents;
}

wxString Function::GetFunction()
{
	wxString Str;
	
	Str << wxT("\n");
	Str << m_documentation;
	Str << wxT("\n");
	Str << m_functionHeading;
	Str << wxT("\n{\n");
	Str << m_functionContents;
	Str << wxT("\n}");

	return  Str;
}


//---------------------------------------------------
//CodeParser
//---------------------------------------------------

void CCodeParser::ParseCFiles(wxString className)
{
	m_className = className;
	wxTextFile headerFile(m_hFile);
	wxTextFile sourceFile(m_cFile);

	wxString header;
	wxString source;

	//start opening files
	if (headerFile.Open())
	{
		wxString Str;

		Str = headerFile.GetFirstLine();
		while (!(headerFile.Eof()))
		{
			header << Str;
			header << wxT('\n');
			Str = headerFile.GetNextLine();
		}
		headerFile.Close();
	}
	else
	{
		header = wxT("");
	}
	if (sourceFile.Open())
	{
		wxString Str;

		source = sourceFile.GetFirstLine();
		while (!(sourceFile.Eof()))
		{
			source << Str;
			source << wxT('\n');
			Str = sourceFile.GetNextLine();
		}
		sourceFile.Close();
	}
	else
	{
		source = wxT("");
	}

	//parse the file contents
	ParseCCode(header, source);
}

void CCodeParser::ParseCCode(wxString header, wxString source)
{
	ParseCInclude(header);
	ParseCClass(header);

	ParseSourceFunctions(source);
}

void CCodeParser::ParseCInclude(wxString code)
{
	int userIncludeEnd;

	//find the begining of the user include
	int userIncludeStart = code.Find(wxT("//// end generated include"));
	if (userIncludeStart != wxNOT_FOUND)
	{
		userIncludeStart = code.find(wxT('\n'), userIncludeStart);
		if (userIncludeStart != wxNOT_FOUND)
		{
			//find the end of the user include
			userIncludeEnd = code.find(wxT("\n/** Implementing "), userIncludeStart);

			if (userIncludeEnd != wxNOT_FOUND)
			{
				userIncludeStart++;
				m_userInclude = code.substr(userIncludeStart, userIncludeEnd - userIncludeStart);

			}
		}

	}
	m_userInclude = wxT("");
}

void CCodeParser::ParseCClass(wxString code)
{
	int startClass = code.Find(wxT("class ") + m_className);
	if (startClass != wxNOT_FOUND)
	{
		code = ParseBrackets(code, startClass);
		if (startClass != wxNOT_FOUND)
		{
			ParseCUserMembers(code);
		}
	}
}

void CCodeParser::ParseCUserMembers(wxString code)
{
	int userMembersStart = code.Find(wxT("//// end generated class members"));
	if (userMembersStart != wxNOT_FOUND)
	{
		userMembersStart = code.find('\n', userMembersStart);
		userMembersStart++;
		m_userMemebers = code.Mid(userMembersStart);
	}
	else
	{
		m_userMemebers = wxT("");
	}
}

wxString CCodeParser::ParseSourceFunctions(wxString code)
{
	int functionStart = 0;
	int functionEnd = 0;
	int previousFunctionEnd = 0;
	int contentSize;
	wxString funcName, funcArg;
	Function *func;
	wxString Str, R;
	int num;

	int loop = 0;
	while (1)
	{
		//find the begining of the function name
		Str = m_className + wxT("::");
		functionStart = code.find(Str, previousFunctionEnd);
		if (functionStart == wxNOT_FOUND)
		{
			//Get the last bit of remaining code after the last function in the file
			m_trailingCode = code.Mid(previousFunctionEnd);
			m_trailingCode.RemoveLast();
			return wxT("");
		}
		//found a function now creat a new function class
		func = new Function();

		//find end of function name
		functionEnd = code.find_first_of(wxT("("), functionStart);
		functionStart += m_className.Len() + 2;
		funcName = code.Mid(functionStart, functionEnd - functionStart);
		
		m_functions[funcName] = func;

		//find the begining of the line on which the function name resides
		functionStart = code.rfind('\n', functionStart);
		func->SetDocumentation(code.Mid(previousFunctionEnd, functionStart - previousFunctionEnd));
		functionStart++;

		functionEnd = code.find('\n', functionStart);
		func->SetHeading(code.Mid(functionStart, functionEnd - functionStart));

		//find the opening brackets of the function
		func->SetContents(ParseBrackets(code,  functionStart));
		if (functionStart != wxNOT_FOUND)
		{
			functionEnd = functionStart;
		}
		else
		{
			wxMessageBox(wxT("Brackets Missing in Source File!"));
			code.insert(functionEnd +1, wxT("//The Following Block is missing a closing bracket\n//and has been set aside by wxFormbuilder\n"));
			func->SetContents(wxT(""));
		}

		previousFunctionEnd = functionEnd + 1;
		if (loop == 100)
		{
			return wxT("");
		}
		loop++;
	}
	
	
	return wxT("");
}

wxString CCodeParser::ParseBrackets(wxString code, int& functionStart)
{
	int openingBrackets = 0;
	int closingBrackets = 0;
	int index = 0;
	wxString Str;

	index = code.find('{', functionStart);
	if (index != wxNOT_FOUND)
	{
		openingBrackets++;
		index +=2;
		functionStart = index;
		int loop = 0;
		while (openingBrackets > closingBrackets)
		{
			index = code.find_first_of(wxT("{}"), index);
			if (index == wxNOT_FOUND)
			{
				Str = code.Mid(functionStart, index);
				functionStart = index;
				return Str;
			}
			if (code.GetChar(index) == '{')
			{
				index++;
				openingBrackets++;
			}
			else
			{
				index++;
				closingBrackets++;
			}
			if (loop == 100)
			{
				return wxT("");
			}
			loop++;
		}
		index = index - functionStart;
		index -= 2;
	}
	else
	{
		wxMessageBox(wxT("no brackets found"));
	}
	Str = code.Mid(functionStart, index);
	functionStart = functionStart + index + 2;
	return Str;
}

wxString CodeParser::GetFunctionDocumentation(wxString function)
{
	wxString contents = wxT("");
	Function *func;

	m_functionIter = m_functions.find(function);
	if (m_functionIter != m_functions.end())
	{
		func = m_functionIter->second;
		contents = func->GetDocumentation();
	}
	return contents;
}

wxString CodeParser::GetFunctionContents(wxString function)
{
	wxString contents = wxT("");
	Function *func;

	m_functionIter = m_functions.find(function);
	if (m_functionIter != m_functions.end())
	{
		func = m_functionIter->second;
		contents = func->GetContents();
		m_functions.erase(m_functionIter);
		delete func;
	}
	return contents;
}

wxString CodeParser::GetRemainingFunctions()
{
	wxString functions;
	m_functionIter = m_functions.begin();
	while (m_functionIter != m_functions.end())
	{
		functions +=  m_functionIter->second->GetFunction();
		m_functionIter++;
	}
	return functions;
}


