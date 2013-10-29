//=========================================================
//  MusE
//  Linux Music Editor
//  $Id: eventlist.cpp,v 1.7.2.3 2009/11/05 03:14:35 terminator356 Exp $
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

#include "tempo.h"
#include "event.h"
#include "xml.h"

namespace MusECore {



//---------------------------------------------------------
//   add
//---------------------------------------------------------

iEvent EventList::add(Event& event)
      {
      // Order Events by FRAMES or TICKS, depending on their Pos type
      // WaveTrack::fetchData() relies on the sorting order, and
      //  there was a bug that waveparts were sometimes muted because of
      //  incorrect sorting order (by ticks).
      // Also, when the tempo map is changed, every wave event would have to be
      //  re-added to the event list so that the proper sorting order (by ticks)
      //  could be achieved.
      // Note that in a med file, the tempo list is loaded AFTER all the tracks.
      // There was a bug that all the wave events' tick values were not correct,
      // since they were computed BEFORE the tempo map was loaded.
      
      if(event.posType() == Pos::FRAMES)
        return insert(std::pair<const unsigned, Event> (event.frame(), event));          

      unsigned key = event.tick();
      if(event.type() == Note)      // Place notes after controllers.
      {
        iEvent i = upper_bound(key);
        return insert(i, std::pair<const unsigned, Event> (key, event));   
      }
      else
      {
        iEvent i = lower_bound(key);
        while(i != end() && i->first == key && i->second.type() != Note)
          ++i;
        return insert(i, std::pair<const unsigned, Event> (key, event));   
      }
      }

//---------------------------------------------------------
//   find
//---------------------------------------------------------

iEvent EventList::find(const Event& event)
{
      std::pair<iEvent,iEvent> range = equal_range(event.posType() == Pos::FRAMES ? event.frame() : event.tick());

      for (iEvent i = range.first; i != range.second; ++i) {
            if (i->second == event)
                  return i;
            }
      return end();
}

ciEvent EventList::find(const Event& event) const
      {
      EventRange range = equal_range(event.posType() == Pos::FRAMES ? event.frame() : event.tick());

      
      for (ciEvent i = range.first; i != range.second; ++i) {
            if (i->second == event)
                  return i;
            }
      return end();
      }

iEvent EventList::findSimilar(const Event& event)
{
      std::pair<iEvent,iEvent> range = equal_range(event.posType() == Pos::FRAMES ? event.frame() : event.tick());

      for (iEvent i = range.first; i != range.second; ++i) {
            if (i->second.isSimilarTo(event))
                  return i;
            }
      return end();
}

ciEvent EventList::findSimilar(const Event& event) const
      {
      EventRange range = equal_range(event.posType() == Pos::FRAMES ? event.frame() : event.tick());

      
      for (ciEvent i = range.first; i != range.second; ++i) {
            if (i->second.isSimilarTo(event))
                  return i;
            }
      return end();
      }

//---------------------------------------------------------
//   dump
//---------------------------------------------------------

void EventList::dump() const
      {
      for (ciEvent i = begin(); i != end(); ++i)
            i->second.dump();
      }

} // namespace MusECore
