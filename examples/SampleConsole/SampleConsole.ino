/*
 * TextConsole based console
 * 
 * Demonstrates the use of the TextConsole library for Arduino.
 * 
 * Hardware required :
 *   * Any Arduino board with a serial communication interface will be just fine.
 * 
 * Original by Victor Lorenzo (EDesignsForge), April 26, 2015
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of GNU Lesser General Public License version 3.0,
 * as published by the Free Software Foundation.
 *
 * This demonstration code is distributed in the hope that it will 
 * be useful,but WITHOUT ANY WARRANTY; without even the implied 
 * warranty ofMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this sample code; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 * 
 * This example code is in the public domain
 * 
*/
#include <arduino.h>
#include <TextConsole.h>


/* Forward declaration of command implementations */
void CmdHelp( TextConsole* lpSilly );
void CmdListArgs( TextConsole* lpSilly );
void CmdWho( TextConsole* lpSilly );


static const struct ConsoleCommandEntry  CommandEntries[] =
{
  { "args",     CmdListArgs },
  { "?",        CmdHelp },
  { "who",      CmdWho },
  { NULL,       NULL }
};


/* Declare the buffer space for handling string input/output */
char Buffer[ 64 ];

/* Initialize the interpreter */
TextConsole    Interpreter( Serial, Buffer, sizeof(Buffer), &CommandEntries[ 0 ] );


// This is the text sent by the help (?) command.
#define HELP \
  TEXTCONSOLE_EOLN \
  "Available commands:" TEXTCONSOLE_EOLN \
  "  args" TEXTCONSOLE_EOLN \
  "    list received command arguments" TEXTCONSOLE_EOLN \
  "  who" TEXTCONSOLE_EOLN \
  "    returns the name of this sample application" TEXTCONSOLE_EOLN \
  "  ?" TEXTCONSOLE_EOLN \
  "    shows this help"
  

void CmdHelp( TextConsole* lpSilly )
{
  lpSilly->sendResponse( CONSOLESUCCESS, F(HELP) );
}


// Returns back the list of values received as command parameters.
void CmdListArgs( TextConsole* lpSilly )
{
  char Buffer[32];
  
  lpSilly->beginResponse( CONSOLESUCCESS );
  for (int Index = 0; Index < lpSilly->argsCount(); Index++)
  {
    snprintf( Buffer, sizeof(Buffer), "arg[%d]=\"", Index );
    lpSilly->sendPartialResponse( Buffer );
    lpSilly->sendPartialResponse( lpSilly->getArg( Index ) );
    lpSilly->sendPartialResponse( "\"; " );
  }
  lpSilly->endResponse();
}


void CmdWho( TextConsole* lpSilly )
{
  lpSilly->sendResponse( CONSOLESUCCESS, F("TextConsole Demonstration") );
}


void setup() {
  // Setup the serial port
  Serial.begin( 115200 );
  Interpreter.begin( "TextConsole Sample! Use ? command for help" );
}


// Main application loop
void loop() {
  // Handle the characters string received by the console.
  if (!Interpreter.handleInput()) {
    // Nothing done by the command interpreter.
  }
}

