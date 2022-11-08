#pragma once

#define PLAYBKAC_SIZE (1 << 6)

#define RING_BUFFER_ADVANCE(var, mask) \
	(var) = ((var) + 1) & (mask)

struct FTime;

enum Notion : unsigned
{
	Notion_C3,
	Notion_D3,
	Notion_E3,
	Notion_F3,
	Notion_G3,
	Notion_A3,
	Notion_B3,
	Notion_C4,
	Notion_D4,
	Notion_E4,
	Notion_F4,
	Notion_G4,
	Notion_A4,
	Notion_B4,
	Notion_Count
};

struct Note
{
	Notion notion;
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
	static const unsigned queue_mask = (PLAYBKAC_SIZE - 1);
	unsigned queue_front; /*read index*/
	unsigned queue_back; /*write index*/
	Playback queue[PLAYBKAC_SIZE]{};

	float c_major_freq[Notion_Count] = {
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
	void PlayTune(Note notes[], unsigned count, int BPM);
	void Update(const FTime& time);

};

