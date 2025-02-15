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

static HFont font;

void draw::init()
{
	font = ifs::surface->CreateFont();
	ifs::surface->SetFontGlyphSet(font, "Default", 14, 450, 0, 0, ISurface::FONTFLAG_OUTLINE);
}

static void setc(const color& clr)
{
	ifs::surface->DrawSetColor(clr);
}
void draw::line(const point& p1, const point& p2, const color& c)
{
	setc(c);
	ifs::surface->DrawLine(p1.x, p1.y, p2.x, p2.y);
}
void draw::line_box(const point& p1, const point& p2, const color& c)
{
	setc(c);
	ifs::surface->DrawOutlinedRect(p1.x, p1.y, p2.x, p2.y);
}
void draw::fill_box(const point& p1, const point& p2, const color& c)
{
	setc(c);
	ifs::surface->DrawFilledRect(p1.x, p1.y, p2.x, p2.y);
}

void draw::string(const point& p, const color& c, const std::string& text)
{
	std::wstring tmp (text.begin(), text.end());
	draw::string(p, c, tmp);
}
void draw::string(const point& p, const color& c, const std::wstring& text)
{
	ifs::surface->DrawSetTextFont(font);
	ifs::surface->DrawSetTextColor(c);
	ifs::surface->DrawSetTextPos(p.x, p.y);
	ifs::surface->DrawPrintText(text.c_str(), text.length());
}

bool draw::world_point(const Vector& wpt, point& spt)
{
	float x, y, z;
	const VMatrix &screen_matrix = ifs::engine->WorldToScreenMatrix();
	x = screen_matrix[0][0] * wpt[0] + screen_matrix[0][1] * wpt[1] + screen_matrix[0][2] * wpt[2] + screen_matrix[0][3];
	y = screen_matrix[1][0] * wpt[0] + screen_matrix[1][1] * wpt[1] + screen_matrix[1][2] * wpt[2] + screen_matrix[1][3];
	z = screen_matrix[3][0] * wpt[0] + screen_matrix[3][1] * wpt[1] + screen_matrix[3][2] * wpt[2] + screen_matrix[3][3];

	if( z < 0.001f )
		return false;

	x /= z;
	y /= z;

	int scrw, scrh;
	ifs::engine->GetScreenSize( scrw, scrh );
	float cx = scrw / 2;
	float cy = scrh / 2;
	cx += x * 0.5f * scrw + 0.5f;
	cy -= y * 0.5f * scrh + 0.5f;
	spt.x = cx;
	spt.y = cy;
	return true;
}
bool draw::world_line(const Vector& pt1, const Vector& pt2, const color& c)
{
	point scr1, scr2;
	if (!world_point(pt1, scr1) || !world_point(pt2, scr2))
		return false;
	line(scr1, scr2, c);
	return true;
}

} /* namespace skim */
