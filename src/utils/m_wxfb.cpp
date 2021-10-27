#include <wx/html/forcelnk.h>
#include <wx/html/m_templ.h>

#include "../rad/revision.h"
#include "../rad/version.h"


FORCE_LINK_ME(m_wxfb)


TAG_HANDLER_BEGIN(wxFBVersion, "WXFB-VERSION")

TAG_HANDLER_PROC(WXUNUSED(tag))
{
	auto* cell = new wxHtmlWordCell(getVersion(), *m_WParser->GetDC());
	m_WParser->ApplyStateToCell(cell);
	m_WParser->GetContainer()->InsertCell(cell);

	return false;
}

TAG_HANDLER_END(wxFBVersion)


TAG_HANDLER_BEGIN(wxFBRevision, "WXFB-REVISION")

TAG_HANDLER_PROC(WXUNUSED(tag))
{
	auto* cell = new wxHtmlWordCell(getPostfixRevision(getVersion()), *m_WParser->GetDC());
	m_WParser->ApplyStateToCell(cell);
	m_WParser->GetContainer()->InsertCell(cell);

	return false;
}

TAG_HANDLER_END(wxFBRevision)


TAGS_MODULE_BEGIN(wxFB)

TAGS_MODULE_ADD(wxFBVersion)
TAGS_MODULE_ADD(wxFBRevision)

TAGS_MODULE_END(wxFB)
