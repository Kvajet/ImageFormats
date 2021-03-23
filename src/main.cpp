#include <iostream>
#include "CImage_PNG.h"

int main( void )
{
    CImage_PNG image( "images/tower-main.png" );
    image.Load();

    return 0;
}