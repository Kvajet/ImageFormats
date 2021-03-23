#include "CImage_PNG.h"

CImage_PNG::CImage_PNG( const std::string & fileName )
    : m_fileName( std::move( fileName ) ) ,
      m_file( m_fileName )
{
}

CImage_PNG::~CImage_PNG()
{
}

bool CImage_PNG::Load()
{
    if( ! ReadHeader() )
    {
        std::cout << "Invalid header.\n";
        return false;
    }

    while( true )
    {
        m_file.Read( m_chunkLenBuffer  );
        m_file.Read( m_chunkNameBuffer );

        switch( m_chunkNameBuffer )
        {
            // IDAT - 0x49444154
            case 0x49444154: if( ! m_idat.Load( m_chunkLenBuffer , m_file ) ) return false; break;
            // IEND - 0x49454e44
            case 0x49454e44: std::cout << "END\n"; return true;
            // IHDR - 0x49484452
            case 0x49484452: if( ! m_ihdr.Load( m_chunkLenBuffer , m_file ) ) return false; break;
            // PLTE - 0x504c5445
            case 0x504c5445: if( ! ReadPLTE() ) return false; break;
            // DEFAULT
            default: CHUNK::UnknownChunk( m_chunkLenBuffer , m_chunkNameBuffer , m_file ); break;
        }
    }

    return true;
}

bool CImage_PNG::ReadHeader()
{
    m_file.Read( m_header );
    return m_header == HEADER_TEMPLATE;
}

bool CImage_PNG::ReadIHDR()
{
    m_ihdr.m_len = m_chunkLenBuffer;
    m_ihdr.m_name = m_chunkNameBuffer;

    m_file.Read( m_ihdr.m_width             );
    m_file.Read( m_ihdr.m_height            );
    m_file.Read( m_ihdr.m_bitDepth          );
    m_file.Read( m_ihdr.m_colorType         );
    m_file.Read( m_ihdr.m_compressionMethod );
    m_file.Read( m_ihdr.m_filterMethod      );
    m_file.Read( m_ihdr.m_interlaceMethod   );

    return true;
}

bool CImage_PNG::ReadPLTE()
{
    if( m_ihdr.m_colorType == 2 || m_ihdr.m_colorType == 6 ) return true;
    
    std::cout << "\033[1;31m[ERROR] PLTE is present but is not implemented (meaning: is different from values 2 or 6.).\033[0m\n";
    return false;
}

bool CImage_PNG::ReadIEND()
{
    m_ihdr.m_len = m_chunkLenBuffer;
    m_ihdr.m_name = m_chunkNameBuffer;
    return true;
}

// SEGMENTS IMPLEMENTATION

void CImage_PNG::CHUNK::PrintCommonChunkData() const
{
    std::cout << std::hex 
                << "len: "    << m_len
                << "\nname: " << m_name
                << "\nCRC: "  << m_CRC
                << "\n";
}

void CImage_PNG::CHUNK::PrintHexName( uint32_t name ) const
{
    std::cout << std::hex << name << "\n";
}

void CImage_PNG::CHUNK::PrintAsciiName( uint32_t name )
{
    std::cout << static_cast< char >( (   name & 0xFF000000 ) >> 24 )
                << static_cast< char >( ( name & 0x00FF0000 ) >> 16 )
                << static_cast< char >( ( name & 0x0000FF00 ) >>  8 )
                << static_cast< char >(   name & 0x000000FF ) << "\n";
}

void CImage_PNG::CHUNK::UnknownChunk( uint32_t m_chunkLenBuffer , uint32_t m_chunkNameBuffer , CFileHandler & m_file )
{
    std::cout << "\033[1;31m[ERROR] Unknown chunk name ";
    CHUNK::PrintAsciiName( m_chunkNameBuffer );
    std::cout << "\033[0m";
    for( size_t i = 0 ; i < m_chunkLenBuffer + 4 ; i++ ) m_file.Read();
}

CImage_PNG::IHDR::IHDR()
{
    m_name = 0x49484452;
}

void CImage_PNG::IHDR::Print() const
{
    PrintCommonChunkData();
    std::cout << std::hex 
                << m_width << "\n"
                << m_height << "\n"
                << (short)m_bitDepth << "\n"
                << (short)m_colorType << "\n"
                << (short)m_compressionMethod << "\n"
                << (short)m_filterMethod << "\n"
                << (short)m_interlaceMethod << "\n";
}

void CImage_PNG::IHDR::PrintHex() const
{
    std::cout << std::hex << m_len << m_name << m_width << m_height << m_bitDepth << m_colorType
                << m_compressionMethod << m_filterMethod << m_interlaceMethod << m_CRC << "\n";
}

bool CImage_PNG::IHDR::Load( uint32_t m_chunkLenBuffer , CFileHandler & m_file )
{
    m_len = m_chunkLenBuffer;

    m_file.Read( m_width             );
    m_file.Read( m_height            );
    
    m_file.Read( m_bitDepth          );
    m_file.Read( m_colorType         );
    m_file.Read( m_compressionMethod );
    m_file.Read( m_filterMethod      );
    m_file.Read( m_interlaceMethod   );
    m_file.Read( m_CRC );

    return true;
}

void CImage_PNG::IHDR::ErrorMessage() const
{
    std::cout << "\033[1;31mInvalid IHDR.\033[0m\n";
}

CImage_PNG::IDAT::IDAT()
{
    m_name = 0x49444154;
}

bool CImage_PNG::IDAT::Load( uint32_t m_chunkLenBuffer , CFileHandler & m_file )
{
    m_data = std::make_unique< uint8_t[] >( m_chunkLenBuffer );
    for( size_t i = 0 ; i < m_chunkLenBuffer ; i++ ) m_data[ i ] = static_cast< uint8_t >( m_file.Read() );
    for( size_t i = 0 ; i < 4 ; i++ ) m_file.Read();

    return true;
}

void CImage_PNG::IDAT::ErrorMessage() const
{
    std::cout << "\033[1;31mInvalid IDAT.\033[0m\n";
}