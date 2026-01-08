#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <limits.h>

typedef int16_t i16;
typedef uint16_t u16;
typedef uint32_t u32;

typedef float f32;

void write_u16_le(u16 value, FILE *file) {
  fputc(value & 0xFF, file); // LSB
  fputc((value >> 8) & 0xFF, file); // MSB

  // fwrite(&value, sizeof(u16), 1, file);
}

void write_i16_le(i16 value, FILE *file) {
  fputc(value & 0xFF, file); // LSB
  fputc((value >> 8) & 0xFF, file); // MSB

  // fwrite(&value, sizeof(i16), 1, file);
}

void write_u32_le(u32 value, FILE *file){
  fputc(value & 0xFF, file); // Byte 0
  fputc((value >> 8) & 0xFF, file); // Byte 1
  fputc((value >> 16) & 0xFF, file); // Byte 2
  fputc((value >> 24) & 0xFF, file); // Byte 3

  // fwrite(&value, sizeof(u32), 1, file);
}

#define WRITE_STR_LIT(str, file) fwrite((str), 1, sizeof(str) - 1, (file))

#define FREQ 44100
#define DURATION 3
#define NUM_CHANNELS 1
#define BITS_PER_SAMPLE 16

int main(void) {
  FILE *file = fopen("output.wav", "wb");
  
  u32 num_samples = FREQ * DURATION;
  u32 file_size = num_samples * sizeof(u16) + 44; // 44 bytes for WAV header

  // Write WAV header
  WRITE_STR_LIT("RIFF", file);
  write_u32_le(file_size - 8, file);
  WRITE_STR_LIT("WAVE", file);

  WRITE_STR_LIT("fmt ", file);
  write_u32_le(16, file); // Subchunk1Size for PCM
  write_u16_le(1, file); // AudioFormat PCM
  write_u16_le(NUM_CHANNELS, file); // NumChannels
  write_u32_le(FREQ, file); // SampleRate
  write_u32_le(FREQ * NUM_CHANNELS * BITS_PER_SAMPLE / 8, file); // ByteRate
  write_u16_le(NUM_CHANNELS * BITS_PER_SAMPLE / 8, file); // BlockAlign (NbrChannels * BitsPerSample / 8)
  write_u16_le(BITS_PER_SAMPLE, file); // BitsPerSample

  WRITE_STR_LIT("data", file);
  write_u32_le(num_samples * sizeof(u16), file); // Subchunk2Size

  // Write audio data
  for (u32 i = 0; i < num_samples; i++) {
    f32 t = (f32)i / FREQ;

    f32 sample = 0.25f * sinf(2.0f * 3.14159265f * 440.0f * t); // 440 Hz sine wave
    i16 int_sample = (i16)(sample * INT16_MAX); // Scale to int16 range

    write_i16_le(int_sample, file);
  }

  fclose(file);
  
  return 0;
}