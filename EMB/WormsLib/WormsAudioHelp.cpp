// Copyright Embark Studios AB

// Do not modify this file! See ASSIGNMENT.md.

#include "WormsAudioHelp.h"
#include <cstring>

void FWormsAudioHelp::PlayTone(float Frequency, float Duration)
{
	ActiveSounds.emplace_back(FSound{Frequency, Duration});
}

void FWormsAudioHelp::Mix(const FAudio& Audio)
{
	const float Step = 1.0f/Audio.SampleRate;
	for (auto Sound = ActiveSounds.begin(); Sound != ActiveSounds.end(); )
	{
		for (int i = 0; i < Audio.SamplesToWrite; ++i)
		{
			auto V = 0.2f * sinf(Sound->Frequency * TAU * Sound->ElapsedDuration) * (1.0f - Sound->ElapsedDuration / Sound->Duration);
			Audio.LChannel[i] += V;
			Audio.RChannel[i] += V;
			Sound->ElapsedDuration += Step;
			if (Sound->ElapsedDuration > Sound->Duration)
			{
				break;
			}
		}

		if (Sound->ElapsedDuration > Sound->Duration)
		{
			Sound = ActiveSounds.erase(Sound);
		}
		else
		{
			++Sound;
		}
	}
}

void FWormsAudioHelp::Silence(const FAudio& Audio)
{
	memset(Audio.LChannel, 0, Audio.SamplesToWrite * sizeof(Audio.LChannel[0]));
	memset(Audio.RChannel, 0, Audio.SamplesToWrite * sizeof(Audio.RChannel[0]));
}
