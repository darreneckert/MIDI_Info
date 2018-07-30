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

#ifndef MIDIINFO_H_
#define MIDIINFO_H_

#ifndef MIDI_TYPES_H
#define MIDI_TYES_H
#define MIDI_INSTRUMENTS 128

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
	"Helicopter", "Applause", "Gunshot" 
};
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


#endif