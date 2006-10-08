#ifndef WXFBEXCEPTION
#define WXFBEXCEPTION

#include <wx/string.h>

/**
Exception class for wxFormBuilder
*/
class wxFBException
{
public:
	explicit wxFBException( const wxString& what )
	:
	m_what(what)
	{}

	virtual ~wxFBException() throw() {}

	virtual const wxChar* what() const throw()
	{
		return m_what.c_str();
	}

private:
   wxString m_what;
};

/**
This allows you to stream your exceptions in.
It will take care of the conversion	and throwing the exception.
*/
#define THROW_WXFBEX( message )																								\
	{																														\
	wxString hopefullyThisNameWontConflictWithOtherVariables;																\
	wxString file( __FILE__, wxConvUTF8 );																					\
	file = file.substr( file.find_last_of( wxT("\\/") ) + 1 );																\
	hopefullyThisNameWontConflictWithOtherVariables << message << wxT(" <") << file << wxT("@");							\
	hopefullyThisNameWontConflictWithOtherVariables << wxString::Format( wxT("%i"), __LINE__ ) << wxT(">");					\
	throw wxFBException( hopefullyThisNameWontConflictWithOtherVariables );													\
	}

#endif //WXFBEXCEPTION
