#include "auitabart.h"

#ifndef WX_PRECOMP
    #include <wx/dc.h>
    #include <wx/dcclient.h>
    #include <wx/settings.h>
    #include <wx/bitmap.h>
#endif

#include <wx/aui/auibook.h>
#include <wx/renderer.h>

#if defined( __WXMAC__ )
 static const unsigned char close_bits[]={
     0xFF, 0xFF, 0xFF, 0xFF, 0x0F, 0xFE, 0x03, 0xF8, 0x01, 0xF0, 0x19, 0xF3,
     0xB8, 0xE3, 0xF0, 0xE1, 0xE0, 0xE0, 0xF0, 0xE1, 0xB8, 0xE3, 0x19, 0xF3,
     0x01, 0xF0, 0x03, 0xF8, 0x0F, 0xFE, 0xFF, 0xFF };
#elif defined( __WXGTK__)
 static const unsigned char close_bits[]={
     0xff, 0xff, 0xff, 0xff, 0x07, 0xf0, 0xfb, 0xef, 0xdb, 0xed, 0x8b, 0xe8,
     0x1b, 0xec, 0x3b, 0xee, 0x1b, 0xec, 0x8b, 0xe8, 0xdb, 0xed, 0xfb, 0xef,
     0x07, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
#else
 static const unsigned char close_bits[]={
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe7, 0xf3, 0xcf, 0xf9,
     0x9f, 0xfc, 0x3f, 0xfe, 0x3f, 0xfe, 0x9f, 0xfc, 0xcf, 0xf9, 0xe7, 0xf3,
     0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
#endif

static const unsigned char left_bits[] = {
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x7f, 0xfe, 0x3f, 0xfe,
   0x1f, 0xfe, 0x0f, 0xfe, 0x1f, 0xfe, 0x3f, 0xfe, 0x7f, 0xfe, 0xff, 0xfe,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

static const unsigned char right_bits[] = {
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xdf, 0xff, 0x9f, 0xff, 0x1f, 0xff,
   0x1f, 0xfe, 0x1f, 0xfc, 0x1f, 0xfe, 0x1f, 0xff, 0x9f, 0xff, 0xdf, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

static const unsigned char list_bits[] = {
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
   0x0f, 0xf8, 0xff, 0xff, 0x0f, 0xf8, 0x1f, 0xfc, 0x3f, 0xfe, 0x7f, 0xff,
   0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

static bool IsDark()
{
    const wxColour colour = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW);
    int average = ( colour.Red() + colour.Green() + colour.Blue() ) / 3;
    if ( average < 128 )
        return true;

    return false;
}

static wxBitmap wxAuiBitmapFromBits(const unsigned char bits[], int w, int h,
                             const wxColour& color)
{
    wxImage img = wxBitmap((const char*)bits, w, h).ConvertToImage();
    if (color.Alpha() == wxALPHA_OPAQUE)
    {
        img.Replace(0,0,0,123,123,123);
        img.Replace(255,255,255,color.Red(),color.Green(),color.Blue());
        img.SetMaskColour(123,123,123);
    }
    else
    {
        img.InitAlpha();
        const int newr = color.Red();
        const int newg = color.Green();
        const int newb = color.Blue();
        const int newa = color.Alpha();
        for (int x = 0; x < w; x++)
        {
            for (int y = 0; y < h; y++)
            {
                int r = img.GetRed(x, y);
                int g = img.GetGreen(x, y);
                int b = img.GetBlue(x, y);
                if (r == 0 && g == 0 && b == 0)
                {
                    img.SetAlpha(x, y, wxALPHA_TRANSPARENT);
                }
                else
                {
                    img.SetRGB(x, y, newr, newg, newb);
                    img.SetAlpha(x, y, newa);
                }
            }
        }
    }
    return wxBitmap(img);
}

static void IndentPressedBitmap(wxRect* rect, int button_state)
{
    if (button_state == wxAUI_BUTTON_STATE_PRESSED)
    {
        rect->x++;
        rect->y++;
    }
}

static wxString wxAuiChopText(wxDC& dc, const wxString& text, int max_size)
{
    wxCoord x,y;

    // first check if the text fits with no problems
    dc.GetTextExtent(text, &x, &y);
    if (x <= max_size)
        return text;

    size_t i, len = text.Length();
    size_t last_good_length = 0;
    for (i = 0; i < len; ++i)
    {
        wxString s = text.Left(i);
        s += wxT("...");

        dc.GetTextExtent(s, &x, &y);
        if (x > max_size)
            break;

        last_good_length = i;
    }

    wxString ret = text.Left(last_good_length);
    ret += wxT("...");
    return ret;
}

inline float wxAuiGetSRGB(float r)
{
    return r <= 0.03928f ? r / 12.92f : std::pow((r + 0.055f) / 1.055f, 2.4f);
}

static float wxAuiGetRelativeLuminance(const wxColour& c)
{
    // based on https://www.w3.org/TR/2008/REC-WCAG20-20081211/#relativeluminancedef
    return
        0.2126f * wxAuiGetSRGB(c.Red()   / 255.0f) +
        0.7152f * wxAuiGetSRGB(c.Green() / 255.0f) +
        0.0722f * wxAuiGetSRGB(c.Blue()  / 255.0f);
}

static float wxAuiGetColourContrast(const wxColour& c1, const wxColour& c2)
{
    // based on https://www.w3.org/TR/UNDERSTANDING-WCAG20/visual-audio-contrast7.html
    float L1 = wxAuiGetRelativeLuminance(c1);
    float L2 = wxAuiGetRelativeLuminance(c2);
    return L1 > L2 ? (L1 + 0.05f) / (L2 + 0.05f) : (L2 + 0.05f) / (L1 + 0.05f);
}

// Check if the color has sufficient contrast ratio (4.5 recommended)
// (based on https://www.w3.org/TR/UNDERSTANDING-WCAG20/visual-audio-contrast7.html)
static bool wxAuiHasSufficientContrast(const wxColour& c1, const wxColour& c2)
{
    return wxAuiGetColourContrast(c1, c2) >= 4.5f;
}

// Pick a color that provides better contrast against the background
static wxColour wxAuiGetBetterContrastColour(const wxColour& back_color,
    const wxColour& c1, const wxColour& c2)
{
    return wxAuiGetColourContrast(back_color, c1)
        > wxAuiGetColourContrast(back_color, c2) ? c1 : c2;
}

void AuiTabArt::DrawTab(wxDC& dc,
                        wxWindow* wnd,
                        const wxAuiNotebookPage& page,
                        const wxRect& in_rect,
                        int close_button_state,
                        wxRect* out_tab_rect,
                        wxRect* out_button_rect,
                        int* x_extent)
{
    wxCoord normal_textx, normal_texty;
    wxCoord selected_textx, selected_texty;
    wxCoord texty;

    // if the caption is empty, measure some temporary text
    wxString caption = page.caption;
    if (caption.empty())
        caption = wxT("Xj");

    dc.SetFont(m_selectedFont);
    dc.GetTextExtent(caption, &selected_textx, &selected_texty);

    dc.SetFont(m_normalFont);
    dc.GetTextExtent(caption, &normal_textx, &normal_texty);

    // figure out the size of the tab
    wxSize tab_size = GetTabSize(dc,
                                 wnd,
                                 page.caption,
                                 page.bitmap,
                                 page.active,
                                 close_button_state,
                                 x_extent);

    wxCoord tab_height = m_tabCtrlHeight - 3;
    wxCoord tab_width = tab_size.x;
    wxCoord tab_x = in_rect.x;
    wxCoord tab_y = in_rect.y + in_rect.height - tab_height;


    caption = page.caption;


    // select pen, brush and font for the tab to be drawn

    if (page.active)
    {
        dc.SetFont(m_selectedFont);
        texty = selected_texty;
    }
    else
    {
        dc.SetFont(m_normalFont);
        texty = normal_texty;
    }
    // create points that will make the tab outline
    int clip_width = tab_width;
    if (tab_x + clip_width > in_rect.x + in_rect.width)
        clip_width = (in_rect.x + in_rect.width) - tab_x;
/*
    wxPoint clip_points[6];
    clip_points[0] = wxPoint(tab_x,              tab_y+tab_height-3);
    clip_points[1] = wxPoint(tab_x,              tab_y+2);
    clip_points[2] = wxPoint(tab_x+2,            tab_y);
    clip_points[3] = wxPoint(tab_x+clip_width-1, tab_y);
    clip_points[4] = wxPoint(tab_x+clip_width+1, tab_y+2);
    clip_points[5] = wxPoint(tab_x+clip_width+1, tab_y+tab_height-3);

    // FIXME: these ports don't provide wxRegion ctor from array of points
#if !defined(__WXDFB__)
    // set the clipping region for the tab --
    wxRegion clipping_region(WXSIZEOF(clip_points), clip_points);
    dc.SetClippingRegion(clipping_region);
#endif // !wxDFB && !wxCocoa
*/
    // since the above code above doesn't play well with WXDFB or WXCOCOA,
    // we'll just use a rectangle for the clipping region for now --
    dc.SetClippingRegion(tab_x, tab_y, clip_width+1, tab_height-3);


    wxPoint border_points[6];
    if (m_flags &wxAUI_NB_BOTTOM)
    {
        border_points[0] = wxPoint(tab_x,             tab_y);
        border_points[1] = wxPoint(tab_x,             tab_y+tab_height-6);
        border_points[2] = wxPoint(tab_x+2,           tab_y+tab_height-4);
        border_points[3] = wxPoint(tab_x+tab_width-2, tab_y+tab_height-4);
        border_points[4] = wxPoint(tab_x+tab_width,   tab_y+tab_height-6);
        border_points[5] = wxPoint(tab_x+tab_width,   tab_y);
    }
    else //if (m_flags & wxAUI_NB_TOP) {}
    {
        border_points[0] = wxPoint(tab_x,             tab_y+tab_height-4);
        border_points[1] = wxPoint(tab_x,             tab_y+2);
        border_points[2] = wxPoint(tab_x+2,           tab_y);
        border_points[3] = wxPoint(tab_x+tab_width-2, tab_y);
        border_points[4] = wxPoint(tab_x+tab_width,   tab_y+2);
        border_points[5] = wxPoint(tab_x+tab_width,   tab_y+tab_height-4);
    }
    // TODO: else if (m_flags &wxAUI_NB_LEFT) {}
    // TODO: else if (m_flags &wxAUI_NB_RIGHT) {}

    int drawn_tab_yoff = border_points[1].y;
    int drawn_tab_height = border_points[0].y - border_points[1].y;

    bool isdark = IsDark();

    // draw active tab
    wxColor back_color = m_baseColour;
    if (page.active)
    {
        // draw base background color
        wxRect r(tab_x, tab_y, tab_width, tab_height);
        dc.SetPen(wxPen(m_activeColour));
        dc.SetBrush(wxBrush(m_activeColour));
        dc.DrawRectangle(r.x+1, r.y+1, r.width-1, r.height-4);

        // this white helps fill out the gradient at the top of the tab
        wxColor gradient = *wxWHITE;
        if (isdark)
        {
            //dark mode, we go darker
            gradient = m_activeColour.ChangeLightness(70);
        }
        back_color = gradient;

        dc.SetPen(wxPen(gradient));
        dc.SetBrush(wxBrush(gradient));
        dc.DrawRectangle(r.x+2, r.y+1, r.width-3, r.height-4);

        // these two points help the rounded corners appear more antialiased
        dc.SetPen(wxPen(m_activeColour));
        dc.DrawPoint(r.x+2, r.y+1);
        dc.DrawPoint(r.x+r.width-2, r.y+1);

        // set rectangle down a bit for gradient drawing
        r.SetHeight(r.GetHeight()/2);
        r.x += 2;
        r.width -= 3;
        r.y += r.height;
        r.y -= 2;

        // draw gradient background
        wxColor top_color = gradient;
        wxColor bottom_color = m_activeColour;
        dc.GradientFillLinear(r, bottom_color, top_color, wxNORTH);
    }
    else
    {
        // draw inactive tab

        wxRect r(tab_x, tab_y+1, tab_width, tab_height-3);

        // start the gradient up a bit and leave the inside border inset
        // by a pixel for a 3D look.  Only the top half of the inactive
        // tab will have a slight gradient
        r.x += 3;
        r.y++;
        r.width -= 4;
        r.height /= 2;
        r.height--;

        // -- draw top gradient fill for glossy look
        wxColor top_color = m_baseColour;
        wxColor bottom_color = top_color.ChangeLightness(160);
        if (isdark)
        {
            //dark mode, we go darker
            top_color = m_activeColour.ChangeLightness(70);
            bottom_color = m_baseColour;
        }

        dc.GradientFillLinear(r, bottom_color, top_color, wxNORTH);

        r.y += r.height;
        r.y--;

        // -- draw bottom fill for glossy look
        top_color = m_baseColour;
        bottom_color = m_baseColour;
        dc.GradientFillLinear(r, top_color, bottom_color, wxSOUTH);
    }

    // draw tab outline
    dc.SetPen(m_borderPen);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawPolygon(WXSIZEOF(border_points), border_points);

    // there are two horizontal grey lines at the bottom of the tab control,
    // this gets rid of the top one of those lines in the tab control
    if (page.active)
    {
        if (m_flags &wxAUI_NB_BOTTOM)
            dc.SetPen(wxPen(m_baseColour.ChangeLightness(170)));
        // TODO: else if (m_flags &wxAUI_NB_LEFT) {}
        // TODO: else if (m_flags &wxAUI_NB_RIGHT) {}
        else //for wxAUI_NB_TOP
            dc.SetPen(m_baseColourPen);

        dc.DrawLine(border_points[0].x+1,
                    border_points[0].y,
                    border_points[5].x,
                    border_points[5].y);
    }

    int text_offset;
    int bitmap_offset = 0;
    if (page.bitmap.IsOk())
    {
        bitmap_offset = tab_x + 8;

        // draw bitmap
        dc.DrawBitmap(page.bitmap,
                      bitmap_offset,
                      drawn_tab_yoff + (drawn_tab_height / 2)
                      - (page.bitmap.GetScaledHeight() / 2), true);

        text_offset = bitmap_offset + page.bitmap.GetScaledWidth();
        text_offset += 3; // bitmap padding
    }
    else
    {
        text_offset = tab_x + 8;
    }

    // draw close button if necessary
    int close_button_width = 0;
    if (close_button_state != wxAUI_BUTTON_STATE_HIDDEN)
    {
        wxBitmap bmp = m_disabledCloseBmp;

        if (close_button_state == wxAUI_BUTTON_STATE_HOVER ||
            close_button_state == wxAUI_BUTTON_STATE_PRESSED)
        {
            bmp = m_activeCloseBmp;
        }

        int offsetY = tab_y-1;
        if (m_flags & wxAUI_NB_BOTTOM)
            offsetY = 1;

        wxRect rect(tab_x + tab_width - bmp.GetScaledWidth() - 1,
                    offsetY + (tab_height/2) - (bmp.GetScaledHeight()/2),
                    bmp.GetScaledWidth(),
                    tab_height);

        IndentPressedBitmap(&rect, close_button_state);
        dc.DrawBitmap(bmp, rect.x, rect.y, true);

        *out_button_rect = rect;
        close_button_width = bmp.GetScaledWidth();
    }

    wxString draw_text = wxAuiChopText(dc,
                          caption,
                          tab_width - (text_offset-tab_x) - close_button_width);

    // draw tab text
    wxColor sys_color = wxSystemSettings::GetColour(
        page.active ? wxSYS_COLOUR_CAPTIONTEXT : wxSYS_COLOUR_INACTIVECAPTIONTEXT);
    wxColor font_color = wxAuiHasSufficientContrast(back_color, sys_color) ? sys_color
        : wxAuiGetBetterContrastColour(back_color, *wxWHITE, *wxBLACK);

    dc.SetTextForeground(font_color);
    dc.DrawText(draw_text,
                text_offset,
                drawn_tab_yoff + (drawn_tab_height)/2 - (texty/2) - 1);

    // draw focus rectangle
    if (page.active && (wnd->FindFocus() == wnd))
    {
        wxRect focusRectText(text_offset,
            (drawn_tab_yoff + drawn_tab_height / 2 - (texty / 2) - 1),
            selected_textx, selected_texty);

        wxRect focusRect;
        wxRect focusRectBitmap;

        if (page.bitmap.IsOk())
            focusRectBitmap =
                wxRect(bitmap_offset, drawn_tab_yoff + (drawn_tab_height/2) -
                    (page.bitmap.GetScaledHeight()/2),
                    page.bitmap.GetScaledWidth(), page.bitmap.GetScaledHeight());

        if (page.bitmap.IsOk() && draw_text.IsEmpty())
            focusRect = focusRectBitmap;
        else if (!page.bitmap.IsOk() && !draw_text.IsEmpty())
            focusRect = focusRectText;
        else if (page.bitmap.IsOk() && !draw_text.IsEmpty())
            focusRect = focusRectText.Union(focusRectBitmap);

        focusRect.Inflate(2, 2);

        wxRendererNative::Get().DrawFocusRect(wnd, dc, focusRect, 0);
    }

    *out_tab_rect = wxRect(tab_x, tab_y, tab_width, tab_height);

    dc.DestroyClippingRegion();
}

wxSize AuiTabArt::GetTabSize(wxDC& dc,
                             wxWindow* wnd,
                             const wxString& caption,
                             const wxBitmap& bitmap,
                             bool active,
                             int close_button_state,
                             int* x_extent)
{
    return wxAuiGenericTabArt::GetTabSize(
        dc, wnd, caption, bitmap, active, close_button_state, x_extent) + wxSize(0, 2);
}

void AuiTabArt::UpdateColoursFromSystem()
{
    wxColor baseColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);

    // the baseColour is too pale to use as our base colour,
    // so darken it a bit --
    if ((255-baseColour.Red()) +
    (255-baseColour.Green()) +
    (255-baseColour.Blue()) < 60)
    {
        baseColour = baseColour.ChangeLightness(92);
    }

    m_activeColour = baseColour;
    m_baseColour = baseColour;
    wxColor borderColour = baseColour.ChangeLightness(75);

    m_borderPen = wxPen(borderColour);
    m_baseColourPen = wxPen(m_baseColour);
    m_baseColourBrush = wxBrush(m_baseColour);

    const int disabledLightness = IsDark() ? 130 : 70;

    m_activeCloseBmp = wxAuiBitmapFromBits(close_bits, 16, 16, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
    m_disabledCloseBmp = wxAuiBitmapFromBits(close_bits, 16, 16, wxSystemSettings::GetColour(wxSYS_COLOUR_INACTIVECAPTIONTEXT).ChangeLightness(disabledLightness));
    m_activeLeftBmp = wxAuiBitmapFromBits(left_bits, 16, 16, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
    m_disabledLeftBmp = wxAuiBitmapFromBits(left_bits, 16, 16, wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
    m_activeRightBmp = wxAuiBitmapFromBits(right_bits, 16, 16, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
    m_disabledRightBmp = wxAuiBitmapFromBits(right_bits, 16, 16, wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
    m_activeWindowListBmp = wxAuiBitmapFromBits(list_bits, 16, 16, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
    m_disabledWindowListBmp = wxAuiBitmapFromBits(list_bits, 16, 16, wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
}
