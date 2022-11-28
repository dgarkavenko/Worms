#include "JukeBox.h"

#include "WormsLib/TinyPixelNoise.h"
#include "WormsLib/WormsAudioHelp.h"

void JukeBox::PlayTune(Note notes[], unsigned count, int BPM)
{	
	const double beat_length = 60.0 / (double)BPM;
	const double bar_length = 4 * beat_length;
	double play_time = Time->ElapsedTime;

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
}

void JukeBox::Update(FWormsAudioHelp &AudioHelp)
{
	for (unsigned i = queue_front; i != queue_back; RING_BUFFER_ADVANCE(i, queue_mask)) {
		if (!queue[i].mixed && Time->ElapsedTime > queue[i].Time)
		{
			AudioHelp.PlayTone(queue[i].Freq, queue[i].Duration);
			queue[i].mixed = true;
		}
	}

	for (unsigned i = queue_front; i != queue_back; RING_BUFFER_ADVANCE(i, queue_mask)) {

		if (!queue[i].mixed)
			break;

		RING_BUFFER_ADVANCE(queue_front, queue_mask);
	}
}
