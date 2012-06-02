/* -*- mode: C; c-basic-offset: 4; intent-tabs-mode: nil -*-
 *
 * Sifteo Thundercracker simulator
 * M. Elizabeth Scott <beth@sifteo.com>
 *
 * Copyright <c> 2012 Sifteo, Inc. All rights reserved.
 */

#include "wavefile.h"


WaveWriter::WaveWriter() : file(0), sampleCount(0) {}

void WaveWriter::write32(uint32_t word)
{
    fwrite(&word, sizeof word, 1, file);
}

bool WaveWriter::open(const char *filename, unsigned sampleRate)
{
    close();

    file = fopen(filename, "wb");
    if (!file)
        return false;

    write32(0x46464952);        // 'RIFF'
    write32(0xFFFFFFFF);        // Chunk size (placeholder)
    write32(0x45564157);        // 'WAVE'
    write32(0x20746d66);        // 'fmt'
    write32(16);                // Subchunk size
    write32(0x00010001);        // Mono / PCM
    write32(sampleRate);        // Samples per second
    write32(sampleRate * 2);    // Bytes per second
    write32(0x00100002);        // Bits per sample / Frame size
    write32(0x61746164);        // 'data'
    write32(0xFFFFFFFF);        // Subchunk size (placeholder)

    return true;
}

void WaveWriter::close()
{
    if (!file)
        return;

    // Patch RIFF chunk size
    fseek(file, 4, SEEK_SET);
    write32(36 + sampleCount * 2);

    // Patch subchunk size
    fseek(file, 40, SEEK_SET);
    write32(sampleCount * 2);

    fclose(file);
    file = 0;
}

void WaveWriter::write(const int16_t *samples, unsigned count)
{
    sampleCount += count;
    if (file)
        fwrite(samples, sizeof *samples, count, file);
}
