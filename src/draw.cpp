/*
 * draw.cpp
 *
 *  Created on: Mar 5, 2015
 *      Author: skim
 */

#include "draw.h"

#include "ifs.h"
#include "sdk/surface.h"
#include "sdk/engine.h"

namespace skim
{

HFont font;

void draw::init()
{
	font = ifs::surface->CreateFont();
	ifs::surface->SetFontGlyphSet(font, "Default", 12, 450, 0, 0, ISurface::FONTFLAG_OUTLINE);
}

void draw::color(const ::color& clr)
{
	ifs::surface->DrawSetColor(clr);
}
void draw::line(const point& p1, const point& p2)
{
	ifs::surface->DrawLine(p1.x, p1.y, p2.x, p2.y);
}
void draw::line_box(const point& p1, const point& p2)
{
	ifs::surface->DrawOutlinedRect(p1.x, p1.y, p2.x, p2.y);
}
void draw::fill_box(const point& p1, const point& p2)
{
	ifs::surface->DrawFilledRect(p1.x, p1.y, p2.x, p2.y);
}

bool draw::world_point(const Vector& wpt, point& spt)
{
    float x, y, z;
    const VMatrix& wscr = ifs::engine->WorldToScreenMatrix();
    x = wscr[0][0] * wpt[0] + wscr[0][1] * wpt[1] + wscr[0][2] * wpt[2] + wscr[0][3];
    y = wscr[1][0] * wpt[0] + wscr[1][1] * wpt[1] + wscr[1][2] * wpt[2] + wscr[1][3];
    z = wscr[3][0] * wpt[0] + wscr[3][1] * wpt[1] + wscr[3][2] * wpt[2] + wscr[3][3];

    if( z < 0.001f )
        return false;

    int w, h;
    ifs::engine->GetScreenSize(w, h);

    x /= z;
    y /= z;

    spt.x = w / 2;
    spt.y = h / 2;
    spt.x += x / 2.f * w + .5f;
    spt.y += x / 2.f * w + .5f;
    return true;
}
bool draw::world_line(const Vector& pt1, const Vector& pt2)
{
	point scr1, scr2;
	if (!world_point(pt1, scr1) || !world_point(pt2, scr2))
		return false;
	line(scr1, scr2);
	return true;
}

} /* namespace skim */
