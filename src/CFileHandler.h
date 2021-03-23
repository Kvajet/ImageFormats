#pragma once
#include <iostream>
#include <fstream>
#include <string>

class CFileHandler
{
public:
    CFileHandler() = delete;
    CFileHandler( const CFileHandler & ) = delete;

    CFileHandler( const std::string & fileName );
    ~CFileHandler();

    char Read();
    void Read( uint8_t  & buffer );
    void Read( uint16_t & buffer );
    void Read( uint32_t & buffer );
    void Read( uint64_t & buffer );

private:
    const std::string m_fileName;
    std::ifstream m_input;
    bool m_isOpen = false;
    char m_readBuffer = 0x00;

    // readbuffers
    uint8_t  m_buffer8  = 0x00;
    uint16_t m_buffer16 = 0x0000;
    uint32_t m_buffer32 = 0x00000000;
    uint64_t m_buffer64 = 0x0000000000000000;
};