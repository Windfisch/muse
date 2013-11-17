//=========================================================
//  MusE
//  Linux Music Editor
//  $Id: event.cpp,v 1.8.2.5 2009/12/20 05:00:35 terminator356 Exp $
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
#include "event.h"
#include "eventbase.h"
#include "waveevent.h"
#include "midievent.h"

namespace MusECore {

//---------------------------------------------------------
//   Event
//---------------------------------------------------------

//---------------------------------------------------------
//   clone
//---------------------------------------------------------

Event Event::clone() const
      {
      return Event(ev->clone());
      }

Event::Event() 
{ 
  ev = 0; 
}

Event::Event(EventType t) {
            if (t == Wave)
                  ev = new MusECore::WaveEventBase(t);
            else
                  ev = new MusECore::MidiEventBase(t);
            ++(ev->refCount);
            }
Event::Event(const Event& e) {
            ev = e.ev;
            if(ev)
              ++(ev->refCount);
          }
Event::Event(EventBase* eb) {
            ev = eb;
            ++(ev->refCount);
            
            }
            
Event::~Event() {
            if (ev && --(ev->refCount) == 0) {
                  delete ev;
                  ev=0;
                  }
            }

bool Event::empty() const      { return ev == 0; }
EventType Event::type() const  { return ev ? ev->type() : Note;  }

void Event::setType(EventType t) {
            if (ev && --(ev->refCount) == 0) {
                  delete ev;
                  ev = 0;
                  }
            if (t == Wave)
                  ev = new MusECore::WaveEventBase(t);
            else
                  ev = new MusECore::MidiEventBase(t);
            ++(ev->refCount);
            }

Event& Event::operator=(const Event& e) {
            if (ev != e.ev)
            {
              if (ev && --(ev->refCount) == 0) {
                    delete ev;
                    ev = 0;
                    }
              ev = e.ev;
              if (ev)
                    ++(ev->refCount);
            }      
            
            return *this;
            }

bool Event::operator==(const Event& e) const {
            return ev == e.ev;
            }
bool Event::isSimilarTo(const Event& other) const
{
		return ev->isSimilarTo(*other.ev);
}

int Event::getRefCount() const    { return ev->getRefCount(); }
bool Event::selected() const      { return ev->_selected; }
void Event::setSelected(bool val) { ev->_selected = val; }
void Event::move(int offset)      { ev->move(offset); }

void Event::read(Xml& xml)            
{ 
  ev->read(xml); 
}


void Event::write(int a, Xml& xml, const Pos& o, bool forceWavePaths) const { ev->write(a, xml, o, forceWavePaths); }
void Event::dump(int n) const     { ev->dump(n); }
Event Event::mid(unsigned a, unsigned b) const { return Event(ev->mid(a, b)); }

bool Event::isNote() const                   { return ev->isNote();        }
bool Event::isNoteOff() const                { return ev->isNoteOff();     }
bool Event::isNoteOff(const Event& e) const  { return ev->isNoteOff(e); }
int Event::dataA() const                     { return ev->dataA();  }
int Event::pitch() const                     { return ev->dataA();  }
void Event::setA(int val)                    { ev->setA(val);       }
void Event::setPitch(int val)                { ev->setA(val);       }
int Event::dataB() const                     { return ev->dataB();  }
int Event::velo() const                      { return ev->dataB();  }
void Event::setB(int val)                    { ev->setB(val);       }
void Event::setVelo(int val)                 { ev->setB(val);       }
int Event::dataC() const                     { return ev->dataC();  }
int Event::veloOff() const                   { return ev->dataC();  }
void Event::setC(int val)                    { ev->setC(val);       }
void Event::setVeloOff(int val)              { ev->setC(val);       }

const unsigned char* Event::data() const     { return ev->data();    }
int Event::dataLen() const                   { return ev->dataLen(); }
void Event::setData(const unsigned char* data, int len) { ev->setData(data, len); }
const EvData Event::eventData() const        { return ev->eventData(); }

const QString Event::name() const            { return ev->name();  }
void Event::setName(const QString& s)        { ev->setName(s);     }
int Event::spos() const                      { return ev->spos();  }
void Event::setSpos(int s)                   { ev->setSpos(s);     }

void Event::setAudioFile(const QString& path) { ev->setAudioFile(path); }
void Event::reloadAudioFile()                 { ev->reloadAudioFile(); }
QString Event::audioFilePath() const             { return ev->audioFilePath(); }
AudioStream::stretch_mode_t Event::stretchMode() const { return ev->stretchMode(); }
const AudioStream* Event::getAudioStream() const { return ev->getAudioStream(); }
void Event::readAudio(MusECore::WavePart* part, unsigned int offset, float** bpp, int channels, int nn, MusECore::XTick fromXTick, MusECore::XTick toXTick, bool doSeek, bool overwrite)
      {
        ev->readAudio(part, offset, bpp, channels, nn, fromXTick, toXTick, doSeek, overwrite);
      }




void Event::setPosType(Pos::TType t) { ev->setPosType(t); }
Pos::TType Event::posType() const    { return ev->posType(); }
void Event::setLenType(Pos::TType t) { ev->setLenType(t); }
Pos::TType Event::lenType() const    { return ev->lenType(); }


void Event::setTick(XTick xt)           { ev->setTick(xt); }
void Event::setTick(unsigned val)       { ev->setTick(val); }
unsigned Event::tick() const            { return ev->tick(); }
XTick Event::xtick() const              { return ev->xtick(); }
unsigned Event::frame() const           { return ev->frame(); }
void Event::setFrame(unsigned val)      { ev->setFrame(val); }
void Event::setLenTick(XTick xt)        { ev->setLenTick(xt); }
void Event::setLenTick(unsigned val)    { ev->setLenTick(val); }
void Event::setLenFrame(unsigned val)   { ev->setLenFrame(val); }
unsigned Event::lenTick() const         { return ev->lenTick(); }
XTick Event::lenXTick() const           { return ev->lenXTick(); }
unsigned Event::lenFrame() const        { return ev->lenFrame(); }
unsigned Event::endTick() const         { return ev->endTick(); }
XTick Event::endXTick() const           { return ev->endXTick(); }
unsigned Event::endFrame() const        { return ev->endFrame(); }

bool Event::needCopyOnWrite() { return ev->needCopyOnWrite(); }

void Event::setParentalPart(Part* p) { ev->setParentalPart(p); }

} // namespace MusECore
