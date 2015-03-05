/*
 * draw.h
 *
 *  Created on: Mar 5, 2015
 *      Author: skim
 */

#ifndef DRAW_H_
#define DRAW_H_

#include "sdk/common.h"

namespace skim
{

// Cartesian point
struct point
{
	point(int _x, int _y) : x(_x), y(_y) {}
	point() {}
	int x, y;

    inline point operator +(const point& p) const {
        return point(x + p.x, y + p.y);
    }
    inline point operator -(const point& p) const {
        return point(x - p.x, y - p.y);
    }
    inline point operator *(int a) const {
        return point(x*a, y*a);
    }
    inline point& operator *=(int a) {
        x *= a;
        y *= a;
        return *this;
    }
    inline point& operator +=(const point& p) {
        x += p.x;
        y += p.y;
        return *this;
    }
    inline point& operator -=(const point& p) {
        x -= p.x;
        y -= p.y;
        return *this;
    }
};

class draw
{
public:
	static void init();

	static void color(const color& clr);
	static void line(const point& p1, const point& p2);
	static void line_box(const point& p1, const point& p2);
	static void fill_box(const point& p1, const point& p2);

	// Get a point from the world as it looks on screen
	static bool world_point(const Vector& worldpt, point& pt);
	static bool world_line(const Vector& pt1, const Vector& pt2);
};

} /* namespace skim */

#endif /* DRAW_H_ */
