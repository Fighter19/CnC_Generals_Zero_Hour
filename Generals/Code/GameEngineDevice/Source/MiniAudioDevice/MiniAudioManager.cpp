/*
**	Command & Conquer Generals(tm)
**	Copyright 2026 Stephan Vedder
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

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  (c) 2001-2003 Electronic Arts Inc.                                        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

// FILE: MiniAudioManager.cpp
/*---------------------------------------------------------------------------*/
/* Project:    RTS3                                                          */
/* File name:  MiniAudioManager.cpp                                         */
/* Created:    Stephan Vedder, June 2026                                     */
/* Desc:       Implementation for the MiniAudioManager, interfacing with    */
/*             the Miniaudio Sound System.                                   */
/*---------------------------------------------------------------------------*/

#include "Lib/BaseType.h"
#define MINIAUDIO_IMPLEMENTATION
#include "MiniAudioDevice/MiniAudioManager.h"

#include "Common/AudioAffect.h"
#include "Common/AudioHandleSpecialValues.h"
#include "Common/AudioRequest.h"
#include "Common/AudioSettings.h"
#include "Common/AsciiString.h"
#include "Common/AudioEventInfo.h"
#include "Common/FileSystem.h"
#include "Common/GameCommon.h"
#include "Common/GameSounds.h"
#include "Common/CRCDebug.h"
#include "Common/GlobalData.h"

#include "GameClient/DebugDisplay.h"
#include "GameClient/Drawable.h"
#include "GameClient/GameClient.h"
#include "GameClient/VideoPlayer.h"
#include "GameClient/View.h"

#include "GameLogic/GameLogic.h"
#include "GameLogic/TerrainLogic.h"

#include "Common/File.h"

// Callback functions for miniaudio's virtual file system
static ma_result vfsFileOpen(ma_vfs *pVFS, const char *filename, ma_uint32 mode, ma_vfs_file *pFile);
static ma_result vfsFileInfo(ma_vfs *pVFS, ma_vfs_file file, ma_file_info *pInfo);
static ma_result vfsFileRead(ma_vfs *pVFS, ma_vfs_file file, void *pBuffer, size_t bytesToRead, size_t *pBytesRead);

//-------------------------------------------------------------------------------------------------
MiniAudioManager::MiniAudioManager() :
	m_playbackDeviceCount(0),
	m_selectedPlaybackDevice(PROVIDER_ERROR),
	m_selectedSpeakerType(0),
	m_lastSelectedPlaybackDevice(PROVIDER_ERROR),
	m_binkHandle(NULL),
	m_pref3DProvider(AsciiString::TheEmptyString),
	m_prefSpeaker(AsciiString::TheEmptyString)
{
}

//-------------------------------------------------------------------------------------------------
MiniAudioManager::~MiniAudioManager()
{
	DEBUG_ASSERTCRASH(m_binkHandle == NULL, ("Leaked a Bink handle. Chuybregts"));
	releaseHandleForBink();
	closeDevice();
	
	DEBUG_ASSERTCRASH(this == TheAudio, ("Umm...\n"));
	TheAudio = NULL;
}

#if defined(_DEBUG) || defined(_INTERNAL)
//-------------------------------------------------------------------------------------------------
AudioHandle MiniAudioManager::addAudioEvent( const AudioEventRTS *eventToAdd )
{
	return AudioManager::addAudioEvent(eventToAdd);
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::audioDebugDisplay( DebugDisplayInterface *dd, void *, FILE *fp )
{
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::dumpAllAssetsUsed()
{
}
#endif

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::init()
{
	AudioManager::init();
#ifdef INTENSE_DEBUG
	DEBUG_LOG(("Sound has temporarily been disabled in debug builds only. jkmcd\n"));
	// for now, _DEBUG builds only should have no sound. ask jkmcd or srj about this.
	return;
#endif

	// We should now know how many samples we want to load
	openDevice();
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::postProcessLoad()
{
	AudioManager::postProcessLoad();
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::reset()
{
#if defined(_DEBUG) || defined(_INTERNAL)
	dumpAllAssetsUsed();
	m_allEventsLoaded.clear();
#endif

	AudioManager::reset();
	stopAllAudioImmediately();
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::update()
{
	AudioManager::update();
	setDeviceListenerPosition();
	processRequestList();
	processPlayingList();
	processFadingList();
	processStoppedList();	
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::stopAudio( AudioAffect which )
{
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::pauseAudio( AudioAffect which )
{
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::resumeAudio( AudioAffect which )
{
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::pauseAmbient( Bool shouldPause )
{
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::stopAllAmbientsBy( Object *obj )
{
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::stopAllAmbientsBy( Drawable *draw )
{
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::playAudioEvent( AudioEventRTS *event )
{
#ifdef INTENSIVE_AUDIO_DEBUG
	DEBUG_LOG(("MILES (%d) - Processing play request: %d (%s)", TheGameLogic->getFrame(), event->getPlayingHandle(), event->getEventName().str()));
#endif
	const AudioEventInfo *info = event->getAudioEventInfo();
	if (!info) {
		return;
	}

	std::list<PlayingAudio *>::iterator it;
	PlayingAudio *playing = NULL;

	AudioHandle handleToKill = event->getHandleToKill();

	AsciiString fileToPlay = event->getFilename();
	PlayingAudio *audio = allocatePlayingAudio();

	Bool foundSoundToReplace = false;
	if (handleToKill) {
		for (it = m_playingSounds.begin(); it != m_playingSounds.end(); ++it) {
			playing = (*it);
			if (!playing) {
				continue;
			}

			if (playing->m_audioEventRTS && playing->m_audioEventRTS->getPlayingHandle() == handleToKill) 
			{
				//Release this streaming channel immediately because we are going to play another sound in it's place.
				releasePlayingAudio(playing);
				m_playingSounds.erase(it);
				foundSoundToReplace = true;
				break;
			}
		}
	}

	ma_sound_group *groupToUse = NULL;
	switch(info->m_soundType)
	{
		case AT_Music:
			groupToUse = &m_musicGroup;
			break;
		case AT_Streaming:
			groupToUse = &m_speechGroup;
			break;
		case AT_SoundEffect:
			if (event->isPositionalAudio()) {
				groupToUse = &m_sound3DGroup;
			} else {
				groupToUse = &m_soundGroup;
			}
			break;
	}

	ma_sound* sound = NULL;
	if (!handleToKill || foundSoundToReplace) {
		sound = (ma_sound*)malloc(sizeof(ma_sound));
		ma_result result = ma_sound_init_from_file(&m_engine, fileToPlay.str(), 0, groupToUse, NULL, sound);
		if (result != MA_SUCCESS) {
			DEBUG_LOG(("Failed to initialize sound from file: %s", fileToPlay.str()));
			releasePlayingAudio(audio);
			return;
		}
	} else {
		DEBUG_LOG(("Skipping sound!"));
		releasePlayingAudio(audio);
		return;
	}

	switch(info->m_soundType)
	{
		case AT_Music:
		case AT_Streaming:
		{
		#ifdef INTENSIVE_AUDIO_DEBUG
			DEBUG_LOG(("- Stream\n"));
		#endif
			
			if ((info->m_soundType == AT_Streaming) && event->getUninterruptable()) {
				stopAllSpeech();
			}

			// Put this on here, so that the audio event RTS will be cleaned up regardless.
			audio->m_audioEventRTS = event; 
			audio->m_sound = sound;
			audio->m_type = PAT_Stream;

			if ((info->m_soundType == AT_Streaming) && event->getUninterruptable()) {
				setDisallowSpeech(TRUE);
			}
			break;
		}

		case AT_SoundEffect:
		{
		#ifdef INTENSIVE_AUDIO_DEBUG
			DEBUG_LOG(("- Sound"));
		#endif


			if (event->isPositionalAudio()) {
				// Sounds that are non-global are positional 3-D sounds. Deal with them accordingly
			#ifdef INTENSIVE_AUDIO_DEBUG
				DEBUG_LOG((" Positional"));
			#endif
				// Push it onto the list of playing things
				audio->m_audioEventRTS = event; 
				audio->m_sound = sound;
				audio->m_type = PAT_3DSample;

				const Coord3D* pos = event->getCurrentPosition();
				ma_sound_set_position(sound, pos->x, pos->y, pos->z);
				m_sound->notifyOf3DSampleStart();
			} 
			else 
			{
				// Push it onto the list of playing things
				audio->m_audioEventRTS = event; 
				audio->m_sound = sound;
				audio->m_type = PAT_Sample;
				m_sound->notifyOf2DSampleStart();
				#ifdef INTENSIVE_AUDIO_DEBUG
					DEBUG_LOG((" Playing.\n"));
				#endif
			}
			break;
		}
	}

	ma_sound_set_volume(sound, event->getVolume());
	ma_sound_start(sound);
	m_playingSounds.push_back(audio);
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::stopAudioEvent( AudioHandle handle )
{
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::killAudioEventImmediately( AudioHandle audioEvent )
{
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::pauseAudioEvent( AudioHandle handle )
{
}

//-------------------------------------------------------------------------------------------------
void *MiniAudioManager::loadFileForRead( AudioEventRTS *eventToLoadFrom )
{
	return NULL;
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::closeFile( void *fileRead )
{
}

//-------------------------------------------------------------------------------------------------
PlayingAudio *MiniAudioManager::allocatePlayingAudio( void )
{
	PlayingAudio *aud = NEW PlayingAudio;	// poolify
	return aud;
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::releaseMilesHandles( PlayingAudio *release )
{
	if (release->m_sound) {
		ma_sound_uninit(release->m_sound);
		free(release->m_sound);
	}
	release->m_type = PAT_INVALID;
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::releasePlayingAudio( PlayingAudio *release )
{
	if (release->m_audioEventRTS->getAudioEventInfo()->m_soundType == AT_SoundEffect && release->m_sound) {
		if (release->m_type == PAT_Sample) {
			m_sound->notifyOf2DSampleCompletion();
		} else {
			m_sound->notifyOf3DSampleCompletion();
		}
	}
	releaseMilesHandles(release);	// forces stop of this audio
	if (release->m_cleanupAudioEventRTS) {
		releaseAudioEventRTS(release->m_audioEventRTS);
	}
	delete release;
	release = NULL;
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::stopAllAudioImmediately( void )
{
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::freeAllMilesHandles( void )
{
}

//-------------------------------------------------------------------------------------------------
PlayingAudio *MiniAudioManager::getFirst2DSample( AudioEventRTS *event )
{
	return NULL;
}

//-------------------------------------------------------------------------------------------------
PlayingAudio *MiniAudioManager::getFirst3DSample( AudioEventRTS *event )
{
	return NULL;
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::adjustPlayingVolume( PlayingAudio *audio )
{
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::stopAllSpeech( void )
{
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::nextMusicTrack( void )
{
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::prevMusicTrack( void )
{
}

//-------------------------------------------------------------------------------------------------
Bool MiniAudioManager::isMusicPlaying( void ) const
{
	return FALSE;
}

//-------------------------------------------------------------------------------------------------
Bool MiniAudioManager::hasMusicTrackCompleted( const AsciiString& trackName, Int numberOfTimes ) const
{
	return FALSE;
}

//-------------------------------------------------------------------------------------------------
AsciiString MiniAudioManager::getMusicTrackName( void ) const
{
	return AsciiString::TheEmptyString;
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::openDevice( void )
{
	if (!TheGlobalData->m_audioOn) {
		return;
	}

	ma_result result;
    ma_resource_manager_config resourceManagerConfig;
	ma_engine_config engineConfig;
	static ma_vfs_callbacks vfs = {};
	vfs.onOpen = vfsFileOpen;
	vfs.onInfo = vfsFileInfo;
	vfs.onRead = vfsFileRead;

	// Use a custom resource manager, so we can load from our virtual file system.
	resourceManagerConfig = ma_resource_manager_config_init();
	resourceManagerConfig.pVFS = &vfs;
	result = ma_resource_manager_init(&resourceManagerConfig, &m_resourceManager);
	if (result != MA_SUCCESS) {
		DEBUG_LOG(("Failed to initialize the Miniaudio resource manager. Error code: %d\n", result));
		// if we couldn't initialize any devices, turn sound off (fail silently)
		setOn( false, AudioAffect_All );
		return;  // Failed to initialize the resource manager.
	}

	/* We're going to want a context so we can enumerate our playback devices. */
    result = ma_context_init(NULL, 0, NULL, &m_context);
    if (result != MA_SUCCESS) {
        DEBUG_LOG(("Failed to initialize context."));
		setOn( false, AudioAffect_All );
        return;
    }

    /*
    Now that we have a context we will want to enumerate over each device so we can display them to the user and give
    them a chance to select the output devices they want to use.
    */
    result = ma_context_get_devices(&m_context, &m_playbackDevices, &m_playbackDeviceCount, NULL, NULL);
    if (result != MA_SUCCESS) {
        DEBUG_LOG(("Failed to enumerate playback devices."));
        ma_context_uninit(&m_context);
		setOn( false, AudioAffect_All );
        return;
    }

	engineConfig = ma_engine_config_init();
	engineConfig.pResourceManager = &m_resourceManager;

	result = ma_engine_init(&engineConfig, &m_engine);
	if (result != MA_SUCCESS) {
		DEBUG_LOG(("Failed to initialize the Miniaudio engine. Error code: %d\n", result));
		// if we couldn't initialize any devices, turn sound off (fail silently)
		setOn( false, AudioAffect_All );
		return;  // Failed to initialize the engine.
	}
	
	ma_sound_group_init(&m_engine, 0, NULL, &m_musicGroup);
	ma_sound_group_init(&m_engine, 0, NULL, &m_soundGroup);
	ma_sound_group_init(&m_engine, 0, NULL, &m_sound3DGroup);
	ma_sound_group_init(&m_engine, 0, NULL, &m_speechGroup);
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::closeDevice( void )
{
	ma_engine_uninit(&m_engine);
}

//-------------------------------------------------------------------------------------------------
Bool MiniAudioManager::isCurrentlyPlaying( AudioHandle handle )
{
	std::list<PlayingAudio *>::iterator it;
	PlayingAudio *playing;

	for (it = m_playingSounds.begin(); it != m_playingSounds.end(); ++it) {
		playing = *it;
		if (playing && playing->m_audioEventRTS->getPlayingHandle() == handle) {
			return true;
		}
	}

	// if something is requested, it is also considered playing
	std::list<AudioRequest *>::iterator ait;
	AudioRequest *req = NULL;
	for (ait = m_audioRequests.begin(); ait != m_audioRequests.end(); ++ait) {
		req = *ait;
		if (req && req->m_usePendingEvent && req->m_pendingEvent->getPlayingHandle() == handle) {
			return true;
		}
	}

	return false;
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::notifyOfAudioCompletion( UnsignedInt audioCompleted, UnsignedInt flags )
{
}

//-------------------------------------------------------------------------------------------------
PlayingAudio *MiniAudioManager::findPlayingAudioFrom( UnsignedInt audioCompleted, UnsignedInt flags )
{
	return NULL;
}

//-------------------------------------------------------------------------------------------------
UnsignedInt MiniAudioManager::getProviderCount( void ) const
{
	return m_playbackDeviceCount;
}

//-------------------------------------------------------------------------------------------------
AsciiString MiniAudioManager::getProviderName( UnsignedInt providerNum ) const
{
	return AsciiString::TheEmptyString;
}

//-------------------------------------------------------------------------------------------------
UnsignedInt MiniAudioManager::getProviderIndex( AsciiString providerName ) const
{
	return PROVIDER_ERROR;
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::selectProvider( UnsignedInt providerNdx )
{
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::unselectProvider( void )
{
}

//-------------------------------------------------------------------------------------------------
UnsignedInt MiniAudioManager::getSelectedProvider( void ) const
{
	return m_selectedPlaybackDevice;
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::setSpeakerType( UnsignedInt speakerType )
{
}

//-------------------------------------------------------------------------------------------------
UnsignedInt MiniAudioManager::getSpeakerType( void )
{
	return 0;
}

//-------------------------------------------------------------------------------------------------
UnsignedInt MiniAudioManager::getNum2DSamples( void ) const
{
	return 0;
}

//-------------------------------------------------------------------------------------------------
UnsignedInt MiniAudioManager::getNum3DSamples( void ) const
{
	return 0;
}

//-------------------------------------------------------------------------------------------------
UnsignedInt MiniAudioManager::getNumStreams( void ) const
{
	return 0;
}

//-------------------------------------------------------------------------------------------------
Bool MiniAudioManager::doesViolateLimit( AudioEventRTS *event ) const
{
	return FALSE;
}

//-------------------------------------------------------------------------------------------------
Bool MiniAudioManager::isPlayingAlready( AudioEventRTS *event ) const
{
	return FALSE;
}

//-------------------------------------------------------------------------------------------------
Bool MiniAudioManager::isObjectPlayingVoice( UnsignedInt objID ) const
{
	return FALSE;
}

//-------------------------------------------------------------------------------------------------
AudioEventRTS *MiniAudioManager::findLowestPrioritySound( AudioEventRTS *event )
{
	return NULL;
}

//-------------------------------------------------------------------------------------------------
Bool MiniAudioManager::isPlayingLowerPriority( AudioEventRTS *event ) const
{
	return FALSE;
}

//-------------------------------------------------------------------------------------------------
Bool MiniAudioManager::killLowestPrioritySoundImmediately( AudioEventRTS *event )
{
	return FALSE;
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::adjustVolumeOfPlayingAudio( AsciiString eventName, Real newVolume )
{
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::removePlayingAudio( AsciiString eventName )
{
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::removeAllDisabledAudio()
{
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::processRequestList( void )
{
	std::list<AudioRequest*>::iterator it;
	for (it = m_audioRequests.begin(); it != m_audioRequests.end(); /* empty */) {
		AudioRequest *req = (*it);
		if (req == NULL) {
			continue;
		}

		if (!shouldProcessRequestThisFrame(req)) {
			adjustRequest(req);
			++it;
			continue;
		}

		if (!req->m_requiresCheckForSample || checkForSample(req)) {
			processRequest(req);
		}
		req->deleteInstance();
		it = m_audioRequests.erase(it);
	}
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::processPlayingList( void )
{
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::processFadingList( void )
{
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::processStoppedList( void )
{
}

//-------------------------------------------------------------------------------------------------
Bool MiniAudioManager::shouldProcessRequestThisFrame( AudioRequest *req ) const
{
	if (!req->m_usePendingEvent) {
		return true;
	}

	if (req->m_pendingEvent->getDelay() < MSEC_PER_LOGICFRAME_REAL) {
		return true;
	}

	return false;
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::adjustRequest( AudioRequest *req )
{
	if (!req->m_usePendingEvent) {
		return;
	}

	req->m_pendingEvent->decrementDelay(MSEC_PER_LOGICFRAME_REAL);
	req->m_requiresCheckForSample = true;
}

//-------------------------------------------------------------------------------------------------
Bool MiniAudioManager::checkForSample( AudioRequest *req )
{
	if (!req->m_usePendingEvent) {
		return true;
	}

	if (req->m_pendingEvent->getAudioEventInfo()->m_type != AT_SoundEffect) {
		return true;
	}

	return m_sound->canPlayNow(req->m_pendingEvent);
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::setHardwareAccelerated( Bool accel )
{
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::setSpeakerSurround( Bool surround )
{
}

//-------------------------------------------------------------------------------------------------
Real MiniAudioManager::getFileLengthMS( AsciiString strToLoad ) const
{
	return 0.0f;
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::closeAnySamplesUsingFile( const void *fileToClose )
{
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::setDeviceListenerPosition( void )
{
	ma_engine_listener_set_direction(&m_engine, 0, m_listenerOrientation.x, m_listenerOrientation.y, m_listenerOrientation.z);
	ma_engine_listener_set_position(&m_engine, 0, m_listenerPosition.x, m_listenerPosition.y, m_listenerPosition.z);
}

//-------------------------------------------------------------------------------------------------
const Coord3D *MiniAudioManager::getCurrentPositionFromEvent( AudioEventRTS *event )
{
	return NULL;
}

//-------------------------------------------------------------------------------------------------
Bool MiniAudioManager::isOnScreen( const Coord3D *pos ) const
{
	return FALSE;
}

//-------------------------------------------------------------------------------------------------
Real MiniAudioManager::getEffectiveVolume( AudioEventRTS *event ) const
{
	return 0.0f;
}

//-------------------------------------------------------------------------------------------------
Bool MiniAudioManager::startNextLoop( PlayingAudio *looping )
{
	return FALSE;
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::buildProviderList( void )
{
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::createListener( void )
{
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::initDelayFilter( void )
{
}

//-------------------------------------------------------------------------------------------------
Bool MiniAudioManager::isValidProvider( void )
{
	return FALSE;
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::initSamplePools( void )
{
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::processRequest( AudioRequest *req )
{
	switch (req->m_request)
	{
		case AR_Play:
		{
			playAudioEvent(req->m_pendingEvent);
			break;
		}
		case AR_Pause:
		{
			pauseAudioEvent(req->m_handleToInteractOn);
			break;
		}
		case AR_Stop:
		{
			stopAudioEvent(req->m_handleToInteractOn);
			break;
		}
	}
}

//-------------------------------------------------------------------------------------------------
void *MiniAudioManager::getHandleForBink( void )
{
	return NULL;
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::releaseHandleForBink( void )
{
}

//-------------------------------------------------------------------------------------------------
void MiniAudioManager::friend_forcePlayAudioEventRTS( const AudioEventRTS *eventToPlay )
{
	playAudioEvent(const_cast<AudioEventRTS *>(eventToPlay));
}


static ma_result vfsFileOpen(ma_vfs *pVFS, const char *filename, ma_uint32 mode, ma_vfs_file *pFile)
{
	Int access = 0;
	if (mode & MA_OPEN_MODE_READ) {
		access |= File::READ;
	}
	if (mode & MA_OPEN_MODE_WRITE) {
		access |= File::WRITE;
	}

	File *file = TheFileSystem->openFile(filename, access);
	if (!file)
	{
		return MA_DOES_NOT_EXIST;
	}

	*pFile = file;
	return MA_SUCCESS;
}

static ma_result vfsFileInfo(ma_vfs *pVFS, ma_vfs_file file, ma_file_info *pInfo)
{
	File *f = (File *)file;
	if (!f) {
		return MA_INVALID_FILE;
	}

	pInfo->sizeInBytes = f->size();
	return MA_SUCCESS;
}

static ma_result vfsFileRead(ma_vfs *pVFS, ma_vfs_file file, void *pBuffer, size_t bytesToRead, size_t *pBytesRead)
{
	File *f = (File *)file;
	if (!f) {
		return MA_INVALID_FILE;
	}

	size_t bytesActuallyRead = f->read(pBuffer, bytesToRead);
	if (pBytesRead) {
		*pBytesRead = bytesActuallyRead;
	}

	return MA_SUCCESS;
}