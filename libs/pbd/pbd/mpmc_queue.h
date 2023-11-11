/*
 * Copyright (C) 2010-2011 Dmitry Vyukov
 * Copyright (C) 2019 Robin Gareus <robin@gareus.org>
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

#ifndef _pbd_mpc_queue_h_
#define _pbd_mpc_queue_h_

#include <cassert>
#include <stdint.h>
#include <stdlib.h>

# include <atomic>
# define MPMC_QUEUE_TYPE std::atomic<size_t>

namespace PBD {

/* Lock free multiple producer, multiple consumer queue
 *
 * Inspired by http://www.1024cores.net/home/lock-free-algorithms/queues/bounded-mpmc-queue
 * Kudos to Dmitry Vyukov who licensed that code in terms of a 2-clause BSD license.
 */
template <typename T>
class /*LIBPBD_API*/ MPMCQueue
{
public:
	MPMCQueue (size_t buffer_size = 8)
		: _buffer (0)
		, _buffer_mask (0)
	{
		reserve (buffer_size);
	}

	~MPMCQueue ()
	{
		delete[] _buffer;
	}

	size_t capacity () const {
		return _buffer_mask + 1;
	}

	static size_t
	power_of_two_size (size_t sz)
	{
		int32_t power_of_two;
		for (power_of_two = 1; 1U << power_of_two < sz; ++power_of_two) ;
		return 1U << power_of_two;
	}

	void
	reserve (size_t buffer_size)
	{
		buffer_size = power_of_two_size (buffer_size);
		assert ((buffer_size >= 2) && ((buffer_size & (buffer_size - 1)) == 0));
		if (_buffer_mask >= buffer_size - 1) {
			return;
		}
		delete[] _buffer;
		_buffer      = new cell_t[buffer_size];
		_buffer_mask = buffer_size - 1;
		clear ();
	}

	void
	clear ()
	{
		for (size_t i = 0; i <= _buffer_mask; ++i) {
			_buffer[i]._sequence.store (i, std::memory_order_relaxed);
		}
		_enqueue_pos.store (0, std::memory_order_relaxed);
		_dequeue_pos.store (0, std::memory_order_relaxed);
	}

	bool
	push_back (T const& data)
	{
		cell_t* cell;
		size_t pos = _enqueue_pos.load (std::memory_order_relaxed);

		for (;;) {
			cell = &_buffer[pos & _buffer_mask];
			size_t seq = cell->_sequence.load (std::memory_order_acquire);
			intptr_t dif = (intptr_t)seq - (intptr_t)pos;
			if (dif == 0) {
				if (_enqueue_pos.compare_exchange_weak (pos, pos + 1, std::memory_order_relaxed))
				{
					break;
				}
			} else if (dif < 0) {
				return false;
			} else {
				pos = _enqueue_pos.load (std::memory_order_relaxed);
			}
		}

		cell->_data = data;
		cell->_sequence.store (pos + 1, std::memory_order_release);

		return true;
	}

	bool
	pop_front (T& data)
	{
		cell_t* cell;
		size_t pos = _dequeue_pos.load (std::memory_order_relaxed);

		for (;;) {
			cell = &_buffer[pos & _buffer_mask];
			size_t seq = cell->_sequence.load (std::memory_order_acquire);
			intptr_t dif = (intptr_t)seq - (intptr_t) (pos + 1);
			if (dif == 0) {
				if (_dequeue_pos.compare_exchange_weak (pos, pos + 1, std::memory_order_relaxed))
				{
					break;
				}
			} else if (dif < 0) {
				return false;
			} else {
				pos = _dequeue_pos.load (std::memory_order_relaxed);
			}
		}

		data = cell->_data;
		cell->_sequence.store (pos + _buffer_mask + 1, std::memory_order_release);
		return true;
	}

private:
	struct cell_t {
		MPMC_QUEUE_TYPE _sequence;
		T               _data;
	};

	char            _pad0[64];
	cell_t*         _buffer;
	size_t          _buffer_mask;
	char            _pad1[64 - sizeof (cell_t*) - sizeof (size_t)];
	MPMC_QUEUE_TYPE _enqueue_pos;
	char            _pad2[64 - sizeof (size_t)];
	MPMC_QUEUE_TYPE _dequeue_pos;
	char            _pad3[64 - sizeof (size_t)];
};

} // namespace PBD

#undef MPMC_QUEUE_TYPE

#endif
