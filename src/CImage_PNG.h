#pragma once
#include <iostream>
#include <string>
#include <iomanip>
#include <memory>
#include "CFileHandler.h"

/**
 * @class CImage_PNG
 * This class serves as loader of PNG files.
 * 
 * My only source for PNG format was wikipedia: https://en.wikipedia.org/wiki/Portable_Network_Graphics .
 */
class CImage_PNG
{
public:
    CImage_PNG() = delete;
    CImage_PNG( const CImage_PNG & ) = delete;
    
    CImage_PNG( const std::string & fileName );
    ~CImage_PNG();

    bool Load();

private:
    struct CHUNK 
    {
        void PrintCommonChunkData() const;
        void PrintHexName( uint32_t name ) const;
        static void PrintAsciiName( uint32_t name );
        static void UnknownChunk( uint32_t m_chunkLenBuffer , uint32_t m_chunkNameBuffer , CFileHandler & m_file );

        virtual bool Load( uint32_t m_chunkLenBuffer , CFileHandler & m_file ) = 0;
        virtual void ErrorMessage() const = 0;

        uint32_t m_len  = 0x00000000;
        uint32_t m_name = 0x00000000;
        uint32_t m_CRC  = 0x00000000;
    };

    /* 
        IHDR data segment
        4 bytes - W
        4 bytes - H
        1 byte  - bit depth          (1 byte, values 1, 2, 4, 8, or 16)
        1 byte  - color type         (1 byte, values 0, 2, 3, 4, or 6)
        1 byte  - compression method (1 byte, value 0)
        1 byte  - filter method      (1 byte, value 0)
        1 byte  - interlace method   (1 byte, values 0 "no interlace" or 1 "Adam7 interlace")
    */
    struct IHDR : public CHUNK
    {
        IHDR();

        void Print() const;
        void PrintHex() const;

        bool Load( uint32_t m_chunkLenBuffer , CFileHandler & m_file ) override;
        void ErrorMessage() const override;

        uint32_t m_width             = 0x00000000;
        uint32_t m_height            = 0x00000000;
        uint8_t  m_bitDepth          = 0x00;
        uint8_t  m_colorType         = 0x00;
        uint8_t  m_compressionMethod = 0x00;
        uint8_t  m_filterMethod      = 0x00;
        uint8_t  m_interlaceMethod   = 0x00;
    };

    struct IDAT : public CHUNK
    {
        IDAT();

        bool Load( uint32_t m_chunkLenBuffer , CFileHandler & m_file ) override;
        void ErrorMessage() const override;

        std::unique_ptr< uint8_t[] > m_data;
    };

    bool ReadHeader();
    bool ReadIHDR();
    bool ReadPLTE();
    bool ReadIDAT();
    bool ReadIEND();

    const std::string m_fileName;
    CFileHandler m_file;

    // header
    uint64_t m_header = 0x00000000;

    // segments
    IHDR m_ihdr;
    IDAT m_idat;

    uint32_t m_chunkLenBuffer = 0x00000000 , m_chunkNameBuffer = 0x00000000 , m_chunkCRCBuffer = 0x00000000;

    static constexpr uint64_t HEADER_TEMPLATE = 0x89504E470D0A1A0A;
};