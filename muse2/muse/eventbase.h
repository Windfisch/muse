//=========================================================
//  MusE
//  Linux Music Editor
//  $Id: eventbase.h,v 1.3.2.3 2009/12/20 05:00:35 terminator356 Exp $
//
//  (C) Copyright 1999-2004 Werner Schweer (ws@seh.de)
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

#ifndef __EVENTBASE_H__
#define __EVENTBASE_H__

#include <sys/types.h>

#include "pos.h"
#include "event.h"

namespace MusECore {
class WavePart;
class Part;

//---------------------------------------------------------
//   EventBase
//---------------------------------------------------------

class EventBase {
      EventType _type;

   protected:
      int refCount;
      bool _selected;
      Part* parental_part;

      Pos::TType _posType;
      Pos::TType _lenType;

      XTick _tick;
      audioframe_t _frame;
      XTick _lenTick;
      audioframe_t _lenFrame;

   public:
      EventBase(EventType t);
      EventBase(const EventBase& ev);

      virtual ~EventBase() {}
      
      bool operator==(const EventBase&) const;

      int getRefCount() const    { return refCount; }
      EventType type() const     { return _type;  }
      void setType(EventType t)  { _type = t;  }
      
      void setPosType(Pos::TType t);
      Pos::TType posType() { return _posType; }
      void setLenType(Pos::TType t);
      Pos::TType lenType() { return _lenType; }
      
      bool selected() const      { return _selected; }
      void setSelected(bool val) { _selected = val; }

      XTick xtick() const;
      unsigned tick() const;
      audioframe_t frame() const;
      void setTick(unsigned);
      void setTick(XTick);
      void setFrame(audioframe_t);
      void setLenTick(unsigned);
      void setLenTick(XTick);
      void setLenFrame(audioframe_t);
      unsigned lenTick() const;
      XTick lenXTick() const;
      audioframe_t lenFrame() const;
      unsigned endTick() const;
      XTick endXTick() const;
      audioframe_t endFrame() const;


      virtual bool isSimilarTo(const EventBase& other) const = 0;

      virtual void read(Xml&) = 0;
      virtual void write(int, Xml&, const Pos& offset, bool forcePath = false) const = 0;
      virtual void dump(int n = 0) const;
      virtual EventBase* mid(unsigned, unsigned) const = 0;
      friend class Event;

      virtual bool isNote() const                   { return false; }
      virtual bool isNoteOff() const                { return false; }
      virtual bool isNoteOff(const Event&) const    { return false; }
      virtual int pitch() const                     { return 0;      }
      virtual int program() const                   { return 0;      }
      virtual int cntrl() const                     { return 0;      }
      virtual int dataA() const                     { return 0;      }
      virtual void setA(int)                        { }
      virtual void setPitch(int)                    { }

      virtual int cntrlVal() const                  { return 0;      }
      virtual int dataB() const                     { return 0;      }
      virtual int velo() const                      { return 0;      }
      virtual void setB(int)                        { }
      virtual void setVelo(int)                     { }

      virtual int veloOff() const                   { return 0;      }
      virtual int dataC() const                     { return 0;      }
      virtual void setC(int)                        { }
      virtual void setVeloOff(int)                  { }

      virtual const unsigned char* data() const     { return 0; }
      virtual int dataLen() const                   { return 0; }
      virtual void setData(const unsigned char*, int) { }
      virtual const EvData eventData() const        { return EvData(); }

      virtual const QString name() const            { return QString("?");  }
      virtual void setName(const QString&)          { }
      virtual int spos() const                      { return 0;  }
      virtual void setSpos(int)                     { }
      virtual SndFileR sndFile() const              { return 0;      }
      virtual void setSndFile(SndFileR&)            { }
      virtual EventBase* clone() const = 0;
      
      virtual void setAudioFile(const QString&) { }
      virtual QString audioFilePath()               { return QString(""); }
      virtual void reloadAudioFile() { } 
      virtual AudioStream::stretch_mode_t stretchMode() { return AudioStream::NO_STRETCHING; }
      virtual const AudioStream* getAudioStream()   { return NULL; }
      virtual void readAudio(WavePart* /*part*/, unsigned /*offset*/, 
                             float** /*bpp*/, int /*channels*/, int /*nn*/, XTick /*fromXTick*/, XTick /*toXTick*/, bool /*doSeek*/, bool /*overwrite*/) { }
      virtual bool needCopyOnWrite() { return false; }
      
      void setParentalPart(Part* p) { parental_part = p; }
      Part* getParentalPart() { return parental_part; }

      };

} // namespace MusECore

#endif

