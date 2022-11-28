#pragma once

#define PLAYBKAC_SIZE (1 << 6)

#define RING_BUFFER_ADVANCE(var, mask) \
	(var) = ((var) + 1) & (mask)

struct FWormsAudioHelp;
struct FTime;

enum Tone : unsigned
{
	Tone_0,
	Tone_C2,
	Tone_D2,
	Tone_E2,
	Tone_F2,
	Tone_G2,
	Tone_A2,
	Tone_B2,
	Tone_C3,
	Tone_D3,
	Tone_E3,
	Tone_F3,
	Tone_G3,
	Tone_A3,
	Tone_B3,
	Tone_C4,
	Tone_D4,
	Tone_E4,
	Tone_F4,
	Tone_G4,
	Tone_A4,
	Tone_B4,
	Tone_Count
};

struct Note
{
	Tone notion;
	float duration = 4;
};

struct Playback
{
	float Freq;
	float Duration;
	double Time;
	bool mixed = false;
};




class JukeBox
{
	const FTime* Time;
	static const unsigned queue_mask = (PLAYBKAC_SIZE - 1);
	unsigned queue_front; /*read index*/
	unsigned queue_back; /*write index*/
	Playback queue[PLAYBKAC_SIZE]{};

	float c_major_freq[Tone_Count] = {
		0.0f,
		65.41f,
		73.42f,
		82.41f,
		87.31f,
		98.00f,
		110.00f,
		123.47f,
		130.81f,
		146.83f,
		164.81f,
		174.61f,
		196.00f,
		220.00f,
		246.94f,
		261.63f,
		293.66f,
		329.63f,
		349.23f,
		392.00f,
		440.00f,
		493.88f,
	};


public:
	explicit JukeBox(const FTime *time) :
		Time(time),
		queue_front(0),
		queue_back(0)
	{}

	void PlayTune(Note notes[], unsigned count, int BPM);
	void Update(FWormsAudioHelp &AudioHelp);

};

