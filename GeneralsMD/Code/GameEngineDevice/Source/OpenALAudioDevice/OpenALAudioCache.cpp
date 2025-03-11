#include "OpenALAudioCache.h"

extern "C" {
#include <libavcodec/avcodec.h>
}

#include "Common/AudioEventInfo.h"
#include "Common/AudioEventRTS.h"
#include "Common/File.h"
#include "Common/FileSystem.h"

#include <cstring>

struct WavHeader
{
	uint8_t riff_id[4] = { 'R', 'I', 'F', 'F' };
	uint32_t chunk_size;
	uint8_t wave_id[4] = { 'W', 'A', 'V', 'E' };
	/* "fmt" sub-chunk */
	uint8_t fmt_id[4] = { 'f', 'm', 't', ' ' }; // FMT header
	uint32_t subchunk1_size = 16; // Size of the fmt chunk
	uint16_t audio_format = 1; // Audio format 1=PCM
	uint16_t channels = 1; // Number of channels 1=Mono 2=Sterio
	uint32_t samples_per_sec = 16000; // Sampling Frequency in Hz
	uint32_t bytes_per_sec = 16000 * 2; // bytes per second
	uint16_t block_align = 2; // 2=16-bit mono, 4=16-bit stereo
	uint16_t bits_per_sample = 16; // Number of bits per sample
	/* "data" sub-chunk */
	uint8_t subchunk2_id[4] = { 'd', 'a', 't', 'a' }; // "data"  string
	uint32_t subchunk2_size; // Sampled data length
};

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
AudioFileCache::AudioFileCache() : m_maxSize(0), m_currentlyUsedSize(0)
{
}

Bool AudioFileCache::decodeFFmpeg(OpenAudioFile* file)
{
	auto on_frame = [](AVFrame* frame, int stream_idx, int stream_type, void* user_data) {
		OpenAudioFile* file = static_cast<OpenAudioFile*>(user_data);
		if (stream_type != AVMEDIA_TYPE_AUDIO) {
			return;
		}

		if (av_sample_fmt_is_planar(static_cast<AVSampleFormat>(frame->format))) {
			return;
		}

		const int frame_data_size = file->m_ffmpegFile->getSizeForSamples(frame->nb_samples);
		file->m_file = static_cast<uint8_t*>(av_realloc(file->m_file, file->m_fileSize + frame_data_size));
		memcpy(file->m_file + file->m_fileSize, frame->data[0], frame_data_size);
		file->m_fileSize += frame_data_size;
		file->m_totalSamples += frame->nb_samples;
		};

	file->m_ffmpegFile->setFrameCallback(on_frame);
	file->m_ffmpegFile->setUserData(file);

	// Read all packets inside the file
	while (file->m_ffmpegFile->decodePacket()) {
	}

	// Calculate the duration in MS
	file->m_duration = (file->m_totalSamples / (float)file->m_ffmpegFile->getSampleRate()) * 1000.0f;

	return true;
}

//-------------------------------------------------------------------------------------------------
AudioFileCache::~AudioFileCache()
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

void* AudioFileCache::openFile(AsciiString& filename)
{
	// Protect the entire openFile function
	std::lock_guard mut(m_mutex);

	auto it = m_openFiles.find(filename);

	if (it != m_openFiles.end()) {
		++it->second.m_openCount;
		return it->second.m_file;
	}

	// Couldn't find the file, so actually open it.
	File* file = TheFileSystem->openFile(filename.str());
	if (!file) {
		DEBUG_ASSERTLOG(strToFind.isEmpty(), ("Missing Audio File: '%s'\n", strToFind.str()));
		return NULL;
	}

	UnsignedInt fileSize = file->size();

	OpenAudioFile openedAudioFile;
	openedAudioFile.m_file = static_cast<uint8_t*>(av_malloc(sizeof(WavHeader)));
	openedAudioFile.m_fileSize = sizeof(WavHeader);
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

	fillWaveData(&openedAudioFile);
	openedAudioFile.m_ffmpegFile->close();

	openedAudioFile.m_fileSize = fileSize;
	m_currentlyUsedSize += openedAudioFile.m_fileSize;
	if (m_currentlyUsedSize > m_maxSize) {
		// We need to free some samples, or we're not going to be able to play this sound.
		if (!freeEnoughSpaceForSample(openedAudioFile)) {
			m_currentlyUsedSize -= openedAudioFile.m_fileSize;
			releaseOpenAudioFile(&openedAudioFile);
			return NULL;
		}
	}

	m_openFiles[filename] = openedAudioFile;
	return openedAudioFile.m_file;
}

//-------------------------------------------------------------------------------------------------
void* AudioFileCache::openFile(AudioEventRTS* eventToOpenFrom)
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
		return it->second.m_file;
	}

	// Couldn't find the file, so actually open it.
	File* file = TheFileSystem->openFile(strToFind.str());
	if (!file) {
		DEBUG_ASSERTLOG(strToFind.isEmpty(), ("Missing Audio File: '%s'\n", strToFind.str()));
		return NULL;
	}

	UnsignedInt fileSize = file->size();
	char* buffer = file->readEntireAndClose();

	OpenAudioFile openedAudioFile;
	openedAudioFile.m_file = static_cast<uint8_t*>(av_malloc(sizeof(WavHeader)));
	openedAudioFile.m_fileSize = sizeof(WavHeader);
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
			delete[] buffer;
			return NULL;
		}
	}

	if (!decodeFFmpeg(&openedAudioFile)) {
		releaseOpenAudioFile(&openedAudioFile);
		return nullptr;
	}

	fillWaveData(&openedAudioFile);
	openedAudioFile.m_ffmpegFile->close();

	openedAudioFile.m_fileSize = fileSize;
	m_currentlyUsedSize += openedAudioFile.m_fileSize;
	if (m_currentlyUsedSize > m_maxSize) {
		// We need to free some samples, or we're not going to be able to play this sound.
		if (!freeEnoughSpaceForSample(openedAudioFile)) {
			m_currentlyUsedSize -= openedAudioFile.m_fileSize;
			releaseOpenAudioFile(&openedAudioFile);
			return NULL;
		}
	}

	m_openFiles[strToFind] = openedAudioFile;
	return openedAudioFile.m_file;
}

//-------------------------------------------------------------------------------------------------
void AudioFileCache::closeFile(void* fileToClose)
{
	if (!fileToClose) {
		return;
	}

	// Protect the entire closeFile function
	std::lock_guard mut(m_mutex);

	OpenFilesHash::iterator it;
	for (it = m_openFiles.begin(); it != m_openFiles.end(); ++it) {
		if (it->second.m_file == fileToClose) {
			--it->second.m_openCount;
			return;
		}
	}
}

float AudioFileCache::getFileLength(void* file)
{
	if (file == nullptr) {
		return 0.0f;
	}

	std::lock_guard mut(m_mutex);

	for (auto it = m_openFiles.begin(); it != m_openFiles.end(); ++it) {
		if (it->second.m_file == file) {
			return it->second.m_duration;
		}
	}

	return 0.0f;
}

//-------------------------------------------------------------------------------------------------
void AudioFileCache::setMaxSize(UnsignedInt size)
{
	// Protect the function, in case we're trying to use this value elsewhere.
	std::lock_guard mut(m_mutex);

	m_maxSize = size;
}

//-------------------------------------------------------------------------------------------------
void AudioFileCache::releaseOpenAudioFile(OpenAudioFile* fileToRelease)
{
	if (fileToRelease->m_openCount > 0) {
		// This thing needs to be terminated IMMEDIATELY.
		TheAudio->closeAnySamplesUsingFile(fileToRelease->m_file);
	}

	if (fileToRelease->m_file) {
		if (fileToRelease->m_ffmpegFile) {
			// Free FFMPEG handles
			delete fileToRelease->m_ffmpegFile;
		}
		if (fileToRelease->m_file)
		{
			// Otherwise, we read it, we own it, blow it away.
			delete[] fileToRelease->m_file;
		}
		fileToRelease->m_ffmpegFile = NULL;
		fileToRelease->m_file = NULL;
		fileToRelease->m_eventInfo = NULL;
	}
}

//-------------------------------------------------------------------------------------------------
Bool AudioFileCache::freeEnoughSpaceForSample(const OpenAudioFile& sampleThatNeedsSpace)
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

void AudioFileCache::getWaveData(void* wave_data,
	uint8_t*& data,
	UnsignedInt& size,
	UnsignedInt& freq,
	UnsignedInt& channels,
	UnsignedInt& bitsPerSample)
{
	WavHeader* header = reinterpret_cast<WavHeader*>(wave_data);
	data = static_cast<uint8_t*>(wave_data) + sizeof(WavHeader);

	size = header->subchunk2_size;
	freq = header->samples_per_sec;
	channels = header->channels;
	bitsPerSample = header->bits_per_sample;
}

void AudioFileCache::fillWaveData(OpenAudioFile* open_audio)
{
	WavHeader wav;
	wav.chunk_size = open_audio->m_fileSize - (offsetof(WavHeader, chunk_size) + sizeof(uint32_t));
	wav.subchunk2_size = open_audio->m_fileSize - (offsetof(WavHeader, subchunk2_size) + sizeof(uint32_t));
	wav.channels = open_audio->m_ffmpegFile->getNumChannels();
	wav.bits_per_sample = open_audio->m_ffmpegFile->getBytesPerSample() * 8;
	wav.samples_per_sec = open_audio->m_ffmpegFile->getSampleRate();
	wav.bytes_per_sec = open_audio->m_ffmpegFile->getSampleRate() * open_audio->m_ffmpegFile->getNumChannels()
		* open_audio->m_ffmpegFile->getBytesPerSample();
	wav.block_align = open_audio->m_ffmpegFile->getNumChannels() * open_audio->m_ffmpegFile->getBytesPerSample();
	memcpy(open_audio->m_file, &wav, sizeof(WavHeader));
}