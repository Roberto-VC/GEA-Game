#pragma once
#include <SDL.h>
#include <string>
#include <iostream>
#include "Scene/Engine/Systems.h"


enum class SoundType {
  BACKGROUND,
  EFFECT,
  DIALOGUE
};

struct AudioComponent {
  std::string filePath;
  int volume; // Volume level (0-128 for SDL audio)
  bool isPlaying; // Indicates if audio is currently playing
  bool triggerPlay; // Indicates if the audio should be played next update
};

class AudioSetupSystem : public SetupSystem {
public:
  void run() override {
    Entity* jumpsound= scene->createEntity("Jump");
    const std::string& wavfile = "../Assets/Sounds/jump.wav";
    jumpsound ->addComponent<AudioComponent>(wavfile, 100, false, false);
  }
};

class AudioSystem : public UpdateSystem {
public:
  AudioSystem() {
    // Initialize SDL audio
    SDL_AudioSpec desiredSpec;
    SDL_zero(desiredSpec);
    desiredSpec.freq = 44100; // Sample rate
    desiredSpec.format = AUDIO_S16; // Format
    desiredSpec.channels = 2; // Stereo
    desiredSpec.samples = 4096; // Buffer size

    // Open the audio device
    audioDevice = SDL_OpenAudioDevice(NULL, 0, &desiredSpec, NULL, 0);
    if (audioDevice == 0) {
      SDL_Log("Failed to open audio device: %s", SDL_GetError());
    }
  }

  ~AudioSystem() {
    if (audioDevice) {
      SDL_CloseAudioDevice(audioDevice);
    }
  }

  void run(float dt) override {
    auto view = scene->r.view<AudioComponent>();
    for (auto e : view) {
      auto& audio = view.get<AudioComponent>(e);

      if (audio.triggerPlay) {
        SDL_Log("Playing audio: %s", audio.filePath.c_str());
        playAudio(audio);
        audio.triggerPlay = false; // Reset the trigger after queuing
      }
    }
  }

private:
  void playAudio(AudioComponent& audio) {
    SDL_AudioSpec wavSpec;
    Uint32 wavLength;
    Uint8* wavBuffer;

    if (SDL_LoadWAV(audio.filePath.c_str(), &wavSpec, &wavBuffer, &wavLength) == NULL) {
      SDL_Log("Error loading WAV: %s", SDL_GetError());
      return;
    }

    // Queue the audio data
    if (SDL_QueueAudio(audioDevice, wavBuffer, wavLength) < 0) {
      SDL_Log("Failed to queue audio: %s", SDL_GetError());
    }

    // Start playback (only if not already playing)
    SDL_PauseAudioDevice(audioDevice, 0);

    // Free the WAV buffer after queuing
    SDL_FreeWAV(wavBuffer);
  }

  SDL_AudioDeviceID audioDevice = 0; // Store the audio device ID
};