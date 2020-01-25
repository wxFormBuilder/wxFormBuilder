#include "codeparser.h"

wxString RemoveWhiteSpace(wxString str) {
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

void Function::SetHeading(wxString heading) {
	m_functionHeading = heading;
}

void Function::SetContents(wxString contents) {
	if (contents.Left(1) == '\n') {
		contents.Remove(0, 1);
	}
	if (contents.Right(1) == '\n') {
		contents.Remove(contents.Len() - 1, 1);
	}
	m_functionContents = contents;
}

wxString Function::GetFunction() {
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

//---------------------------------------------------
// CodeParser
//---------------------------------------------------

void CCodeParser::ParseFiles(wxString className) {

	m_className = className;
	wxTextFile headerFile(m_hFile);
	wxTextFile sourceFile(m_cFile);

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

void CCodeParser::ParseCode(wxString header, wxString source) {
	ParseHInclude(header);
	ParseHClass(header);

	ParseCFunctions(source);
}

void CCodeParser::ParseHInclude(wxString code) {
	int userIncludeEnd;
	m_userInclude_H = wxT("");

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
				m_userInclude_H = code.substr(userIncludeStart, userIncludeEnd - userIncludeStart);
			}
		}
	}
}

void CCodeParser::ParseCInclude(wxString code) {
	int userIncludeEnd;
	m_userInclude_CPP = wxT("");

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
					m_userInclude_CPP = code.substr(userIncludeStart, userIncludeEnd - userIncludeStart);
				}
			}
		}
	}
}

void CCodeParser::ParseHClass(wxString code) {
	int startClass = code.Find(wxT("class ") + m_className);
	if (startClass != wxNOT_FOUND) {
		code = ParseBrackets(code, startClass);
		if (startClass != wxNOT_FOUND) {
			ParseHUserMembers(code);
		}
	}
}

void CCodeParser::ParseHUserMembers(wxString code) {
	m_userMembers_CPP = wxT("");
	int userMembersStart = code.Find(wxT("//// end generated class members. Place User Code Below"));
	if (userMembersStart != wxNOT_FOUND) {
		userMembersStart = code.find('\n', userMembersStart);
		if (userMembersStart == wxNOT_FOUND) {
			m_userMembers_CPP = wxT("");
		} else {
			userMembersStart++;
			if (userMembersStart < (int)code.Len()) {
				m_userMembers_CPP = code.Mid(userMembersStart);
			}
		}
	}
}

void CCodeParser::ParseCFunctions(wxString code) {
	int functionStart = 0;
	int functionEnd = 0;
	int previousFunctionEnd = 0;
	wxString funcName, funcArg;
	Function* func;
	wxString Str, R;

	ParseCInclude(code);

	int loop = 0;
	while (1) {
		// find the begining of the function name
		Str = m_className + wxT("::");
		functionStart = code.find(Str, previousFunctionEnd);
		if (functionStart == wxNOT_FOUND) {
			// Get the last bit of remaining code after the last function in the file
			m_trailingCode_CPP = code.Mid(previousFunctionEnd);
			m_trailingCode_CPP.RemoveLast();
			return;
		}
		// found a function now creat a new function class
		func = new Function();

		// find the begining of the line on which the function name resides
		functionStart = code.rfind('\n', functionStart);
		func->SetDocumentation(code.Mid(previousFunctionEnd, functionStart - previousFunctionEnd));
		functionStart++;

		functionEnd = code.find('{', functionStart);
		wxString heading = code.Mid(functionStart, functionEnd - functionStart);
		if (heading.Right(1) == '\n') {
			heading.RemoveLast();
		}
		func->SetHeading(heading);

		AddFunction(heading, func);
		//m_functions[std::string(RemoveWhiteSpace(heading).ToUTF8())] = func;

		// find the opening brackets of the function
		func->SetContents(ParseBrackets(code, functionStart));
		if (functionStart != wxNOT_FOUND) {
			functionEnd = functionStart;
		} else {
			wxMessageBox(wxT("Brackets Missing in Source File!"));
			code.insert(functionEnd + 1,
			            wxT("//The Following Block is missing a closing bracket\n//and has been "
			                "set aside by wxFormbuilder\n"));
			func->SetContents(wxT(""));
		}

		previousFunctionEnd = functionEnd;
		if (loop == 100) {
			return;
		}
		loop++;
	}
}

wxString CCodeParser::ParseBrackets(wxString code, int& functionStart) {
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
	functionStart = functionStart + functionLength + 1;
	return Str;
}

wxString CodeParser::GetFunctionDocumentation(wxString function) {
	wxString contents = wxT("");
	Function* func;

	m_functionIter = m_functions.find(std::string(RemoveWhiteSpace(function).ToUTF8()));
	if (m_functionIter != m_functions.end()) {
		func = m_functionIter->second;
		contents = func->GetDocumentation();
	}
	return contents;
}

wxString CodeParser::GetFunctionContents(wxString function) {
	wxString contents = wxT("");
	Function* func;

	m_functionIter = m_functions.find(std::string(RemoveWhiteSpace(function).ToUTF8()));
	if (m_functionIter != m_functions.end()) {
		func = m_functionIter->second;
		contents = func->GetContents();
		m_functions.erase(m_functionIter);
		delete func;
	}
	return contents;
}


wxString CodeParser::GetAllFunctions() {
	wxString functions;	
	int index = 0;

	while(index < m_functionList.GetCount())
	{
		m_functionIter = m_functions.find(std::string(RemoveWhiteSpace(m_functionList[index]).ToUTF8()));
		if (m_functionIter != m_functions.end()) {
			functions += m_functionIter->second->GetFunction();
			m_functions.erase(m_functionIter);
		}
		index++;
	}
	return functions;
}


wxString CodeParser::GetRemainingFunctions() {
	wxString functions;
	m_functionIter = m_functions.begin();
	while (m_functionIter != m_functions.end()) {
		functions += m_functionIter->second->GetFunction();
		m_functionIter++;
	}
	return functions;
}
