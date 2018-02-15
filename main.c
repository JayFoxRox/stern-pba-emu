// (C) 2018 Jannik Vogel
// Licensed under GPLv3, see license.txt for more information

#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <SDL.h>

int main(int argc, char* argv[]) {
  int ret;
  
  // Check arguments
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <dll-path> <rom-path>\n", argv[0]);
    return 1;
  }
  
  // Load the DLL
  SetLastError(0);
  printf("Last error 0x%08X\n", GetLastError());
  char* path = argv[1];
  printf("Loading '%s'\n", path);
  SetLastError(0);
  HMODULE dll = LoadLibrary(path);
  if (dll == NULL) {
    printf("Failed to load DLL\n");
    return 1;
  }
  printf("dll handle is %p\n", dll);
  printf("Last error 0x%08X\n", GetLastError());

  // Load the ROM
  FILE* f = fopen(argv[2], "rb");
  if (f == NULL) {
    //FIXME!
    return 1;
  }
  fseek(f, 0, SEEK_END);
  size_t rom_size = ftell(f);
  uint8_t* rom = malloc(rom_size);
  fseek(f, 0, SEEK_SET);
  fread(rom, rom_size, 1, f);
  fclose(f);
  
  // Startup SDL
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  
  // Create window
  int w = 128;
  int h = 32;
  int s = 4; // Size per pixel
  SDL_Window* window = SDL_CreateWindow("Stern Spike DMD", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w * s, h * s, 0);
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
  SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, w, h);
  Uint32* pixels = malloc(w * h * sizeof(Uint32));
  memset(pixels, 0x00, w * h * sizeof(Uint32));
  
  // Create audio device and start playback
  SDL_AudioDeviceID dev;
  SDL_AudioSpec want;
  memset(&want, 0x00, sizeof(want));
  want.freq = 24000;
  want.format = AUDIO_S16LSB;
  want.channels = 2;
  want.samples = 4096;
  want.callback = NULL;
  dev = SDL_OpenAudioDevice(NULL, 0, &want, NULL, 0);
  assert(dev > 0);
  SDL_PauseAudioDevice(dev, 0);

  
  // Resolve some of the exports
  int(*stern_set_rom_data_fn)(int, int) = GetProcAddress(dll, "stern_set_rom_data_fn");
  int(*stern_set_persistant_data_fn)(int, int) = GetProcAddress(dll, "stern_set_persistant_data_fn");
  int(*stern_init_fn)(void) = GetProcAddress(dll, "stern_init_fn");
  int(*stern_reset_fn)(int) = GetProcAddress(dll, "stern_reset_fn");
  void*(*stern_get_dmd_bbuffer_fn)(void) = GetProcAddress(dll, "stern_get_dmd_bbuffer_fn");
  void*(*stern_get_dmd_fbuffer_fn)(void) = GetProcAddress(dll, "stern_get_dmd_fbuffer_fn");
  int(*stern_step_fn)(void) = GetProcAddress(dll, "stern_step_fn");
  void*(*stern_get_sound_buffer)(int*) = GetProcAddress(dll, "stern_get_sound_buffer");
  void(*stern_set_switch_state_fn)(int, int, int) = GetProcAddress(dll, "stern_set_switch_state_fn");
  void(*stern_set_dedicated_switch_state_fn)(int, int, int) = GetProcAddress(dll, "stern_set_dedicated_switch_state_fn");
  int(*stern_term_fn)(void) = GetProcAddress(dll, "stern_term_fn");
  

  // Setup ROM data
  stern_set_rom_data_fn(rom, rom_size);
    
#if 0
  //FIXME: Don't care
  stern_set_persistant_data_fn(persistant_data, persistant_data_size);
#endif

  // Init VM, not sure if this has to be before ROM data setup - all I know is: this works
  ret = stern_init_fn();
  printf("stern_init_fn returned %p\n", ret);

  // No idea what this does
  ret = stern_reset_fn(1); // Some boolean parameter
  printf("stern_reset_fn returned %p\n", ret);

  // Presumably backbuffer
  uint8_t* bbuffer = stern_get_dmd_bbuffer_fn();
  printf("stern_get_dmd_bbuffer_fn returned %p\n", bbuffer);

  // Presumably frontbuffer
  uint8_t* fbuffer = stern_get_dmd_fbuffer_fn();
  printf("stern_get_dmd_fbuffer_fn returned %p\n", fbuffer);  
  
  // Start mainloop  
  bool quit = false;
  while(!quit) {

    // Do SDL events
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        quit = true;
        break;
      }
    }
    
    // Update the DMD
    for(int y = 0; y < h; y++) {
      for(int x = 0; x < w; x++) {
        uint8_t* buffer = fbuffer;
        uint8_t brightness = fbuffer[(y * 128 + x)];
        if (brightness > 0xF) {
          brightness = 0x0;
        }
        pixels[y * w + x] = brightness * 0x111111;
      }
    }
    SDL_UpdateTexture(texture, NULL, pixels, w * sizeof(Uint32));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    
    // Get audio buffer and feed SDL, not sure about exact timing, it seems to give 792 samples, which is somewhere in the range of 44100-48000 HZ at 60 FPS
    if (SDL_GetQueuedAudioSize(dev) < 4096*2*4) {
      int audio_samples = 0;
      void* audio = stern_get_sound_buffer(&audio_samples);
      //printf("audio at %p, %d samples\n", audio, audio_samples);
      int ret = SDL_QueueAudio(dev, audio, audio_samples * 2);
      assert(ret == 0);
    }

    // Get SDL input
    uint8_t* keys = SDL_GetKeyboardState(NULL);

    // Actual switch ID from manual, calculated as: a1 + a2 * 16, set as boolean
    #define SWITCH(n) ((n) % 16), ((n) / 16)
    
    // Put balls in the trough
    stern_set_switch_state_fn(SWITCH(15), 1);
    stern_set_switch_state_fn(SWITCH(16), 1);
    stern_set_switch_state_fn(SWITCH(17), 1);
    stern_set_switch_state_fn(SWITCH(18), 1);
    stern_set_switch_state_fn(SWITCH(19), 1);
    stern_set_switch_state_fn(SWITCH(20), 1);
    
    // Start button
    stern_set_switch_state_fn(SWITCH(78), keys[SDL_SCANCODE_RETURN]);
    
    // Flippers
    stern_set_switch_state_fn(SWITCH(9), keys[SDL_SCANCODE_LCTRL]);
    stern_set_switch_state_fn(SWITCH(10), keys[SDL_SCANCODE_RCTRL]);
    
    // Coin door switches (sets bitmask)
    stern_set_dedicated_switch_state_fn(8, 0, keys[SDL_SCANCODE_F5] ? 1 : 0);
    stern_set_dedicated_switch_state_fn(9, 0, keys[SDL_SCANCODE_F6] ? 1 : 0);
    stern_set_dedicated_switch_state_fn(10, 0, keys[SDL_SCANCODE_F7] ? 1 : 0);
    stern_set_dedicated_switch_state_fn(11, 0, keys[SDL_SCANCODE_F8] ? 1 : 0);
    
    // Now update the virtual machine
    //printf("going to step!\n");
    stern_step_fn();
    
  }

  // Terminate VM
  ret = stern_term_fn();
  printf("stern_term_fn returned %p\n", ret);

  // Free SDL stuff
  //FIXME: free audio
  free(pixels);
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}