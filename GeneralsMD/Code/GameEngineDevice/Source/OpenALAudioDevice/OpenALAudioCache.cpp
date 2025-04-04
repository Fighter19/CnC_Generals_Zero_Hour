#include "OpenALAudioCache.h"

extern "C" {
#include <libavcodec/avcodec.h>
}

#include "Common/AudioEventInfo.h"
#include "Common/AudioEventRTS.h"
#include "Common/File.h"
#include "Common/FileSystem.h"

#include <cstring>

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
OpenALAudioFileCache::OpenALAudioFileCache() : m_maxSize(0), m_currentlyUsedSize(0)
{
}

Bool OpenALAudioFileCache::decodeFFmpeg(OpenAudioFile* file)
{
	std::vector<uint8_t> audioData;
	auto on_frame = [&audioData](AVFrame* frame, int stream_idx, int stream_type, void* user_data) {
		OpenAudioFile* file = static_cast<OpenAudioFile*>(user_data);
		if (stream_type != AVMEDIA_TYPE_AUDIO) {
			return;
		}

		if (av_sample_fmt_is_planar(static_cast<AVSampleFormat>(frame->format))) {
			return;
		}

		const int frame_data_size = file->m_ffmpegFile->getSizeForSamples(frame->nb_samples);
		audioData.reserve(audioData.size() + frame_data_size);
		audioData.insert(audioData.end(), frame->data[0], frame->data[0] + frame_data_size);
		file->m_fileSize += frame_data_size;
		file->m_totalSamples += frame->nb_samples;
		};

	file->m_ffmpegFile->setFrameCallback(on_frame);
	file->m_ffmpegFile->setUserData(file);

	// Read all packets inside the file
	while (file->m_ffmpegFile->decodePacket()) {
	}

	// Fill the buffer with the audio data
	alBufferData(file->m_buffer, OpenALAudioManager::getALFormat(file->m_ffmpegFile->getNumChannels(), file->m_ffmpegFile->getBytesPerSample() * 8),
		audioData.data(), audioData.size(), file->m_ffmpegFile->getSampleRate());

	// Calculate the duration in MS
	file->m_duration = (file->m_totalSamples / (float)file->m_ffmpegFile->getSampleRate()) * 1000.0f;

	return true;
}

//-------------------------------------------------------------------------------------------------
OpenALAudioFileCache::~OpenALAudioFileCache()
{
	{
		std::lock_guard mut(m_mutex);

		// Free all the samples that are open.
		OpenFilesHashIt it;
		for (it = m_openFiles.begin(); it != m_openFiles.end(); ++it) {
			if (it->second.m_openCount > 0) {
				DEBUG_CRASH(("Sample '%s' is still playing, and we're trying to quit.\n", it->second.m_eventInfo->m_audioName.str()));
			}

			releaseOpenAudioFile(&it->second);
			// Don't erase it from the map, cause it makes this whole process way more complicated, and 
			// we're about to go away anyways.
		}
	}

}

void* OpenALAudioFileCache::openFile(AsciiString& filename)
{
	// Protect the entire openFile function
	std::lock_guard mut(m_mutex);

	auto it = m_openFiles.find(filename);

	if (it != m_openFiles.end()) {
		++it->second.m_openCount;
		return (void*)it->second.m_buffer;
	}

	// Couldn't find the file, so actually open it.
	File* file = TheFileSystem->openFile(filename.str());
	if (!file) {
		DEBUG_ASSERTLOG(filename.isEmpty(), ("Missing Audio File: '%s'\n", filename.str()));
		return NULL;
	}

	UnsignedInt fileSize = file->size();

	OpenAudioFile openedAudioFile;
	alGenBuffers(1, &openedAudioFile.m_buffer);
	openedAudioFile.m_ffmpegFile = new FFmpegFile();

	// This transfer ownership of file
	if (!openedAudioFile.m_ffmpegFile->open(file)) {
		releaseOpenAudioFile(&openedAudioFile);
		return nullptr;
	}

	if (!decodeFFmpeg(&openedAudioFile)) {
		releaseOpenAudioFile(&openedAudioFile);
		return nullptr;
	}

	openedAudioFile.m_ffmpegFile->close();

	openedAudioFile.m_fileSize = fileSize;
	m_currentlyUsedSize += openedAudioFile.m_fileSize;
	if (m_currentlyUsedSize > m_maxSize) {
		DEBUG_LOG(("Audio Cache is full, trying to free some space\n"));
		// We need to free some samples, or we're not going to be able to play this sound.
		if (!freeEnoughSpaceForSample(openedAudioFile)) {
			DEBUG_LOG(("Couldn't free enough space for sample\n"));
			m_currentlyUsedSize -= openedAudioFile.m_fileSize;
			releaseOpenAudioFile(&openedAudioFile);
			return NULL;
		}
	}

	m_openFiles[filename] = openedAudioFile;
	return (void*)openedAudioFile.m_buffer;
}

//-------------------------------------------------------------------------------------------------
void* OpenALAudioFileCache::openFile(AudioEventRTS* eventToOpenFrom)
{
	// Protect the entire openFile function
	std::lock_guard mut(m_mutex);

	AsciiString strToFind;
	switch (eventToOpenFrom->getNextPlayPortion())
	{
	case PP_Attack:
		strToFind = eventToOpenFrom->getAttackFilename();
		break;
	case PP_Sound:
		strToFind = eventToOpenFrom->getFilename();
		break;
	case PP_Decay:
		strToFind = eventToOpenFrom->getDecayFilename();
		break;
	case PP_Done:
		return NULL;
	}

	auto it = m_openFiles.find(strToFind);

	if (it != m_openFiles.end()) {
		++it->second.m_openCount;
		return  (void*)it->second.m_buffer;
	}

	// Couldn't find the file, so actually open it.
	File* file = TheFileSystem->openFile(strToFind.str());
	if (!file) {
		DEBUG_ASSERTLOG(strToFind.isEmpty(), ("Missing Audio File: '%s'\n", strToFind.str()));
		return NULL;
	}

	UnsignedInt fileSize = file->size();

	OpenAudioFile openedAudioFile;
	alGenBuffers(1, &openedAudioFile.m_buffer);
	openedAudioFile.m_eventInfo = eventToOpenFrom->getAudioEventInfo();
	openedAudioFile.m_ffmpegFile = new FFmpegFile();

	// This transfer ownership of file
	if (!openedAudioFile.m_ffmpegFile->open(file)) {
		releaseOpenAudioFile(&openedAudioFile);
		return nullptr;
	}

	if (eventToOpenFrom->isPositionalAudio()) {
		if (openedAudioFile.m_ffmpegFile->getNumChannels() > 1) {
			DEBUG_CRASH(("Requested Positional Play of audio '%s', but it is in stereo.", strToFind.str()));
			return NULL;
		}
	}

	if (!decodeFFmpeg(&openedAudioFile)) {
		releaseOpenAudioFile(&openedAudioFile);
		return nullptr;
	}

	openedAudioFile.m_ffmpegFile->close();

	openedAudioFile.m_fileSize = fileSize;
	m_currentlyUsedSize += openedAudioFile.m_fileSize;
	if (m_currentlyUsedSize > m_maxSize) {
		DEBUG_LOG(("Audio Cache is full, trying to free some space\n"));
		// We need to free some samples, or we're not going to be able to play this sound.
		if (!freeEnoughSpaceForSample(openedAudioFile)) {
			DEBUG_LOG(("Couldn't free enough space for sample\n"));
			m_currentlyUsedSize -= openedAudioFile.m_fileSize;
			releaseOpenAudioFile(&openedAudioFile);
			return NULL;
		}
	}

	m_openFiles[strToFind] = openedAudioFile;
	return (void*)openedAudioFile.m_buffer;
}

//-------------------------------------------------------------------------------------------------
void OpenALAudioFileCache::closeFile(void* fileToClose)
{
	if (!fileToClose) {
		return;
	}

	// Protect the entire closeFile function
	std::lock_guard mut(m_mutex);

	OpenFilesHash::iterator it;
	for (it = m_openFiles.begin(); it != m_openFiles.end(); ++it) {
		if (it->second.m_buffer == (ALuint)(uintptr_t)fileToClose) {
			--it->second.m_openCount;
			return;
		}
	}
}

float OpenALAudioFileCache::getFileLength(void* handle)
{
	if (handle == nullptr) {
		return 0.0f;
	}

	std::lock_guard mut(m_mutex);

	for (auto it = m_openFiles.begin(); it != m_openFiles.end(); ++it) {
		if (it->second.m_buffer == (ALuint)(uintptr_t)handle) {
			return it->second.m_duration;
		}
	}

	return 0.0f;
}

//-------------------------------------------------------------------------------------------------
void OpenALAudioFileCache::setMaxSize(UnsignedInt size)
{
	// Protect the function, in case we're trying to use this value elsewhere.
	std::lock_guard mut(m_mutex);

	m_maxSize = size;
}

//-------------------------------------------------------------------------------------------------
void OpenALAudioFileCache::releaseOpenAudioFile(OpenAudioFile* fileToRelease)
{
	if (fileToRelease->m_openCount > 0) {
		// This thing needs to be terminated IMMEDIATELY.
		TheAudio->closeAnySamplesUsingFile((const void*)fileToRelease->m_buffer);
	}

	if (fileToRelease->m_buffer) {
		if (fileToRelease->m_ffmpegFile) {
			// Free FFMPEG handles
			delete fileToRelease->m_ffmpegFile;
		}
		if (fileToRelease->m_buffer)
		{
			// Free the OpenAL buffer
			alDeleteBuffers(1, &fileToRelease->m_buffer);
		}
		fileToRelease->m_ffmpegFile = NULL;
		fileToRelease->m_buffer = NULL;
		fileToRelease->m_eventInfo = NULL;
	}
}

//-------------------------------------------------------------------------------------------------
Bool OpenALAudioFileCache::freeEnoughSpaceForSample(const OpenAudioFile& sampleThatNeedsSpace)
{

	Int spaceRequired = m_currentlyUsedSize - m_maxSize;
	Int runningTotal = 0;

	std::list<AsciiString> filesToClose;
	// First, search for any samples that have ref counts of 0. They are low-hanging fruit, and 
	// should be considered immediately.
	OpenFilesHashIt it;
	for (it = m_openFiles.begin(); it != m_openFiles.end(); ++it) {
		if (it->second.m_openCount == 0) {
			// This is said low-hanging fruit.
			filesToClose.push_back(it->first);

			runningTotal += it->second.m_fileSize;

			if (runningTotal >= spaceRequired) {
				break;
			}
		}
	}

	// If we don't have enough space yet, then search through the events who have a count of 1 or more
	// and who are lower priority than this sound.
	// Mical said that at this point, sounds shouldn't care if other sounds are interruptable or not.
	// Kill any files of lower priority necessary to clear our the buffer.
	if (runningTotal < spaceRequired) {
		for (it = m_openFiles.begin(); it != m_openFiles.end(); ++it) {
			if (it->second.m_openCount > 0) {
				if (it->second.m_eventInfo->m_priority < sampleThatNeedsSpace.m_eventInfo->m_priority) {
					filesToClose.push_back(it->first);
					runningTotal += it->second.m_fileSize;

					if (runningTotal >= spaceRequired) {
						break;
					}
				}
			}
		}
	}

	// We weren't able to find enough sounds to truncate. Therefore, this sound is not going to play.
	if (runningTotal < spaceRequired) {
		return FALSE;
	}

	std::list<AsciiString>::iterator ait;
	for (ait = filesToClose.begin(); ait != filesToClose.end(); ++ait) {
		OpenFilesHashIt itToErase = m_openFiles.find(*ait);
		if (itToErase != m_openFiles.end()) {
			releaseOpenAudioFile(&itToErase->second);
			m_currentlyUsedSize -= itToErase->second.m_fileSize;
			m_openFiles.erase(itToErase);
		}
	}

	return TRUE;
}