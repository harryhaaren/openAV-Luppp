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


#ifndef AVTK_CLIP_SELECTOR_H
#define AVTK_CLIP_SELECTOR_H

#include <string>
#include <sstream>
#include <FL/Fl_Button.H>

#include "../config.hxx"

#include "../looper.hxx"
#include "../gridlogic.hxx"
#include "../audiobuffer.hxx"
#include "../eventhandler.hxx"

using namespace std;

namespace Avtk
{

class ClipState
{
public:
	ClipState() :
		state(GridLogic::STATE_EMPTY),
		name(""),
		barsText("")
	{}

	void setName(std::string n)
	{
		name = n;
	}

	void setState(GridLogic::State s)
	{
		state = s;
	}

	std::string getName()
	{
		return name;
	}

	GridLogic::State getState()
	{
		return state;
	}

	int getBeatsToRecord(){
		return beatsToRecord;
	}

	int getBeats(){
		return beats;
	}

	void setBeats(int numBeats, bool isBeatsToRecord);

	const char *getBeatsText() {
		return beatsText.c_str();
	}

	const char *getBarsText() {
		return barsText.c_str();
	}

private:
	GridLogic::State state;
	std::string name;
	std::string beatsText;
	std::string barsText;

	int beatsToRecord = -1;
	int beats = 0;
};

class ClipSelector : public Fl_Button
{
public:
	ClipSelector( int _x, int _y, int _w, int _h,
		      const char *_label, bool master = false);

	int ID;

	static const int numClips = NSCENES;
	ClipState clips[numClips];

	/// indicates if a clip is the "special" clip
	void setSpecial(int scene);
	int special;

	bool _master;
	bool mouseOver;
	bool highlight;
	int x, y, w, h;

	void setID( int id );

	/** converts the Looper::State into the UI represnted ClipSelector state.
	 * It puts some of the data into clips[], and stores unique state into the class.
	**/
	void setState( int clipNum, GridLogic::State s );

	void clipName(int clip, std::string name);
	void setClipBeats(int scene, int beats, bool isBeatsToRecord);

	std::string clipName(int clip);

	void draw();
	int handle(int event);

	void resize(int X, int Y, int W, int H);
	int getLastClipNum();

private:
	int clipNum;
};
} // Avtk

#endif // AVTK_CLIP_SELECTOR_H
