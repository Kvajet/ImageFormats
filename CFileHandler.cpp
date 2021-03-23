#include <iostream>
#include <fstream>
#include <string>

class CFileHandler
{
public:
    CFileHandler( const std::string & fileName )
        : m_fileName( std::move( fileName ) ) ,
          m_input( std::ifstream( m_fileName , std::ios::binary ) ) ,
          m_isOpen( m_input.is_open() && m_input.good() )
    {
    }

    ~CFileHandler()
    {
        m_input.close();
    }

    char Read()
    {
        if( m_isOpen ) m_input.get( m_readBuffer );
        return m_readBuffer;
    }

private:
    const std::string m_fileName;
    std::ifstream m_input;
    bool m_isOpen = false;
    char m_readBuffer = 0x00;
};