#include "CFileHandler.h"

CFileHandler::CFileHandler( const std::string & fileName )
    : m_fileName( std::move( fileName ) ) ,
      m_input( std::ifstream( m_fileName , std::ios::binary ) ) ,
      m_isOpen( m_input.is_open() && m_input.good() )
{
}

CFileHandler::~CFileHandler()
{
    m_input.close();
}

char CFileHandler::Read()
{
    if( m_isOpen )
    {
        m_input.get( m_readBuffer );
    }
    return m_readBuffer;
}

void CFileHandler::Read( uint8_t & buffer )
{
    buffer = static_cast< unsigned char >( Read() );
}

void CFileHandler::Read( uint16_t & buffer )
{
    uint16_t innerbuffer;
    buffer = 0x0000;
    for( size_t i = 2 ; i ; i-- )
    {
        innerbuffer = static_cast< unsigned char >( Read() );
        buffer |= ( innerbuffer << ( 8 * ( i - 1 ) ) );
    }
}

void CFileHandler::Read( uint32_t & buffer )
{
    uint32_t innerbuffer;
    buffer = 0x00000000;
    for( size_t i = 4 ; i ; i-- )
    {
        innerbuffer = static_cast< unsigned char >( Read() );
        buffer |= ( innerbuffer << ( 8 * ( i - 1 ) ) );
    }
}

void CFileHandler::Read( uint64_t & buffer )
{
    uint64_t innerbuffer;
    for( size_t i = 8 ; i ; i-- )
    {
        innerbuffer = static_cast< unsigned char >( Read() );
        buffer |= ( innerbuffer << ( 8 * ( i - 1 ) ) );
    }
}

void CFileHandler::Ignore( size_t len )
{
    m_input.ignore( len );
}