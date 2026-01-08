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
}

void write_i16_le(i16 value, FILE *file) {
  fputc(value & 0xFF, file); // LSB
  fputc((value >> 8) & 0xFF, file); // MSB
}

void write_u32_le(u32 value, FILE *file){
  fputc(value & 0xFF, file); // Byte 0
  fputc((value >> 8) & 0xFF, file); // Byte 1
  fputc((value >> 16) & 0xFF, file); // Byte 2
  fputc((value >> 24) & 0xFF, file); // Byte 3
}

typedef struct {
  f32 freq;
  f32 duration;
} Note;


#define WRITE_STR_LIT(str, file) fwrite((str), 1, sizeof(str) - 1, (file))

#define FREQ 44100
#define NUM_CHANNELS 1
#define BITS_PER_SAMPLE 16

int main(void) {
  const Note happy_birthday[] = {
    {392.00f, 0.5f}, {392.00f, 0.5f}, {440.00f, 1.0f}, {392.00f, 1.0f}, {523.25f, 1.0f}, {493.88f, 2.0f},
    {392.00f, 0.5f}, {392.00f, 0.5f}, {440.00f, 1.0f}, {392.00f, 1.0f}, {587.33f, 1.0f}, {523.25f, 2.0f},
    {392.00f, 0.5f}, {392.00f, 0.5f}, {783.99f, 1.0f}, {659.25f, 1.0f}, {523.25f, 1.0f}, {493.88f, 1.0f}, {440.00f, 2.0f},
    {698.46f, 0.5f}, {698.46f, 0.5f}, {659.25f, 1.0f}, {523.25f, 1.0f}, {587.33f, 1.0f}, {523.25f, 2.0f}
  };

  size_t song_len = sizeof(happy_birthday)/sizeof(happy_birthday[0]);

  FILE *file = fopen("output.wav", "wb");
  
  u32 total_samples = 0;
  for (u32 i = 0; i < song_len; i++) {
    total_samples += (u32)(happy_birthday[i].duration * FREQ);
  }

  u32 data_size = total_samples * sizeof(u16);
  u32 file_size = data_size + 44; // 44 bytes for WAV header

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
  write_u32_le(data_size, file); // Subchunk2Size
  
  // Write audio data
  f32 phase = 0.0f;

  for (u32 i = 0; i < song_len; i++) {
    Note note = happy_birthday[i];
    u32 samples = (u32)(note.duration * FREQ);
    f32 phase_increment = 2.0f * 3.14159265f * note.freq / FREQ;

    for (u32 j = 0; j < samples; j++) {
      f32 sample = 0.25f * sinf(phase);
      i16 int_sample = (i16)(sample * INT16_MAX); // Scale to int16 range

      write_i16_le(int_sample, file);
      phase += phase_increment;
    }

  }

  fclose(file);
  
  return 0;
}