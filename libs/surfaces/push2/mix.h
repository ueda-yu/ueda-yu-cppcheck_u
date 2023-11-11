/*
 * Copyright (C) 2016-2018 Paul Davis <paul@linuxaudiosystems.com>
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

#ifndef __ardour_push2_mix_layout_h__
#define __ardour_push2_mix_layout_h__

#include "layout.h"
#include "push2.h"

namespace ARDOUR {
	class Stripable;
}

namespace ArdourCanvas {
	class Rectangle;
	class Text;
	class Line;
}

namespace ArdourSurface {

class Push2Knob;
class LevelMeter;

class MixLayout : public Push2Layout
{
   public:
	MixLayout (Push2& p, ARDOUR::Session&, std::string const &);
	~MixLayout ();

	void render (ArdourCanvas::Rect const &, Cairo::RefPtr<Cairo::Context>) const;
	void show ();

	void button_upper (uint32_t n);
	void button_lower (uint32_t n);
	void button_down ();
	void button_up ();
	void button_left ();
	void button_right ();
	void button_select_press ();
	void button_select_release ();
	void button_solo ();
	void button_mute ();

	void strip_vpot (int, int);
	void strip_vpot_touch (int, bool);

	void update_meters ();

  private:
	ArdourCanvas::Rectangle*              _bg;
	ArdourCanvas::Line*                   _upper_line;
	std::vector<ArdourCanvas::Text*>      _upper_text;
	std::vector<ArdourCanvas::Text*>      _lower_text;
	std::vector<ArdourCanvas::Rectangle*> _upper_backgrounds;
	std::vector<ArdourCanvas::Rectangle*> _lower_backgrounds;

	struct GainMeter : public ArdourCanvas::Container {
		GainMeter (Item* parent, Push2&);

		Push2Knob*  knob;
		LevelMeter* meter;
	};

	GainMeter* gain_meter[8];

	/* stripables */

	int32_t                              _bank_start;
	PBD::ScopedConnectionList            _stripable_connections;
	std::shared_ptr<ARDOUR::Stripable> _stripable[8];
	PBD::ScopedConnectionList            _session_connections;

	void stripables_added ();
	void stripable_property_change (PBD::PropertyChange const& what_changed, uint32_t which);

	void switch_bank (uint32_t base);

	enum VPotMode {
		Volume,
		PanAzimuth,
		PanWidth,
		Send1, Send2, Send3, Send4, Send5
	};

	std::shared_ptr<Push2::Button> _mode_button;
	VPotMode                         _vpot_mode;

	void show_vpot_mode ();

	void solo_changed (uint32_t n);
	void mute_changed (uint32_t n);
	void solo_mute_changed (uint32_t n);
	void show_selection (uint32_t n);
	void hide_selection (uint32_t n);
};

} /* namespace */

#endif /* __ardour_push2_layout_h__ */
