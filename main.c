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
 *  @version 0.1
 *  @bug No known bugs currently.
 */

// Includes
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "MidiInfo.h"

int main(int argc, char **argv)
{

	// Variables
	FILE *fMIDI = NULL;

	if (argc != 2)
	{
		printf("Usage: %s filename\n", argv[0]);
		return 0;
	}

	fMIDI = fopen(argv[1], "rb");
	if (fMIDI == NULL)
	{
		printf("Unable to open file: %s\n", argv[1]);
		return 1;
	}

	fclose(fMIDI);
	return 0;
}