#ifndef COMMANDS_H
#define COMMANDS_H

// Define generic command codes
#define ID_OPEN 100
#define ID_NEW 101
#define ID_SAVE 102
#define ID_SAVEAS 103
#define ID_CLOSE 104
#define ID_NEWWINDOW 105
#define ID_CLOSEWINDOW 106
#define ID_COPY 107
#define ID_CUT 108
#define ID_PASTE 109
#define ID_OPTIONS 110
#define ID_EXIT 111
#define ID_HELPCONTENTS 112
#define ID_ABOUT 113 
#define ID_OPENURL 114
#define ID_SAVEURL 115
#define ID_SHOWTOOLBAR 116
#define ID_SHOWMENUBAR 117
#define ID_SHOWSTATUSBAR 118
#define ID_SAVEOPTIONS 119

// Define app specific command codes
#define ID_FORMAT 200
#define ID_PASTEIMAGE 201
#define ID_ZOOMIN 202
#define ID_ZOOMOUT 203
#define ID_MASK 204
#define ID_INFO 205
#define ID_PALETTE 206
#define ID_DEPTH 207
#define ID_RELEASENOTES 208
#define ID_SHOWTOOLSTOOLBAR 209
#define ID_SHOWCOMMANDSTOOLBAR 210
#define ID_RESIZEIMAGE 211

// Tags for statusbar items
#define ID_FILESIZE 300
#define ID_ZOOMFACTOR 301
#define ID_FILENAME 302

// Tags for toolbars
#define ID_COMMANDSTOOLBAR 0
#define ID_TOOLSTOOLBAR 1

#endif // COMMANDS_H
