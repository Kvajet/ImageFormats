#include <iostream>
#include <string>
#include <iomanip>
#include "CFileHandler.cpp"

class CImage_PNG
{
public:
    CImage_PNG( const std::string & fileName )
        : m_fileName( std::move( fileName ) ) ,
          m_file( m_fileName )
    {
    }

    ~CImage_PNG()
    {
    }

    bool Load()
    {
        if( ! ReadHeader() )
        {
            std::cout << "Invalid header.\n";
            return false;
        }

        size_t tmp = 0;

        while( tmp < 3 )
        {
            Read32bits( m_chunkLenBuffer  );
            Read32bits( m_chunkNameBuffer );

            switch( m_chunkNameBuffer )
            {
                // IDAT - 0x49444154
                case 0x49444154:
                {
                    ReadIDAT();
                } break;

                // IEND - 0x49454e44
                case 0x49454e44:
                    return true;

                // IHDR - 0x49484452
                case 0x49484452:
                {
                    if( ! ReadIHDR() )
                    {
                        std::cout << "Invalid IHDR.\n";
                        return false;
                    }
                } break;

                // PLTE - 0x504c5445
                case 0x504c5445:
                {
                    if( ! ReadPLTE() )
                    {
                        std::cout << "PLTE is present but is not implemented.\n";
                        return false;
                    }
                } break;

                default:
                {
                    uint8_t dump;
                    for( size_t i = 0 ; i < m_chunkLenBuffer ; i++ ) Read8bits( dump );
                    break;
                }
            }
            Read32bits( m_chunkCRCBuffer );
            tmp++;
        }

        return true;
    }

private:
    struct CHUNK 
    {
        uint32_t m_len  = 0x00000000;
        uint32_t m_name = 0x00000000;
        uint32_t m_CRC  = 0x00000000;

        void PrintCommonChunkData() const
        {
            std::cout << std::hex 
                      << "len: "    << m_len
                      << "\nname: " << m_name
                      << "\nCRC: "  << m_CRC
                      << "\n";
        }

        static void PrintHexName( uint32_t name )
        {
            std::cout << std::hex << name << "\n";
        }

        static void PrintAsciiName( uint32_t name )
        {
            std::cout << static_cast< char >( ( name & 0xFF000000 ) >> 24 )
                      << static_cast< char >( ( name & 0x00FF0000 ) >> 16 )
                      << static_cast< char >( ( name & 0x0000FF00 ) >>  8 )
                      << static_cast< char >( name & 0x000000FF ) << "\n";
        }
    };

    /*
        4 bytes - W
        4 bytes - H
        1 byte  - bit depth
        1 byte  - color type
        1 byte  - compression method
        1 byte  - filter method
        1 byte  - interlace method
    */
    struct IHDR : CHUNK
    {
        uint32_t m_width             = 0x00000000;
        uint32_t m_height            = 0x00000000;
        uint8_t  m_bitDepth          = 0x00;
        uint8_t  m_colorType         = 0x00;
        uint8_t  m_compressionMethod = 0x00;
        uint8_t  m_filterMethod      = 0x00;
        uint8_t  m_interlaceMethod   = 0x00;

        void Print() const
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

        void PrintHex() const
        {
            std::cout << std::hex << m_len << m_name << m_width << m_height << m_bitDepth << m_colorType
                      << m_compressionMethod << m_filterMethod << m_interlaceMethod << m_CRC << "\n";
        }
    };

    bool ReadHeader()
    {
        Read64bits( m_header );
        return m_header == HEADER_TEMPLATE;
    }

    /* 
        IHDR must be the first chunk; it contains (in this order) the image's width (4 bytes);
        height (4 bytes); bit depth (1 byte, values 1, 2, 4, 8, or 16); color type (1 byte, values 0, 2, 3, 4, or 6); 
        compression method (1 byte, value 0); filter method (1 byte, value 0); 
        and interlace method (1 byte, values 0 "no interlace" or 1 "Adam7 interlace") (13 data bytes total).
        As stated in the World Wide Web Consortium, 
        bit depth is defined as "the number of bits per sample or per palette index (not per pixel)".

        4 bytes - W
        4 bytes - H
        1 byte  - bit depth
        1 byte  - color type
        1 byte  - compression method
        1 byte  - filter method
        1 byte  - interlace method
    */
    bool ReadIHDR()
    {
        m_ihdr.m_len = m_chunkLenBuffer;
        m_ihdr.m_name = m_chunkNameBuffer;

        Read32bits( m_ihdr.m_width            );
        Read32bits( m_ihdr.m_height           );
        Read8bits( m_ihdr.m_bitDepth          );
        Read8bits( m_ihdr.m_colorType         );
        Read8bits( m_ihdr.m_compressionMethod );
        Read8bits( m_ihdr.m_filterMethod      );
        Read8bits( m_ihdr.m_interlaceMethod   );

        return true;
    }

    bool ReadPLTE()
    {
        if( m_ihdr.m_colorType == 2 || m_ihdr.m_colorType == 6 ) return true;
        return false;
    }

    bool ReadIDAT()
    {
        uint8_t dump;
        for( size_t i = 0 ; i < m_chunkLenBuffer ; i++ ) Read8bits( dump );
        return true;
    }

    bool ReadIEND()
    {
        m_ihdr.m_len = m_chunkLenBuffer;
        m_ihdr.m_name = m_chunkNameBuffer;
        return true;
    }

    void Read8bits( uint8_t & buffer )
    {
        buffer = m_file.Read();
    }

    void Read16bits( uint16_t & buffer )
    {
        uint16_t innerbuffer;
        buffer = 0x0000;
        for( size_t i = 2 ; i ; i--  )
        {
            innerbuffer = m_file.Read();
            buffer |= ( innerbuffer << ( 8 * ( i - 1 ) ) );
        }
    }

    void Read32bits( uint32_t & buffer )
    {
        uint32_t innerbuffer;
        buffer = 0x00000000;
        for( size_t i = 4 ; i ; i--  )
        {
            innerbuffer = m_file.Read();
            buffer |= ( innerbuffer << ( 8 * ( i - 1 ) ) );
        }
    }

    void Read64bits( uint64_t & buffer )
    {
        uint64_t innerbuffer;
        for( size_t i = 8 ; i ; i--  )
        {
            innerbuffer = m_file.Read();
            buffer |= ( innerbuffer << ( 8 * ( i - 1 ) ) );
        }
    }

    const std::string m_fileName;
    CFileHandler m_file;

    uint64_t m_header = 0x00000000;
    IHDR m_ihdr;

    uint32_t m_chunkLenBuffer = 0x00000000 , m_chunkNameBuffer = 0x00000000 , m_chunkCRCBuffer = 0x00000000;

    static constexpr uint64_t HEADER_TEMPLATE = 0x89504E470D0A1A0A;
};