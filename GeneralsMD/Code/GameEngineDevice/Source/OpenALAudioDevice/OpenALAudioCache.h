/*
**	Command & Conquer Generals Zero Hour(tm)
**	Copyright 2025 Electronic Arts Inc.
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// FILE: OpenALAudioCache.h //////////////////////////////////////////////////////////////////////////
// OpenALAudioCache implementation
// Author: Stephan Vedder, March 2025
#pragma once

// #ifndef SAGE_USE_FFMPEG
// #error "SAGE_USE_FFMPEG must be defined to use the OpenALAudioCache."
// #endif

#include "OpenALAudioDevice/OpenALAudioManager.h"
#include "VideoDevice/FFmpeg/FFmpegFile.h"

#include <mutex>
#include <unordered_map>

struct PlayingAudio
{
	ALuint m_source;
	ALuint m_buffer;

	PlayingAudioType m_type;
	volatile PlayingStatus m_status;	// This member is adjusted by another running thread.
	AudioEventRTS* m_audioEventRTS;
	void* m_file;		// The file that was opened to play this
	Bool m_requestStop;
	Bool m_cleanupAudioEventRTS;
	Int m_framesFaded;

	PlayingAudio() :
		m_type(PAT_INVALID),
		m_audioEventRTS(NULL),
		m_requestStop(false),
		m_cleanupAudioEventRTS(true),
		m_source(0),
		m_buffer(0),
		m_framesFaded(0)
	{ }
};

struct OpenAudioFile
{
	FFmpegFile* m_ffmpegFile;
	uint8_t* m_file;
	UnsignedInt m_openCount;
	UnsignedInt m_fileSize;

	Bool m_compressed;	// if the file was compressed, then we need to free it with a miles function.

	// Note: OpenAudioFile does not own this m_eventInfo, and should not delete it.
	const AudioEventInfo* m_eventInfo;	// Not mutable, unlike the one on AudioEventRTS.
	int m_totalSamples = 0;
	float m_duration = 0.0f;
};

typedef std::unordered_map< AsciiString, OpenAudioFile, rts::hash<AsciiString>, rts::equal_to<AsciiString> > OpenFilesHash;
typedef OpenFilesHash::iterator OpenFilesHashIt;

class AudioFileCache
{
public:
	AudioFileCache();

	// Protected by mutex
	virtual ~AudioFileCache();
	void* openFile(AsciiString& filename);
	void* openFile(AudioEventRTS* eventToOpenFrom);
	void closeFile(void* fileToClose);
	void setMaxSize(UnsignedInt size);
	float getFileLength(void* file) const;
	// End Protected by mutex

	// Note: These functions should be used for informational purposes only. For speed reasons,
	// they are not protected by the mutex, so they are not guarenteed to be valid if called from
	// outside the audio cache. They should be used as a rough estimate only.
	UnsignedInt getCurrentlyUsedSize() const { return m_currentlyUsedSize; }
	UnsignedInt getMaxSize() const { return m_maxSize; }

	static void getWaveData(void* wave_data,
		uint8_t*& data,
		UnsignedInt& size,
		UnsignedInt& freq,
		UnsignedInt& channels,
		UnsignedInt& bitsPerSample);

protected:
	void releaseOpenAudioFile(OpenAudioFile* fileToRelease);

	// This function will return TRUE if it was able to free enough space, and FALSE otherwise.
	Bool freeEnoughSpaceForSample(const OpenAudioFile& sampleThatNeedsSpace);

	// FFmpeg related
	Bool decodeFFmpeg(OpenAudioFile* fileToDecode);
	void fillWaveData(OpenAudioFile* fileToFill);

	OpenFilesHash m_openFiles;
	UnsignedInt m_currentlyUsedSize;
	UnsignedInt m_maxSize;
	std::mutex m_mutex;
};
