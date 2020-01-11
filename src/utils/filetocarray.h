#ifndef FILE_TO_C_ARRAY_H
#define FILE_TO_C_ARRAY_H

#include <wx/string.h>

class FileToCArray
{
public:
	static wxString Generate( const wxString& sourcepath );
};

#endif // FILE_TO_C_ARRAY_H
