/** @file MidiInfo.c
 *  @brief Constants, Structures and Functions for parsing MIDI format files
 *
 *  This contains the constants, data structures and functions
 *  needed to parse files in the MIDI file format
 *
 *  @author Darren Eckert
 *  @version 0.1
 *  @bug No known bugs currently.
 */

#include "MidiInfo.h"

#ifndef MIDI_TYPES_H
#define MIDI_TYES_H
#define MIDI_INSTRUMENTS 128

// Standard MIDI instrument names
char *instrTable[MIDI_INSTRUMENTS] = {
    "Acoustic Grand Piano", "Bright Acoustic Piano", "Electric Grand Piano", "Honky Tonk Piano", "Electric Piano 1",
    "Electric Piano 2", "Harpsichord", "Clavinet", "Celesta", "Glockenspiel", "Music Box", "Vibrahone", "Marimba",
    "Xylophone", "Tubular Bells", "Dulcimer", "Drawbar Organ", "Percussive Organ", "Rock Organ", "Church Organ",
    "Reed Organ", "Accordion", "Harmonica", "Tango Accordion", "Acoustic Guitar", "Acoustic Steel Guitar",
    "Electric Jazz Guitar", "Electric Guitar", "Electric Muted Guitar", "Overdriven Guitar", "Distortion Guitar",
    "Guitar Harmonics", "Acoustic Bass", "Electric Fingered Bass", "Electric Picked Bass", "Fretless Bass",
    "Slap Bass 1", "Slap Bass 2", "Synth Bass 1", "Synth Bass 2", "Violin", "Viola", "Cello", "Contrabass",
    "Tremolo Strings", "Pizzicato Strings", "Orchestral Harp", "Timpani", "String Ensamble 1", "String Ensamble 2",
    "Synth Strings 1", "Synth Strings 2", "Choir Aahs", "Vocal Oohs", "Synth Voice", "Orchestral Hit", "Trumpet",
    "Trombone", "Tuba", "Muted Trumpet", "French Horn", "Brass Section", "Synth Brass 1", "Synth Brass 2", "Soprano Sax",
    "Alto Sax", "Tenor Sax", "Baritone Sax", "Oboe", "English Horn", "Bassoon", "Clarinet", "Piccolo", "Flute",
    "Recorder", "Pan Flute", "Blown Bottle", "Shakuhachi", "Whistle", "Ocarina", "Lead 1", "Lead 2", "Lead 3", "Lead 4",
    "Lead 5", "Lead 6", "Lead 7", "Lead 8", "Pad 1", "Pad 2", "Pad 3", "Pad 4", "Pad 5", "Pad 6", "Pad 7", "Pad 8",
    "FX 1", "FX 2", "FX 3", "FX 4", "FX 5", "FX 6", "FX 7", "FX 8", "Sitar", "Banjo", "Shamisen", "Koto", "Kalimba",
    "Bagpipes", "Fiddle", "Shanai", "Tinkle Bell", "Agogo", "Steel Drums", "Woodblock", "Taiko Drum", "Melodic Tom",
    "Synth Drum", "Reverse Cymbal", "Guitar Fret Noise", "Breath Noise", "Seashore", "Bird Tweet", "Telephone Ring",
    "Helicopter", "Applause", "Gunshot"};
#endif

/// @brief MIDI Files must start with "MThd"
#ifndef MIDI_HEADER_ID
#define MIDI_HEADER_ID "MThd"
#endif

/// @brief MIDI headers must be 6 bytes in size
#ifndef MIDI_HEADER_CHUNK_SIZE
#define MIDI_HEADER_CHUNK_SIZE 6
#endif

/// @brief MIDI tracks must start with "MTrk"
#ifndef MIDI_TRACK_ID
#define MIDI_TRACK_ID "MTrk"
#endif

/// @brief Microseconds Per Minute
#ifndef MS_PER_MIN
#define MS_PER_MIN 60000000
#endif

/** @fn int intPow(int base, int exp)
 * @brief Integer Power of function
 *
 * This function computes the power of in integer\n
 * NOTE: Only works for positive exponents
 *
 * Code taken from https://stackoverflow.com/a/29787467
 *
 * @param base: The base of the function
 * @param exp:  The exponent of the function
 * @return Integer: base^exp
 */
int intPow(int base, int exp)
{
    int result = 1;
    while (exp)
    {
        if (exp & 1)
            result *= base;
        exp /= 2;
        base *= base;
    }
    return result;
}

// Endian swap functions.
/** @fn uint16_t swapUInt16(uint16_t val)
 * @brief Byte swap unsigned short
 *
 * Swaps bytes in an unsigned 16bit integer
 *
 * Code taken from https://stackoverflow.com/a/2637138
 *
 * @param val: unsigned 16bit integer containing 2 bytes to be swapped
 * @return val: swapped bytes in an unsigned 16bit integer
 */
uint16_t swapUInt16(uint16_t val)
{
    return (val << 8) | (val >> 8);
}

/** @fn int16_t swapInt16(int16_t val)
 *  @brief Byte swap short
 *
 * Swaps bytes in a signed 16bit integer
 *
 * Code taken from https://stackoverflow.com/a/2637138
 *
 * @param val: signed 16bit integer containing 2 bytes to be swapped
 * @return val: swapped bytes in a signed 16bit integer
 */
int16_t swapInt16(int16_t val)
{
    return (val << 8) | ((val >> 8) & 0xFF);
}

/** @fn uint32_t swapUInt32(uint32_t val)
 *  @brief Byte swap unsigned int
 *
 * Swaps bytes in an unsigned 32bit integer
 *
 * Code taken from https://stackoverflow.com/a/2637138
 *
 * @param val: unsigned 32bit integer containing 4 bytes to be swapped
 * @return val: swapped bytes in an unsigned 32bit integer
 */
uint32_t swapUInt32(uint32_t val)
{
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
    return (val << 16) | (val >> 16);
}

/** @fn int32_t swapInt32(int32_t val)
 @brief Byte swap int
 *
 * Swaps bytes in a signed 326bit integer
 *
 * Code taken from https://stackoverflow.com/a/2637138
 *
 * @param val: signed 32bit integer containing 4 bytes to be swapped
 * @return val: swapped bytes in a signed 32bit integer
 */
int32_t swapInt32(int32_t val)
{
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
    return (val << 16) | ((val >> 16) & 0xFFFF);
}

/** @fn unsigned long readVarLen(FILE *f)
 *  @brief Delta times can be stored in Variable-Length Quantity format
 *
 * Some numbers in MIDI Files are represented in a form called VARIABLE-LENGTH QUANTITY.\n
 * These numbers are represented 7 bits per byte, most significant bits first.\n
 * All bytes except the last have bit 7 set, and the last byte has bit 7 clear.\n
 * If the number is between 0 and 127, it is thus represented exactly as one byte.
 * Examples:\n
 * Number (hex)	| Representation (hex)
 * -----------: | :-------------------
 * 00000000		| 00
 * 00000040		| 40
 * 0000007F		| 7F
 * 00000080		| 81 00
 * 00002000		| C0 00
 * 00003FFF		| FF 7F
 * 001FFFFF		| FF FF 7F
 * 08000000		| C0 80 80 00
 * 0FFFFFFF		| FF FF FF 7F
 *
 * Code and explanation taken from MIDI 1.0 Specification document available at https://www.midi.org
 *
 * @param f:  File to read from
 * @return Unsigned Long: Variable-Length Quantity value
 */
unsigned long readVarLen(FILE *f)
{
    register unsigned long val;
    register unsigned char c;

    if ((val = getc(f)) & 0x80)
    {
        val &= 0x7F;
        do
        {
            val = (val << 7) + ((c = getc(f)) & 0x7F);
        } while (c & 0x80);
    }
    return (val);
}