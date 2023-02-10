#include "xmlutils.h"

#include <wx/ffile.h>
#include <wx/intl.h>

#include "utils/wxfbexception.h"


namespace XMLUtils
{

std::unique_ptr<tinyxml2::XMLDocument> LoadXMLFile(const wxString& path, bool collapseWhitespace)
{
    // Use wxFFile to get full unicode support for path on Windows
    wxFFile file;
    if (wxLogNull noLog; !file.Open(path, "rb")) {
        THROW_WXFBEX(_("Failed to open file for reading: ") << path)
    }
    auto doc = std::make_unique<tinyxml2::XMLDocument>(
      true, collapseWhitespace ? tinyxml2::COLLAPSE_WHITESPACE : tinyxml2::PRESERVE_WHITESPACE);
    if (doc->LoadFile(file.fp()) != tinyxml2::XML_SUCCESS) {
        THROW_WXFBEX(_("Failed to parse file: ") << doc->ErrorStr())
    }

    return doc;
}

}  // namespace XMLUtils
