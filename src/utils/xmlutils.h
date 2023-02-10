#ifndef UTILS_XMLUTILS_H
#define UTILS_XMLUTILS_H

#include <memory>

#include <tinyxml2.h>
#include <wx/string.h>


namespace XMLUtils
{

/**
 * @brief Load the specified XML file
 *
 * The file must use UTF-8 encoding, this is not verified and simply assumed.
 *
 * @param path Path to XML file
 * @param collapseWhitespace If true, collapse whitespace, otherwise preserve it
 * @return The loaded XML file
 *
 * @throw wxFBException Failed to open or parse the file
 */
std::unique_ptr<tinyxml2::XMLDocument> LoadXMLFile(const wxString& path, bool collapseWhitespace = false);

}  // namespace XMLUtils

#endif  // UTILS_XMLUTILS_H
