/** @file MidiInfo.h
 *  @brief Constants, Structures and Functions for parsing MIDI format files
 *
 *  This contains the constants, data structures and functions
 *  needed to parse files in the MIDI file format
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

#ifndef MIDIINFO_H_
#define MIDIINFO_H_

// Data Structures
/** @struct MidiHeader
 *  @brief MIDI File Header structure
 *
 * Chunk Type is 5 bytes, "MThd" + NULL terminator '\0',\n
 * Length must be 6.\n
 * Format type can be 0, 1 or 2.\n
 * Number of tracks:
 * - Format 0 can have one track only.
 * - Format 1 can have multiple simultaneous tracks.
 * - Format 2 can have multiple sequential tracks.\n
 * Time division can be is one of two formats:
 * - Ticks per quarter note.
 * - Negative SMPTE format, e.g. -24 = 24fps, -30 = 30fps, plus number of ticks per frame.\n
 */
struct MidiHeader
{
	char cChunkType[5];
	unsigned int uLength;
	unsigned short uFormat, uNumTracks;
	short sTimeDiv;

	struct TrackHeader *trackHeaders;
};

/** @struct TrackHeader
 * @brief Track Header Structure
 *
 * Header ID is 5 bytes, "MTrk" + NULL terminator '\0'.\n
 * the NULL terminator is needed for strcmp() and printf()\n
 * Track chunk size is the total number of bytes in the track\n
 */
struct TrackHeader
{
	char cChunkType[5];
	unsigned int uLength;
};

// Function Prototypes
int intPow(int base, int exp);
uint16_t swapUInt16(uint16_t val);
int16_t swapInt16(int16_t val);
int16_t swapInt16(int16_t val);
uint32_t swapUInt32(uint32_t val);
int32_t swapInt32(int32_t val);
unsigned long readVarLen(FILE *f);

#endif