/** @file main.c
 *  @brief A MIDI file parser.
 *
 *  This program parses the MIDI file supplied as an argument.
 *
 *  It was initially written to determine the reason why a
 *  number of MIDI files I had downloaded were not readable
 *  by several MIDI editors I tried using. The only program
 *  that would open the files was Windows Media Player.
 *
 *	The program is currently undergoing a rewrite to improve
 *	the inner workings and also to incorporate doxygen.
 *
 *  @author Darren Eckert
 *  @version 0.2
 *  @bug No known bugs currently.
 *  @todo Read track event data
 */

#ifndef MIDIINFO_H_
#include "MidiInfo.h"
#endif

// Main entrypoint
int main(int argc, char **argv)
{

	// Variables
	FILE *fMIDI = NULL;
	struct MidiHeader midiHead;
	struct TrackHeader trackHead;
	short val, fps, ticks;
	unsigned char *cBuffer;
	int i;

	// Usage check
	if (argc != 2)
	{
		printf("Usage: %s filename\n", argv[0]);
		return 0;
	}

	// Attempt to open file, exit with error if it fails
	fMIDI = fopen(argv[1], "rb");
	if (fMIDI == NULL)
	{
		printf("Unable to open file: %s\n", argv[1]);
		return 1;
	}

	// Attempt to read the MIDI file header chunk
	midiHead = readMidiChunk(fMIDI);

	if (strcmp(midiHead.cChunkType, MIDI_HEADER_ID) != 0)
	{
		printf("Incorrect file header id: %s\n", midiHead.cChunkType);
		fclose(fMIDI);
		return 1;
	}

	if (midiHead.uLength != MIDI_HEADER_CHUNK_SIZE)
	{
		printf("Incorrect chunk size: %d\n", midiHead.uLength);
		fclose(fMIDI);
		return 1;
	}

	if ((midiHead.uFormat == 0) && (midiHead.uNumTracks > 1))
	{
		printf("Incorrect number of tracks for a format 0 file: %d\n", midiHead.uNumTracks);
		fclose(fMIDI);
		return 1;
	}
	printf("Valid MIDI header chunk found\n");
	printf("MIDI format:   %d, ", midiHead.uFormat);
	printf("%d tracks found\n", midiHead.uNumTracks);

	// Determine time divsion format being used
	val = (midiHead.sTimeDiv & 0x8000);
	if (val != 0)
	{
		// Frames per second, extract negative SMPTE frame rate (mask 0x7f00) and tick per frame (mask 0x00ff)
		// FPS is in negative 2's compliment format,
		fps = midiHead.sTimeDiv >> 8;		  // Get the first 2 bytes for frame rate
		fps = ~fps + 1;						  // 1's compliment + 1 to get 2's compliment
		ticks = (midiHead.sTimeDiv & 0x00ff); // Get the last 2 bytes for tick per frame
		printf("Time division: (SMPTE format) %d frames per second, %d ticks per frame\n", fps, ticks);
	}
	else
	{
		printf("Time division: %d ticks per quarter note\n", midiHead.sTimeDiv);
	}

	midiHead.trackHeaders = malloc(sizeof(struct TrackHeader) * midiHead.uNumTracks);
	if (midiHead.trackHeaders == NULL)
	{
		printf("Error allocating memory for track headers\n");
		fclose(fMIDI);
		return 1;
	}

	for (i = 0; i < midiHead.uNumTracks; i++)
	{
		printf("Reading track %d - ", i);
		trackHead = readTrackChunk(fMIDI);
		midiHead.trackHeaders[i] = trackHead;
		if (strcmp(trackHead.cChunkType, MIDI_TRACK_ID) != 0)
		{
			printf("incorrect track header id: %s\n", trackHead.cChunkType);
			free(midiHead.trackHeaders);
			fclose(fMIDI);
			return 1;
		}
		/*
		printf("   Found track, event data is %d bytes long.\n", trackHead.uLength);
		readTrackEvents(fMIDI);
		printf("   End of track\n");
		/ */
		//*
		printf("TODO: read events , skip them for now\n");
		cBuffer = (unsigned char *)malloc(trackHead.uLength);
		if (cBuffer == NULL)
		{
			printf("Unable to allocate memory, exiting\n");
			free(midiHead.trackHeaders);
			fclose(fMIDI);
			exit(1);
		}
		fread(cBuffer, trackHead.uLength, 1, fMIDI);
		free(cBuffer);
		// */
	}
	// Everything is done, close the file and exit
	printf("All done, closing file and exiting.\n");
	fclose(fMIDI);
	return 0;
}