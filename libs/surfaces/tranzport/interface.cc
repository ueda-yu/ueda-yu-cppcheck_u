/*
 *   Copyright (C) 2006 Paul Davis
 *   Copyright (C) 2007 Michael Taht
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   */

#include "control_protocol/control_protocol.h"
#include "tranzport_control_protocol.h"

using namespace ARDOUR;

static ControlProtocol*
new_tranzport_protocol (Session* s)
{
	TranzportControlProtocol* tcp = new TranzportControlProtocol (*s);

	if (tcp->set_active (true)) {
		delete tcp;
		return 0;
	}

	return tcp;

}

static void
delete_tranzport_protocol (ControlProtocol* cp)
{
	delete cp;
}

static ControlProtocolDescriptor tranzport_descriptor = {
	/* name       */ "Tranzport",
	/* id         */ "uri://ardour.org/surfaces/tranzport:0",
	/* module     */ 0,
	/* available  */ TranzportControlProtocol::available,
	/* probe_port */ 0,
	/* match usb  */ 0,
	/* initialize */ new_tranzport_protocol,
	/* destroy    */ delete_tranzport_protocol,
};


extern "C" ARDOURSURFACE_API ControlProtocolDescriptor* protocol_descriptor () { return &tranzport_descriptor; }


