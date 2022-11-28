// Copyright Embark Studios AB

// Do not modify this file! See ASSIGNMENT.md.

#pragma once

#include "TinyPixelNoise.h"

#include <list>

// Helper class for playing simple tones. Use of this helper is optional.
struct FWormsAudioHelp
{
	// Start playing a tone with specified frequency (Hertz) and duration (seconds)
	void PlayTone(float Frequency, float Duration);

	// Call Mix every frame to mix in the playing sounds
	// Mix adds to the outbut buffer, so don't forget to clear the buffer at the beginning of a
	// frame with FWormsAudioHelp::Silence(...)
	void Mix(const FAudio& Audio);

	// Call each frame to silence all audio
	static void Silence(const FAudio& Audio);

private:
	struct FSound
	{
		float Frequency;
		float Duration;
		float ElapsedDuration;
	};
	
	std::list<FSound> ActiveSounds;
};
