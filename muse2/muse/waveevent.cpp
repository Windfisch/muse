//=========================================================
//  MusE
//  Linux Music Editor
//  $Id: waveevent.cpp,v 1.9.2.6 2009/12/20 05:00:35 terminator356 Exp $
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

#include "globals.h"
#include "event.h"
#include "waveevent.h"
#include "xml.h"
#include "song.h"
#include "wave.h"
#include "track.h"
#include <iostream>
#include <math.h>

//#define WAVEEVENT_DEBUG

namespace MusECore {

//---------------------------------------------------------
//   WaveEvent
//---------------------------------------------------------

WaveEventBase::WaveEventBase(EventType t)
   : EventBase(t)
      {
      deleted = false;
      stretch_mode = (_posType==Pos::TICKS && _lenType==Pos::TICKS) ? AudioStream::DO_STRETCHING : AudioStream::NO_STRETCHING;
      audiostream=NULL;
      _spos = 0;
      }

      
WaveEventBase::WaveEventBase(const WaveEventBase& src) : EventBase(src)
{
	this->_name = src._name;
	this->stretch_mode = src.stretch_mode;
	this->_spos = src._spos;
	this->deleted = src.deleted;
	audiostream=NULL;
	
	setAudioFile(src.filename); // this will set filename, audiostream and streamPosition
}

WaveEventBase::~WaveEventBase()
{
	// TODO FINDMICH delete the audiostream
}

      
//---------------------------------------------------------
//   WaveEventBase::clone
//---------------------------------------------------------

EventBase* WaveEventBase::clone() const
{ 
  return new WaveEventBase(*this); 
}

bool WaveEventBase::isSimilarTo(const EventBase& other_) const
{
	const WaveEventBase* other = dynamic_cast<const WaveEventBase*>(&other_);
	if (other==NULL) // dynamic cast hsa failed: "other_" is not of type WaveEventBase.
		return false;
	
	return filename==other->filename && _spos==other->_spos && this->EventBase::operator==(*other);
}

//---------------------------------------------------------
//   WaveEvent::mid
//---------------------------------------------------------

EventBase* WaveEventBase::mid(unsigned b, unsigned e) const
      {
      WaveEventBase* ev = new WaveEventBase(*this);
      unsigned fr = frame();
      unsigned start = fr - b;
      if(b > fr)
      {  
        start = 0;
        ev->setSpos(spos() + b - fr);
      }
      unsigned end = endFrame();
      
      if (e < end)
            end = e;

      ev->setFrame(start);
      ev->setLenFrame(end - b - start);
      return ev;
      }

//---------------------------------------------------------
//   dump
//---------------------------------------------------------

void WaveEventBase::dump(int n) const
      {
      EventBase::dump(n);
      }

void WaveEventBase::setPosType(Pos::TType type)
{
	EventBase::setPosType(type);
	if (type == Pos::FRAMES)
		stretch_mode = AudioStream::NO_STRETCHING;
	
	reloadAudioFile();
}

void WaveEventBase::setLenType(Pos::TType type)
{
	if (type == Pos::FRAMES || _posType == Pos::FRAMES)
		stretch_mode = AudioStream::NO_STRETCHING;
	else if (type == Pos::TICKS && stretch_mode == AudioStream::NO_STRETCHING)
		stretch_mode = AudioStream::DO_STRETCHING;
	
	EventBase::setLenType(type);
	reloadAudioFile();
}


      
void WaveEventBase::reloadAudioFile()
{
	setAudioFile(filename);
}
      
void WaveEventBase::setAudioFile(const QString& path)
{
	filename = path;
	if (audiostream) delete audiostream;
	audiostream=NULL;
	if (path=="") return;
	
	audiostream = new MusECore::AudioStream(filename, MusEGlobal::sampleRate, stretch_mode, this);

	if (!audiostream->isGood())
	{
		delete audiostream;
		audiostream=NULL;
	}
	
	streamPosition = 0; // cause a seek on the next readAudio call.
}
      
//---------------------------------------------------------
//   WaveEventBase::read
//---------------------------------------------------------

void WaveEventBase::read(Xml& xml)
{
	for (;;) {
		Xml::Token token = xml.parse();
		const QString& tag = xml.s1();
		switch (token) {
			case Xml::Error:
			case Xml::End:
			case Xml::Attribut:
				return;
			case Xml::TagStart:
				if (tag == "poslen")
				{
					PosLen tmp;
					tmp.read(xml, "poslen");

					_posType = tmp.type();
					_lenType = tmp.lenType();
					if (tmp.type()==Pos::TICKS)
					     _tick = tmp.tick();
					else
					     _frame = tmp.frame();

					if (tmp.lenType()==Pos::TICKS)
						_lenTick = tmp.lenTick();
					else
						_lenFrame = tmp.lenFrame();
				}
				else if (tag == "frame") // FIXME possibly deprecated, should be managed by audiostreams, check?
					_spos = xml.parseInt();
				else if (tag == "stretch_mode")
					stretch_mode = (AudioStream::stretch_mode_t) xml.parseInt();
				else if (tag == "file") {
					if (audiostream) delete audiostream;
					
					filename = xml.parse1();
					setAudioFile(filename);
				}
				else
					xml.unknown("Event");
				break;
			case Xml::TagEnd:
				if (tag == "event") {
					return;
				}
			default:
				break;
		}
	}
}

//---------------------------------------------------------
//   write
//---------------------------------------------------------

void WaveEventBase::write(int level, Xml& xml, const Pos& offset, bool forcePath) const
      {
      xml.tag(level++, "event");
      
      PosLen wpos;
      wpos.setType(_posType);
      wpos.setLenType(_lenType);
      if (_posType == Pos::TICKS)
            wpos.setTick(_tick);
      else
            wpos.setFrame(_frame);

      if (_lenType == Pos::TICKS)
            wpos.setLenTick(_lenTick);
      else
            wpos.setLenFrame(_lenFrame);

      wpos += offset;
      wpos.write(level, xml, "poslen");
      

      xml.intTag(level, "frame", _spos);  // offset in wave file

      //
      // waves in the project dirctory are stored
      // with relative path name, others with absolute path
      //

      if (!forcePath && filename.contains(MusEGlobal::museProject)) {
            // extract MusEGlobal::museProject.
            QString newName = filename;
            newName = newName.remove(MusEGlobal::museProject+"/");
            xml.strTag(level, "file", newName);
            }
      else
            xml.strTag(level, "file", filename);
      
      xml.intTag(level, "stretch_mode", stretch_mode);
      xml.etag(level, "event");
      }

      
#define STREAM_SEEK_THRESHOLD 10

void WaveEventBase::readAudio(WavePart* part, audioframe_t firstFrame, float** buffer, int channels, int nFrames, XTick fromXTick, XTick toXTick, bool doSeek, bool overwrite)
{
  if (MusEGlobal::heavyDebugMsg)
    printf("readAudio, firstFrame=%i, channels=%i\n", (int)firstFrame, channels);
// doSeek is unreliable! it does not respect moving the part or event!
  
// firstFrame is the sample position to read from. usually, that's the last firstFrame + the last nFrames

// TODO: this will horribly break with clone parts! each clone must have its own audiostream (and streamPosition)!.
  
  if (doSeek || firstFrame != streamPosition)
  {
    if (!doSeek && firstFrame >= streamPosition && firstFrame <= streamPosition+STREAM_SEEK_THRESHOLD)
    {
      // reading some frames from the stream into /dev/null is enough.
      fprintf(stderr, "DEBUG: WaveEventBase::readAudio has to drop frames, diff is %i, threshold is %u!\n",(int)(firstFrame-streamPosition),STREAM_SEEK_THRESHOLD);
      
      int len = audiostream->readAudio(NULL, channels, firstFrame-streamPosition, false);
      streamPosition += len;
    }
    else
    {
      // we need to seek.
      // this might cause crackles and thus is only rarely done.
      fprintf(stderr, "DEBUG: WaveEventBase::readAudio has to seek, diff is %i, threshold is %u!\n",(int)(firstFrame-streamPosition),STREAM_SEEK_THRESHOLD);

      audiostream->seek(firstFrame,fromXTick);
      streamPosition = firstFrame;
    }
  }
  
  streamPosition += audiostream->readAudio(buffer, channels, nFrames, overwrite);
  // now streamPosition == firstFrame+nFrames 
}

bool WaveEventBase::needCopyOnWrite()
{
	QFileInfo this_finfo(this->audioFilePath());
	QString path_this = this_finfo.canonicalPath();
	if(path_this.isEmpty())
		return false;  
	
	bool fwrite = this_finfo.isWritable();
	
	// No exceptions: Even if this wave event is a clone, if it ain't writeable we gotta copy the wave.
	if(!fwrite)
		return true;
	
	
	
	// Count the number of non-clone part wave events (including possibly this one) using this file.
	// Search all active wave events
	int use_count = 0;
	WaveTrackList* wtl = MusEGlobal::song->waves();
	for(ciTrack it = wtl->begin(); it != wtl->end(); ++it)
	{
		PartList* pl = (*it)->parts();
		for(ciPart ip = pl->begin(); ip != pl->end(); ++ip)
		{
			const EventList& el = ip->second->events();
			// We are looking for active independent non-clone parts
			if(ip->second->hasClones()) // TODO i think this is wrong. (flo)
				continue;
			for(ciEvent ie = el.begin(); ie != el.end(); ++ie)
			{
				if(ie->second.type() != Wave)
					continue;
				const Event& ev = ie->second;
				if(ev.empty())
					continue;
				
				QFileInfo finfo(ev.audioFilePath());
				QString path = finfo.canonicalPath();
				if(path.isEmpty())
					continue;
				if(path == path_this)
					++use_count;
				// If more than one non-clone part wave event is using the file, signify that the caller should make a copy of it.
				if(use_count > 1)
					return true;
			}
		}
	}
	
	return false;
}

} // namespace MusECore
