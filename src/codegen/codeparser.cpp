#include "codeparser.h"

wxString RemoveWhiteSpace(wxString &str) {
	size_t index = 0;
	while (index < str.Len()) {
		if (str.GetChar(index) == ' ' || str.GetChar(index) == '\t' || str.GetChar(index) == '\n') {
			str.Remove(index, 1);
		} else {
			index++;
		}
	}
	return str;
}

void CppFunction::SetHeading(wxString &heading) {
	m_functionHeading = heading;
}

void CppFunction::SetContents(wxString &contents) {
	if (contents.Left(1) == '\n') {
		contents.Remove(0, 1);
	}
	if (contents.Right(1) == '\n') {
		contents.Remove(contents.Len() - 1, 1);
	}
	m_functionContents = contents;
}

wxString CppFunction::GetFunction() {
	wxString Str;

	Str << wxT("\n");
	Str << m_documentation;
	Str << wxT("\n");
	Str << m_functionHeading;
	Str << wxT("\n{\n");
	Str << m_functionContents;
	Str << wxT("\n}");

	return Str;
}

ArrayCppFunction::ArrayCppFunction()
: ArrayCppFunctionBase()
{}

//---------------------------------------------------
// CodeParser
//---------------------------------------------------

void CppCodeParser::ParseFiles(wxString &className) {

	m_className = className;
	wxTextFile headerFile(m_headerFileName);
	wxTextFile sourceFile(m_sourceFileName);

	wxString header;
	wxString source;

	// start opening files
	if (headerFile.Open()) {
		wxString Str;

		Str = headerFile.GetFirstLine();
		while (!(headerFile.Eof())) {
			header << Str;
			header << wxT('\n');
			Str = headerFile.GetNextLine();
		}
		headerFile.Close();
	} else {
		header = wxT("");
	}
	if (sourceFile.Open()) {
		wxString Str;

		source = sourceFile.GetFirstLine();
		while (!(sourceFile.Eof())) {
			source << Str;
			source << wxT('\n');
			Str = sourceFile.GetNextLine();
		}
		sourceFile.Close();
	} else {
		source = wxT("");
	}

	// parse the file contents
	ParseCode(header, source);
}

void CppCodeParser::ParseCode(wxString &header, wxString &source) {
	ParseHeaderFilePreProc(header);
	ParseHeaderFileClass(header);

	ParseSourceFileFunctions(source);
}

void CppCodeParser::ParseHeaderFilePreProc(wxString &code) {
	int userIncludeEnd;
	m_userIncludeFilePreProc.Clear();

	// find the begining of the user include

	int userIncludeStart = code.Find(wxT("//// end generated include. Place User Code Below"));
	if (userIncludeStart != wxNOT_FOUND) 
	{

		userIncludeStart = code.find(wxT('\n'), userIncludeStart);
		if (userIncludeStart != wxNOT_FOUND) 
		{
			// find the end of the user include
			userIncludeEnd = code.find(wxT("\n/** Implementing "), userIncludeStart);

			if (userIncludeEnd != wxNOT_FOUND) 
			{
				userIncludeStart++;
				m_userIncludeFilePreProc = code.substr(userIncludeStart, userIncludeEnd - userIncludeStart);
			}
		}
	}
}

void CppCodeParser::ParseSourceFilePreProc(wxString &code) {
	int userIncludeEnd;
	m_userSourceFilePreProc.Clear();

	// find the begining of the user include

	int userIncludeStart = code.Find(wxT("//// end generated include. Place User Code Below"));
	if (userIncludeStart != wxNOT_FOUND) 
	{
		userIncludeStart = code.find(wxT('\n'), userIncludeStart);
		if (userIncludeStart != wxNOT_FOUND) 
		{
			// find the end of the user include
			wxString Str = m_className + wxT("::");
			userIncludeEnd = code.find(Str, userIncludeStart);
			if (userIncludeEnd != wxNOT_FOUND) 
			{
				userIncludeEnd = code.rfind(wxT('\n'), userIncludeEnd);
				if (userIncludeEnd != wxNOT_FOUND) 
				{
					userIncludeStart++;
					m_userSourceFilePreProc = code.substr(userIncludeStart, userIncludeEnd - userIncludeStart);
				}
			}
		}
	}
}

void CppCodeParser::ParseHeaderFileClass(wxString &code) {
	int startClass = code.Find(wxT("class ") + m_className);
	if (startClass != wxNOT_FOUND) {
		code = ParseBrackets(code, startClass);
		if (startClass != wxNOT_FOUND) {
			ParseHeaderFileUserMembers(code);
		}
	}
}

void CppCodeParser::ParseHeaderFileUserMembers(wxString &code) {
	m_userHeaderFileMembers = wxT("");
	int userMembersStart = code.Find(wxT("//// end generated class members. Place User Code Below"));
	if (userMembersStart != wxNOT_FOUND) {
		userMembersStart = code.find('\n', userMembersStart);
		if (userMembersStart == wxNOT_FOUND) {
			m_userHeaderFileMembers = wxT("");
		} else {
			userMembersStart++;
			if (userMembersStart < (int)code.Len()) {
				m_userHeaderFileMembers = code.Mid(userMembersStart);
			}
		}
	}
}

void CppCodeParser::ParseSourceFileFunctions(wxString &code) {
	int functionStart = 0;
	int functionEnd = 0;
	int previousFunctionEnd = 0;
	wxString funcName, funcArg;
	CppFunction* function;
	wxString Str, R;

	ParseSourceFilePreProc(code);

	wxString codeSection;
	while (functionStart != wxNOT_FOUND) {
		// find the begining of the function name
		Str = m_className + wxT("::");
		functionStart = code.find(Str, previousFunctionEnd);
		if (functionStart == wxNOT_FOUND) {
			// Get the last bit of remaining code after the last function in the file
			if(code.GetChar(previousFunctionEnd) == '\n')
				{previousFunctionEnd++;}
			m_userSourceFileTrailing = code.Mid(previousFunctionEnd);
			m_userSourceFileTrailing.RemoveLast();
			return;
		}
		// found a function now creat a new function class
		function = new CppFunction();

		// find the begining of the line on which the function name resides
		functionStart = code.rfind('\n', functionStart);
		codeSection = code.Mid(previousFunctionEnd, functionStart - previousFunctionEnd);
		function->SetDocumentation(codeSection);
		functionStart++;

		functionEnd = code.find('{', functionStart);
		codeSection = code.Mid(functionStart, functionEnd - functionStart);
		if (codeSection.Right(1) == '\n') {
			codeSection.RemoveLast();
		}
		function->SetHeading(codeSection);

		AddFunction(function);

		// find the opening brackets of the function
		codeSection = ParseBrackets(code, functionStart);

		if (functionStart == wxNOT_FOUND) {
			wxMessageBox(wxT("Brackets Missing in Source File!"));
			code.insert(functionEnd + 1,
			            wxT("//The Following Block is missing a closing bracket\n//and has been "
			                "set aside by wxFormbuilder\n"));

			codeSection.Clear();
		}
		else {
			functionEnd = functionStart;
		}  
		function->SetContents(codeSection);

		previousFunctionEnd = functionEnd;
	}
}

wxString CppCodeParser::ParseBrackets(wxString &code, int &functionStart) {
	int openingBrackets = 0;
	int closingBrackets = 0;
	int index = 0;
	wxString Str;

	int functionLength = 0;
	index = code.find('{', functionStart);
	if (index != wxNOT_FOUND) {
		openingBrackets++;
		index++;
		functionStart = index;
		int loop = 0;
		while (openingBrackets > closingBrackets) {
			index = code.find_first_of(wxT("{}"), index);
			if (index == wxNOT_FOUND) {
				Str = code.Mid(functionStart, index);
				functionStart = index;
				return Str;
			}
			if (code.GetChar(index) == '{') {
				index++;
				openingBrackets++;
			} else {
				index++;
				closingBrackets++;
			}
			if (loop == 100) {
				return wxT("");
			}
			loop++;
		}
		index--;
		functionLength = index - functionStart;
	} else {
		wxMessageBox(wxT("no brackets found"));
	}
	Str = code.Mid(functionStart, functionLength);
	if(code.GetChar((functionStart + functionLength)-1) == '\n')
		{Str.RemoveLast();}
	functionStart = functionStart + functionLength + 1;
	return Str;
}

wxString CppCodeParser::GetFunctionDocumentation(wxString &functionHeading) {
	wxString code = wxT("");
	CppFunction* function;

	function = GetFunction(functionHeading);
	if(function)
	{
		code = function->GetDocumentation();
	}
	return code;
}

wxString CppCodeParser::GetFunctionContents(wxString &functionHeading) {
	wxString code = wxT("");
	CppFunction* function;

	function = GetFunction(functionHeading);
	if(function)
	{
		code = function->GetContents();
	}
	return code;
}


wxString CppCodeParser::GetAllFunctions() {
	wxString code;	
	int index = 0;

	while(index < m_functionList.GetCount())
	{
		code += m_functionList.Item(index)->GetFunction();
		index++;
	}
	return code;
}
