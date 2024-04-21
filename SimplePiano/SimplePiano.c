//compiled with Microsoft's Visual C/C++ 6.0 (with the Feb 2003 MSDN library of API's) and later with Visual Studio 2008
//MidiPlyr.c in the MSDN Samples got me going,
//Petzold's Programming Windows showed how simple it could be,
//and MIDImon.c in the MSDN Samples showed how to read MIDI keyboard data.
#include "comctl6.h"
#include <windows.h>
#include <mmsystem.h>//add winmm.lib to Project -Settings -Link
#include <math.h>//for sin and cos
//#include <commctrl.h>//add comctl32.lib
#include "resource.h"

#define PI 3.141592653589793
#define IDM_EXIT 0x100
#define IDM_INSTRUMENT 0x200
#define IDM_INPUT 0X300
#define IDM_DEVICE 0x400
#define IDM_VELOCITY 0x500
#define IDM_KEYS 0x600
#define IDM_CHORDS 0x700
#define IDM_ACCIDENTAL 0x800
#define IDM_TEST 0x900
#define IDM_UNSTICK 0xA00
#define IDM_ABOUT 0xB00
#define NEITHER 0
#define LEFT 1
#define RIGHT 2
#define BOTH 3
#define ERROR_MSG_SIZE 1024

wchar_t About[] = L"Version 1.17\nMar 18, 2012\nDoug Cox\nhttp://jdmcox.com\njdmcox@jdmcox.com";

int BlackKeyNotes[25] = { 37,39,42,44,46,49,51,54,56,58,61,63,66,68,70,73,75,78,80,82,85,87,90,92,94 };
int saveAccidental;

//keys                       2   3       5   6   7   8   9   0       =           Q   W   E   R   T   Y   U   I   O   P   [   ]           A   S   D       G   H       K   L  ;                    Z   X   C   V   B   N   M   <   >   /
//scan codes     0,  1,  2,  3,  4, -1,  6,  7,  8, -1, 10, 11, -1, 13, -1, -1, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, -1, -1, 30, 31, 32, -1, 34, 35, -1, 37, 38, 39, -1, -1, -1, -1, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53  -1  55  56  57  58  -1  60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80
//notes                     C#  D#      F#  G#  A#      C#  D#      F#           C   D   E   F   G   A   B   C   D   E   F   G          F#  G#  A#      C#  D#      F#  G#  A#                   G   A   B   C   D   E   F   G   A   B       C   D   E   F       A   B   C   D   E   F   G   A   B  C#  D#      G#  A#  C#  D#  F#  G#  A#  B#   C
int Notes[] = { -1, -1, -1, 61, 63, -1, 66, 68, 70, -1, 73, 75, -1, 78, -1, -1, 60, 62, 64, 65, 67, 69, 71, 72, 74, 76, 77, 79, -1, -1, 42, 44, 46, -1, 49, 51, -1, 54, 56, 58, -1, -1, -1, -1, 43, 45, 47, 48, 50, 52, 53, 55, 57, 59, -1, 36, 38, 40, 41, -1, 81, 83, 84, 86, 88, 89, 91, 93, 95, 37, 39, -1, 80, 82, 85, 87, 90, 92, 94, 96, 97 };
int xKey[] = { -1, -1, -1, 15, 16, -1, 18, 19, 20, -1, 22, 23, -1, 25, -1, -1, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1,  4,  5,  6, -1,  8,  9, -1, 11, 12, 13, -1, -1, -1, -1,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, -1,  0,  1,  2,  3, -1, 26, 27, 28, 29, 30, 31, 32, 33, 34,  1,  2, -1, 26, 27, 29, 30, 32, 33, 34 };
int yKey[] = { -1, -1, -1,  0,  0, -1,  0,  0,  0, -1,  0,  0, -1,  0, -1, -1,  2,  3,  1,  2,  3,  3,  1,  2,  3,  1,  2,  3, -1, -1,  0,  0,  0, -1,  0,  0, -1,  0,  0,  0, -1, -1, -1, -1,  3,  3,  1,  2,  3,  1,  2,  3,  3,  1, -1,  2,  3,  1,  2, -1,  3,  1,  2,  3,  1,  2,  3,  3,  1,  0,  0, -1,  0,  0,  0,  0,  0,  0,  0,  0,  0 };
//t yStaff[] ={ -1, -1, -1,382,368, -1,356,350,344,338,332,326, -1,314, -1, -1,382,368,362,356,350,344,338,332,326,320,314,308, -1, -1,458,452,446, -1,434,428, -1,416,410,404, -1, -1, -1, -1,452,446,440,434,428,422,416,410,404,398, -1,476,470,464,458, -1,302,296,290,284,278,272,266,260,254,476,470, -1,308,302,290,284,272,266,260,254,248};
int yStaff[61];//36 is the lowest note

//               C   D   E   F   G   A   B   C   D   E   F   G   A   B   C   D   E   F   G   A   B   C   D   E   F   G   A   B   C   D   E   F   G   A   B
int LKeys[] = { 55, 69, 70, 58, 30, 31, 32, 47, 34, 35, 50, 37, 38, 39, 16,  3,  4, 19,  6,  7,  8, 23, 10, 11, 26, 13, 72, 73, 62, 74, 75, 65, 76, 77, 78 };
int RKeys[] = { 69, 70, 57, 30, 31, 32, 46, 34, 35, 49, 37, 38, 39, 53,  3,  4, 18,  6,  7,  8, 22, 10, 11, 25, 13, 72, 73, 61, 74, 75, 64, 76, 77, 78, 68 };
int MouseKeys[] = { -1, -1, -1, -1, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 55, 56, 57, 58, 60, 61, 62, 63, 64, 65, 66, 67, 68 };

//              36  37  38  39  40  41  42  43  44  45  46  47  48  49  50  51  52  53  54  55  56  57  58  59  60  61  62  63  64  65  66  67  68  69  70  71  72  73  74  75  76  77  78  79  80  81  82  83  84  85  86  87  88  89  90  91  92  93  94  95  96
int Codes[] = { 55, 69, 56, 70, 57, 58, 30, 44, 31, 45, 32, 46, 47, 34, 48, 35, 49, 50, 37, 51, 38, 52, 39, 53, 16,  3, 17,  4, 18, 19,  6, 20,  7, 21,  8, 22, 23, 10, 24, 11, 25, 26, 13, 27, 72, 60, 73, 61, 62, 74, 63, 75, 64, 65, 76, 66, 77, 67, 78, 68, 80 };

wchar_t CDEFGAB[] = L"CDEFGAB";
wchar_t ComputerKeys1[] = L"ZXCVBNM,./QWERTYUIOP[]";
wchar_t ComputerKeys2[] = L"ASDFGHJKL;'234567890-=";
wchar_t Staff[128];
int StaffWidth[1];
int NoteWidth[1];
int GClefWidth[1];
int FClefWidth[1];
int NumOfStaffs;
int Widths[7];
int Widths1[22];
int Widths2[22];
int ScanCodes[40];

int x, y, z, left, saveleft, top, top260, top260min30, top260min30div2, xPos, OldxPos = 0, yPos, accidental = 1, TitleAndMenu;
int index;
int iNumDevs, iInDevice = 0, iOutDevice = MIDIMAPPER;//-1 (pointer to driver for default Windows MIDI sounds)
int WhiteKeyWidth, BlackKeyWidth, ExtraSpace, noteloc, RandomNoteLoc, ChordType = 0, KeyName = 0, Inversion = 0;
int ScanCode, Note, PreviousNote = -1, RandomNote, PreviousRandomNote = 36, SavedNote, BracketingKeys, Velocity, DefaultVelocity = 127;
DWORD dwInstance, dwParam1, dwParam2;
DWORD fileSize, dwBytesRead, dwBytesWritten;
double d1, d2, d3, d4;
wchar_t szAppName[] = L"SimplePiano";
wchar_t SimplePianoIni[] = L"SimplePiano.ini";
wchar_t Ini[512];
wchar_t InDev[] = L"Input Device=";
wchar_t OutDev[] = L"Output Device=";
wchar_t Vel[] = L"Default Velocity=";
wchar_t ShowChords[] = L"Play a &Chord";
wchar_t HideChords[] = L"Hide &Chords";
wchar_t ShowKeys[] = L"Show &Keys";
wchar_t HideKeys[] = L"Hide &Keys";
wchar_t temp[16];
wchar_t VelocityChoice[8][4] = { L"128",L"112",L" 96",L" 80",L" 64",L" 48",L" 32",L" 16" };
//wchar_t Keys[12][3] = {"C ","F ","Bb","Eb","Ab","Db","Gb","B ","E ","A ","D ","G "};
wchar_t Keys[12][3] = { L"C ",L"G ",L"D ",L"A ",L"E ",L"B ",L"Gb",L"Db",L"Ab",L"Eb",L"Bb",L"F " };
//int Scale[12] = {60,65,70,63,68,61,66,71,64,69,62,67};
int Scale[12] = { 60,67,62,69,64,71,66,61,68,63,70,65 };
int Interval[] = { 0,2,4,5,7,9,11,12 };
int xKeyLoc[12];
int yKeyLoc[12];
wchar_t Chords[10][16] = { L"No Chord       ",L"Major Triad    ",L"Minor Triad    ",L"Dimished Triad ",L"Augmented Triad",L"Dominant 7th   ",L"Major 7th      ",L"Minor 7th      ",L"Diminished 7th ",L"All Scale Notes" };
wchar_t Inversions[4][14] = { L"No Inversion ",L"1st Inversion",L"2nd Inversion",L"3rd Inversion" };
wchar_t PlayNote[] = L"&Note Reading Test";
wchar_t StopNotes[] = L"&Stop Note Reading";
wchar_t Arial[] = L"Arial";
wchar_t Maestro[] = L"Maestro";
wchar_t ArialRounded[] = L"Arial Rounded MT Bold";
wchar_t ErrorMsg[1024];
BOOL first = TRUE, fromkeydown, midi_in = FALSE, showchords = FALSE, showkeys = FALSE, showtest = FALSE, showscale = FALSE, showinginstruments = FALSE;
HWND hwnd, hwndChords[9], hwndInversions[4], hwndKeys[12], hwndShowScale, hwndList;
HINSTANCE hInst;
HANDLE hFile;
HMENU hMenu, hMenuPopup;
HFONT hFont, hSmallFont, hMaestroFont;
HPEN hPen;
HBRUSH hWhiteBrush, hBlackBrush, hBlueBrush;
HGDIOBJ hOldFont, hOldPen, hOldBrush;
RECT rect, testRect;
HDC hdc, hdcMem;
HBITMAP hMemBitmap;
PAINTSTRUCT ps;
LOGFONT lf, lf1, lf2;
HMIDIOUT hMidiOut;
MIDIOUTCAPS moc;
HMIDIIN hMidiIn;
MIDIINCAPS mic;
SYSTEMTIME st;
FILETIME ft;
ULARGE_INTEGER ul;
POINT point;
//TRACKMOUSEEVENT tme;

wchar_t Piano[] = L"000 Acoustic Grand Piano";
wchar_t Piano2[] = L"001 Bright Acoustic Piano";
wchar_t Piano3[] = L"002 Electric Grand Piano";
wchar_t Piano4[] = L"003 Honky Tonk Piano";
wchar_t Piano5[] = L"004 Electric Piano 1";
wchar_t Piano6[] = L"005 Electric Piano 2";
wchar_t Piano7[] = L"006 Harpsichord";
wchar_t Piano8[] = L"007 Clavinet";
wchar_t ChromaticPercussion[] = L"008 Celesta";
wchar_t ChromaticPercussion2[] = L"009 Glockenspiel";
wchar_t ChromaticPercussion3[] = L"010 Music Box";
wchar_t ChromaticPercussion4[] = L"011 Vibraphone";
wchar_t ChromaticPercussion5[] = L"012 Marimba";
wchar_t ChromaticPercussion6[] = L"013 Xylophone";
wchar_t ChromaticPercussion7[] = L"014 Tubular Bells";
wchar_t ChromaticPercussion8[] = L"015 Dulcimer";
wchar_t Organ[] = L"016 Drawbar Organ";
wchar_t Organ2[] = L"017 Percussive Organ";
wchar_t Organ3[] = L"018 Rock Organ";
wchar_t Organ4[] = L"019 Church Organ";
wchar_t Organ5[] = L"020 Reed Organ";
wchar_t Organ6[] = L"021 Accoridan";
wchar_t Organ7[] = L"022 Harmonica";
wchar_t Organ8[] = L"023 Tango Accordian";
wchar_t Guitar[] = L"024 Nylon Acoustic Guitar";
wchar_t Guitar2[] = L"025 Steel Acoustic Guitar";
wchar_t Guitar3[] = L"026 Jazz Electric Guitar";
wchar_t Guitar4[] = L"027 Clean Electric Guitar";
wchar_t Guitar5[] = L"028 Muted Electric Guitar";
wchar_t Guitar6[] = L"029 Overdrive Guitar";
wchar_t Guitar7[] = L"030 Distorted Guitar";
wchar_t Guitar8[] = L"031 Harmonica Guitar";
wchar_t Bass[] = L"032 Acoustic Bass";
wchar_t Bass2[] = L"033 Electric Fingered Bass";
wchar_t Bass3[] = L"034 Electric Picked Bass";
wchar_t Bass4[] = L"035 Fretless Bass";
wchar_t Bass5[] = L"036 Slap Bass 1";
wchar_t Bass6[] = L"037 Slap Bass 2";
wchar_t Bass7[] = L"038 Syn Bass 1";
wchar_t Bass8[] = L"039 Syn Bass 2";
wchar_t Strings[] = L"040 Violin";
wchar_t Strings2[] = L"041 Viola";
wchar_t Strings3[] = L"042 Cello";
wchar_t Strings4[] = L"043 Contrabass";
wchar_t Strings5[] = L"044 Tremolo Strings";
wchar_t Strings6[] = L"045 Pizzicato Strings";
wchar_t Strings7[] = L"046 Orchestral Harp";
wchar_t Strings8[] = L"047 Timpani";
wchar_t Ensemble[] = L"048 String Ensemble 1";
wchar_t Ensemble2[] = L"049 String Ensemble 2 (Slow)";
wchar_t Ensemble3[] = L"050 Syn Strings 1";
wchar_t Ensemble4[] = L"051 Syn Strings 2";
wchar_t Ensemble5[] = L"052 Choir Aahs";
wchar_t Ensemble6[] = L"053 Voice Olhs";
wchar_t Ensemble7[] = L"054 Syn Choir";
wchar_t Ensemble8[] = L"055 Orchestral Hit";
wchar_t Brass[] = L"056 Trumpet";
wchar_t Brass2[] = L"057 Trombone";
wchar_t Brass3[] = L"058 Tuba";
wchar_t Brass4[] = L"059 Muted Trumpet";
wchar_t Brass5[] = L"060 French Horn";
wchar_t Brass6[] = L"061 Brass Section";
wchar_t Brass7[] = L"062 Syn Brass 1";
wchar_t Brass8[] = L"063 Syn Brass 2";
wchar_t Reed[] = L"064 Soprano Sax";
wchar_t Reed2[] = L"065 Alto Sax";
wchar_t Reed3[] = L"066 Tenor Sax";
wchar_t Reed4[] = L"067 Baritone Sax";
wchar_t Reed5[] = L"068 Oboe";
wchar_t Reed6[] = L"069 English Horn";
wchar_t Reed7[] = L"070 Bassoon";
wchar_t Reed8[] = L"071 Clarinet";
wchar_t Pipe[] = L"072 Piccolo";
wchar_t Pipe2[] = L"073 Flute";
wchar_t Pipe3[] = L"074 Recorder";
wchar_t Pipe4[] = L"075 Pan Flute";
wchar_t Pipe5[] = L"076 Bottle Blow";
wchar_t Pipe6[] = L"077 Shakuhachi";
wchar_t Pipe7[] = L"078 Whistle";
wchar_t Pipe8[] = L"079 Ocarina";
wchar_t SynthLead[] = L"080 Syn Square Wave";
wchar_t SynthLead2[] = L"081 Syn Sawtooth Wave";
wchar_t SynthLead3[] = L"082 Syn Calliope";
wchar_t SynthLead4[] = L"083 Syn Chiff";
wchar_t SynthLead5[] = L"084 Syn Chrang";
wchar_t SynthLead6[] = L"085 Syn Voice";
wchar_t SynthLead7[] = L"086 Syn Fifths Sawtooth Wave";
wchar_t SynthLead8[] = L"087 Syn Brass & Lead";
wchar_t SynthPad[] = L"088 New Age Syn Pad";
wchar_t SynthPad2[] = L"089 Warm Syn Pad";
wchar_t SynthPad3[] = L"090 Polysynth Syn Pad";
wchar_t SynthPad4[] = L"091 Choir Syn Pad";
wchar_t SynthPad5[] = L"092 Bowed Syn Pad";
wchar_t SynthPad6[] = L"093 Metal Syn Pad";
wchar_t SynthPad7[] = L"094 Halo Syn Pad";
wchar_t SynthPad8[] = L"095 Sweep Syn Pad";
wchar_t SynthEffects[] = L"096 SFX Rain";
wchar_t SynthEffects2[] = L"097 SFX Soundtrack";
wchar_t SynthEffects3[] = L"098 SFX Crystal";
wchar_t SynthEffects4[] = L"099 SFX Atmosphere";
wchar_t SynthEffects5[] = L"100 SFX Brightness";
wchar_t SynthEffects6[] = L"101 SFX Goblins";
wchar_t SynthEffects7[] = L"102 SFX Echoes";
wchar_t SynthEffects8[] = L"103 SFX Sci-Fi";
wchar_t Ethnic[] = L"104 Sitar";
wchar_t Ethnic2[] = L"105 Banjo";
wchar_t Ethnic3[] = L"106 Shamisen";
wchar_t Ethnic4[] = L"107 Koto";
wchar_t Ethnic5[] = L"108 Kalimba";
wchar_t Ethnic6[] = L"109 Bag Pipe";
wchar_t Ethnic7[] = L"110 Fiddle";
wchar_t Ethnic8[] = L"111 Shanai";
wchar_t Percussive[] = L"112 Tinkle Bell";
wchar_t Percussive2[] = L"113 Agogo";
wchar_t Percussive3[] = L"114 Steel Drum";
wchar_t Percussive4[] = L"115 Woodblock";
wchar_t Percussive5[] = L"116 Taiko Drum";
wchar_t Percussive6[] = L"117 Melodic Tom";
wchar_t Percussive7[] = L"118 Syn Drum";
wchar_t Percussive8[] = L"119 Reverse Cymbal";
wchar_t SoundEffects[] = L"120 Guitar Fret Noise";
wchar_t SoundEffects2[] = L"121 Breath Noise";
wchar_t SoundEffects3[] = L"122 Seashore";
wchar_t SoundEffects4[] = L"123 Bird Tweet";
wchar_t SoundEffects5[] = L"124 Telephone Ring";
wchar_t SoundEffects6[] = L"125 Helicopter";
wchar_t SoundEffects7[] = L"126 Applause";
wchar_t SoundEffects8[] = L"127 Gun Shot";

LPARAM Instruments[128] = { \
(LPARAM)Piano,
(LPARAM)Piano2,
(LPARAM)Piano3,
(LPARAM)Piano4,
(LPARAM)Piano5,
(LPARAM)Piano6,
(LPARAM)Piano7,
(LPARAM)Piano8,
(LPARAM)ChromaticPercussion,
(LPARAM)ChromaticPercussion2,
(LPARAM)ChromaticPercussion3,
(LPARAM)ChromaticPercussion4,
(LPARAM)ChromaticPercussion5,
(LPARAM)ChromaticPercussion6,
(LPARAM)ChromaticPercussion7,
(LPARAM)ChromaticPercussion8,
(LPARAM)Organ,
(LPARAM)Organ2,
(LPARAM)Organ3,
(LPARAM)Organ4,
(LPARAM)Organ5,
(LPARAM)Organ6,
(LPARAM)Organ7,
(LPARAM)Organ8,
(LPARAM)Guitar,
(LPARAM)Guitar2,
(LPARAM)Guitar3,
(LPARAM)Guitar4,
(LPARAM)Guitar5,
(LPARAM)Guitar6,
(LPARAM)Guitar7,
(LPARAM)Guitar8,
(LPARAM)Bass,
(LPARAM)Bass2,
(LPARAM)Bass3,
(LPARAM)Bass4,
(LPARAM)Bass5,
(LPARAM)Bass6,
(LPARAM)Bass7,
(LPARAM)Bass8,
(LPARAM)Strings,
(LPARAM)Strings2,
(LPARAM)Strings3,
(LPARAM)Strings4,
(LPARAM)Strings5,
(LPARAM)Strings6,
(LPARAM)Strings7,
(LPARAM)Strings8,
(LPARAM)Ensemble,
(LPARAM)Ensemble2,
(LPARAM)Ensemble3,
(LPARAM)Ensemble4,
(LPARAM)Ensemble5,
(LPARAM)Ensemble6,
(LPARAM)Ensemble7,
(LPARAM)Ensemble8,
(LPARAM)Brass,
(LPARAM)Brass2,
(LPARAM)Brass3,
(LPARAM)Brass4,
(LPARAM)Brass5,
(LPARAM)Brass6,
(LPARAM)Brass7,
(LPARAM)Brass8,
(LPARAM)Reed,
(LPARAM)Reed2,
(LPARAM)Reed3,
(LPARAM)Reed4,
(LPARAM)Reed5,
(LPARAM)Reed6,
(LPARAM)Reed7,
(LPARAM)Reed8,
(LPARAM)Pipe,
(LPARAM)Pipe2,
(LPARAM)Pipe3,
(LPARAM)Pipe4,
(LPARAM)Pipe5,
(LPARAM)Pipe6,
(LPARAM)Pipe7,
(LPARAM)Pipe8,
(LPARAM)SynthLead,
(LPARAM)SynthLead2,
(LPARAM)SynthLead3,
(LPARAM)SynthLead4,
(LPARAM)SynthLead5,
(LPARAM)SynthLead6,
(LPARAM)SynthLead7,
(LPARAM)SynthLead8,
(LPARAM)SynthPad,
(LPARAM)SynthPad2,
(LPARAM)SynthPad3,
(LPARAM)SynthPad4,
(LPARAM)SynthPad5,
(LPARAM)SynthPad6,
(LPARAM)SynthPad7,
(LPARAM)SynthPad8,
(LPARAM)SynthEffects,
(LPARAM)SynthEffects2,
(LPARAM)SynthEffects3,
(LPARAM)SynthEffects4,
(LPARAM)SynthEffects5,
(LPARAM)SynthEffects6,
(LPARAM)SynthEffects7,
(LPARAM)SynthEffects8,
(LPARAM)Ethnic,
(LPARAM)Ethnic2,
(LPARAM)Ethnic3,
(LPARAM)Ethnic4,
(LPARAM)Ethnic5,
(LPARAM)Ethnic6,
(LPARAM)Ethnic7,
(LPARAM)Ethnic8,
(LPARAM)Percussive,
(LPARAM)Percussive2,
(LPARAM)Percussive3,
(LPARAM)Percussive4,
(LPARAM)Percussive5,
(LPARAM)Percussive6,
(LPARAM)Percussive7,
(LPARAM)Percussive8,
(LPARAM)SoundEffects,
(LPARAM)SoundEffects2,
(LPARAM)SoundEffects3,
(LPARAM)SoundEffects4,
(LPARAM)SoundEffects5,
(LPARAM)SoundEffects6,
(LPARAM)SoundEffects7,
(LPARAM)SoundEffects8 };

int Instrument = 0;

void DrawKey(void);
void StartNote(void);
void EndNote(void);
void CALLBACK MidiInProc(HMIDIIN hMidiIn, WORD wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
int Atoi(wchar_t*);
void WriteIni(void);
void NoteTest(void);
WNDPROC pListProc;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);



int
WINAPI
wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
)
{

	MSG          msg;
	WNDCLASS     wndclass;

	hInst = hInstance;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;

	if (!RegisterClassW(&wndclass))
		return 0;

	hwnd = CreateWindowW(szAppName, szAppName,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, SW_SHOWMAXIMIZED);
	UpdateWindow(hwnd);

	while (GetMessageW(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	return (int)msg.wParam;
}


//sub-class procedure
LRESULT ListProc(HWND hwnd2, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_LBUTTONUP)
	{
		index = (int)SendMessageW(hwndList, LB_GETCURSEL, 0, 0);
		Instrument = index;
		midiOutShortMsg(hMidiOut, 0x0C0 | (Instrument << 8));
		DestroyWindow(hwndList);
	}
	else if (message == WM_KEYUP)
	{
		if ((wParam == VK_DOWN) && (index < 127))
			index++;
		else if ((wParam == VK_UP) && (index != 0))
			index--;

		else if (wParam == VK_ESCAPE) {
			DestroyWindow(hwndList);
		}
		else if (wParam == VK_RETURN) {
			index = (int)SendMessageW(hwndList, LB_GETCURSEL, 0, 0);
			if (index < 128) {
				DestroyWindow(hwndList);
				Instrument = index;
				midiOutShortMsg(hMidiOut, 0x0C0 | (Instrument << 8));
			}
		}
	}
	else if (message == WM_DESTROY)
		showinginstruments = FALSE;

	return CallWindowProcW(pListProc, hwnd2, message, wParam, lParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		if (0 == AddFontResourceW(L"MAESTRO_.TTF"))
			MessageBoxW(hwnd, L"Couldn't load the MAESTRO_.TTF font!\nMake sure it's in the Simple Piano folder,\nor the folder that Windows7 moves data files to.", ERROR, MB_OK);
		GetLocalTime(&st);
		SystemTimeToFileTime(&st, &ft);
		ul.LowPart = ft.dwLowDateTime;
		ul.HighPart = ft.dwHighDateTime;
		ul.QuadPart /= 10000;//because low 4 digits are 0's
		srand(ul.LowPart);

		hwndKeys[0] = hwndChords[0] = NULL;
		hFile = CreateFileW(SimplePianoIni, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			fileSize = GetFileSize(hFile, NULL);
			if ((fileSize > 0) && (fileSize < 512))
			{
				ReadFile(hFile, Ini, fileSize, &dwBytesRead, NULL);
				for (x = 0; (x < (int)fileSize) && (Ini[x] != '='); x++)
					;
				if (Ini[x] == '=')
				{
					if ((Ini[x - 12] == 'I') && (Ini[x - 11] == 'n'))
					{
						z = Ini[x + 1];
						if (z == '-')
							iInDevice = -1;
						else
							iInDevice = z - '0';
						for (x++; (x < (int)fileSize) && (Ini[x] != '='); x++)
							;
					}
					z = Ini[x + 1];
					if (z == '-')
						iOutDevice = -1;
					else
						iOutDevice = z - '0';
					for (x++; (x < (int)fileSize) && (Ini[x] != '='); x++)
						;
					if (Ini[x] == '=')
						DefaultVelocity = Atoi(&Ini[x + 1]);
				}
			}
			CloseHandle(hFile);
		}
		hMenu = CreateMenu();
		hMenuPopup = CreateMenu();
		AppendMenuW(hMenu, MF_STRING, IDM_EXIT, L"&Exit");
		hMenuPopup = CreateMenu();
		AppendMenuW(hMenu, MF_STRING, IDM_INSTRUMENT, L"&Instrument");
		iNumDevs = midiOutGetNumDevs();
		z = midiInGetNumDevs();
		if (z) {
			AppendMenuW(hMenu, MF_STRING | MF_POPUP, (LONG_PTR)hMenuPopup, L"Input &Device");
			for (x = 0; x < z; x++) {
				if (MMSYSERR_NOERROR == midiInGetDevCapsW(x, &mic, sizeof(mic)))
					AppendMenuW(hMenuPopup, MF_STRING, IDM_INPUT + x, mic.szPname);
			}
			CheckMenuItem(hMenuPopup, IDM_INPUT + iInDevice, MF_CHECKED);
		}
		hMenuPopup = CreateMenu();
		if (MMSYSERR_NOERROR == midiOutGetDevCapsW(MIDIMAPPER, &moc, sizeof(moc)))//Microsoft MIDI Mapper
		{
			AppendMenuW(hMenu, MF_STRING | MF_POPUP, (LONG_PTR)hMenuPopup, L"&Output Device");
			AppendMenuW(hMenuPopup, MF_STRING, IDM_DEVICE + (int)MIDIMAPPER, moc.szPname);
			for (x = 0; x < iNumDevs; x++)
			{
				midiOutGetDevCapsW(x, &moc, sizeof(moc));
				AppendMenuW(hMenuPopup, MF_STRING, IDM_DEVICE + x, moc.szPname);
			}
			CheckMenuItem(hMenuPopup, IDM_DEVICE + iOutDevice, MF_CHECKED);
		}
		hMenuPopup = CreateMenu();
		AppendMenuW(hMenu, MF_STRING | MF_POPUP, (LONG_PTR)hMenuPopup, L"Default Key &Volume");
		for (x = 0; x < 8; x++)
		{
			AppendMenu(hMenuPopup, MF_STRING, IDM_VELOCITY + x, VelocityChoice[x]);
		}
		CheckMenuItem(hMenuPopup, 8 - ((DefaultVelocity + 1) / 16) + IDM_VELOCITY, MF_CHECKED);
		hMenuPopup = CreateMenu();
		AppendMenuW(hMenu, MF_STRING | MF_POPUP, (LONG_PTR)hMenuPopup, L"&Accidental");
		AppendMenuW(hMenuPopup, MF_STRING, IDM_ACCIDENTAL + 1, L"#");
		AppendMenuW(hMenuPopup, MF_STRING, IDM_ACCIDENTAL + 2, L"b");
		CheckMenuItem(hMenuPopup, IDM_ACCIDENTAL + 1, MF_CHECKED);
		hMenuPopup = CreateMenu();
		AppendMenuW(hMenu, MF_STRING, IDM_KEYS, ShowKeys);
		hMenuPopup = CreateMenu();
		AppendMenuW(hMenu, MF_STRING, IDM_CHORDS, ShowChords);
		hMenuPopup = CreateMenu();
		AppendMenuW(hMenu, MF_STRING, IDM_TEST, PlayNote);
		hMenuPopup = CreateMenu();
		//		AppendMenu(hMenu, MF_STRING, IDM_UNSTICK, L"&Unstick Notes");
		//		SetMenu(hwnd, hMenu);
		AppendMenuW(hMenu, MF_STRING, IDM_ABOUT, L"&About");
		SetMenu(hwnd, hMenu);

		if (MMSYSERR_NOERROR == midiOutOpen(&hMidiOut, iOutDevice, 0, 0, 0)) {
			midiOutShortMsg(hMidiOut, 0x0C0 | (Instrument << 8));//channel 0 and grand piano (instrument 0)
			midiConnect((HMIDI)hMidiIn, (HMIDIOUT)hMidiOut, NULL);//THRU MIDI
		}

		if (MMSYSERR_NOERROR == midiInOpen((LPHMIDIIN)&hMidiIn, iInDevice, (LONG_PTR)MidiInProc, 0, CALLBACK_FUNCTION))
		{//if a MIDI keyboard is attached
			midi_in = TRUE;
			midiInStart(hMidiIn);
		}

		lf.lfHeight = -29;
		lf.lfWeight = 700;
		lf.lfItalic = 0;
		lf.lfUnderline = 0;
		lf.lfStrikeOut = 0;
		lf.lfCharSet = 0;
		lf.lfOutPrecision = 3;
		lf.lfClipPrecision = 2;
		lf.lfQuality = 1;
		lf.lfPitchAndFamily = 0x22;
		for (x = 0; Arial[x] != 0; x++)
			lf.lfFaceName[x] = Arial[x];
		lf.lfFaceName[x] = 0;
		hFont = CreateFontIndirectW(&lf);

		lf1.lfHeight = -13;
		lf1.lfWeight = 400;
		lf1.lfItalic = 0;
		lf1.lfUnderline = 0;
		lf1.lfStrikeOut = 0;
		lf1.lfCharSet = 0;
		lf1.lfOutPrecision = 3;
		lf1.lfClipPrecision = 2;
		lf1.lfQuality = 1;
		lf1.lfPitchAndFamily = 0x22;
		for (x = 0; Arial[x] != 0; x++)
			lf1.lfFaceName[x] = Arial[x];
		lf1.lfFaceName[x] = 0;
		hSmallFont = CreateFontIndirectW(&lf1);

		lf2.lfHeight = -48;
		lf2.lfWeight = 400;
		lf2.lfItalic = 0;
		lf2.lfUnderline = 0;
		lf2.lfStrikeOut = 0;
		lf2.lfCharSet = 2;
		lf2.lfOutPrecision = 3;
		lf2.lfClipPrecision = 2;
		lf2.lfQuality = 1;
		lf2.lfPitchAndFamily = 0x02;
		for (x = 0; Maestro[x] != 0; x++)
			lf2.lfFaceName[x] = Maestro[x];
		lf2.lfFaceName[x] = 0;
		hMaestroFont = CreateFontIndirectW(&lf2);

		hPen = CreatePen(PS_SOLID, 2, 0x808080);
		hWhiteBrush = CreateSolidBrush(0xFFFFFF);
		hBlackBrush = CreateSolidBrush(0);
		hBlueBrush = CreateSolidBrush(0xFF0000);
		for (x = 0; x < 40; x++)
			ScanCodes[x] = 0;
		TitleAndMenu = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYMENU);
		return 0;


	case WM_SIZE:
		rect.left = rect.top = 0;
		rect.right = LOWORD(lParam);
		rect.bottom = HIWORD(lParam);
		top = rect.bottom - 180;
		top260 = top - 260;
		top260min30 = top260 - 30;
		if (top260min30 > 252)//top of screen in 1024x768
			top260min30 = 252;
		top260min30div2 = top260min30 / 2;
		noteloc = top - 98;//low C, note 36
		for (x = 0; x < 60; x += 12)
		{
			for (z = 0; z < 12; )
			{
				if ((x + z) == (60 - 36))//middle C
					noteloc -= 10;
				yStaff[x + (z++)] = noteloc;//36 C
				yStaff[x + z++] = noteloc;//37 C#
				noteloc -= 6;
				if ((x + z) == (62 - 36))//D above middle C
					noteloc -= 8;
				yStaff[x + z++] = noteloc;//38 D
				yStaff[x + z++] = noteloc;//39 D#
				noteloc -= 6;
				yStaff[x + z++] = noteloc;//40 E
				noteloc -= 6;
				yStaff[x + z++] = noteloc;//41 F
				yStaff[x + z++] = noteloc;//42 F#
				noteloc -= 6;
				yStaff[x + z++] = noteloc;//43 G
				yStaff[x + z++] = noteloc;//44 G#
				noteloc -= 6;
				yStaff[x + z++] = noteloc;//45 A
				yStaff[x + z++] = noteloc;//46 A#
				noteloc -= 6;
				yStaff[x + z++] = noteloc;//47 B
				noteloc -= 6;
			}
		}
		//                      0    1    2    3    4    5    6    7    8    9   10   11
		//wchar_t Keys[12][3] = {"C ","F ","Bb","Eb","Ab","Db","Gb","B ","E ","A ","D ","G "};
		//wchar_t Keys[12][3] = {"C ","G ","D ","A ","E ","B ","Gb","Db","Ab","Eb","Bb","F "};
		d1 = (double)top260min30div2 * cos(60.0 * PI / 180.0);//for circle of fifths
		d2 = (double)top260min30div2 * sin(60.0 * PI / 180.0);
		d3 = (double)top260min30div2 * cos(30.0 * PI / 180.0);
		d4 = (double)top260min30div2 * sin(30.0 * PI / 180.0);
		xKeyLoc[0] = top260min30div2;
		yKeyLoc[0] = 0;
		xKeyLoc[1] = (int)d1 + top260min30div2;
		yKeyLoc[1] = top260min30div2 - (int)d2;
		xKeyLoc[2] = (int)d3 + top260min30div2;
		yKeyLoc[2] = top260min30div2 - (int)d4;
		xKeyLoc[3] = top260min30;
		yKeyLoc[3] = top260min30div2;
		xKeyLoc[4] = xKeyLoc[2];
		yKeyLoc[4] = top260min30div2 + (int)d4;
		xKeyLoc[5] = xKeyLoc[1];
		yKeyLoc[5] = top260min30div2 + (int)d2;
		xKeyLoc[6] = top260min30div2;
		yKeyLoc[6] = top260min30;
		xKeyLoc[7] = top260min30div2 - (int)d1;
		yKeyLoc[7] = top260min30 - top260min30div2 + (int)d2;
		xKeyLoc[8] = top260min30div2 - (int)d3;
		yKeyLoc[8] = top260min30 - top260min30div2 + (int)d4;
		xKeyLoc[9] = 0;
		yKeyLoc[9] = top260min30div2;
		xKeyLoc[10] = top260min30div2 - (int)d3;
		yKeyLoc[10] = top260min30 - top260min30div2 - (int)d4;
		xKeyLoc[11] = top260min30div2 - (int)d1;
		yKeyLoc[11] = top260min30 - top260min30div2 - (int)d2;
		if (hwndKeys[0] != NULL)
		{
			for (x = 0; x < 12; x++)
			{
				DestroyWindow(hwndKeys[x]);
				hwndKeys[x] = NULL;
			}
			for (x = 0, z = top260 - 270; x < 12; x++)
			{
				hwndKeys[x] = CreateWindowW(L"BUTTON", Keys[x],
					WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | WS_DLGFRAME,
					xKeyLoc[x], yKeyLoc[x], 50, 30,
					hwnd, NULL, hInst, NULL);
			}
			DestroyWindow(hwndShowScale);
			hwndShowScale = CreateWindowW(L"BUTTON", L"Show Scale",
				WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | WS_DLGFRAME,
				top260min30 - (top260min30 / 2) - 35, top260min30 - (top260min30 / 2), 120, 30,
				hwnd, NULL, hInst, NULL);
		}
		if (hwndChords[0] != NULL)
		{
			for (x = 0; x < 9; x++)
			{
				DestroyWindow(hwndChords[x]);
				hwndChords[x] = NULL;
			}
			for (x = 0, z = 0; x < 9; x++, z += 30)
			{
				hwndChords[x] = CreateWindowW(L"BUTTON", Chords[x],
					WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | WS_DLGFRAME,
					top260min30 + 50, z, 135, 30,
					hwnd, NULL, hInst, NULL);
			}
			SendMessageW(hwndChords[ChordType], BM_SETCHECK, BST_CHECKED, 0);
			for (x = 0; x < 4; x++)
				DestroyWindow(hwndInversions[x]);
			for (x = 0, z = 30; x < 3; x++, z += 30)
			{
				hwndInversions[x] = CreateWindowW(L"BUTTON", Inversions[x],
					WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | WS_DLGFRAME,
					top260min30 + 50 + 135, z, 120, 30,
					hwnd, NULL, hInst, NULL);
			}
			hwndInversions[3] = CreateWindowW(L"BUTTON", Inversions[3],
				WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | WS_DLGFRAME,
				top260min30 + 50 + 135, 150, 120, 30,
				hwnd, NULL, hInst, NULL);
			SendMessageW(hwndInversions[Inversion], BM_SETCHECK, BST_CHECKED, 0);
		}

		if (showtest)
		{
			SendMessageW(hwnd, WM_COMMAND, IDM_ACCIDENTAL + saveAccidental, 0);
			showtest = FALSE;
			ModifyMenuW(hMenu, IDM_TEST, MF_BYCOMMAND | MF_STRING, IDM_TEST, PlayNote);
			DrawMenuBar(hwnd);
		}
		WhiteKeyWidth = rect.right / 35;//5 octaves = 60 keys = 35 white keys
		BlackKeyWidth = WhiteKeyWidth * 2 / 3;
		ExtraSpace = (rect.right % 35) / 2;
		return 0;


	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			if (showtest)
			{
				showtest = FALSE;
				SendMessageW(hwnd, WM_COMMAND, IDM_ACCIDENTAL + saveAccidental, 0);
				ModifyMenuW(hMenu, IDM_TEST, MF_BYCOMMAND | MF_STRING, IDM_TEST, PlayNote);
				DrawMenuBar(hwnd);
			}
			for (x = 0; x < 40; x++)
				ScanCodes[x] = 0;
		}
		else
			InvalidateRect(hwnd, &rect, FALSE);
		break;

	case WM_CAPTURECHANGED:
		for (x = 0; x < 40; x++)
			ScanCodes[x] = 0;
		break;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDM_EXIT)
			DestroyWindow(hwnd);

		else if (LOWORD(wParam) == IDM_INSTRUMENT)
		{
			if (showinginstruments == FALSE)
			{
				showinginstruments = TRUE;
				InvalidateRect(hwnd, &rect, FALSE);
				hwndList = CreateWindowW(L"LISTBOX", L"Instruments",
					WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_VSCROLL,// | LBS_NOTIFY,
					0, TitleAndMenu, 240, rect.bottom,
					hwnd, NULL, hInst, NULL);
				pListProc = (WNDPROC)SetWindowLongPtr(hwndList, GWLP_WNDPROC, (LONG_PTR)ListProc);
				for (x = 0; x < 128; x++)
					SendMessageW(hwndList, LB_ADDSTRING, 0, Instruments[x]);
				//SendMessageW(hwndList, LB_ADDSTRING, 0, *(DWORD*)&Instruments[x]);
				index = Instrument;
				SendMessageW(hwndList, LB_SETCURSEL, index, 0);
				SetFocus(hwndList);
			}
		}

		else if ((LOWORD(wParam) >= IDM_INPUT - 1) && (LOWORD(wParam) < (IDM_DEVICE - 1)))
		{
			CheckMenuItem(hMenu, IDM_INPUT + iInDevice, MF_UNCHECKED);
			iInDevice = LOWORD(wParam) - IDM_INPUT;
			if (iInDevice)
				WriteIni();
			CheckMenuItem(hMenu, IDM_INPUT + iInDevice, MF_CHECKED);

			midiInClose(hMidiIn);
			if (MMSYSERR_NOERROR == midiInOpen((LPHMIDIIN)&hMidiIn, iInDevice, (LONG_PTR)MidiInProc, 0, CALLBACK_FUNCTION))
			{//if a MIDI keyboard is attached
				midi_in = TRUE;
				midiInStart(hMidiIn);
			}
		}

		else if ((LOWORD(wParam) >= IDM_DEVICE - 1) && (LOWORD(wParam) < (IDM_VELOCITY - 1)))
		{
			CheckMenuItem(hMenu, IDM_DEVICE + iOutDevice, MF_UNCHECKED);
			iOutDevice = LOWORD(wParam) - IDM_DEVICE;
			WriteIni();
			CheckMenuItem(hMenu, IDM_DEVICE + iOutDevice, MF_CHECKED);

			midiOutClose(hMidiOut);
			if (MMSYSERR_NOERROR == midiOutOpen(&hMidiOut, iOutDevice, 0, 0, 0))
				midiOutShortMsg(hMidiOut, 0x0C0 | (Instrument << 8));
		}

		else if ((LOWORD(wParam) >= (IDM_VELOCITY - 1)) && (LOWORD(wParam) < (IDM_KEYS - 1)))
		{
			CheckMenuItem(hMenu, 7 - (DefaultVelocity / 16) + IDM_VELOCITY, MF_UNCHECKED);
			DefaultVelocity = ((8 - (LOWORD(wParam) - IDM_VELOCITY)) * 16) - 1;
			WriteIni();
			CheckMenuItem(hMenu, LOWORD(wParam), MF_CHECKED);
		}

		else if ((LOWORD(wParam) >= (IDM_ACCIDENTAL - 1)) && (LOWORD(wParam) < IDM_TEST))
		{
			CheckMenuItem(hMenu, IDM_ACCIDENTAL + accidental, MF_UNCHECKED);
			accidental = LOWORD(wParam) - IDM_ACCIDENTAL;
			CheckMenuItem(hMenu, IDM_ACCIDENTAL + accidental, MF_CHECKED);
			if (showkeys)
			{
				if (accidental == 1)
					*(WORD*)&Keys[6][0] = '#F';
				else//if (accidental == 2)
					*(WORD*)&Keys[6][0] = 'bG';
				DestroyWindow(hwndKeys[6]);
				hwndKeys[6] = CreateWindowW(L"BUTTON", Keys[6],
					WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_DLGFRAME,
					xKeyLoc[6], yKeyLoc[6], 50, 30,
					hwnd, NULL, hInst, NULL);
				InvalidateRect(hwnd, &rect, FALSE);
				UpdateWindow(hwnd);
			}
		}

		else if (LOWORD(wParam) == IDM_KEYS)
		{
			if (showkeys == FALSE)
			{
				showkeys = TRUE;
				ModifyMenuW(hMenu, IDM_KEYS, MF_BYCOMMAND | MF_STRING, IDM_KEYS, HideKeys);
				DrawMenuBar(hwnd);
				if (accidental == 1)
					*(WORD*)&Keys[6][0] = '#F';
				else//if (accidental == 2)
					*(WORD*)&Keys[6][0] = 'bG';
				for (x = 0, z = top260 - 270; x < 12; x++)
				{
					hwndKeys[x] = CreateWindowW(L"BUTTON", Keys[x],
						WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | WS_DLGFRAME,
						xKeyLoc[x], yKeyLoc[x], 50, 30,
						hwnd, NULL, hInst, NULL);
				}
				SendMessageW(hwndKeys[KeyName], BM_SETCHECK, BST_CHECKED, 0);
				hwndShowScale = CreateWindowW(L"BUTTON", L"Show Scale",
					WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | WS_DLGFRAME,
					top260min30 - (top260min30 / 2) - 35, top260min30 - (top260min30 / 2), 120, 30,
					hwnd, NULL, hInst, NULL);
				SendMessageW(hwndShowScale, BM_SETCHECK, BST_UNCHECKED, 0);
			}
			else
			{
				showkeys = FALSE;
				showscale = FALSE;
				ChordType = 0;
				ModifyMenuW(hMenu, IDM_KEYS, MF_BYCOMMAND | MF_STRING, IDM_KEYS, ShowKeys);
				DrawMenuBar(hwnd);
				for (x = 0; x < 12; x++)
				{
					DestroyWindow(hwndKeys[x]);
					hwndKeys[x] = NULL;
				}
				DestroyWindow(hwndShowScale);
				hwndKeys[0] = NULL;//flag
			}
			InvalidateRect(hwnd, &rect, FALSE);
			UpdateWindow(hwnd);
		}

		else if (LOWORD(wParam) == IDM_CHORDS) // Play a Chord
		{
			if (showchords == FALSE)
			{
				showchords = TRUE;
				ModifyMenuW(hMenu, IDM_CHORDS, MF_BYCOMMAND | MF_STRING, IDM_CHORDS, HideChords);
				DrawMenuBar(hwnd);
				for (x = 0, z = 0; x < 9; x++, z += 30)
				{
					hwndChords[x] = CreateWindowW(L"BUTTON", Chords[x],
						WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | WS_DLGFRAME,
						top260min30 + 50, z, 135, 30,
						hwnd, NULL, hInst, NULL);
				}
				SendMessageW(hwndChords[ChordType], BM_SETCHECK, BST_CHECKED, 0);
				for (x = 0, z = 30; x < 3; x++, z += 30)
				{
					hwndInversions[x] = CreateWindowW(L"BUTTON", Inversions[x],
						WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | WS_DLGFRAME,
						top260min30 + 50 + 135, z, 120, 30,
						hwnd, NULL, hInst, NULL);
				}
				hwndInversions[3] = CreateWindowW(L"BUTTON", Inversions[3],
					WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON | WS_DLGFRAME,
					top260min30 + 50 + 135, 150, 120, 30,
					hwnd, NULL, hInst, NULL);
				SendMessageW(hwndInversions[Inversion], BM_SETCHECK, BST_CHECKED, 0);
			}
			else
			{
				showchords = FALSE;
				ModifyMenuW(hMenu, IDM_CHORDS, MF_BYCOMMAND | MF_STRING, IDM_CHORDS, ShowChords);
				DrawMenuBar(hwnd);
				for (x = 0; x < 9; x++)
					DestroyWindow(hwndChords[x]);
				hwndChords[0] = NULL;//flag
				for (x = 0; x < 4; x++)
					DestroyWindow(hwndInversions[x]);
			}
			InvalidateRect(hwnd, &rect, FALSE);
			UpdateWindow(hwnd);
		}

		else if (LOWORD(wParam) == IDM_TEST)
		{
			if (showtest == FALSE)
			{
				showtest = TRUE;
				ModifyMenuW(hMenu, IDM_TEST, MF_BYCOMMAND | MF_STRING, IDM_TEST, StopNotes);
				DrawMenuBar(hwnd);
				testRect.left = top260min30 + 325;
				testRect.right = testRect.left + (2 * StaffWidth[0]);
				testRect.top = top - 462 - 59;
				testRect.bottom = top260;
				hdc = GetDC(hwnd);
				SelectObject(hdc, hMaestroFont);
				NoteTest();
				ReleaseDC(hwnd, hdc);
			}
			else//if (showtest)
			{
				SendMessageW(hwnd, WM_COMMAND, IDM_ACCIDENTAL + saveAccidental, 0);
				showtest = FALSE;
				ModifyMenuW(hMenu, IDM_TEST, MF_BYCOMMAND | MF_STRING, IDM_TEST, PlayNote);
				DrawMenuBar(hwnd);
				hdc = GetDC(hwnd);
				FillRect(hdc, &rect, hWhiteBrush);
				ReleaseDC(hwnd, hdc);
				InvalidateRect(hwnd, &rect, FALSE);
				UpdateWindow(hwnd);
			}
		}

		else if (LOWORD(wParam) == IDM_ABOUT)
			MessageBoxW(hwnd, About, szAppName, MB_OK);

		else if (HIWORD(wParam) == BN_CLICKED)
		{
			if (showkeys)
			{
				for (x = 0; x < 12; x++)
				{
					if (lParam == (LONG_PTR)hwndShowScale)
					{
						if (showscale)
						{
							SendMessageW(hwndShowScale, BM_SETCHECK, BST_UNCHECKED, 0);
							SavedNote = Scale[KeyName];
							ScanCode = Codes[SavedNote - 36];//36 is the lowest MIDI note number in this program
							EndNote();
							showscale = FALSE;
						}
						else
						{
							showscale = TRUE;
							EndNote();
							SendMessageW(hwndShowScale, BM_SETCHECK, BST_CHECKED, 0);
							SavedNote = Scale[KeyName];
							ScanCode = Codes[SavedNote - 36];//36 is the lowest MIDI note number in this program
							StartNote();
						}
						break;
					}
					else if (lParam == (LONG_PTR)hwndKeys[x])
					{
						SendMessageW(hwndKeys[KeyName], BM_SETCHECK, BST_UNCHECKED, 0);
						KeyName = x;
						SendMessageW(hwndKeys[KeyName], BM_SETCHECK, BST_CHECKED, 0);
						if ((KeyName >= 7) && (KeyName <= 11))
						{
							CheckMenuItem(hMenu, IDM_ACCIDENTAL + accidental, MF_UNCHECKED);
							accidental = 2;
							CheckMenuItem(hMenu, IDM_ACCIDENTAL + accidental, MF_CHECKED);
							*(WORD*)&Keys[6][0] = 'bG';
							DestroyWindow(hwndKeys[6]);
							hwndKeys[6] = CreateWindowW(L"BUTTON", Keys[6],
								WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_DLGFRAME,
								xKeyLoc[6], yKeyLoc[6], 50, 30,
								hwnd, NULL, hInst, NULL);
						}
						else if ((KeyName >= 1) && (KeyName <= 5))
						{
							CheckMenuItem(hMenu, IDM_ACCIDENTAL + accidental, MF_UNCHECKED);
							accidental = 1;
							CheckMenuItem(hMenu, IDM_ACCIDENTAL + accidental, MF_CHECKED);
							*(WORD*)&Keys[6][0] = '#F';
							DestroyWindow(hwndKeys[6]);
							hwndKeys[6] = CreateWindowW(L"BUTTON", Keys[6],
								WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_DLGFRAME,
								xKeyLoc[6], yKeyLoc[6], 50, 30,
								hwnd, NULL, hInst, NULL);
						}
						InvalidateRect(hwnd, &rect, FALSE);
						UpdateWindow(hwnd);
						if (showscale)
						{
							SendMessageW(hwndShowScale, BM_SETCHECK, BST_UNCHECKED, 0);
							SavedNote = Scale[KeyName];
							ScanCode = Codes[SavedNote - 36];//36 is the lowest MIDI note number in this program
							EndNote();
							SendMessageW(hwndShowScale, BM_SETCHECK, BST_CHECKED, 0);
							SavedNote = Scale[KeyName];
							ScanCode = Codes[SavedNote - 36];//36 is the lowest MIDI note number in this program
							StartNote();
						}
						break;
					}
				}
			}
			if (showchords)
			{
				for (x = 0; x < 9; x++)
				{
					if (lParam == (LONG_PTR)hwndChords[x])
					{
						SendMessageW(hwndChords[ChordType], BM_SETCHECK, BST_UNCHECKED, 0);
						ChordType = x;
						SendMessageW(hwndChords[ChordType], BM_SETCHECK, BST_CHECKED, 0);
						break;
					}
				}
				for (x = 0; x < 4; x++)
				{
					if (lParam == (LONG_PTR)hwndInversions[x])
					{
						SendMessageW(hwndInversions[Inversion], BM_SETCHECK, BST_UNCHECKED, 0);
						Inversion = x;
						SendMessageW(hwndInversions[Inversion], BM_SETCHECK, BST_CHECKED, 0);
						break;
					}
				}
			}
			SetFocus(hwnd);
			for (x = 0; x < 40; x++)
				ScanCodes[x] = 0;//just in case
		}
		return 0;

	case WM_KEYUP:
		ScanCode = HIWORD(lParam) & 0x0FF;
		if (ScanCode <= 53)
		{
			SavedNote = Notes[ScanCode];
			if (SavedNote != -1)
				EndNote();
		}
		return 0;

	case WM_KEYDOWN:
		if (0x40000000 & lParam)//bit 30
			return 0;//ignore typematics
		ScanCode = HIWORD(lParam) & 0x0FF;
		if (ScanCode <= 53)
		{
			SavedNote = Notes[ScanCode];
			if (SavedNote != -1)
			{
				Velocity = DefaultVelocity;
				StartNote();
			}
		}
		return 0;

	case WM_NCLBUTTONDBLCLK:
		return 0;//because the WM_LBUTTONUP message is also sent with the wrong lParam valuse

	case WM_LBUTTONUP:
		x = (xPos - ExtraSpace) / WhiteKeyWidth;
		if ((x + 26) > 60)
			return 0;
		if ((x >= 4) && (x <= 25))
			ScanCode = MouseKeys[x];
		else if (x < 4)
			ScanCode = MouseKeys[x + 26];
		else if (x > 25)
			ScanCode = MouseKeys[x + 4];
		if (yPos > (top + 100))
		{
			SavedNote = Notes[ScanCode];
			if (SavedNote != -1)
				EndNote();
		}
		else//if black key, get it from table
		{
			if (((xPos - ExtraSpace) % WhiteKeyWidth) > (WhiteKeyWidth * 2 / 3))
				ScanCode = RKeys[x];
			else if (((xPos - ExtraSpace) % WhiteKeyWidth) < (WhiteKeyWidth / 3))
				ScanCode = LKeys[x];
			if (ScanCode != -1)
			{
				SavedNote = Notes[ScanCode];
				if (SavedNote != -1)
					EndNote();
			}
		}
		//		InvalidateRect(hwnd, &rect, FALSE);
		return 0;

	case WM_LBUTTONDOWN:
		if ((yPos > (top)) && (yPos < (top + 150)) && (xPos > ExtraSpace) && (xPos < (WhiteKeyWidth * 35)))
		{
			x = (xPos - ExtraSpace) / WhiteKeyWidth;
			if ((x >= 4) && (x <= 25))
				ScanCode = MouseKeys[x];
			else if (x < 4)
				ScanCode = MouseKeys[x + 26];
			else if (x > 25)
				ScanCode = MouseKeys[x + 4];

			if (yPos > (top + 100))
			{
				SavedNote = Notes[ScanCode];
				if (SavedNote != -1)
				{
					if (ChordType == 0)
						EndNote();
					Velocity = DefaultVelocity;
					StartNote();
				}
			}
			else//if black key, get it from table
			{
				if (((xPos - ExtraSpace) % WhiteKeyWidth) > (WhiteKeyWidth * 2 / 3))
					ScanCode = RKeys[x];
				else if (((xPos - ExtraSpace) % WhiteKeyWidth) < (WhiteKeyWidth / 3))
					ScanCode = LKeys[x];
				if (ScanCode != -1)
				{
					SavedNote = Notes[ScanCode];
					if (SavedNote != -1)
					{
						if (ChordType == 0)
							EndNote();
						Velocity = DefaultVelocity;
						StartNote();
					}
				}
			}
		}
		return 0;

	case WM_MOUSEMOVE:
		xPos = LOWORD(lParam);
		yPos = HIWORD(lParam);
		if ((wParam == MK_LBUTTON) && !showchords && !showscale && !showtest)
		{
			if (ChordType != 0)
				EndNote();
			if ((saveleft != -1) && (fromkeydown) && (ChordType == 0))
			{
				if ((yPos > top) && (yPos < (top + 150)) && (xPos > ExtraSpace) && (xPos < (WhiteKeyWidth * 35)))
				{
					if (yKey[ScanCode] == 0)//on a black key
					{
						if ((xPos < (saveleft - WhiteKeyWidth / 3)) || (xPos > (saveleft + (WhiteKeyWidth / 3))))
						{
							EndNote();
							SendMessageW(hwnd, WM_LBUTTONDOWN, 0, lParam);
						}
					}
					else if (yKey[ScanCode] == 3)//on a white key with black keys on the left and right
					{
						if (yPos > (top + 100))
						{
							if ((xPos < saveleft) || (xPos > (saveleft + WhiteKeyWidth)))
							{
								EndNote();
								SendMessageW(hwnd, WM_LBUTTONDOWN, 0, lParam);
							}
						}
						else if ((xPos < (saveleft + WhiteKeyWidth / 3)) || (xPos > (saveleft + (WhiteKeyWidth * 2 / 3))))
						{
							EndNote();
							SendMessageW(hwnd, WM_LBUTTONDOWN, 0, lParam);
						}
					}
					else if (yKey[ScanCode] == 1)//on a white key with a black key on the left
					{
						if (yPos > (top + 100))
						{
							if ((xPos < saveleft) || (xPos > (saveleft + WhiteKeyWidth)))
							{
								EndNote();
								SendMessageW(hwnd, WM_LBUTTONDOWN, 0, lParam);
							}
						}
						else if ((xPos < (saveleft + WhiteKeyWidth / 3)) || (xPos > (saveleft + WhiteKeyWidth)))
						{
							EndNote();
							SendMessageW(hwnd, WM_LBUTTONDOWN, 0, lParam);
						}
					}
					else if (yKey[ScanCode] == 2)//on a white key with a black key on the right
					{
						if (yPos > (top + 100))
						{
							if ((xPos < saveleft) || (xPos > (saveleft + WhiteKeyWidth)))
							{
								EndNote();
								SendMessageW(hwnd, WM_LBUTTONDOWN, 0, lParam);
							}
						}
						else if ((xPos < saveleft) || (xPos > (saveleft + (WhiteKeyWidth * 2 / 3))))
						{
							EndNote();
							SendMessageW(hwnd, WM_LBUTTONDOWN, 0, lParam);
						}
					}
					else
						return 0;
				}
				else
					EndNote();
			}
		}
		return 0;

	case WM_USER:
		if ((wParam & 0xF0) == 0x80) { // note off
			wParam = 0x90;
			lParam &= 0xFFFF00FF;// make velocity 0
		}
		if (wParam == 0x90)//Note On or Off
		{
			Velocity = (lParam >> 8) & 0xFF;
			SavedNote = lParam & 0xFF;
			ScanCode = Codes[SavedNote - 36];//36 is the lowest MIDI note number in this program
			if (Velocity)
			{
				Velocity += 25;
				if (Velocity > 127)
					Velocity = 127;
				StartNote();
			}
			else//0 velocity means end note
			{
				EndNote();
			}
		}
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		if (first)
		{
			first = FALSE;
			hOldFont = SelectObject(hdc, hMaestroFont);
			GetCharWidth32W(hdc, 61, 61, StaffWidth);//61 is '='
			GetCharWidth32W(hdc, 119, 119, NoteWidth);//119 is 'w'
			GetCharWidth32W(hdc, 38, 38, GClefWidth);//38 is '&'
			GetCharWidth32W(hdc, 63, 63, FClefWidth);//63 is '?'
			NumOfStaffs = rect.right / StaffWidth[0];
			if (NumOfStaffs > 128)
				NumOfStaffs = 128;
			for (x = 0; x < NumOfStaffs; x++)
				Staff[x] = '=';
			Staff[x] = 0;
			SelectObject(hdc, hSmallFont);
			for (x = 0; x < 22; x++)
			{
				GetCharWidth32W(hdc, ComputerKeys1[x], ComputerKeys1[x], &Widths1[x]);
				GetCharWidth32W(hdc, ComputerKeys2[x], ComputerKeys2[x], &Widths2[x]);
			}
			SelectObject(hdc, hFont);
			for (x = 0; x < 7; x++)
				GetCharWidth32W(hdc, CDEFGAB[x], CDEFGAB[x], &Widths[x]);

			hdcMem = CreateCompatibleDC(hdc);
			hMemBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
			SelectObject(hdcMem, hMemBitmap);
		}
		SelectObject(hdc, hMaestroFont);
		TextOutW(hdc, ExtraSpace, top - 212, Staff, NumOfStaffs);
		TextOutW(hdc, ExtraSpace, top - 121, Staff, NumOfStaffs);
		SetBkMode(hdc, TRANSPARENT);
		TextOutW(hdc, ExtraSpace, top - 224, L"&", 1); // treble clef
		TextOutW(hdc, ExtraSpace - 1, top - 212, L"\\", 1);
		TextOutW(hdc, ExtraSpace, top - 158, L"?", 1); // bass clef
		TextOutW(hdc, ExtraSpace - 1, top - 121, L"\\", 1);
		TextOutW(hdc, ExtraSpace - 1, top - 169, L"\\", 1);
		//wchar_t Keys[12][3] = {"C ","F ","Bb","Eb","Ab","Db","Gb","B ","E ","A ","D ","G "};
		//wchar_t Keys[12][3] = {"C ","G ","D ","A ","E ","B ","Gb","Db","Ab","Eb","Bb","F "};
		if ((showkeys) && (KeyName))
		{
			switch (KeyName)
			{
			case 5:
				TextOutW(hdc, ExtraSpace + 100, top - 230, L"#", 1);
				TextOutW(hdc, ExtraSpace + 100, top - 127, L"#", 1);
			case 4:
				TextOutW(hdc, ExtraSpace + 85, top - 248, L"#", 1);
				TextOutW(hdc, ExtraSpace + 85, top - 145, L"#", 1);
			case 3:
				TextOutW(hdc, ExtraSpace + 70, top - 266, L"#", 1);
				TextOutW(hdc, ExtraSpace + 70, top - 163, L"#", 1);
			case 2:
				TextOutW(hdc, ExtraSpace + 55, top - 242, L"#", 1);
				TextOutW(hdc, ExtraSpace + 55, top - 139, L"#", 1);
			case 1:
				TextOutW(hdc, ExtraSpace + 40, top - 260, L"#", 1);
				TextOutW(hdc, ExtraSpace + 40, top - 157, L"#", 1);
				break;
			case 6:
				if (accidental == 2)
				{
					TextOutW(hdc, ExtraSpace + 115, top - 242, L"b", 1);
					TextOutW(hdc, ExtraSpace + 115, top - 139, L"b", 1);
				}
				else//if (accidental == 1)
				{
					TextOutW(hdc, ExtraSpace + 115, top - 254, L"#", 1);
					TextOutW(hdc, ExtraSpace + 115, top - 151, L"#", 1);

					TextOutW(hdc, ExtraSpace + 100, top - 230, L"#", 1);
					TextOutW(hdc, ExtraSpace + 100, top - 127, L"#", 1);

					TextOutW(hdc, ExtraSpace + 85, top - 248, L"#", 1);
					TextOutW(hdc, ExtraSpace + 85, top - 145, L"#", 1);

					TextOutW(hdc, ExtraSpace + 70, top - 266, L"#", 1);
					TextOutW(hdc, ExtraSpace + 70, top - 163, L"#", 1);

					TextOutW(hdc, ExtraSpace + 55, top - 242, L"#", 1);
					TextOutW(hdc, ExtraSpace + 55, top - 139, L"#", 1);

					TextOutW(hdc, ExtraSpace + 40, top - 260, L"#", 1);
					TextOutW(hdc, ExtraSpace + 40, top - 157, L"#", 1);
					break;
				}
			case 7:
				TextOutW(hdc, ExtraSpace + 100, top - 224, L"b", 1);
				TextOutW(hdc, ExtraSpace + 100, top - 121, L"b", 1);
			case 8:
				TextOutW(hdc, ExtraSpace + 85, top - 248, L"b", 1);
				TextOutW(hdc, ExtraSpace + 85, top - 145, L"b", 1);
			case 9:
				TextOutW(hdc, ExtraSpace + 70, top - 230, L"b", 1);
				TextOutW(hdc, ExtraSpace + 70, top - 127, L"b", 1);
			case 10:
				TextOutW(hdc, ExtraSpace + 55, top - 254, L"b", 1);
				TextOutW(hdc, ExtraSpace + 55, top - 151, L"b", 1);
			case 11:
				TextOutW(hdc, ExtraSpace + 40, top - 236, L"b", 1);
				TextOutW(hdc, ExtraSpace + 40, top - 133, L"b", 1);
			}
		}
		//		SetTextColor(hdc, 0xD0D0D0);
		//		SelectObject(hdc, hFont);
		//		if (showkeys == FALSE)
		//			for (x = 0; x < 35; x++)
		//				TextOutW(hdc, (x*WhiteKeyWidth) + ExtraSpace + (WhiteKeyWidth-Widths[x%7])/2, top-139, &CDEFGAB[x % 7], 1);
		SelectObject(hdc, hSmallFont);
		if (showkeys == FALSE)
		{
			SetTextColor(hdc, 0x6060D0);
			TextOutW(hdc, 50, top - 200, L"F", 1);
			TextOutW(hdc, 50, top - 188, L"D", 1);
			TextOutW(hdc, 50, top - 176, L"B", 1);
			TextOutW(hdc, 50, top - 164, L"G", 1);
			TextOutW(hdc, 50, top - 152, L"E", 1);

			TextOutW(hdc, 65, top - 205, L"G", 1);
			TextOutW(hdc, 65, top - 193, L"E", 1);
			TextOutW(hdc, 65, top - 181, L"C", 1);
			TextOutW(hdc, 65, top - 169, L"A", 1);
			TextOutW(hdc, 65, top - 157, L"F", 1);
			TextOutW(hdc, 65, top - 145, L"D", 1);

			TextOutW(hdc, 50, top - 109, L"A", 1);
			TextOutW(hdc, 50, top - 97, L"F", 1);
			TextOutW(hdc, 50, top - 85, L"D", 1);
			TextOutW(hdc, 50, top - 73, L"B", 1);
			TextOutW(hdc, 50, top - 61, L"G", 1);

			TextOutW(hdc, 65, top - 114, L"B", 1);
			TextOutW(hdc, 65, top - 102, L"G", 1);
			TextOutW(hdc, 65, top - 90, L"E", 1);
			TextOutW(hdc, 65, top - 78, L"C", 1);
			TextOutW(hdc, 65, top - 66, L"A", 1);
			TextOutW(hdc, 65, top - 54, L"F", 1);

			TextOutW(hdc, 50, top - 130, L"C", 1);
		}
		SetTextColor(hdc, 0x6060D0);
		for (x = ExtraSpace + (4 * WhiteKeyWidth), z = 0; z < 10; x += WhiteKeyWidth, z++)
			TextOutW(hdc, x + ((WhiteKeyWidth - Widths1[z]) / 2), top + 155, &ComputerKeys1[z], 1);
		SetTextColor(hdc, 0xFF0000);//blue
		for (; z < 22; x += WhiteKeyWidth, z++)
			TextOutW(hdc, x + ((WhiteKeyWidth - Widths1[z]) / 2), top + 155, &ComputerKeys1[z], 1);
		SetBkMode(hdc, OPAQUE);

		for (x = (4 * WhiteKeyWidth) + ExtraSpace - (WhiteKeyWidth / 2), z = 0; z < 10; x += WhiteKeyWidth, z++)
		{
			if ((z == 3) || (z == 6) || (z == 10) || (z == 13) || (z == 17) || (z == 20))
				SetTextColor(hdc, 0xD0D0D0);
			else
				SetTextColor(hdc, 0x6060D0);//reddish
			TextOutW(hdc, x + ((WhiteKeyWidth - Widths2[z]) / 2), top - 22, &ComputerKeys2[z], 1);
		}
		SetTextColor(hdc, 0x6060D0);
		for (; z < 22; x += WhiteKeyWidth, z++)
		{
			if ((z == 3) || (z == 6) || (z == 10) || (z == 13) || (z == 17) || (z == 20))
				SetTextColor(hdc, 0xD0D0D0);
			else
				SetTextColor(hdc, 0xFF0000);//blue
			TextOutW(hdc, x + ((WhiteKeyWidth - Widths2[z]) / 2), top - 22, &ComputerKeys2[z], 1);
		}

		hOldPen = SelectObject(hdc, hPen);
		hOldBrush = SelectObject(hdc, hWhiteBrush);

		for (x = ExtraSpace; x < (35 * WhiteKeyWidth); x += WhiteKeyWidth)
			Rectangle(hdc, x, top, x + WhiteKeyWidth, top + 150);
		SelectObject(hdc, hBlackBrush);
		for (x = ExtraSpace; x < (ExtraSpace + (35 * WhiteKeyWidth)); x += (7 * WhiteKeyWidth))
		{
			left = x + BlackKeyWidth;
			Rectangle(hdc, left, top, left + BlackKeyWidth, top + 100);
			left = x + (WhiteKeyWidth * 5 / 3);
			Rectangle(hdc, left, top, left + BlackKeyWidth, top + 100);
			left = x + (WhiteKeyWidth * 11 / 3);
			Rectangle(hdc, left, top, left + BlackKeyWidth, top + 100);
			left = x + (WhiteKeyWidth * 14 / 3);
			Rectangle(hdc, left, top, left + BlackKeyWidth, top + 100);
			left = x + (WhiteKeyWidth * 17 / 3);
			Rectangle(hdc, left, top, left + BlackKeyWidth, top + 100);
			left = -1;
		}
		if (showkeys == FALSE)
		{
			SetTextColor(hdc, 0xE4E4E4);
			SelectObject(hdc, hFont);
			for (x = 0; x < 35; x++)
				TextOutW(hdc, (x * WhiteKeyWidth) + ExtraSpace + (WhiteKeyWidth - Widths[x % 7]) / 2, top + 110, &CDEFGAB[x % 7], 1);
		}
		SelectObject(hdc, hOldPen);
		SelectObject(hdc, hOldBrush);
		/////////////////////////////////////////////////////////////////
		BitBlt(hdcMem, 0, top260, rect.right, rect.bottom, hdc, 0, top260, SRCCOPY);
		/////////////////////////////////////////////////////////////////
		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		RemoveFontResourceW(L"MAESTRO_.TTF");
		if (midi_in)
		{
			midiInStop(hMidiIn);
			midiInReset(hMidiIn);
			midiInClose(hMidiIn);
		}
		midiOutShortMsg(hMidiOut, 0xB0 | (123 << 8));//set controller for channel 0 to all voices off
		midiOutReset(hMidiOut);
		midiOutClose(hMidiOut);
		DeleteObject(hPen);
		DeleteObject(hWhiteBrush);
		DeleteObject(hBlackBrush);
		DeleteObject(hBlueBrush);
		DeleteObject(hFont);
		DeleteObject(hSmallFont);
		DeleteObject(hMaestroFont);
		DeleteObject(hMemBitmap);
		DeleteObject(hOldFont);
		DeleteObject(hOldPen);
		DeleteObject(hOldBrush);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProcW(hwnd, message, wParam, lParam);
}

//SUBROUTINES////////////////////////////////////////////////////////////
void DrawKey(void)
{
	UpdateWindow(hwnd);
	hdc = GetDC(hwnd);
	//	{
	//		wchar_t asdf[10];
	//		_itoa(Note, asdf, 10);
	//		TextOutW(hdc, 0, 0, asdf, lstrlen(asdf));
	//	}
	hOldPen = SelectObject(hdc, hPen);
	hOldFont = SelectObject(hdc, hMaestroFont);
	if (fromkeydown == FALSE)//clear notation
	{
		/////////////////////////////////////////////////////////////////
		BitBlt(hdc, 0, top260, rect.right, rect.bottom, hdcMem, 0, top260, SRCCOPY);
		BitBlt(hdc, rect.right - 140, top260 - 20, 140, 20, hdc, rect.right - 180, top260 - 20, SRCCOPY);//to clear parts of high accidentals
		/////////////////////////////////////////////////////////////////
		if ((showtest) && (Note == RandomNote))
			NoteTest();
	}
	left = (xKey[ScanCode] * WhiteKeyWidth) + ExtraSpace;
	saveleft = left;
	BracketingKeys = yKey[ScanCode];
	if (BracketingKeys == NEITHER)
	{//black keys
		if (fromkeydown)
		{
			SetTextColor(hdc, 0xFF0000);
			SelectObject(hdc, hFont);
			SetBkMode(hdc, TRANSPARENT);
			if (accidental == 2) // flat
				TextOutW(hdc, left - 20 + (WhiteKeyWidth - Widths[xKey[ScanCode] % 7]) / 2, top - 139, &CDEFGAB[xKey[ScanCode] % 7], 1);
			else//if (accidental == 1)
				if (ScanCode != 3) // not middle C
					TextOutW(hdc, left - (WhiteKeyWidth + Widths[(xKey[ScanCode] - 1) % 7]) / 2, top - 139, &CDEFGAB[(xKey[ScanCode] - 1) % 7], 1);
			SelectObject(hdc, hMaestroFont);
			if (accidental == 1)
				TextOutW(hdc, left - (WhiteKeyWidth / 3) - 26, yStaff[Note - 36], L"#w", 2);
			else
				TextOutW(hdc, left - (WhiteKeyWidth / 3) + 5, yStaff[Note - 35], L"bw", 2);
			SetBkMode(hdc, OPAQUE);
			SelectObject(hdc, hBlueBrush);
		}
		else
			SelectObject(hdc, hBlackBrush);
		SetTextColor(hdc, 0);//black
		left -= WhiteKeyWidth / 3;
		Rectangle(hdc, left, top, left + BlackKeyWidth, top + 100);
		SelectObject(hdc, hOldBrush);
		if (fromkeydown)
		{
			if (accidental == 1)
			{
				switch (xKey[ScanCode])
				{
				case 15://middle C#
					//					MoveToEx(hdc, left-(BlackKeyWidth/3), top-122, NULL);//-1
					//					LineTo(hdc, left+(BlackKeyWidth*4/3), top-122);//+29
					MoveToEx(hdc, left - BlackKeyWidth, top - 122, NULL);//-1
					LineTo(hdc, left + WhiteKeyWidth - BlackKeyWidth, top - 122);//+29
					break;
				case 1://low C#
					MoveToEx(hdc, left - BlackKeyWidth, top - 28, NULL);
					LineTo(hdc, left + WhiteKeyWidth - BlackKeyWidth, top - 28);
				case 2://low D#
					MoveToEx(hdc, left - BlackKeyWidth, top - 40, NULL);
					LineTo(hdc, left + WhiteKeyWidth - BlackKeyWidth, top - 40);
					break;
				case 33://very high G#
				case 34://very high A#
					MoveToEx(hdc, left - BlackKeyWidth, top - 238, NULL);
					LineTo(hdc, left + WhiteKeyWidth - BlackKeyWidth, top - 238);
				case 32://very high F#
					MoveToEx(hdc, left - BlackKeyWidth, top - 226, NULL);
					LineTo(hdc, left + WhiteKeyWidth - BlackKeyWidth, top - 226);
				case 30://high C#
				case 29://high D#
					MoveToEx(hdc, left - BlackKeyWidth, top - 214, NULL);
					LineTo(hdc, left + WhiteKeyWidth - BlackKeyWidth, top - 214);
				case 27://high A#
					MoveToEx(hdc, left - BlackKeyWidth, top - 202, NULL);
					LineTo(hdc, left + WhiteKeyWidth - BlackKeyWidth, top - 202);
					break;
				}
			}
			else//if (accidental == 2)
			{
				switch (Note)
				{
				case 37://low Db
				case 39://low Eb
					//						MoveToEx(hdc, left-(BlackKeyWidth/3), top-40, NULL);
					//						LineTo(hdc, left+(BlackKeyWidth*4/3), top-40);
					MoveToEx(hdc, left, top - 40, NULL);
					LineTo(hdc, left + WhiteKeyWidth, top - 40);
					break;
					MoveToEx(hdc, left, top - 202, NULL);
					LineTo(hdc, left + WhiteKeyWidth, top - 202);
					break;
				case 94://very high Bb
					MoveToEx(hdc, left, top - 250, NULL);
					LineTo(hdc, left + WhiteKeyWidth, top - 250);
				case 92://very high Ab
				case 90://high Gb
					MoveToEx(hdc, left, top - 238, NULL);
					LineTo(hdc, left + WhiteKeyWidth, top - 238);
				case 87://high Db
					MoveToEx(hdc, left, top - 226, NULL);
					LineTo(hdc, left + WhiteKeyWidth, top - 226);
				case 85://high Cb
					MoveToEx(hdc, left, top - 214, NULL);
					LineTo(hdc, left + WhiteKeyWidth, top - 214);
				case 80://high Ab
				case 82://high Bb
					MoveToEx(hdc, left, top - 202, NULL);
					LineTo(hdc, left + WhiteKeyWidth, top - 202);
					break;

				}
			}
		}
	}
	else if (BracketingKeys != -1)
	{//white keys
		if (fromkeydown)
		{
			SetTextColor(hdc, 0xFF0000);
			SelectObject(hdc, hFont);
			SetBkMode(hdc, TRANSPARENT);
			if (ScanCode != 16) // not middle C
				TextOutW(hdc, left + (WhiteKeyWidth - Widths[xKey[ScanCode] % 7]) / 2, top - 139, &CDEFGAB[xKey[ScanCode] % 7], 1);
			SetBkMode(hdc, OPAQUE);

			SetTextColor(hdc, 0);
			SelectObject(hdc, hMaestroFont);
			SetBkMode(hdc, TRANSPARENT);
			SetTextColor(hdc, 0xFF0000);
			//			if (ScanCode != 16) // not middle C
			TextOutW(hdc, left + (WhiteKeyWidth - Widths[xKey[ScanCode] % 7]) / 2, yStaff[Note - 36], L"w", 1);
			//			SelectObject(hdc,hMaestroFont);
			SetTextColor(hdc, 0);
			switch (xKey[ScanCode])
			{
			case 14://middle C
				MoveToEx(hdc, left, top - 122, NULL);
				LineTo(hdc, left + WhiteKeyWidth, top - 122);
				break;
			case 0://low C
				MoveToEx(hdc, left, top - 28, NULL);
				LineTo(hdc, left + WhiteKeyWidth, top - 28);
				MoveToEx(hdc, left, top - 40, NULL);
				LineTo(hdc, left + WhiteKeyWidth, top - 40);
				break;
			case 1://low D
			case 2://low E
				MoveToEx(hdc, left, top - 40, NULL);
				LineTo(hdc, left + WhiteKeyWidth, top - 40);
				break;
			case 34://very high B
				MoveToEx(hdc, left, top - 250, NULL);
				LineTo(hdc, left + WhiteKeyWidth, top - 250);
			case 32://very high G
			case 33://very high A
				MoveToEx(hdc, left, top - 238, NULL);
				LineTo(hdc, left + WhiteKeyWidth, top - 238);
			case 30://high E
			case 31://high F
				MoveToEx(hdc, left, top - 226, NULL);
				LineTo(hdc, left + WhiteKeyWidth, top - 226);
			case 28://high C
			case 29://high D
				MoveToEx(hdc, left, top - 214, NULL);
				LineTo(hdc, left + WhiteKeyWidth, top - 214);
			case 26://high A
			case 27://high B
				MoveToEx(hdc, left, top - 202, NULL);
				LineTo(hdc, left + WhiteKeyWidth, top - 202);
				break;
			}
			SetBkMode(hdc, OPAQUE);
			SelectObject(hdc, hOldFont);
			SelectObject(hdc, hBlueBrush);
		}
		Rectangle(hdc, left, top, left + WhiteKeyWidth, top + 150);
		if ((!fromkeydown) && (showkeys == FALSE))
		{
			SetTextColor(hdc, 0xE4E4E4);
			SelectObject(hdc, hFont);
			x = xKey[ScanCode];
			TextOutW(hdc, (x * WhiteKeyWidth) + ExtraSpace + (WhiteKeyWidth - Widths[x % 7]) / 2, top + 110, &CDEFGAB[x % 7], 1);
		}
		if (PreviousNote != (Note - 1))
			SelectObject(hdc, hBlackBrush);
		else
			SelectObject(hdc, hBlueBrush);
		if (BracketingKeys == LEFT)
		{
			left -= (WhiteKeyWidth / 3);
			Rectangle(hdc, left, top, left + BlackKeyWidth, top + 100);
		}
		else if (BracketingKeys == RIGHT)
		{
			left += (WhiteKeyWidth * 2 / 3);
			Rectangle(hdc, left, top, left + BlackKeyWidth, top + 100);
		}
		else if (BracketingKeys == BOTH)
		{
			left -= (WhiteKeyWidth / 3);
			Rectangle(hdc, left, top, left + BlackKeyWidth, top + 100);
			left += WhiteKeyWidth;
			Rectangle(hdc, left, top, left + BlackKeyWidth, top + 100);
		}
		SelectObject(hdc, hOldBrush);
	}
	SelectObject(hdc, hOldPen);
	SelectObject(hdc, hOldFont);
	ReleaseDC(hwnd, hdc);
}

static void StartShowAndPlay(void)
{
	midiOutShortMsg(hMidiOut, 0x90 | (Velocity << 16) | (Note << 8));
	fromkeydown = TRUE;
	if ((Note >= 36) && (Note <= 96))
	{
		DrawKey();
		for (x = 0; (x < 40) && (ScanCodes[x] != 0); x++)
			;
		if (x < 40)
			ScanCodes[x] = ScanCode;
	}
}

void StartNote(void)
{//play note
	if (showscale)//All Notes in Scale
	{
		saveAccidental = accidental;
		if ((SavedNote == 41) || (SavedNote == 53) || (SavedNote == 65) || (SavedNote == 77) || (SavedNote == 89))
			accidental = 2;//Key of F
		else if ((SavedNote != 42) && (SavedNote != 54) && (SavedNote != 66) && (SavedNote != 78) && (SavedNote != 90))
		{
			accidental = 1;
			for (x = 0; x < 25; x++)
			{
				if (SavedNote == BlackKeyNotes[x])
				{
					accidental = 2;//if it's a flat key
					break;
				}
			}
		}
		for (x = 0; x < 7; x++)
		{
			Note = SavedNote + Interval[x];
			if (Note > 96)
				Note = 96;
			ScanCode = Codes[Note - 36];
			fromkeydown = TRUE;
			if ((Note >= 36) && (Note <= 96))
				DrawKey();
			PreviousNote = Note;
		}
		PreviousNote = -1;
		accidental = saveAccidental;
		return;
	}

	//	if ((showtest) && (SavedNote == 95))
	//		SendMessageW(hwnd, WM_COMMAND, IDM_TEST, 0);
	Note = SavedNote;
	StartShowAndPlay();
	if (ChordType == 1)//Major Triad
	{
		Note = SavedNote + 4;
		if (Inversion == 1)
			Note -= 12;
		if (Note <= 96)
		{
			ScanCode = Codes[Note - 36];
			StartShowAndPlay();
			Note = SavedNote + 7;
			if ((Inversion == 1) || (Inversion == 2))
				Note -= 12;
			if (Note <= 96)
			{
				ScanCode = Codes[Note - 36];
				StartShowAndPlay();
			}
		}
	}
	else if (ChordType == 2)//Minor Triad
	{
		Note = SavedNote + 3;
		if (Inversion == 1)
			Note -= 12;
		if (Note <= 96)
		{
			ScanCode = Codes[Note - 36];
			StartShowAndPlay();
			Note = SavedNote + 7;
			if ((Inversion == 1) || (Inversion == 2))
				Note -= 12;
			if (Note <= 96)
			{
				ScanCode = Codes[Note - 36];
				StartShowAndPlay();
			}
		}
	}
	else if (ChordType == 3)//Diminished Triad
	{
		Note = SavedNote + 3;
		if (Inversion == 1)
			Note -= 12;
		if (Note <= 96)
		{
			ScanCode = Codes[Note - 36];
			StartShowAndPlay();
			Note = SavedNote + 6;
			if ((Inversion == 1) || (Inversion == 2))
				Note -= 12;
			if (Note <= 96)
			{
				ScanCode = Codes[Note - 36];
				StartShowAndPlay();
			}
		}
	}
	else if (ChordType == 4)//Augmented Triad
	{
		Note = SavedNote + 4;
		if (Inversion == 1)
			Note -= 12;
		if (Note <= 96)
		{
			ScanCode = Codes[Note - 36];
			StartShowAndPlay();
			Note = SavedNote + 8;
			if ((Inversion == 1) || (Inversion == 2))
				Note -= 12;
			if (Note <= 96)
			{
				ScanCode = Codes[Note - 36];
				StartShowAndPlay();
			}
		}
	}
	else if (ChordType == 5)//Dominant 7th
	{
		Note = SavedNote + 4;
		if (Inversion == 1)
			Note -= 12;
		if (Note <= 96)
		{
			ScanCode = Codes[Note - 36];
			StartShowAndPlay();
			Note = SavedNote + 7;
			if ((Inversion == 1) || (Inversion == 2))
				Note -= 12;
			if (Note <= 96)
			{
				ScanCode = Codes[Note - 36];
				StartShowAndPlay();
				Note = SavedNote + 10;
				if ((Inversion == 1) || (Inversion == 2) || (Inversion == 3))
					Note -= 12;
				if (Note <= 96)
				{
					ScanCode = Codes[Note - 36];
					StartShowAndPlay();
				}
			}
		}
	}
	else if (ChordType == 6)//Major 7th
	{
		Note = SavedNote + 4;
		if (Inversion == 1)
			Note -= 12;
		if (Note <= 96)
		{
			ScanCode = Codes[Note - 36];
			StartShowAndPlay();
			Note = SavedNote + 7;
			if ((Inversion == 1) || (Inversion == 2))
				Note -= 12;
			if (Note <= 96)
			{
				ScanCode = Codes[Note - 36];
				StartShowAndPlay();
				Note = SavedNote + 11;
				if ((Inversion == 1) || (Inversion == 2) || (Inversion == 3))
					Note -= 12;
				if (Note <= 96)
				{
					ScanCode = Codes[Note - 36];
					StartShowAndPlay();
				}
			}
		}
	}
	else if (ChordType == 7)//Minor 7th
	{
		Note = SavedNote + 3;
		if (Inversion == 1)
			Note -= 12;
		if (Note <= 96)
		{
			ScanCode = Codes[Note - 36];
			StartShowAndPlay();
			Note = SavedNote + 7;
			if ((Inversion == 1) || (Inversion == 2))
				Note -= 12;
			if (Note <= 96)
			{
				ScanCode = Codes[Note - 36];
				StartShowAndPlay();
				Note = SavedNote + 10;
				if ((Inversion == 1) || (Inversion == 2) || (Inversion == 3))
					Note -= 12;
				if (Note <= 96)
				{
					ScanCode = Codes[Note - 36];
					StartShowAndPlay();
				}
			}
		}
	}
	else if (ChordType == 8)//Diminished 7th
	{
		Note = SavedNote + 3;
		if (Inversion == 1)
			Note -= 12;
		if (Note <= 96)
		{
			ScanCode = Codes[Note - 36];
			StartShowAndPlay();
			Note = SavedNote + 6;
			if ((Inversion == 1) || (Inversion == 2))
				Note -= 12;
			if (Note <= 96)
			{
				ScanCode = Codes[Note - 36];
				StartShowAndPlay();
				Note = SavedNote + 9;
				if ((Inversion == 1) || (Inversion == 2) || (Inversion == 3))
					Note -= 12;
				if (Note <= 96)
				{
					ScanCode = Codes[Note - 36];
					StartShowAndPlay();
				}
			}
		}
	}
}

static void EndShowAndPlay(void)
{
	midiOutShortMsg(hMidiOut, 0x90 | (Note << 8));//0 Velocity
	fromkeydown = FALSE;
	if ((Note >= 36) && (Note <= 96))
		DrawKey();
	for (x = 0; (x < 40) && (ScanCodes[x] != ScanCode); x++)
		;
	if (x < 40)
		ScanCodes[x] = 0;
	else
		for (x = 0; x < 40; x++)
			ScanCodes[x] = 0;
	fromkeydown = TRUE;
	if (ChordType == 0)
	{
		for (x = 0; x < 40; x++)
		{
			if (ScanCodes[x] != 0)
			{
				ScanCode = ScanCodes[x];
				Note = Notes[ScanCode];
				DrawKey();//doesn't use x
			}
		}
	}
}

void EndNote(void)
{
	if (showscale)//All Notes in Scale
	{
		saveAccidental = accidental;
		accidental = 1;
		for (x = 0; x < 25; x++)
			if (SavedNote == BlackKeyNotes[x])
			{
				accidental = 2;//if it's a flat key
				break;
			}
		for (x = 0; x < 7; x++)
		{
			Note = SavedNote + Interval[x];
			ScanCode = Codes[Note - 36];
			fromkeydown = FALSE;
			if (Note <= 96)
				DrawKey();
			PreviousNote = Note;
		}
		PreviousNote = -1;
		accidental = saveAccidental;
		return;
	}

	Note = SavedNote;
	EndShowAndPlay();
	if (ChordType == 1)
	{
		Note = SavedNote + 4;
		if (Inversion == 1)
			Note -= 12;
		if (Note <= 96)
		{
			ScanCode = Codes[Note - 36];
			EndShowAndPlay();
			Note = SavedNote + 7;
			if ((Inversion == 1) || (Inversion == 2))
				Note -= 12;
			if (Note <= 96)
			{
				ScanCode = Codes[Note - 36];
				EndShowAndPlay();
			}
		}
	}
	else if (ChordType == 2)
	{
		Note = SavedNote + 3;
		if (Inversion == 1)
			Note -= 12;
		if (Note <= 96)
		{
			ScanCode = Codes[Note - 36];
			EndShowAndPlay();
			Note = SavedNote + 7;
			if ((Inversion == 1) || (Inversion == 2))
				Note -= 12;
			if (Note <= 96)
			{
				ScanCode = Codes[Note - 36];
				EndShowAndPlay();
			}
		}
	}
	else if (ChordType == 3)
	{
		Note = SavedNote + 3;
		if (Inversion == 1)
			Note -= 12;
		if (Note <= 96)
		{
			ScanCode = Codes[Note - 36];
			EndShowAndPlay();
			Note = SavedNote + 6;
			if ((Inversion == 1) || (Inversion == 2))
				Note -= 12;
			if (Note <= 96)
			{
				ScanCode = Codes[Note - 36];
				EndShowAndPlay();
			}
		}
	}
	else if (ChordType == 4)
	{
		Note = SavedNote + 4;
		if (Inversion == 1)
			Note -= 12;
		if (Note <= 96)
		{
			ScanCode = Codes[Note - 36];
			EndShowAndPlay();
			Note = SavedNote + 8;
			if ((Inversion == 1) || (Inversion == 2))
				Note -= 12;
			if (Note <= 96)
			{
				ScanCode = Codes[Note - 36];
				EndShowAndPlay();
			}
		}
	}
	else if (ChordType == 5)
	{
		Note = SavedNote + 4;
		if (Inversion == 1)
			Note -= 12;
		if (Note <= 96)
		{
			ScanCode = Codes[Note - 36];
			EndShowAndPlay();
			Note = SavedNote + 7;
			if ((Inversion == 1) || (Inversion == 2))
				Note -= 12;
			if (Note <= 96)
			{
				ScanCode = Codes[Note - 36];
				EndShowAndPlay();
				Note = SavedNote + 10;
				if ((Inversion == 1) || (Inversion == 2) || (Inversion == 3))
					Note -= 12;
				if (Note <= 96)
				{
					ScanCode = Codes[Note - 36];
					EndShowAndPlay();
				}
			}
		}
	}
	else if (ChordType == 6)
	{
		Note = SavedNote + 4;
		if (Inversion == 1)
			Note -= 12;
		if (Note <= 96)
		{
			ScanCode = Codes[Note - 36];
			EndShowAndPlay();
			Note = SavedNote + 7;
			if ((Inversion == 1) || (Inversion == 2))
				Note -= 12;
			if (Note <= 96)
			{
				ScanCode = Codes[Note - 36];
				EndShowAndPlay();
				Note = SavedNote + 11;
				if ((Inversion == 1) || (Inversion == 2) || (Inversion == 3))
					Note -= 12;
				if (Note <= 96)
				{
					ScanCode = Codes[Note - 36];
					EndShowAndPlay();
				}
			}
		}
	}
	else if (ChordType == 7)//Minor 7th
	{
		Note = SavedNote + 3;
		if (Inversion == 1)
			Note -= 12;
		if (Note <= 96)
		{
			ScanCode = Codes[Note - 36];
			EndShowAndPlay();
			Note = SavedNote + 7;
			if ((Inversion == 1) || (Inversion == 2))
				Note -= 12;
			if (Note <= 96)
			{
				ScanCode = Codes[Note - 36];
				EndShowAndPlay();
				Note = SavedNote + 10;
				if ((Inversion == 1) || (Inversion == 2) || (Inversion == 3))
					Note -= 12;
				if (Note <= 96)
				{
					ScanCode = Codes[Note - 36];
					EndShowAndPlay();
				}
			}
		}
	}
	else if (ChordType == 8)//Diminished 7th
	{
		Note = SavedNote + 3;
		if (Inversion == 1)
			Note -= 12;
		if (Note <= 96)
		{
			ScanCode = Codes[Note - 36];
			EndShowAndPlay();
			Note = SavedNote + 6;
			if ((Inversion == 1) || (Inversion == 2))
				Note -= 12;
			if (Note <= 96)
			{
				ScanCode = Codes[Note - 36];
				EndShowAndPlay();
				Note = SavedNote + 9;
				if ((Inversion == 1) || (Inversion == 2) || (Inversion == 3))
					Note -= 12;
				if (Note <= 96)
				{
					ScanCode = Codes[Note - 36];
					EndShowAndPlay();
				}
			}
		}
	}
}

void CALLBACK MidiInProc(HMIDIIN hMidiIn, WORD wMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{//from midiInOpen (if a MIDI keyboard is attached)
	if (wMsg == MIM_DATA)
		PostMessageW(hwnd, WM_USER, (WPARAM)dwParam1 & 0xFF, (LPARAM)(dwParam1 >> 8) & 0xFFFF);//dwParam1 contains velocity, note, and status bytes
}

int Atoi(wchar_t* ptr)
{
	int x;

	for (x = 0; (*ptr >= '0') && (*ptr <= '9'); ptr++)
	{
		x *= 10;
		x += *ptr - '0';
	}
	return x;
}

void WriteIni(void)
{
	hFile = CreateFileW(SimplePianoIni, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	if (iInDevice != 0)
	{
		WriteFile(hFile, InDev, 13, &dwBytesWritten, NULL);
		_itow_s(iInDevice, temp, 16, 10);
		WriteFile(hFile, temp, lstrlen(temp), &dwBytesWritten, NULL);
		WriteFile(hFile, L"\r\n", 2, &dwBytesWritten, NULL);
	}
	WriteFile(hFile, OutDev, 14, &dwBytesWritten, NULL);
	_itow_s(iOutDevice, temp, 16, 10);
	WriteFile(hFile, temp, lstrlen(temp), &dwBytesWritten, NULL);
	WriteFile(hFile, L"\r\n", 2, &dwBytesWritten, NULL);
	WriteFile(hFile, Vel, 17, &dwBytesWritten, NULL);
	_itow_s(DefaultVelocity, temp, 16, 10);
	WriteFile(hFile, temp, lstrlen(temp), &dwBytesWritten, NULL);
	WriteFile(hFile, L"\r\n", 2, &dwBytesWritten, NULL);
	CloseHandle(hFile);
}

double uniform_deviate(int seed)
{
	return seed * (1.0 / (RAND_MAX + 1.0));
}

void NoteTest(void)
{
	saveAccidental = accidental;
	do
	{
		RandomNote = (int)(36.0 + uniform_deviate(rand()) * (85.0 - 36.0));//for Notes 36 thru 84 (from Julienne Walker's Eternally Confuzzled website)
		if (RandomNote & 1)//odd number
			accidental = 1;
		else
			accidental = 2;
	} while (RandomNote == PreviousRandomNote);
	PreviousRandomNote = RandomNote;
	RandomNoteLoc = yStaff[RandomNote - 36] - 250;

	FillRect(hdc, &testRect, hWhiteBrush);
	SetBkMode(hdc, TRANSPARENT);
	TextOutW(hdc, top260min30 + 325, top - 462, L"==", 2);
	TextOutW(hdc, top260min30 + 325, top - 371, L"==", 2);
	TextOutW(hdc, top260min30 + 325, top - 462 - 12, L"&", 1);
	TextOutW(hdc, top260min30 + 325, top - 371 - 37, L"?", 1);
	if (RandomNote <= 40)
		TextOutW(hdc, top260min30 + 325 + StaffWidth[0], top - 371 + 12, L"__", 2);
	if ((RandomNote == 36) || (RandomNote == 37))
		TextOutW(hdc, top260min30 + 325 + StaffWidth[0], top - 371 + 24, L"__", 2);
	if ((RandomNote == 60) || (RandomNote == 61))
		TextOutW(hdc, top260min30 + 325 + StaffWidth[0], top - 462 + 21, L"__", 2);
	if (RandomNote >= 80)
		TextOutW(hdc, top260min30 + 325 + StaffWidth[0], top - 462 - 59, L"__", 2);
	if (RandomNote == 84)
		TextOutW(hdc, top260min30 + 325 + StaffWidth[0], top - 462 - 71, L"__", 2);

	for (x = 0; x < 25; x++)
	{
		if (RandomNote == BlackKeyNotes[x])
		{
			if (accidental == 1)
				TextOutW(hdc, top260min30 + 325 + (StaffWidth[0] + (NoteWidth[0] / 2)), RandomNoteLoc, L"#w", 2);
			else
				TextOutW(hdc, top260min30 + 325 + (StaffWidth[0] + (NoteWidth[0] / 2)), RandomNoteLoc - 6, L"bw", 2);
			break;
		}
	}
	if (x == 25)//not a # or b
		TextOutW(hdc, top260min30 + 325 + (StaffWidth[0] + (NoteWidth[0] / 2)), RandomNoteLoc, L"w", 1);
	SetBkMode(hdc, OPAQUE);
}
