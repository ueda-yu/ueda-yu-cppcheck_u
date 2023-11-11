/*
 * Copyright (C) 2013-2015 Paul Davis <paul@linuxaudiosystems.com>
 * Copyright (C) 2015-2017 Robin Gareus <robin@gareus.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <cmath>
#include <algorithm>

#include <cairomm/context.h>

#include "pbd/compose.h"
#include "canvas/circle.h"
#include "canvas/types.h"
#include "canvas/debug.h"
#include "canvas/canvas.h"

using namespace std;
using namespace ArdourCanvas;

Arc::Arc (Canvas* c)
	: Item (c)
	, _radius (0.0)
	, _arc_degrees (0.0)
	, _start_degrees (0.0)
{
}

Arc::Arc (Item* parent)
	: Item (parent)
	, _radius (0.0)
	, _arc_degrees (0.0)
	, _start_degrees (0.0)
{
}

void
Arc::compute_bounding_box () const
{
	Rect bbox;

	/* this could be smaller in the case of small _arc values
	   but I can't be bothered to optimize it.
	*/

	bbox.x0 = _center.x - _radius;
	bbox.y0 = _center.y - _radius;
	bbox.x1 = _center.x + _radius;
	bbox.y1 = _center.y + _radius;

	bbox = bbox.expand (0.5 + (_outline_width / 2));

	_bounding_box = bbox;
	set_bbox_clean ();
}

void
Arc::render (Rect const & area, Cairo::RefPtr<Cairo::Context> context) const
{
	if (_radius <= 0.0 || (!fill() && !outline())) {
		return;
	}

	Duple c = item_to_window (Duple (_center.x, _center.y));

	if (_arc_degrees != _start_degrees) {

		context->arc (c.x, c.y, _radius, _start_degrees * (M_PI/180.0), _arc_degrees * (M_PI/180.0));

		if (fill()) {
			setup_fill_context (context);
			if (outline()) {
				context->fill_preserve ();
			} else {
				context->fill ();
			}
		}

		if (outline()) {
			setup_outline_context (context);
			context->stroke ();
		}
	}

	render_children (area, context);
}

void
Arc::set_center (Duple const & c)
{
	begin_change ();

	_center = c;

	set_bbox_dirty ();
	end_change ();
}

void
Arc::set_radius (Coord r)
{
	begin_change ();

	_radius = r;

	set_bbox_dirty ();
	end_change ();
}

void
Arc::set_arc (double deg)
{
	begin_change ();

	_arc_degrees = deg;

	set_bbox_dirty ();
	end_change ();
}


void
Arc::set_start (double deg)
{
	begin_change ();

	_start_degrees = deg;

	set_bbox_dirty ();
	end_change ();
}

bool
Arc::covers (Duple const & point) const
{
	Duple p = window_to_item (point);

	double angle_degs = atan (p.y/p.x) * 2.0 * M_PI;
	double radius = sqrt (p.x * p.x + p.y * p.y);

	return (angle_degs >= _start_degrees) &&
		(angle_degs <= (_start_degrees + _arc_degrees)) &&
		(radius < _radius);
}

void
Arc::_size_allocate (Rect const & r)
{
	Item::_size_allocate (r);

	/* This is an arc - some section of a circle, so any difference between
	 * height and width cannot change what is drawn. Pick width arbitrarily
	 * as the "key" dimension
	 */

	_radius = min (r.width(),r.height()) / 2.0;
	_center = Duple ((r.width()/2.), (r.height() /2.));

	set_bbox_dirty ();
}
