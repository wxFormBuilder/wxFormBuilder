#pragma once

#include "../utils/debug.h"

#include <unordered_map>
#include <wx/dynarray.h>
#include <wx/textfile.h>
#include <wx/msgdlg.h>

///debugging 
#include <wx/log.h>

wxString RemoveWhiteSpace(wxString &str);

/** Stores all of the information for all of the parsed funtions */
class CppFunction {
public:
	CppFunction() {
	}
	~CppFunction() {
	}


	/** stores the whole first line of the function as a single string 
	*   ex: "void ClassName::FunctionName(int number)" */
	void SetHeading(wxString &heading);

	/** stores the code contained in the body of the function */
	void SetContents(wxString &contents);
	/** stores any code/documentation located between the previous function and the current function
	 */
	void SetDocumentation(wxString &documentation) {
		m_documentation = documentation;
		if (m_documentation.Left(1) == '\n') {
			m_documentation.Remove(0, 1);
		}
		if (m_documentation.Right(1) == '\n') {
			m_documentation.Remove(m_documentation.Len() - 1, 1);
		}
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


WX_DEFINE_ARRAY(CppFunction*, ArrayCppFunctionBase);

class ArrayCppFunction : public ArrayCppFunctionBase
{
public:
	ArrayCppFunction();

	int Find(wxString &functionHeading)
	{
		int index = 0;
		wxString heading = Item(index)->GetHeading();
		while(index < Count())
		{
			if( RemoveWhiteSpace(functionHeading).IsSameAs( RemoveWhiteSpace(heading) ) )
			{
				return index;
			}
		}
		return wxNOT_FOUND;
	}
};


/** parses the source and header files for all code added to the generated */
class CppCodeParser {
protected:
	wxString m_headerFileName;
	wxString m_sourceFileName;

	wxString m_userIncludeFilePreProc;
	wxString m_userSourceFilePreProc;
	wxString m_className;
	wxString m_userHeaderFileMembers;
	wxString m_userSourceFileTrailing;


	ArrayCppFunction m_functionList;
	int lastRetFunc = 0;

public:
	/** constructor */
	CppCodeParser() {
	}

	CppCodeParser(wxString &headerFileName, wxString &sourceFileName) {
		m_headerFileName = headerFileName;
		m_sourceFileName = sourceFileName;
	}

	~CppCodeParser() {
		m_functionList.Clear();
	}

	/** c++ Parser */

	/** opens the header and source files,  'className' is the Inherited class */
	void ParseFiles(wxString &className);

	/** extracts the contents of the files.  take the the entire contents of both files in string
	 * form */
	void ParseCode(wxString &header, wxString &source);

	/** extracts all user preprocessor definitions before the class declaration in include file*/
	void ParseHeaderFilePreProc(wxString &code);

	/** extracts the contents of the inherited class declaration */
	void ParseHeaderFileClass(wxString &code);

	/** extracts user created class members **/
	void ParseHeaderFileUserMembers(wxString &code);



	/** extracts all user preprocessor definitions before the class declaration in source file*/
	void ParseSourceFilePreProc(wxString &code);

	/** extracts each function in source File**/
	void ParseSourceFileFunctions(wxString &code);

	wxString ParseBrackets(wxString &code, int &functionStart);

	// Accessor Functions
	////////////////////////
	wxString GetUserHeaderFilePreProc() {return m_userIncludeFilePreProc;}
	wxString GetUserSourceFilePreProc() {return m_userSourceFilePreProc;}
	wxString GetUserHeaderFileMembers() {return m_userHeaderFileMembers;}

	void AddFunction(CppFunction* function)
	{
		m_functionList.Add(function);
	}

	CppFunction* GetFunction(wxString &functionHeading)
	{
		int index = m_functionList.Find(functionHeading);
		if(index != wxNOT_FOUND)
		{
			return m_functionList.Item(index);
		}
		return NULL;

	}


	/** returns the Documentation of a function by name */
	wxString GetFunctionDocumentation(wxString &functionHeading);

	/** returns the contents of a function by name and then removes it from the list of remaining
	 * functions */
	wxString GetFunctionContents(wxString &functionHeading);

	/** returns all functions including documentation as one string. but should keep them intact */
	wxString GetAllFunctions();

	//wxString GetRemainingFunctions();

	wxString GetTrailingCode() {
		return m_userSourceFileTrailing;
	}

	void DeleteFunction(CppFunction *function)
	{
		m_functionList.Remove(function);
	}

	/***************/
};

