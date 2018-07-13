#ifndef CODEPARSER_H_INCLUDED
#define CODEPARSER_H_INCLUDED

#include "../utils/debug.h"

#include <wx/textfile.h>
#include <wx/msgdlg.h>

wxString RemoveWhiteSpace(wxString str);

/** Stores all of the information for all of the parsed funtions */
class Function
{
	public:
		Function()
		{}
		~Function()
		{}

		/** stores the code contained in the body of the function */
		void SetContents(wxString contents);

		/** stores the whole first line of the function as a single string ex: "void fubar::DoSomething(int number)" */
		void SetHeading(wxString heading);

		/** stores any code/documentation located between the previous function and the current function */
		void SetDocumentation(wxString documentation)
		{
			if(documentation.Left(1) == '\n')
			{
				documentation.Remove(0, 1);
			}
			if(documentation.Right(1) == '\n')
			{
				documentation.Remove(documentation.Len() -1, 1);
			}
			m_documentation = documentation;
		}

		/** retrieves the body code */
		wxString GetHeading()
		{
			return m_functionHeading;
		}

		/** retrieves the body code */
		wxString GetContents()
		{
			return m_functionContents;
		}

		/** retrieves the documentation */
		wxString GetDocumentation()
		{
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
WX_DECLARE_STRING_HASH_MAP(Function*, FunctionMap);
#define funcIterator FunctionMap::iterator


/** parses the source and header files for all code added to the generated */
class CodeParser
{
	public:
		/** constructor */
		CodeParser()
		{
		}

		~CodeParser()
		{}



		///////////////////////////////////////////////////////////////////

		/** returns all user header include code before the class declaration */
		wxString GetUserIncludes()
		{
			return m_userInclude;
		}

		/** returns user class members */
		wxString GetUserMembers()
		{
			return m_userMemebers;
		}

		/** returns the Documentation of a function by name */
		wxString GetFunctionDocumentation( wxString function );

		/** returns the contents of a function by name and then removes it from the list of remaining functions */
		wxString GetFunctionContents(wxString function);

		/** returns all ramaining functions including documentation as one string.
		 this may rearange functions, but should keep them intact */
		wxString GetRemainingFunctions();

		wxString GetTrailingCode()
		{
			return m_trailingCode;
		}

	protected:
		wxString m_userInclude;
		wxString m_className;
		wxString m_userMemebers;
		wxString m_trailingCode;

		FunctionMap m_functions;
		funcIterator m_functionIter;

};



/** parses the source and header files for all code added to the generated */
class CCodeParser : public CodeParser
{
	private:
		wxString m_hFile;
		wxString m_cFile;

	public:
		/** constructor */
		CCodeParser()
		{
		}

		CCodeParser(wxString headerFileName, wxString sourceFileName)
		{
			m_hFile = headerFileName;
			m_cFile = sourceFileName;
		}

		~CCodeParser()
		{}

		/** c++ Parser */

			/** opens the header and source,  'className' is the Inherited class */
			void ParseCFiles(wxString className);

			/** extracts the contents of the files.  take the the entire contents of both files in string form */
			void ParseCCode(wxString header, wxString source);

			/** extracts all user header include code before the class declaration */
			void ParseCInclude(wxString code);

			/** extracts the contents of the generated class declaration */
			void ParseCClass(wxString code);

			void ParseSourceFunctions(wxString code);

			wxString ParseBrackets(wxString code, int& functionStart);

			void ParseCUserMembers(wxString code);

		/***************/
};

/**class PCodeParser : public CodeParser */

#endif // CODEPARSER_H_INCLUDED

