#include <iostream>
#include "CImage_PNG.cpp"

int main( void )
{
    CImage_PNG image( "tower-main.png" );
    image.Load();

    return 0;
}