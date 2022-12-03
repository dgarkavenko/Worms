#include "JukeBox.h"
#include "WormsLib/TinyPixelNoise.h"
#include "WormsLib/WormsAudioHelp.h"

double JukeBox::AddToSequence(Note notes[], unsigned count, int BPM, const FTime& Time)
{
	double play_time = DBL_MAX;

	for (unsigned i = queue_front; i != queue_back; RING_BUFFER_ADVANCE(i, queue_mask)) {
		if (!queue[i].mixed && queue[i].Freq > 0) {
			play_time = std::min(queue[i].Time, play_time);
		}
	}

	if(play_time == DBL_MAX)
		play_time = Time.ElapsedTime;

	return PlayTune(notes, count, BPM, play_time);
}

double JukeBox::PlayTune(Note notes[], unsigned count, int BPM, double play_time)
{	
	const double beat_length = 60.0 / (double)BPM;
	const double bar_length = 4 * beat_length;

	for (size_t i = 0; i < count; i++)
	{
		double duration = bar_length / notes[i].duration;
		Playback& playback = queue[queue_back];
		playback.Freq = c_major_freq[notes[i].notion];
		playback.Duration = (float)duration;
		playback.Time = play_time;
		playback.mixed = false;
		play_time += duration;
		RING_BUFFER_ADVANCE(queue_back, queue_mask);
	}

	return play_time;
}

void JukeBox::Update(FWormsAudioHelp &AudioHelp, const FTime Time)
{
	for (unsigned i = queue_front; i != queue_back; RING_BUFFER_ADVANCE(i, queue_mask)) {
		if (!queue[i].mixed && Time.ElapsedTime > queue[i].Time)
		{
			AudioHelp.PlayTone(queue[i].Freq, queue[i].Duration);
			queue[i].mixed = true;
		}
	}

	int new_queue_back = queue_front;
	
	for (unsigned i = queue_front; i != queue_back; RING_BUFFER_ADVANCE(i, queue_mask)) {

		if (queue[i].mixed)
			continue;

		queue[new_queue_back] = queue[i];
		RING_BUFFER_ADVANCE(new_queue_back, queue_mask);
	}

	queue_back = new_queue_back;
}

void JukeBox::FoodConsumedTone(const FTime& Time, int value)
{
	if (Time.ElapsedTime < _consume_sound_timeout)
		return;

	Tone tones[] = { Tone_G2, Tone_A2, Tone_B2, Tone_E2, Tone_D2, Tone_C2, Tone_F2 };

	Note tune[] = {
		{tones[RandomInt(0, 5 + value)], (float)4 * RandomInt(1, 3)}		
	};

	double playtime = AddToSequence(tune, 1, 140, Time) - Time.ElapsedTime;
	_consume_sound_timeout = Time.ElapsedTime + playtime / RandomInt(2, 4);
}

void JukeBox::LoopBass(const FTime& Time)
{
	if (_bass_loop_timeout > Time.ElapsedTime)
		return;

	Note bass[] = {
		{Tone_C4, 32},
		{Tone_0, 16},
		{Tone_G3, 32},
		{Tone_0, 8},
		{Tone_0, 32},
		{Tone_C4, 32},
		{Tone_0, 4},

		{Tone_G3, 32},
		{Tone_0, 8},
		{Tone_0, 32},
		{Tone_A3, 32},
		{Tone_0, 16},
		{Tone_E3, 32},
		{Tone_0, 8},
		{Tone_0, 32},
		{Tone_A3, 32},
		{Tone_0, 4},

		{Tone_E3, 32},
		{Tone_0, 8},
		{Tone_0, 32},
		{Tone_E3, 32},
		{Tone_0, 16},
		{Tone_B3, 32},
		{Tone_0, 8},
		{Tone_0, 32},
		{Tone_E3, 32},
		{Tone_0, 4},

		{Tone_B3, 32},
		{Tone_0, 8},
		{Tone_0, 32},
		{Tone_G3, 32},
		{Tone_0, 16},
		{Tone_D4, 32},
		{Tone_0, 8},
		{Tone_0, 32},
		{Tone_G3, 32},
		{Tone_0, 4},

		{Tone_D4, 32},
		{Tone_0, 8},
		{Tone_0, 32},
	};

	_bass_loop_timeout = PlayTune(bass, sizeof(bass) / sizeof(Note), 100, Time.ElapsedTime);
}

void JukeBox::LoopHighs(const FTime& Time)
{
	if (_highs_loop_timeout > Time.ElapsedTime)
		return;

	Note highs[] = {
		{Tone_E4, 32},
		{Tone_0, 16},
		{Tone_C5, 32},
		{Tone_B4, 32},
		{Tone_A4, 32},
		{Tone_F4, 32},
		{Tone_0, 8},
		{Tone_0, 32},
		{Tone_G4, 32},
		{Tone_F4, 32},
		{Tone_G4, 32},
		{Tone_0, 32},
		{Tone_0, 16},
		{Tone_E4, 32},
		{Tone_0, 16},
		{Tone_B4, 32},
		{Tone_0, 16}
	};

	_highs_loop_timeout = PlayTune(highs, sizeof(highs) / sizeof(Note), 100, Time.ElapsedTime);

}

void JukeBox::DamageTone(const FTime& Time, int value)
{
	if(value < 2)
		return;

	const int max_strength = 4;

	Tone tones[] = { Tone_E4, Tone_F4, Tone_G4, Tone_A4, Tone_B4, Tone_C5, Tone_D5, Tone_E5, Tone_F5 };
	int strength = std::min(max_strength, 1 + (int) (value / 10));

	Note tune[3]{
		{ tones[RandomInt(2 + strength, 4 + strength)], 8.0f },
		{ tones[RandomInt(strength, 2 + strength)], 8.0f},
		{ tones[RandomInt(0, 2)], 8.0f} };
	
	AddToSequence(tune, 3, 100, Time);
}
