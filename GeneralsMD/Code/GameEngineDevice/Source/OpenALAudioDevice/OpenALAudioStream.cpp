#include "OpenALAudioDevice/OpenALAudioStream.h"
#include "OpenALAudioDevice/OpenALAudioManager.h"

OpenALAudioStream::OpenALAudioStream()
{ 
    alGenSources(1, &m_source);
    alGenBuffers(AL_STREAM_BUFFER_COUNT, m_buffers);

    // Make stream ignore positioning
    alSource3i(m_source, AL_POSITION, 0, 0, 0);
    alSourcei(m_source, AL_SOURCE_RELATIVE, AL_TRUE);
    alSourcei(m_source, AL_ROLLOFF_FACTOR, 0);

    DEBUG_LOG(("OpenALAudioStream created: %i\n", m_source));
}

OpenALAudioStream::~OpenALAudioStream()
{
    DEBUG_LOG(("OpenALAudioStream freed: %i\n", m_source));
    // Unbind the buffers first
    alSourceStop(m_source);
    alSourcei(m_source, AL_BUFFER, 0);
    alDeleteSources(1, &m_source);
    // Now delete the buffers
    alDeleteBuffers(AL_STREAM_BUFFER_COUNT, m_buffers);
}

bool OpenALAudioStream::bufferData(uint8_t *data, size_t data_size, ALenum format, int samplerate)
{
    DEBUG_LOG(("Buffering %zu bytes of data (samplerate: %i, format: %i)\n", data_size, samplerate, format));
    ALint num_queued;
    alGetSourcei(m_source, AL_BUFFERS_QUEUED, &num_queued);
    if (num_queued >= AL_STREAM_BUFFER_COUNT) {
        DEBUG_LOG(("Having too many buffers already queued: %i", num_queued));
        return false;
    }

    ALuint &current_buffer = m_buffers[m_current_buffer_idx];
    alBufferData(current_buffer, format, data, data_size, samplerate);
    alSourceQueueBuffers(m_source, 1, &current_buffer);
    m_current_buffer_idx++;

    if (m_current_buffer_idx >= AL_STREAM_BUFFER_COUNT)
        m_current_buffer_idx = 0;

    return true;
}

void OpenALAudioStream::update()
{
    ALint sourceState, looping, sourceType, sampleOffset;
    float pitch;
    alGetSourcei(m_source, AL_SOURCE_STATE, &sourceState);
    switch(sourceState) {
        case AL_PLAYING:
            DEBUG_LOG(("Source is playing\n"));
            break;
        case AL_PAUSED:
            DEBUG_LOG(("Source is paused\n"));
            break;
        case AL_STOPPED:
            DEBUG_LOG(("Source is stopped\n"));
            break;
        default:
            DEBUG_LOG(("Unknown source state: %i\n", sourceState));
            break;
    }
    alGetSourcei(m_source, AL_LOOPING, &looping);
    DEBUG_LOG(("Source is looping: %i\n", looping));

    alGetSourcei(m_source, AL_SOURCE_TYPE, &sourceType);
    DEBUG_LOG(("Source type: %i\n", sourceType));

    alGetSourcei(m_source, AL_SAMPLE_OFFSET, &sampleOffset);
    DEBUG_LOG(("Sample offset: %i\n", sampleOffset));

    alGetSourcef(m_source, AL_PITCH, &pitch);
    DEBUG_LOG(("Pitch: %f\n", pitch));
       
    ALint processed;
    alGetSourcei(m_source, AL_BUFFERS_PROCESSED, &processed);
    DEBUG_LOG(("%i buffers have been processed\n", processed));
    while (processed > 0) {
        ALuint buffer;
        alSourceUnqueueBuffers(m_source, 1, &buffer);
        processed--;
    }

    ALint num_queued;
    alGetSourcei(m_source, AL_BUFFERS_QUEUED, &num_queued);
    DEBUG_LOG(("Having %i buffers queued\n", num_queued));
    if (num_queued < AL_STREAM_BUFFER_COUNT && m_requireDataCallback) {
        // Ask for more data to be buffered
        m_requireDataCallback();
    }
}

void OpenALAudioStream::reset()
{
    DEBUG_LOG(("Resetting stream\n"));
    alSourceRewind(m_source);
    alSourcei(m_source, AL_BUFFER, 0);
    m_current_buffer_idx = 0;
}

bool OpenALAudioStream::isPlaying()
{
    ALint state;
    alGetSourcei(m_source, AL_SOURCE_STATE, &state);
    return state == AL_PLAYING;
}