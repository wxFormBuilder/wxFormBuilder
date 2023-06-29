#ifndef SDK_COMMON_XMLUTILS_H
#define SDK_COMMON_XMLUTILS_H

#include <memory>

#include <tinyxml2.h>
#include <wx/string.h>


/**
 * @brief Helper functions to process XML documents with TinyXML 2
 *
 * Because of the insufficient XML entity processing of TinyXML 2 this feature is not used
 * but the entities are processed by these functions themself. Therefor it is important if
 * tinyxml2::XMLDocument objects are created externally to create them with entity processing disabled.
 */
namespace XMLUtils
{

/**
 * @brief Load the specified XML file
 *
 * The file must use UTF-8 encoding, this is not verified and simply assumed.
 *
 * @param path Path to XML file
 * @param collapseWhitespace If true, collapse whitespace, otherwise preserve it
 * @return The loaded XML file, nullptr if the file could not be opened.
 *         NOTE: The error code of the returned XMLDocument must be checked as well.
 */
std::unique_ptr<tinyxml2::XMLDocument> LoadXMLFile(const wxString& path, bool collapseWhitespace = false);

/**
 * @brief Save the given XML document to file
 *
 * The document must contain a valid XML structure, this is not verified and simply assumed.
 *
 * @param path Destination Path
 * @param document XML document
 * @param compact If true, a compact representation is written, omitting all optional whitespace
 * @return True if successful, false if writing to file failed
 */
bool SaveXMLFile(const wxString& path, const tinyxml2::XMLDocument& document, bool compact = false);

/**
 * @brief Save the given XML document to a wxString
 *
 * The document must contain a valid XML structure, this is not verified and simply assumed.
 *
 * @param document XML document
 * @param compact If true, a compact representation is written, omitting all optional whitespace
 * @return XML
 */
wxString SaveXMLString(const tinyxml2::XMLDocument& document, bool compact = false);

/**
 * @brief Return string typed attribute
 *
 * @param element XML element node
 * @param name Name of the attribute
 * @param defaultValue Default value returned if attribute is missing
 * @return Attribute value
 */
wxString StringAttribute(const tinyxml2::XMLElement* element, const wxString& name, const wxString& defaultValue = wxEmptyString);

/**
 * @brief Set string type attribute
 *
 * @param element XML element node
 * @param name Name of the attribute
 * @param value Value of the attribute
 */
void SetAttribute(tinyxml2::XMLElement* element, const wxString& name, const wxString& value);

/**
 * @brief Return text of the element
 *
 * @param element XML element node
 * @param defaultValue Default value returned if element contains no text
 * @param deepSearch If true, searches for the first text node, otherwise returns the text of the first child node.
 *                   If the first child node is not a text node the default value will be returned.
 * @return Text of the element
 */
wxString GetText(const tinyxml2::XMLElement* element, const wxString& defaultValue = wxEmptyString, bool deepSearch = false);

/**
 * @brief Set text of the element
 *
 * @param element XML element node
 * @param value Text value of the node
 * @param insertElement If true, the text is inserted as text node as last child element, otherwise a simple SetText() is used.
 */
void SetText(tinyxml2::XMLElement* element, const wxString& value, bool insertElement = false);

}  // namespace XMLUtils

#endif  // SDK_COMMON_XMLUTILS_H
