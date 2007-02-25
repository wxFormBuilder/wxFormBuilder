#include <wx/log.h>

class wxLogString : public wxLog
{
private:
    wxString m_str;

public:
    wxLogString(){}

    void DoLogString( const wxChar* str, time_t t )
	{
		m_str += str;
		m_str += wxT("\n");
	}

    void FlushStr( wxString& str )
    {
    	str = m_str;
    	m_str.clear();
	}

    wxString GetStr() const
    {
    	return m_str;
	}
};
