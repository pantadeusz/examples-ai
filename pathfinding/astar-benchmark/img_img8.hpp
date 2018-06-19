/*

MIT License

Copyright (c) 2017 Tadeusz Puźniakowski

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#ifndef ___IMG_IMG8_HPP__
#define ___IMG_IMG8_HPP__

#include <vector>
#include <string>
#include <functional>

namespace tp {
namespace img {

class Img8 : public std::vector < unsigned char > {
protected:
	void drawCircleLine( const int x_0, const int y_0, const int x_1, const int y_1, const int d_, unsigned char color, std::function < bool ( unsigned char, unsigned char ) > compare_ );
	void drawCircle( const int x_, const int y_, const int d_, unsigned char color, std::function < bool ( unsigned char, unsigned char ) > compare_ );

public:
	unsigned width, height;
	Img8( const int w, const int h, unsigned char initColor = 255 );
	Img8( const std::string & fn ) {
		load( fn );
	};

	unsigned char &operator()( int x, int y );
	unsigned char operator()( int x, int y ) const;

	void drawCircleMax( const int x_, const int y_, const int d_, unsigned char color );
	void drawCircleMin( const int x_, const int y_, const int d_, unsigned char color );
	void drawCircleLineToMax( const int x_0, const int y_0, const int x_1, const int y_1, const int d_, unsigned char color );
	void drawCircleLineToMin( const int x_0, const int y_0, const int x_1, const int y_1, const int d_, unsigned char color );

	Img8 dilate_old( double d ) const;
	Img8 dilate( double d ) const;

	Img8 erode( double d ) const;
	Img8 removeNoise() const;

	void setTo( unsigned char v );

	void save( const std::string &fname );

	void load( const std::string &fname );

	double similarTo( const Img8 &dst );
};

bool operator==( const Img8 &a, const Img8 &b );
Img8 operator-( const Img8 &a, const Img8 &b );

}
}

#endif
