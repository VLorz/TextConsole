#include <string.h>
#include "TextConsole.h"




static const char* HEADERS[] = 
{
  /* CONSOLESUCCESS */
  TEXTCONSOLE_RESCODE_OK,
  /* CONSOLEWARNING */
  TEXTCONSOLE_RESCODE_WARN,
  /* CONSOLEERROR */
  TEXTCONSOLE_RESCODE_FATAL
};


static void CmdNotSupported( TextConsole* lpSilly )
{
  lpSilly->sendResponse( CONSOLEERROR, F(TEXTCONSOLE_CMDNOTFOUND) );
}


static void CmdEmptyResponse( TextConsole* lpSilly )
{
  lpSilly->sendResponse( CONSOLESUCCESS, F("") );
}


static const struct ConsoleCommandEntry ConsoleCommandNotSupportedEntry =
{
  "",   CmdNotSupported
};


static const struct ConsoleCommandEntry ConsoleEmptyEntry =
{
  "",   CmdEmptyResponse
};


TextConsole::TextConsole( Stream& IOStream, char* lpBuffer, int BufferSize, const struct ConsoleCommandEntry* lpCommands )
  : m_IOStream( IOStream ), m_lpCommands( lpCommands ), m_lpBuffer( lpBuffer ), m_BufferSize( BufferSize ), 
    m_BuffPos( 0 ), m_lpCurCommand( NULL ), m_ProcessingCommand( false ), m_PreviuousCommand( NULL )
{
}


void TextConsole::begin( const __FlashStringHelper *ifshWelcome )
{
  reset();
  m_IOStream.print( ifshWelcome );
  m_IOStream.write( TEXTCONSOLE_EOLN );
}


void TextConsole::begin( const char* lpWelcome )
{
  reset();
  m_IOStream.print( lpWelcome );
  m_IOStream.write( TEXTCONSOLE_EOLN );
}


void TextConsole::begin()
{
  reset();
  m_IOStream.write( TEXTCONSOLE_EOLN );
}


const struct ConsoleCommandEntry* TextConsole::findCommand( const char* lpName )
{
  const struct ConsoleCommandEntry*  lpCommand = m_lpCommands;
  
  //m_IOStream.print( lpName );
  
  if ((lpName == NULL) || (*lpName == '\0'))
	  return &ConsoleEmptyEntry;
  
  while (lpCommand->Name != NULL)
  {
    if (!strcmp( lpCommand->Name, lpName ))
      return lpCommand;
      
    lpCommand++;
  }
  
  return &ConsoleCommandNotSupportedEntry;
}


void TextConsole::reset()
{
  // Restart!
  m_BuffPos = 0;
  m_lpCurCommand = NULL;
  m_ProcessingCommand = false;
  m_PrevChar = (char)-1; //'\0';
  m_ReadingQuotedString = false;
}


bool TextConsole::handleInput()
{
  bool Result = false;
  
  if (m_PrevChar == (char)-1) {
	m_IOStream.print( F(TEXTCONSOLE_PROMPTSTRING) );
	m_PrevChar = '\0';
  }
  
  while (m_IOStream.available())
  {
    char  data;
    
    Result = true;
    
    data = m_IOStream.read();
    
    if (m_BuffPos < m_BufferSize)
    {
      if (!m_ReadingQuotedString && ((data == ' ') || (data == TEXTCONSOLE_CMDTERMINATOR)))
      {
        m_lpBuffer[m_BuffPos] = '\0';
          
        // Separator or EndOfCommand;
        if (m_lpCurCommand == NULL)
        {          
          // In this case it should be the command. 
          // Not found? Ok, findCommand() handles that situation.
          m_lpCurCommand = findCommand( m_lpBuffer );
          
          // Recover some Buffer space, no need to keep
          // a copy for the command name.
          m_BuffPos = 0;
          m_lpBuffer[0] = '\0';
          m_Arguments[ 0 ] = 0;
          m_ArgsCount = 0;
        }
        else
        {
          // Separator. Is it end of param?
          if (m_PrevChar != ' ')
          {
            m_BuffPos++;
            m_ArgsCount++;
            if (m_ArgsCount < TEXTCONSOLE_MAXARGUMENTS)
              m_Arguments[ m_ArgsCount ] = m_BuffPos;
          }
        }
      }
      else
      {
        if (data == '"')
          m_ReadingQuotedString = !m_ReadingQuotedString;
        else
          m_lpBuffer[m_BuffPos++] = data;
      }
      
      m_PrevChar = data;
      
      if (data == TEXTCONSOLE_CMDTERMINATOR)
      {
        if (m_ReadingQuotedString)
        {
          sendResponse( CONSOLEERROR, F(TEXTCONSOLE_CMDINCOMPLETEARG) );
          // Restart!
          reset();
        }
        else if (m_ArgsCount < TEXTCONSOLE_MAXARGUMENTS)
        {
          m_ProcessingCommand = true;
          m_lpCurCommand->Method( this );
		  m_PreviuousCommand = m_lpCurCommand;
          reset();
          while (m_IOStream.peek() == '\n')
          {
            m_IOStream.read();
          }
        }else
        {
          sendResponse( CONSOLEERROR, F(TEXTCONSOLE_CMDARGSCOUNT) );
          // Restart!
          reset();
        }
      }
    }
    else
    {
      // Too many chars received, discard everything until TEXTCONSOLE_CMDTERMINATOR received;
      if (data == TEXTCONSOLE_CMDTERMINATOR)
      {
        sendResponse( CONSOLEERROR, F(TEXTCONSOLE_CMDTOOLONG) );
        // Restart!
        reset();
      }
    }
  }
  
  return Result;
}


const char* TextConsole::getArg( int Index )
{
  if ((Index < m_ArgsCount) && (Index < TEXTCONSOLE_MAXARGUMENTS))
    return &m_lpBuffer[ m_Arguments[ Index ] ];
  else
    return NULL;
}


void TextConsole::sendResponse( CommandResponseType_t ResponseType, const __FlashStringHelper *ifsh )
{
  /*
  if (ResponseType < 3)
  {
    m_IOStream.write( HEADERS[ (int)ResponseType ] );
  }
  */
  
  m_PreviuousCommandResult = ResponseType;
  m_IOStream.print( ifsh );
  /*
  m_IOStream.write( TEXTCONSOLE_EOLN );
  m_ProcessingCommand = false;
  //sendResponse( ResponseType, reinterpret_cast<const char *>(ifsh) );
  */
  endResponse();
}


void TextConsole::sendResponse( CommandResponseType_t ResponseType, const char* lpResponse )
{
	/*
  if (ResponseType < 3)
  {
    m_IOStream.write( HEADERS[ (int)ResponseType ] );
  }
  */
  
  m_PreviuousCommandResult = ResponseType;
  m_IOStream.write( lpResponse );
  /*
  m_IOStream.write( TEXTCONSOLE_EOLN );
  m_ProcessingCommand = false;
  */
  endResponse();
}


void TextConsole::sendResponse( CommandResponseType_t ResponseType )
{
  sendResponse( ResponseType, "" );
}


void TextConsole::beginResponse( CommandResponseType_t ResponseType )
{
	beginResponse();
	/*
  if (ResponseType < 3)
  {
    m_IOStream.write( HEADERS[ (int)ResponseType ] );
  }
  */
  
  m_PreviuousCommandResult = ResponseType;
}


void TextConsole::beginResponse()
{}


void TextConsole::sendPartialResponse( const __FlashStringHelper *ifsh )
{
  m_IOStream.print( ifsh );
}


void TextConsole::sendPartialResponse( const char* lpResponse )
{
  m_IOStream.write( lpResponse );
}


void TextConsole::sendPartialResponse( const uint8_t* lpSrc, int Count )
{
  m_IOStream.write( lpSrc, Count );
}


void TextConsole::sendPartialResponse( int Value )
{
	m_IOStream.print( Value );
}


void TextConsole::sendPartialResponse( unsigned int Value )
{
	m_IOStream.print( Value );
}


void TextConsole::endResponse()
{
  m_IOStream.write( TEXTCONSOLE_EOLN );
  
  if (m_PreviuousCommandResult < 3)
  {
    m_IOStream.write( HEADERS[ (int)m_PreviuousCommandResult ] );
	m_IOStream.write( TEXTCONSOLE_EOLN );
  }
  
  m_ProcessingCommand = false;
}


void TextConsole::endResponse( CommandResponseType_t ResponseType )
{
  m_PreviuousCommandResult = ResponseType;

  endResponse();
}

