/*
 * Copyright (C) 2008-2015 Paul Davis <paul@linuxaudiosystems.com>
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

#include "pbd/failed_constructor.h"
#include "pbd/error.h"

#include "ardour/session.h"
#include "control_protocol/control_protocol.h"

#include "wiimote.h"

using namespace ARDOUR;
using namespace PBD;

static ControlProtocol*
new_wiimote_protocol (Session* s)
{
	WiimoteControlProtocol* wmcp = new WiimoteControlProtocol (*s);
	wmcp->set_active (true);
	return wmcp;
}

static void
delete_wiimote_protocol (ControlProtocol* cp)
{
	delete cp;
}

static ControlProtocolDescriptor wiimote_descriptor = {
	/* name       */ "Wiimote",
	/* id         */ "uri://ardour.org/surfaces/wiimote:0",
	/* module     */ 0,
	/* available  */ 0,
	/* probe_port */ 0,
	/* match usb  */ 0,
	/* initialize */ new_wiimote_protocol,
	/* destroy    */ delete_wiimote_protocol,
};

extern "C" ARDOURSURFACE_API ControlProtocolDescriptor* protocol_descriptor () { return &wiimote_descriptor; }

