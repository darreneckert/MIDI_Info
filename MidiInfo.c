/** @file MidiInfo.c
 *  @brief Constants, Structures and Functions for parsing MIDI format files
 *
 *  This contains the constants, data structures and functions
 *  needed to parse files in the MIDI file format
 *
 *  @author Darren Eckert
 *  @version 0.2
 *  @bug No known bugs currently.
 *  @todo Read track event data
 */

#ifndef MIDIINFO_H_
#include "MidiInfo.h"
#endif

#ifndef MIDI_INSTRUMENTS
#define MIDI_INSTRUMENTS 128
#endif

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

/** @fn struct MidiHeader readMidiChunk(FILE *f)
 *  @brief Read the MIDI file header
 *
 * This function reads the file header chunk from the supplied file.\n
 * Data values are stores most significant byte first (MSB).\n
 * The header chunk has the following format:\n
 * Chunk Type: 4 bytes, must be "MThd"\n
 * Length: 32bits, must be 6\n
 * Data: 3 16bit words:
 * - Format: 0, 1 or 2
 * - Number of tracks: for format 0 this must be 1
 * - Time division:  format for delta times
 *    + if bit 15 is a zero, the bits 14 thru 0 represent the number of
 *      delta-time "ticks" which make up a quarter-note.
 *    + If bit 15 is a one, delta-times in a file correspond to subdivisions
 *      of a second, in a way consistent with SMPTE and MIDI time code.\n
 *      Bits 14 thru 8 contain one of the four values -24, -25, -29, or -30,
 *      corresponding to the four standard SMPTE and MIDI time code formats\n
 *      (-29 corresponds to 30 drop frame), and represents the number of
 *      frames per second. These negative numbers are stored in two's complement form.
 *
 * @param f: The file to read from
 * @return struct MidiHeader containing the header chunk data
 */
struct MidiHeader readMidiChunk(FILE *f)
{
    char cChunkType[5];
    unsigned int uLength = 0;
    unsigned short uFormat = 0, uNumTracks = 0;
    short sTimeDiv = 0;
    struct MidiHeader midiHead;

    fread(&cChunkType, sizeof(char[4]), 1, f);
    cChunkType[4] = '\0';
    fread(&uLength, sizeof(uLength), 1, f);
    fread(&uFormat, sizeof(uFormat), 1, f);
    fread(&uNumTracks, sizeof(uNumTracks), 1, f);
    fread(&sTimeDiv, sizeof(sTimeDiv), 1, f);

    strcpy(midiHead.cChunkType, cChunkType);

    // Correct endianess
    midiHead.uLength = swapUInt32(uLength);
    midiHead.uFormat = swapUInt16(uFormat);
    midiHead.uNumTracks = swapUInt16(uNumTracks);
    midiHead.sTimeDiv = swapUInt16(sTimeDiv);

    return midiHead;
}

/** @fn struct TrackHeader readTrackChunk(FILE *f)
 *  @brief Read the header chunk for a track
 *
 *  This function will read the header chunk for a track from the given file\n
 *  A track chunk has the following format:
 *  Chunk TypeL 4 bytes, must be "MTrk"\n
 *  Length : 32 bits\n
 *
 *  @param f: The file to read from
 *  @return struct TrackHeader containing the header chunk data
 */
struct TrackHeader readTrackChunk(FILE *f)
{
    struct TrackHeader trackHead;
    char cChunkType[5];
    unsigned int uLength = 0;

    fread(&cChunkType, sizeof(char[4]), 1, f);
    cChunkType[4] = '\0';
    fread(&uLength, sizeof(uLength), 1, f);

    strcpy(trackHead.cChunkType, cChunkType);
    trackHead.uLength = swapUInt32(uLength);

    return trackHead;
}

void noteOff(FILE *f, unsigned char channel)
{
    unsigned char cNote, cVelocity;

    fread(&cNote, 1, 1, f);
    fread(&cVelocity, 1, 1, f);
    printf("Note Off Event - Channel %d, Note %d, Velocity %d\n", channel, cNote, cVelocity);
    return;
}

void noteOn(FILE *f, unsigned char channel)
{
    unsigned char cNote, cVelocity;

    fread(&cNote, 1, 1, f);
    fread(&cVelocity, 1, 1, f);
    printf("Note On Event - Channel %d, Note %d Velocity %d\n", channel, cNote, cVelocity);
    return;
}

void noteAftertouch(FILE *f, unsigned char channel)
{
    unsigned char cNote, cATValue;

    fread(&cNote, 1, 1, f);
    fread(&cATValue, 1, 1, f);
    printf("Note Aftertouch Event - Channel %d, Note %d, Aftertouch Value %d\n", channel, cNote, cATValue);
    return;
}

void controller(FILE *f, unsigned char channel)
{
    unsigned char cCtrlrNum, cCtrlrVal;

    fread(&cCtrlrNum, 1, 1, f);
    fread(&cCtrlrVal, 1, 1, f);
    printf("Controller Event - Channel %d, Controller Number %d, Controller Value %d\n", channel, cCtrlrNum, cCtrlrVal);
    return;
}

void programChange(FILE *f, unsigned char channel)
{
    unsigned char cProgNum, cTemp;

    fread(&cProgNum, 1, 1, f);
    fread(&cTemp, 1, 1, f);
    printf("Program Change Event - Channel %d, Program Number %d\n", channel, cProgNum);
    return;
}

void channelAftertouch(FILE *f, unsigned char channel)
{
    unsigned char cATVal, cTemp;

    fread(&cATVal, 1, 1, f);
    fread(&cTemp, 1, 1, f);
    printf("Channel Aftertouch Event - Channel %d, Aftertouch Value %d\n", channel, cATVal);
    return;
}

void pitchBend(FILE *f, unsigned char channel)
{
    unsigned char cLSB, cMSB;
    unsigned short uPitchBend = 0;

    fread(&cLSB, 1, 1, f);
    fread(&cMSB, 1, 1, f);
    cLSB = (cLSB >> 1); // Only need 7 bits
    cMSB = (cMSB >> 1); // Only need 7 bits
    uPitchBend = ((uPitchBend + cMSB) << 8) + cLSB;
    printf("Pitch Bend Event - Channel %d, Pitch Value LSB 0x%02x,  Pitch Value MSB 0x%02x, Pitch Value 0x%04x (%d)\n", channel, cLSB, cMSB, uPitchBend, uPitchBend);
    return;
}

/** @fn void readMidiEvent(FILE *f, unsigned char channel)
 *  @brief Reads a MIDI type event 
 * 
 *  This function will read a MIDI event from the given file\n
 *  Valid events are:
 *  - 0x8 Note Off
 *  - 0x9 Note On
 *  - 0xA Note Aftertouch
 *  - 0xB Controller
 *  - 0xC Program Change
 *  - 0xD Channel Aftertouch
 *  - 0xE Pitch Bend
 * 
 *  @param f: The file to read from
 *  @param channel: the MIDI channel the event affects
 *  @return No data is returned from the function currently
 */
void readMidiEvent(FILE *f, unsigned char eType, unsigned char channel)
{
    switch (eType)
    {
    case 0x8: // Note Off
        noteOff(f, channel);
        break;
    case 0x9: // Note On
        noteOn(f, channel);
        break;
    case 0xa: // Note Aftertouch
        noteAftertouch(f, channel);
        break;
    case 0xb: // Controller
        controller(f, channel);
        break;
    case 0xc: // Program Change
        programChange(f, channel);
        break;
    case 0xd: // Channel Aftertouch
        channelAftertouch(f, channel);
        break;
    case 0xe: // Pitch Bend
        pitchBend(f, channel);
        break;
    }
}

void seqNumEvent(FILE *f, int len)
{
    unsigned char buffer[5];

    fread(buffer, sizeof(char[4]), 1, f);
    buffer[4] = '\0';
    printf("Type is Sequence Number. Data is %s\n", buffer);
    return;
}

void textEvent(FILE *f, unsigned short type, int len)
{
    unsigned char *buffer;

    buffer = (unsigned char *)malloc(len + 1);
    if (buffer == NULL)
    {
        perror("Error allocating memory to read Text\n");
        fclose(f);
        exit(1);
    }
    fread(buffer, len, 1, f);
    buffer[len] = '\0';
    printf("Type is ");
    switch (type)
    {
    case 0x1:
        printf("Text Event. ");
        break;
    case 0x2:
        printf("Copyright Notice. ");
        break;
    case 0x3:
        printf("Sequence/Track Name. ");
        break;
    case 0x4:
        printf("Instrument Name. ");
        break;
    case 0x5:
        printf("Lyric. ");
        break;
    case 0x6:
        printf("Marker. ");
        break;
    case 0x7:
        printf("Cue Point. ");
        break;
    }
    printf("Data is %s\n", buffer);
    free(buffer);
    return;
}

void channelPrefixEvent(FILE *f)
{
    unsigned char channel;

    fread(&channel, 1, 1, f);
    printf("Type is Channel Prefix. Channel is %d\n", channel);
    return;
}

void portPrefixEvent(FILE *f)
{
    unsigned char port;

    fread(&port, 1, 1, f);
    printf("Type is Port Prefix. Port is %d\n", port);
    return;
}

void tempoEvent(FILE *f)
{
    unsigned int mspqn = 0, tempo = 0; // Microseconds per quarter-note, tempo (beats per minute)
    int j = 0;

    fread(&j, 1, 1, f);
    mspqn = mspqn + j;
    mspqn = (mspqn << 8);
    fread(&j, 1, 1, f);
    mspqn = mspqn + j;
    mspqn = (mspqn << 8);
    fread(&j, 1, 1, f);
    mspqn = mspqn + j;

    tempo = MS_PER_MIN / mspqn;

    printf("Type is Set Tempo. Data is %d BPM\n", tempo);
    return;
}

void SMPTEOffsetEvent(FILE *f, int len)
{
    unsigned char *buffer;

    buffer = (unsigned char *)malloc(len + 1);
    if (buffer == NULL)
    {
        perror("Error allocating memory to read SMPTE Offset data\n");
        fclose(f);
        exit(1);
    }
    fread(buffer, len, 1, f);
    buffer[len] = '\0';
    printf(" Type is SMPTE Offset. Data is %s\n", buffer);
    free(buffer);
    return;
}

void timeSigEvent(FILE *f)
{
    unsigned char buffer[4];
    int j;

    fread(buffer, 4, 1, f);
    j = intPow(2, buffer[1]);
    printf("Type is Time Signature. Signature is %d / %d %d %d\n", buffer[0], j, buffer[2], buffer[3]);
    return;
}

void keySigEvent(FILE *f)
{
    int i, j;
    unsigned char rec[2];

    fread(&rec, 2, 1, f);
    i = (signed char)rec[0];
    j = (signed char)rec[1];
    printf("Type is Key Signature. Signature is ");
    switch (i)
    {
    case -7:
        j ? printf("G Sharp ") : printf("C flat ");
        break;
    case -6:
        j ? printf("E Flat ") : printf("G flat ");
        break;
    case -5:
        j ? printf("B Flat ") : printf("D flat ");
        break;
    case -4:
        j ? printf("F ") : printf("A flat ");
        break;
    case -3:
        j ? printf("C ") : printf("E flat ");
        break;
    case -2:
        j ? printf("G ") : printf("B Flat ");
        break;
    case -1:
        j ? printf("D ") : printf("F ");
        break;
    case 0:
        printf("C ");
        break;
    case 1:
        j ? printf("E ") : printf("G ");
        break;
    case 2:
        j ? printf("B ") : printf("D ");
        break;
    case 3:
        j ? printf("F Sharp ") : printf("A ");
        break;
    case 4:
        j ? printf("C Sharp ") : printf("E ");
        break;
    case 5:
        j ? printf("G Sharp ") : printf("B ");
        break;
    case 6:
        j ? printf("D Sharp ") : printf("F Sharp ");
        break;
    case 7:
        j ? printf("B Flat ") : printf("C Sharp ");
        break;
    } // End switch (i)
    j ? printf("Minor") : printf("Major\n");

    return;
}

void SSMEvent(FILE *f, int len)
{
    unsigned char *buffer;

    buffer = (unsigned char *)malloc(len + 1);
    if (buffer == NULL)
    {
        perror("Error allocating memory to read Time Signature data\n");
        fclose(f);
        exit(1);
    }
    fread(buffer, sizeof(char) * len, 1, f);
    buffer[len] = '\0';
    printf("Type is Sequence Specific Meta Event. Data is %s\n", buffer);

    free(buffer);
    return;
}

void unknownEvent(FILE *f, int len)
{
    int i;
    unsigned char c;

    printf("Type is unknown, reading %d byte(s)\n", len);
    for (i = 0; i < len; i++)
    {
        fread(&c, 1, 1, f);
    }
    return;
}

/** @fn void readMetaEvent(FILE *f, unsigned short eType)
 *  @brief Reads a Meta type event.
 * 
 * This function reads a Meta event from the given file\n
 * Valid event types are:
 *  - 0x00 Sequence Number
 *  - 0x01 Text Event
 *  - 0x02 Copyright Notice
 *  - 0x03 Sequence/Track Name
 *  - 0x04 Instrument Name
 *  - 0x05 Lyric
 *  - 0x06 Marker
 *  - 0x07 Cue Point
 *  - 0x20 MIDI Channel Prefix
 *  - 0x21 MIDI Port Prefix
 *  - 0x2F End of Track
 *  - 0x51 Tempo
 *  - 0x54 SMPTE Offset
 *  - 0x58 Time Signature
 *  - 0x59 Key Signature
 *  - 0x7F Sequence Specific Meta Event
 * 
 * @paran f: The file to read from
 * @param eType: The type of Meta Event to read
 * @return No data is returned from this function currently
 */
void readMetaEvent(FILE *f, unsigned short eType)
{
    int len = 0;
    fread(&len, 1, 1, f);
    switch (eType)
    {
    case 0x0:
        seqNumEvent(f, len);
        break;
    case 0x1:
    case 0x2:
    case 0x3:
    case 0x4:
    case 0x5:
    case 0x6:
    case 0x7:
        textEvent(f, eType, len);
        break;
    case 0x20:
        channelPrefixEvent(f);
        break;
    case 0x21:
        portPrefixEvent(f);
        break;
    case 0x2f:
        printf("End of track event\n");
        break;
    case 0x51:
        tempoEvent(f);
        break;
    case 0x54:
        SMPTEOffsetEvent(f, len);
        break;
    case 0x58:
        timeSigEvent(f);
        break;
    case 0x59:
        keySigEvent(f);
        break;
    case 0x7F:
        SSMEvent(f, len);
        break;
    default:
        unknownEvent(f, len);
        break;
    } // End switch (eType)
    return;
}

/** @fn void readSysExEvent(FILE * f)
 *  @brief Reads a System Exclusive type event
 * 
 *  This function reads a System Exclusive Event from teh given file\n
 *   Valid event types are:
 *  - 0xF0 Normal SysEx Event
 *  - 0xF7 Divided SysEx Event
 * 
 *  @param f: The file to read from
 * @return No data is reurned from this function currently
 */
void readSysExEvent(FILE *f) {}

/** @fn void readTrackEvents(FILE *f)
 *  @brief Reads  events for the current track
 * 
 * This function reads  all the events for teh current track from the given file\n
 *  There are three types of events that can occur within a track:
 *  - MIDI Events
 *  - Meta Events
 *  - System Exclusive Events
 * 
 *  @param f: The file to read from
 *  @return No data is returned from this function currently
 */
void readTrackEvents(FILE *f)
{
    unsigned long deltaTime;
    unsigned short eventID = 0, eventType = 0;
    unsigned char cUpperByte, cLowerByte;

    printf("      Begin Processing Track Chunk\n");

    while (eventType != 0x2f)
    {
        deltaTime = readVarLen(f);
        fread(&eventID, 1, 1, f);
        cUpperByte = eventID >> 4;
        cLowerByte = eventID & 0xf;
        printf("         Delta time: 0x%02lx\n", deltaTime);
        switch (cUpperByte)
        {
        case 0x8:
        case 0x9:
        case 0xA:
        case 0xB:
        case 0xC:
        case 0xD:
        case 0xE:
            printf("         MIDI Event detected - ");
            readMidiEvent(f, cUpperByte, cLowerByte);
            break;
        case 0xF:
            if (cLowerByte == 0xF)
            {
                printf("         Meta Event detected - ");
                fread(&eventType, 1, 1, f);
                readMetaEvent(f, eventType);
            }
            else
            {
                printf("         SysExEvent detected - ");
                readSysExEvent(f);
            }
            break;
        } // End Switch
    }     // End While
}