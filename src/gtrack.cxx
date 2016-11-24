/*
 * Author: Harry van Haaren 2013
 *         harryhaaren@gmail.com
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gtrack.hxx"
#include "gui.hxx"

int GTrack::privateID = 0;
int GMasterTrack::privateID = 0;

static void gtrack_vol_cb(Fl_Widget *w, void *data);
static void gtrack_key_cb(Fl_Widget *w, void *data);
static void gtrack_xsideDial_cb(Fl_Widget *w, void *data);
static void gtrack_sendDial_cb(Fl_Widget *w, void *data);
static void gtrack_send_cb(Fl_Widget *w, void *data);
static void gtrack_record_cb(Fl_Widget *w, void *data);


GTrack::GTrack(int id, int w, int h, const char* l ) :
  Fl_Group(8 + id * (w + 8), 40, w, h),
  bg( 8 + id * (w + 8), 40 , w, h, l ),
  
  radial( 8 + id * (w + 8)+ (w/22), 40+ 26, w-w/11, 100, ""),
  
  clipSel(8 + id * (w + 8) + (w/22), 40 + 26 + 102, w-w/11, 294,""),
  
  volBox(8 + id * (w + 8)+ (w/22), 40+422, w-w/11, 172, ""),
  volume(8 + id * (w + 8)+ (w-w/11)*2/3, 40 +425, (w-w/11)/3, 166, ""),
  
  
  sendDial   (8 + id * (w + 8)+ (w/5), 40 +430 +   0, (w-w/11)*3/10, 30, ""),
  sendActive (8 + id * (w + 8)+ (w/10), 40 +430 +  32, (w-w/11)/2, 25, "S"),
  
  xsideDial    (8 + id * (w + 8)+ (w/5), 40 +430 + 69, (w-w/11)*3/10, 30, ""),
  keyActive  (8 + id * (w + 8)+(w/10), 40 +430 + 101, (w-w/11)/2, 25, "K"),
  
  recordActive  (8 + id * (w + 8)+(w/10), 40 +430 + 132, (w-w/11)/2, 25, "X")
{
  ID = privateID++;
  
  clipSel.setID( ID );
  
  sendDial.callback( gtrack_sendDial_cb, this );
  sendActive.setColor( 0, 1.0, 0.0 );
  sendActive.callback( gtrack_send_cb, this );
  
  xsideDial.callback( gtrack_xsideDial_cb, this );
  xsideDial.align( FL_ALIGN_BOTTOM );
  keyActive.callback( gtrack_key_cb, this );
  keyActive.setColor( 0, 0.6, 1 );
  
  
  recordActive.setColor( 1, 0.0, 0.0 );
  recordActive.callback( gtrack_record_cb, this );
  
  volume.callback( gtrack_vol_cb, this );
  
  //volBox.color( fl_rgb_color( 0,0,0 ) );
  
  end(); // close the group
}

float GTrack::getSend(){return sendDial.value(); }
float GTrack::getXSide(){return xsideDial.value(); }

bool GTrack::getSendActive  (){return sendActive.value(); }
bool GTrack::getKeyActive   (){return keyActive.value(); }
bool GTrack::getRecordActive(){return recordActive.value(); }


void GTrack::setSend(float s){ sendDial.value( s ); }
void GTrack::setXSide(float s){ xsideDial.value( s ); }

void GTrack::setSendActive(bool a){ sendActive.value( a ); }
void GTrack::setKeyActive(bool a){ keyActive.value( a ); }
void GTrack::setRecordActive(bool a){ recordActive.value( a ); }


void gtrack_sendDial_cb(Fl_Widget *w, void *data)
{
  GTrack* track = (GTrack*) data;
  EventTrackSend e( track->ID, SEND_POSTFADER, ((Avtk::Dial*)w)->value() );
  writeToDspRingbuffer( &e );
  //printf("track %i reverb send %f\n", track->ID, ((Avtk::Dial*)w)->value() );
}


void gtrack_key_cb(Fl_Widget *w, void *data)
{
  GTrack* track = (GTrack*) data;
  Avtk::LightButton* d = (Avtk::LightButton*)w;
  bool b = d->value();
  if ( b < 0.5 )
  {
    EventTrackSendActive e( track->ID, SEND_KEY, true );
    writeToDspRingbuffer( &e );
  }
  else
  {
    EventTrackSendActive e( track->ID, SEND_KEY, false );
    writeToDspRingbuffer( &e );
  }
  //printf("track %i post send %s\n", track->ID, b ? "off" : "on" );
}


void gtrack_xsideDial_cb(Fl_Widget *w, void *data)
{
  GTrack* track = (GTrack*) data;
  EventTrackSend e( track->ID, SEND_XSIDE, ((Avtk::Dial*)w)->value() );
  writeToDspRingbuffer( &e );
  //printf("track %i side send %f\n", track->ID, ((Avtk::Dial*)w)->value() );
}

void gtrack_vol_cb(Fl_Widget *w, void *data)
{
  GTrack* track = (GTrack*) data;
  EventTrackVol e( track->ID, ((Avtk::Volume*)w)->value() );
  writeToDspRingbuffer( &e );
  //printf("track %i vol %f\n", track->ID, ((Avtk::Dial*)w)->value() );
}


void gtrack_send_cb(Fl_Widget *w, void *data)
{
  GTrack* track = (GTrack*) data;
  Avtk::LightButton* d = (Avtk::LightButton*)w;
  bool b = d->value();
  d->value( !b );
  if ( b < 0.5 )
  {
    EventTrackSendActive e( track->ID, SEND_POSTFADER, 1.0f );
    writeToDspRingbuffer( &e );
  }
  else
  {
    EventTrackSendActive e( track->ID, SEND_POSTFADER, 0.0f );
    writeToDspRingbuffer( &e );
  }
  //printf("track %i reverb send %s\n", track->ID, b ? "true" : "false" );
}
void gtrack_record_cb(Fl_Widget *w, void *data)
{
  GTrack* track = (GTrack*) data;
  Avtk::LightButton* d = (Avtk::LightButton*)w;
  bool b = d->value();
  if ( b < 0.5 )
  {
    EventTrackRecordArm e( track->ID, 1.0f );
    writeToDspRingbuffer( &e );
  }
  else
  {
    EventTrackRecordArm e( track->ID, 0.0f );
    writeToDspRingbuffer( &e );
  }
  //printf("track %i record Arm %s\n", track->ID, b ? "off" : "on" );
}

