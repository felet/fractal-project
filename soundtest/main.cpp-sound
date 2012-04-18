#include <time.h>
#include <unistd.h>

#include "AudioPlayer.hpp"

int main()
{

    AudioPlayer music(20);
    music.loadWave("sound.wav");
    music.printHeader();
    music.play();

    while (music.isPlaying())
    {
        // Sleep ca 1 frame
        usleep(20000);

        music.printElapsedSec();

        music.doFFT();
        music.getFrequencyBandBetween(0, music.getNumberFrequencies());
    }

    music.freeWave();
    return 0;
}

