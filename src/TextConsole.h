/*
 * TextConsole.h - Library for implementing a char string console using a stream on the Arduino
 * Created by Victor Lorenzo (EDesignsForge), September 10, 2014.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of GNU Lesser General Public License version 3.0,
 * as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/
#ifndef  _TextConsole_h_
#define  _TextConsole_h_

#include <inttypes.h>
#include <Arduino.h>


#if !(defined(__stringify))
#define  __stringify_1(x...)     #x
#define  __stringify(x...)       __stringify_1(x)
#endif

// Library version codes
#define  TEXTCONSOLE_VERSION          __stringify(TEXTCONSOLE_VERSION_MAJOR) "." __stringify(TEXTCONSOLE_VERSION_MINOR)
#define  TEXTCONSOLE_VERSION_MAJOR	1
#define  TEXTCONSOLE_VERSION_MINOR	0

// Frame headers
#define  TEXTCONSOLE_RESPONSEHEADER   ""
#define  TEXTCONSOLE_RESPONSETAIL     TEXTCONSOLE_EOLN
#define  TEXTCONSOLE_EVENTHEADER      "<"
#define  TEXTCONSOLE_EVENTTAIL        TEXTCONSOLE_EOLN

// Result code strings
#define  TEXTCONSOLE_RESCODE_OK       "[:)]"
#define  TEXTCONSOLE_RESCODE_WARN     "[:O]"
#define  TEXTCONSOLE_RESCODE_FATAL    "[:(]"

#define  TEXTCONSOLE_EOLN    		  "\r\n"
#define  TEXTCONSOLE_CMDTERMINATOR	  '\r'
#define  TEXTCONSOLE_PROMPTSTRING	  ">"

// Some error messages
#define  TEXTCONSOLE_CMDTOOLONG       "Too long"
#define  TEXTCONSOLE_CMDNOTFOUND      "Not found"
#define  TEXTCONSOLE_CMDARGSCOUNT     "Args count"
#define  TEXTCONSOLE_CMDNOMEMORY      "No memory"
#define  TEXTCONSOLE_CMDINCOMPLETEARG "Incomplete"
#define  TEXTCONSOLE_CMDARGOVERFLOW   "Overflow"
#define  TEXTCONSOLE_CMDARGOUTOFRANGE "Out of range"
#define  TEXTCONSOLE_CMDARGINVALIDOPT "Invalid option"

// Maximum number of arguments allowed
#define  TEXTCONSOLE_MAXARGUMENTS     10

// Forward declaration
class TextConsole;


typedef void (*ConsoleCommand_t)( TextConsole* lpSilly );
typedef enum CommandResponseType { CONSOLESUCCESS, CONSOLEWARNING, CONSOLEERROR } CommandResponseType_t;


struct ConsoleCommandEntry {
  const char*     Name;
  ConsoleCommand_t  Method;
};


#define	sendPartialResponse( ... )		send( __VA_ARGS__ )


class TextConsole
{
  public:
    TextConsole( Stream& IOStream, char* lpBuffer, int BufferSize, const struct ConsoleCommandEntry* lpCommands );
    
    const struct ConsoleCommandEntry* findCommand( const char* lpName );
    bool handleInput();
    void reset();
	bool hasNewInput() { return m_IOStream.available(); }
	
    void begin( const __FlashStringHelper *ifsh );
    void begin( const char* lpResponse );
	void begin();
    
    void sendResponse( CommandResponseType_t ResponseType, const __FlashStringHelper *ifsh );
    void sendResponse( CommandResponseType_t ResponseType, const char* lpResponse );
    void sendResponse( CommandResponseType_t ResponseType );
    
	void beginResponse();
    void beginResponse( CommandResponseType_t ResponseType );
	
	void beginEvent();
	void endEvent();
	
    void send( const __FlashStringHelper *ifsh );
    void send( const char* lpResponse );
    void send( const uint8_t* lpSrc, int Count );
	void send( int Value );
	void send( float Value )  { m_IOStream.print( Value ); }
	void send( double Value )  { m_IOStream.print( Value ); }
	void send( unsigned int Value );
	void send( long Value ) { m_IOStream.print( Value ); }
	void send( unsigned long Value ) { m_IOStream.print( Value ); }
	
    void endResponse();
	void endResponse( CommandResponseType_t ResponseType );
    
    int argsCount() { return m_ArgsCount; };
    const char* getArg( int Index );
	
	const struct ConsoleCommandEntry* getPreviuousCommand() { return m_PreviuousCommand; }
	CommandResponseType_t getPreviuousCommandResult() { return m_PreviuousCommandResult; }
    
  private:
    const struct ConsoleCommandEntry* m_PreviuousCommand;
	CommandResponseType_t m_PreviuousCommandResult;
    Stream& m_IOStream;
    char*   m_lpBuffer;
    int     m_BufferSize;
    const struct ConsoleCommandEntry* m_lpCommands;
    
    const struct ConsoleCommandEntry* m_lpCurCommand;
    int  m_BuffPos;
    bool m_ProcessingCommand;
    int  m_ArgsCount;
    int  m_Arguments[ TEXTCONSOLE_MAXARGUMENTS ];
    char m_PrevChar;
    bool m_ReadingQuotedString;
	bool m_SendingEvent;
	bool m_SendingResponse;
};



#endif  /* _TextConsole_h_ */

