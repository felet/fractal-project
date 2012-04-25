#include <iostream>
#include <fstream>
#include <math.h> // For sqrt()
#include "AudioPlayer.hpp"

AudioPlayer::AudioPlayer(const int FFTwindowSize)
{
    // Create a audio context
    device = alcOpenDevice(NULL);
    context = alcCreateContext(device, NULL);
    alcMakeContextCurrent(context);

    // Create a audio listener
    alListener3f(AL_POSITION, 0, 0, 0);
    alListener3f(AL_VELOCITY, 0, 0, 0);
    alListener3f(AL_ORIENTATION, 0, 0, -1);

    // Create a audio source
    alGenSources(1, &source);

    // Setup properties of audio source
    alSourcef(source, AL_PITCH, 1);
    alSourcef(source, AL_GAIN, 1);
    alSource3f(source, AL_POSITION, 0, 0, 0);
    alSource3f(source, AL_VELOCITY, 0, 0, 0);
    alSourcei(source, AL_LOOPING, AL_FALSE);

    // Create a audio buffer
    alGenBuffers(1, &buffer);

    // No WAVE file is loaded
    isLoaded = false;

    // Setup FFTW
    N = FFTwindowSize;
    fftIn = new double[N];
    fftOut = new fftw_complex[ getNumberFrequencies() ];
    fftPlan = fftw_plan_dft_r2c_1d(N, fftIn, fftOut, FFTW_ESTIMATE);
}

AudioPlayer::~AudioPlayer()
{
    // Cleanup FFTW
    fftw_destroy_plan(fftPlan);
    delete[] fftIn;
    delete[] fftOut;

    // Cleanup OpenAL
    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);
    alcDestroyContext(context);
    alcCloseDevice(device);
}


int AudioPlayer::loadWave(const char *waveFile)
{
    #define HEADER_SIZE 44

    std::ifstream::pos_type fileSize;
    std::ifstream file(waveFile, std::ios::in|std::ios::binary|std::ios::ate);


    // Open sound file
    if (!file.is_open())
    {
        std::cout << "ERROR: Unable to open sound file!" << std::endl;
        return 1;
    }

    fileSize = file.tellg();

    // Set pointer to beginning
    file.seekg(0, std::ios::beg);

    // Read header data
    // - http://www.lightlink.com/tjweber/StripWav/Canon.html
    // - https://ccrma.stanford.edu/courses/422/projects/WaveFormat/
    if (fileSize < HEADER_SIZE)
    {
        std::cout << "ERROR: The sound file is too short!" << std::endl;
        file.close();
        return 1;
    }
    file.read((char *)&headerData, sizeof(headerDataType));

    if (headerData.bitsPerSample != 16)
    {
        std::cout << "ERROR: bitsPerSample must be 16!" << std::endl;
        file.close();
        return 1;
    }

    // Read all the sampled data from the file
    // The index of the array is Divided by 2 since:
    // sizeof(char) == sizeof(int16_t) / 2
    samplings = new int16_t[ headerData.dataBlockSize/2 ];
    file.read ((char *)samplings, headerData.dataBlockSize);
    // Closing file stream
    file.close();
    std::cout << "Sound file is loaded in memory" << std::endl;

    // Load sampling data to buffer
    alBufferData(buffer, AL_FORMAT_STEREO16, (unsigned char *)samplings, headerData.dataBlockSize, headerData.sampleRate);

    // Assign buffers to source
    alSourcei(source, AL_BUFFER, buffer);

    float duration = float(headerData.dataBlockSize) / float(headerData.bytesPerSecond);
    std::cout << "Sound length: " << duration << std::endl;

    isLoaded = true;

    return 0;
}

void AudioPlayer::freeWave()
{
    // Free memory
    delete[] samplings;
}

void AudioPlayer::play()
{
    if (isLoaded)
    {
        // Play source
        alSourcePlay(source);
        std::cout << "Playing..." << std::endl;
    }
    else
    {
        std::cout << "ERROR: No audio file is loaded!" << std::endl;
    }
}

int AudioPlayer::isPlaying()
{
    // Check if we are still playing sound
    int currentStatus;
    alGetSourcei(source, AL_SOURCE_STATE, &currentStatus);

    if (currentStatus == AL_PLAYING)
        return 1;
    return 0;
}

void AudioPlayer::doFFT()
{
    // Load position in buffer
    int elapsedByte;
    alGetSourcei(source, AL_BYTE_OFFSET, &elapsedByte);

    for (int i = 0, iSample = elapsedByte/2-N/2; i < N; i++, iSample += 2)
    {
        if (iSample >= 0 || (unsigned int)iSample < headerData.dataBlockSize)
            fftIn[i] = (double)samplings[ iSample ];
        else
            fftIn[i] = 0.0;
    }

    fftw_execute(fftPlan);
}

int AudioPlayer::getNumberFrequencies()
{
    return N/2+1;
}

float AudioPlayer::getFrequencyBandBetween(const unsigned int start,
        const unsigned int end)
{
    double sum = 0;
    if (start > end || (int)end > N)
    {
        std::cout << "ERROR: START or/and END value is invalid."
            << std::endl;
        return 0.0;
    }

    for (unsigned int i = start; i <= end; i++)
    {
        sum += sqrt(fftOut[i][0]*fftOut[i][0] + fftOut[i][1]*fftOut[i][1]);
    }

    return sum / (end - start + 1);
}

void AudioPlayer::printElapsedSec()
{
        // Load position in buffer
        int elapsedByte;
        alGetSourcei(source, AL_BYTE_OFFSET, &elapsedByte);

        // Print position in seconds
        std::cout << "Time elapsed: " << float(elapsedByte) / float(headerData.bytesPerSecond) << std::endl;
}

void AudioPlayer::printHeader()
{
    // Print header data
    std::cout << "riffHeader:       "  << headerData.riffHeader[0]
            << headerData.riffHeader[1] << headerData.riffHeader[2]
            << headerData.riffHeader[3] << std::endl
        << "fileSize:         " << headerData.fileSize << std::endl
        << "waveHeader:       " << headerData.waveHeader[0]
            << headerData.waveHeader[1] << headerData.waveHeader[2]
            << headerData.waveHeader[3] << std::endl
        << "formatHeader:     " << headerData.formatHeader[0]
            << headerData.formatHeader[1] << headerData.formatHeader[2]
            << headerData.formatHeader[3]<< std::endl
        << "formatHeaderSize: " << headerData.formatHeaderSize << std::endl
        << "formatCode:       " << headerData.formatCode << std::endl
        << "channelNumber:    " << headerData.channelNumber << std::endl
        << "sampleRate:       " << headerData.sampleRate << std::endl
        << "bytesPerSecond:   " << headerData.bytesPerSecond << std::endl
        << "bytesPerSample:   " << headerData.bytesPerSample << std::endl
        << "bitsPerSample:    " << headerData.bitsPerSample << std::endl
        << "subChunkId:       " << headerData.subChunkId << std::endl
        << "dataBlockSize:    " << headerData.dataBlockSize << std::endl;
}

