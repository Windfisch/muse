//=========================================================
//  MusE
//  Linux Music Editor
//  eventbase.cpp
//
//  (C) Copyright 2000-2003 Werner Schweer (ws@seh.de)
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; version 2 of
//  the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//
//=========================================================

#include <stdio.h>
#include <stdlib.h> // for abort()
#include "event.h"
#include "eventbase.h"
#include "waveevent.h"
#include "midievent.h"
#include "part.h"
#include "tempo.h"

namespace MusECore {

static void die(const char* c)
{
	printf("%s :( aborting now, sorry.\n",c);
	abort();
}

//---------------------------------------------------------
//   EventBase
//---------------------------------------------------------

EventBase::EventBase(EventType t)
{
	_type     = t;
	_posType = Pos::TICKS; // wave parts are also in TICKS now.
	_lenType = ( t==Wave ? Pos::FRAMES : Pos::TICKS );
	_tick=XTick(0); _frame=0;
	_lenTick=XTick(0); _lenFrame=0;
	
	refCount  = 0;
	parental_part=NULL;
	_selected = false;
}

EventBase::EventBase(const EventBase& ev)
{
	_posType = ev._posType;
	_lenType = ev._lenType;
	_tick = ev._tick;
	_frame = ev._frame;
	_lenTick = ev._lenTick;
	_lenFrame = ev._lenFrame;
	
	refCount  = 0;
	_selected = ev._selected;
	_type     = ev._type;
	parental_part = ev.parental_part;
}

bool EventBase::operator==(const EventBase& other) const
{
	return (_type == other._type) && (_posType == other._posType) && (_lenType == other._lenType) &&
	     ( (_posType==Pos::TICKS) ? (_tick==other._tick) : (_frame==other._frame) ) &&
	     ( (_lenType==Pos::TICKS) ? (_lenTick==other._lenTick) : (_lenFrame==other._lenFrame) ); 
	     // && (parental_part == other.parental_part) && (_selected == other._selected);
}

//---------------------------------------------------------
//   dump
//---------------------------------------------------------

void EventBase::dump(int n) const
{
	for (int i = 0; i < n; ++i)
		putchar(' ');
	printf("Event %p refs:%d ", this, refCount);
	//TODO
}


void EventBase::setPosType(Pos::TType type)
{
	if (this->_posType == type) return;

	if (this->_posType == Pos::TICKS)
		_frame = frame();
	else
		_tick = tick();
	
	this->_posType = type;
}

void EventBase::setLenType(Pos::TType type)
{
	if (this->_lenType == type) return;

	if (this->_lenType == Pos::TICKS)
		_lenFrame = lenFrame();
	else
		_lenTick = lenTick();
	
	this->_lenType = type;
}


unsigned EventBase::tick() const { return xtick().tick; }
unsigned EventBase::absTick() const { return absXTick().tick; }

XTick EventBase::xtick() const {
	if (_posType == Pos::TICKS)
		return _tick;
	else
	{
		if (parental_part)
			return MusEGlobal::tempomap.frame2xtick(parental_part->frame()+_frame) - parental_part->xtick();
		else
			die("FATAL ERROR: EventBase::xtick() called, but in FRAMES mode and no parental_part!");
	}
	
	// will never be executed, but makes gcc happy
	return XTick(0);
}

XTick EventBase::absXTick() const
{
	if (!parental_part)
		die("FATAL ERROR: EventBase::absXTick() called, but no parental_part!");
	
	if (_posType == Pos::TICKS)
		return _tick + parental_part->tick();
	else
		return MusEGlobal::tempomap.frame2xtick(parental_part->frame()+_frame);
}


audioframe_t EventBase::frame() const
{
	if (_posType == Pos::FRAMES)
		return _frame;
	else
	{
		if (parental_part)
			return MusEGlobal::tempomap.tick2frame(parental_part->xtick()+_tick) - parental_part->frame();
		else
			die("FATAL ERROR: EventBase::frame() called, but in TICKS mode and no parental_part!");
	}
	
	// will never be executed, but makes gcc happy
	return 0;
}

audioframe_t EventBase::absFrame() const
{
	if (!parental_part)
		die("FATAL ERROR: EventBase::absFrame() called, but no parental_part!");

	if (_posType == Pos::FRAMES)
		return _frame;
	else
		return MusEGlobal::tempomap.tick2frame(parental_part->xtick()+_tick);
}

void EventBase::setTick(unsigned tick) { setTick(XTick(tick)); }

void EventBase::setTick(XTick tick)
{
	if (_posType == Pos::TICKS)
		_tick = tick;
	else
	{
		if (parental_part)
			_frame = MusEGlobal::tempomap.tick2frame(parental_part->xtick() + tick) - parental_part->frame();
		else
			die("FATAL ERROR: EventBase::setTick() called, but in FRAMES mode and no parental_part!");
	}
}

void EventBase::setFrame(audioframe_t frame)
{
	if (_posType == Pos::FRAMES)
		_frame = frame;
	else
	{
		if (parental_part)
			_tick = MusEGlobal::tempomap.frame2xtick(parental_part->frame() + frame) - parental_part->xtick();
		else
			die("FATAL ERROR: EventBase::setFrame() called, but in TICKS mode and no parental_part!");
	}
}


unsigned EventBase::endTick() const { return endXTick().tick; }

XTick EventBase::endXTick() const
{
	if (_lenType == Pos::TICKS)
		return xtick() + _lenTick;
	else
	{
		if (parental_part)
			return MusEGlobal::tempomap.frame2xtick(parental_part->frame()+this->frame()+_lenFrame) - parental_part->xtick();
		else
			die("FATAL ERROR: EventBase::endXTick() called, but in FRAMES mode and no parental_part!");
	}
	
	// will never be executed, but makes gcc happy
	return XTick(0);
}

audioframe_t EventBase::endFrame() const
{
	if (_lenType == Pos::FRAMES)
		return frame() + _lenFrame;
	else
	{
		if (parental_part)
			return MusEGlobal::tempomap.tick2frame(parental_part->xtick()+xtick()+_lenTick) - parental_part->frame();
		else
			die("FATAL ERROR: EventBase::endFrame() called, but in TICKS mode and no parental_part!");
	}
	
	// will never be executed, but makes gcc happy
	return 0;
}

unsigned EventBase::lenTick() const { return lenXTick().tick; }

XTick EventBase::lenXTick() const
{
	if (_lenType == Pos::TICKS)
		return _lenTick;
	else
	{
		if (parental_part)
			return endXTick() - xtick();
		else
			die("FATAL ERROR: EventBase::lenXTick() called, but in FRAMES mode and no parental_part!");
	}
	
	// will never be executed, but makes gcc happy
	return XTick(0);
}

audioframe_t EventBase::lenFrame() const
{
	if (_lenType == Pos::FRAMES)
		return _lenFrame;
	else
	{
		if (parental_part)
			return endFrame() - frame();
		else
			die("FATAL ERROR: EventBase::lenFrame() called, but in TICKS mode and no parental_part!");
	}

	// will never be executed, but makes gcc happy
	return 0;
}


void EventBase::setLenTick(unsigned len) { setLenTick(XTick(len)); }

void EventBase::setLenTick(XTick len)
{
	if (_lenType == Pos::TICKS)
		_lenTick = len;
	else
	{
		if (parental_part)
			_lenFrame = MusEGlobal::tempomap.tick2frame(parental_part->xtick() + xtick() + len) - parental_part->frame() - frame();
		else
			die("FATAL ERROR: EventBase::setLenTick() caled, but in FRAMES mode and no parental_part!");
	}
}

void EventBase::setLenFrame(audioframe_t len)
{
	if (_lenType == Pos::FRAMES)
		_lenFrame = len;
	else
	{
		if (parental_part)
			_lenTick = MusEGlobal::tempomap.frame2xtick(parental_part->frame() + frame() + len) - parental_part->xtick() - xtick();
		else
			die("FATAL ERROR: EventBase::setLenFrame() caled, but in TICKS mode and no parental_part!");
	}
}


} // namespace MusECore
