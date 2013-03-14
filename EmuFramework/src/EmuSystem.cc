/*  This file is part of Imagine.

	Imagine is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Imagine is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Imagine.  If not, see <http://www.gnu.org/licenses/> */

#include <libgen.h>
#include <EmuSystem.hh>
#include <EmuOptions.hh>
#include <audio/Audio.hh>

EmuSystem::State EmuSystem::state = EmuSystem::State::OFF;
FsSys::cPath EmuSystem::gamePath = "";
FsSys::cPath EmuSystem::fullGamePath = "";
FsSys::cPath EmuSystem::savePath_ = "";
char EmuSystem::gameName[256] = "";
char EmuSystem::fullGameName[256] = "";
TimeSys EmuSystem::startTime;
Gfx::FrameTimeBase EmuSystem::startFrameTime = 0;
int EmuSystem::emuFrameNow;
int EmuSystem::saveStateSlot = 0;
Audio::PcmFormat EmuSystem::pcmFormat = Audio::pPCM;
const uint EmuSystem::optionFrameSkipAuto = 32;
EmuSystem::LoadGameCompleteDelegate EmuSystem::loadGameCompleteDel;
Base::CallbackRef *EmuSystem::autoSaveStateCallbackRef = nullptr;
void fixFilePermissions(const char *path);

void saveAutoStateFromTimer();
static const auto autoSaveStateCallback = Base::CallbackDelegate::create<&saveAutoStateFromTimer>();

void saveAutoStateFromTimer()
{
	logMsg("auto-save state timer fired");
	EmuSystem::saveAutoState();
	EmuSystem::autoSaveStateCallbackRef = Base::callbackAfterDelaySec(autoSaveStateCallback, 60*optionAutoSaveState);
}

void EmuSystem::cancelAutoSaveStateTimer()
{
	if(autoSaveStateCallbackRef)
	{
		Base::cancelCallback(autoSaveStateCallbackRef);
		autoSaveStateCallbackRef = nullptr;
	}
}

void EmuSystem::startAutoSaveStateTimer()
{
	if(optionAutoSaveState > 1)
	{
		assert(!autoSaveStateCallbackRef);
		autoSaveStateCallbackRef = Base::callbackAfterDelaySec(autoSaveStateCallback, 60*optionAutoSaveState); // minutes to seconds
	}
}

void EmuSystem::startSound()
{
	if(optionSound)
	{
		Audio::openPcm(pcmFormat);
	}
}

void EmuSystem::stopSound()
{
	if(optionSound)
	{
		//logMsg("stopping sound");
		Audio::closePcm();
	}
}

bool EmuSystem::stateExists(int slot)
{
	FsSys::cPath saveStr;
	sprintStateFilename(saveStr, slot);
	return FsSys::fileExists(saveStr);
}

bool EmuSystem::loadAutoState()
{
	if(optionAutoSaveState)
	{
		if(loadState(-1))
		{
			logMsg("loaded autosave-state");
			return 1;
		}
	}
	return 0;
}

//static int autoFrameSkipLevel = 0;
//static int lowBufferFrames = (audio_maxRate/60)*3, highBufferFrames = (audio_maxRate/60)*5;

int EmuSystem::setupFrameSkip(uint optionVal, Gfx::FrameTimeBase frameTime)
{
	static const uint maxFrameSkip = 6;
	static const uint ntscNSecs = 16666666, palNSecs = 20000000;
	static const auto ntscFrameTime = Gfx::decimalFrameTimeBaseFromSec(1./60.),
			palFrameTime = Gfx::decimalFrameTimeBaseFromSec(1./50.);
	if(!EmuSystem::vidSysIsPAL() && optionVal != optionFrameSkipAuto)
	{
		return optionVal; // constant frame-skip for NTSC source
	}

	int emuFrame;
	if(Base::supportsFrameTime())
	{
		if(!startFrameTime)
		{
			startFrameTime = frameTime;
			emuFrame = 0;
			//logMsg("first frame time %f", (double)frameTime);
		}
		else
		{
			auto timeTotal = frameTime - startFrameTime;
			auto frame = round(timeTotal / (vidSysIsPAL() ? palFrameTime : ntscFrameTime));
			emuFrame = frame;
			//logMsg("last frame time %f, on frame %d, was %d, total time %f", (double)frameTime, emuFrame, emuFrameNow, (double)timeTotal);
		}
	}
	else
	{
		if(!startTime)
		{
			startTime.setTimeNow();
			emuFrame = 0;
			//logMsg("first frame time %f", (double)startTime);
		}
		else
		{
			auto timeTotal = TimeSys::timeNow() - startTime;
			emuFrame = timeTotal.divByNSecs(vidSysIsPAL() ? palNSecs : ntscNSecs);
			//logMsg("on frame %d, was %d, total time %f", emuFrame, emuFrameNow, (double)timeTotal);
		}
	}
	assert(emuFrame >= emuFrameNow);
	if(emuFrame == emuFrameNow)
	{
		//logMsg("repeating frame %d", emuFrame);
		return -1;
	}
	else
	{
		uint skip = IG::min((emuFrame - emuFrameNow) - 1, (int)maxFrameSkip);
		emuFrameNow = emuFrame;
		if(skip)
		{
			//logMsg("skipping %u frames", skip);
		}
		return skip;
	}

	/*uint skip = 0;
	gfx_updateFrameTime();
	//logMsg("%d real frames passed", gfx_frameTimeRel);
	if(gfx_frameTimeRel > 1)
	{
		skip = min(gfx_frameTimeRel - 1, maxFrameSkip);
		if(skip && Audio::framesFree() < Audio::maxRate/12)
		{
			logMsg("not skipping %d frames from full audio buffer", skip);
			skip = 0;
		}
		else
		{
			logMsg("skipping %u frames", skip);
		}
	}
	if(gfx_frameTimeRel == 0)
	{
		logMsg("no frames passed");
		return -1;
	}
	return skip;*/
}

void EmuSystem::setupGamePaths(const char *filePath, bool searchInDocument)
{
    if (searchInDocument) {
        snprintf(fullGamePath, sizeof(fullGamePath), "%s/%s", Base::documentsPath(), filePath);
        
    } else {
        snprintf(fullGamePath, sizeof(fullGamePath), "%s/%s", Base::applicationPath(), filePath);
    }
    
	{
		FsSys::cPath baseNameTemp;
		string_copy(baseNameTemp, filePath);
		string_copy(gameName, basename(baseNameTemp));
        
		// If gameName has an extension, truncate it
		auto dotPos = strrchr(gameName, '.');
		if(dotPos)
			*dotPos = 0;
		logMsg("set game name: %s", gameName);
	}
}

