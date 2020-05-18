#pragma once

#include "../utils/debug.h"

#include <unordered_map>
#include <wx/textfile.h>
#include <wx/msgdlg.h>

///debugging 
#include <wx/log.h>

wxString RemoveWhiteSpace(wxString str);

/** Stores all of the information for all of the parsed funtions */
class Function {
public:
	Function() {
	}
	~Function() {
	}

	/** stores the code contained in the body of the function */
	void SetContents(wxString contents);

	/** stores the whole first line of the function as a single string ex: "void
	 * fubar::DoSomething(int number)" */
	void SetHeading(wxString heading);

	/** stores any code/documentation located between the previous function and the current function
	 */
	void SetDocumentation(wxString documentation) {
		if (documentation.Left(1) == '\n') {
			documentation.Remove(0, 1);
		}
		if (documentation.Right(1) == '\n') {
			documentation.Remove(documentation.Len() - 1, 1);
		}
		m_documentation = documentation;
	}

	/** retrieves the body code */
	wxString GetHeading() {
		return m_functionHeading;
	}

	/** retrieves the body code */
	wxString GetContents() {
		return m_functionContents;
	}

	/** retrieves the documentation */
	wxString GetDocumentation() {
		return m_documentation;
	}
	/** retrieves everything including documentation */
	wxString GetFunction();

protected:
	wxString m_functionContents;
	wxString m_functionHeading;
	wxString m_documentation;
};

/** map class mapping Function* to function name */
using FunctionMap = std::unordered_map<std::string, Function*>;
#define funcIterator FunctionMap::iterator

/** parses the source and header files for all code added to the generated */
class CodeParser {
public:
	/** constructor */
	CodeParser() {
	}

	~CodeParser() {
	}

	///////////////////////////////////////////////////////////////////

	/** returns all user header include code before the class declaration */
	wxString GetUserHIncludes() {
		return m_userInclude_H;
	}

	/** returns all user header include code before the class declaration */
	wxString GetUserCIncludes() {
		return m_userInclude_CPP;
	}

	/** returns user class members */
	wxString GetUserMembers() {
		return m_userMembers_CPP;
	}

	void AddFunction(wxString function, Function* func)
	{
		m_functions[std::string(RemoveWhiteSpace(function).ToUTF8())] = func;
		m_functionList.Add(function);
	}

	Function* GetNextFunction(wxString function)
	{
		m_functionIter = m_functions.find(std::string(RemoveWhiteSpace(function).ToUTF8()));
		if (m_functionIter != m_functions.end())
		{
			lastRetFunc = m_functionList.Index(function);
			return NULL;
		}
		else
		{
			if(lastRetFunc +1 > m_functionList.Count())
			{
				lastRetFunc = m_functionList.Count()-1;
			}
			m_functionList.Insert(function, lastRetFunc+1);
			lastRetFunc++;
			Function* func = new Function();
			m_functions[std::string(RemoveWhiteSpace(function).ToUTF8())] = func;
			return func;
		}
	}

	/** returns the Documentation of a function by name */
	wxString GetFunctionDocumentation(wxString function);

	/** returns the contents of a function by name and then removes it from the list of remaining
	 * functions */
	wxString GetFunctionContents(wxString function);

	/** returns all ramaining functions including documentation as one string.
	 this may rearange functions, but should keep them intact */
	wxString GetAllFunctions();
	wxString GetRemainingFunctions();

	wxString GetTrailingCode() {
		return m_trailingCode_CPP;
	}

protected:
	wxString m_userInclude_H;
	wxString m_userInclude_CPP;
	wxString m_className;
	wxString m_userMembers_CPP;
	wxString m_trailingCode_CPP;


	FunctionMap m_functions;
	funcIterator m_functionIter;

	wxArrayString m_functionList;
	int lastRetFunc = 0;
};

/** parses the source and header files for all code added to the generated */
class CCodeParser : public CodeParser {
private:
	wxString m_hFile;
	wxString m_cFile;

public:
	/** constructor */
	CCodeParser() {
	}

	CCodeParser(wxString headerFileName, wxString sourceFileName) {
		m_hFile = headerFileName;
		m_cFile = sourceFileName;
	}

	~CCodeParser() {
	}

	/** c++ Parser */

	/** opens the header and source,  'className' is the Inherited class */
	void ParseFiles(wxString className);

	/** extracts the contents of the files.  take the the entire contents of both files in string
	 * form */
	void ParseCode(wxString header, wxString source);

	/** extracts all user header include code before the class declaration */
	void ParseHInclude(wxString code);

	/** extracts the contents of the generated class declaration */
	void ParseHClass(wxString code);

	void ParseCFunctions(wxString code);
	void ParseCInclude(wxString code);

	wxString ParseBrackets(wxString code, int& functionStart);

	void ParseHUserMembers(wxString code);

	/***************/
};

/**class PCodeParser : public CodeParser */
