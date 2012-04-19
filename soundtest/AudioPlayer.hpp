#ifndef AUDIOPLAYER_HPP
#define AUDIOPLAYER_HPP

#include <stdint.h> // For uint16_t and uint32_t

#include <AL/al.h>
#include <AL/alc.h>

#include <fftw3.h>

struct headerDataType
{
    unsigned char riffHeader[4];
    uint32_t fileSize;
    unsigned char waveHeader[4];
    unsigned char formatHeader[4];
    uint32_t formatHeaderSize;
    uint16_t formatCode;
    uint16_t channelNumber;
    uint32_t sampleRate;
    uint32_t bytesPerSecond;
    uint16_t bytesPerSample;
    uint16_t bitsPerSample;
    uint32_t subChunkId;
    uint32_t dataBlockSize;
};

class AudioPlayer
{
    public:
        AudioPlayer(const int);
        ~AudioPlayer();
        int loadWave(const char *);
        void freeWave();
        void play();
        int isPlaying();
        void doFFT();
        int getNumberFrequencies();
        float getFrequencyBandBetween(const unsigned int,
                const unsigned int);
        void printElapsedSec();
        void printHeader();

    private:
        headerDataType headerData;
        int16_t *samplings;
        bool isLoaded;

        ALCdevice *device;
        ALCcontext *context;
        ALuint source;
        ALuint buffer;

        int N;
        double *fftIn;
        fftw_complex *fftOut;
        fftw_plan fftPlan;
};

#endif

