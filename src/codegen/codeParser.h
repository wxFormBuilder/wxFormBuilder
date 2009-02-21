#ifndef CODEPARSER_H_INCLUDED
#define CODEPARSER_H_INCLUDED

#include <wx/textfile.h>
#include <wx/string.h>

// Stores all of the information for all of the parsed funtions
class Function
{
	public:
		Function()
		{}
		~Function()
		{}

		//stores the code contained in the body of the function
		void SetContents(wxString contents);

		//stores the whole first line of the function as a single string ex: "void fubar::DoSomething(int number)"
		void SetHeading(wxString heading);

		//stores any code/documentation located between the previous function and the current function
		void SetDocumentation(wxString documentation)
		{
			m_documentation = documentation;
		}

		//retrieves the body code
		wxString GetContents()
		{
			return m_functionContents;
		}

		//retrieves the documentation
		wxString GetDocumentation()
		{
			return m_documentation;
		}
		//retrieves everything including documentation
		wxString GetFunction();

	protected:
		wxString m_functionContents;
		wxString m_functionHeading;
		wxString m_documentation;
};

//map class mapping Function* to function name
WX_DECLARE_STRING_HASH_MAP(Function*, FunctionMap);
#define funcIterator FunctionMap::iterator


//parses the source and header files for all code added to the generated
class CodeParser
{
	public:
		//constructor
		CodeParser()
		{}

		//constructor that calls ParseFiles()
		CodeParser(wxString headerFileName, wxString sourceFileName, wxString className)
{
	ParseFiles(headerFileName, sourceFileName, className);
}

		~CodeParser()
		{}

		//opens the header and source,  'className' is the Inherited class
		void ParseFiles(wxString headerFileName, wxString sourceFileName, wxString className);

		//extracts the contents of the files.  take the the entire contents of both files in string form
		void ParseCode(wxString header, wxString source);

		//extracts all user header include code before the class declaration
		wxString ParseUserHInclude(wxString code);

		//extracts the contents of the generated class declaration
		wxString ParseClass(wxString code);

		//extracts user class members
		void ParseUserMembers(wxString code);

		//parses all functions belonging to the generated class, including user functions
		wxString ParseSourceFunctions(wxString code);

		//extracts the contents of a codes block
		int ParseBrackets(wxString code);

		//returns all user header include code before the class declaration
		wxString GetUserHeaderIncludes()
		{
			return m_userHInclude;
		}

		//returns user class members
		wxString GetUserMembers()
		{
			return m_userMemebers;
		}

		//returns the contents of a function by name and then removes it from the list of remaining functions
		wxString GetFunctionContents(wxString function);

		//returns all ramaining functions including documentation as one string.
		// this may rearange functions, but should keep them intact
		wxString GetRemainingFunctions();

	private:
		wxString m_userHInclude;
		wxString m_userSInclude;
		wxString m_className;
		wxString m_userMemebers;

		FunctionMap m_functions;
		funcIterator m_functionIter;


};


#endif // CODEPARSER_H_INCLUDED

