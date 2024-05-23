#ifndef CCVEC2F_H
#define CCVEC2F_H
#include<math.h>

//点类，用来实现点的坐标以及相关的数学计算
class CCVec2f {
public:

    static const int DIM = 2;

    float x; //横坐标
    float y; //纵坐标

    CCVec2f(); //无参构造
    CCVec2f( float x, float y ); //有参构造

    void set( float x, float y ); //直接设定x和y的值

    void set( const CCVec2f& vec ); //拷贝赋值x和y的值

   float length() const; //获得模长

    bool operator==( const CCVec2f& vec ) const;

    bool operator!=( const CCVec2f& vec ) const;

    bool match( const CCVec2f& vec, float tolerance = 0.0001f ) const; //查看其与另一个点是否重合

    CCVec2f& operator+=( const CCVec2f& vec );

    CCVec2f& operator+=( const float f ); //x和y均加f

    CCVec2f  operator+( const CCVec2f& vec ) const;

    CCVec2f  operator+( const float f ) const;


    CCVec2f& operator-=( const CCVec2f& vec );

    CCVec2f& operator-=( const float f ); //x和y均加f

    CCVec2f  operator-( const CCVec2f& vec ) const;

    CCVec2f  operator-() const;


    CCVec2f  operator*( const CCVec2f& vec ) const;

    CCVec2f  operator*( const float f ) const;

    CCVec2f& operator*=( const CCVec2f& vec );

    CCVec2f& operator*=( const float f );

    CCVec2f  operator/( const CCVec2f& vec ) const;


    CCVec2f  operator/( const float f ) const;

    CCVec2f& operator/=( const CCVec2f& vec );

    CCVec2f& operator/=( const float f );


    float * getPtr() {
        return (float*)&x;
    }

    const float * getPtr() const {
        return (const float *)&x;
    }

    float& operator[]( int n ){
        return getPtr()[n];
    }

    float operator[]( int n ) const {
        return getPtr()[n];
    }

};

inline CCVec2f::CCVec2f(): x(0), y(0) {}
inline CCVec2f::CCVec2f( float _x, float _y ):x(_x), y(_y) {}



inline void CCVec2f::set( float _x, float _y ) {
    x = _x;
    y = _y;
}

inline void CCVec2f::set( const CCVec2f& vec ) {
    x = vec.x;
    y = vec.y;
}


inline bool CCVec2f::operator==( const CCVec2f& vec ) const {
    return (x == vec.x) && (y == vec.y);
}

inline bool CCVec2f::operator!=( const CCVec2f& vec ) const {
    return (x != vec.x) || (y != vec.y);
}

inline bool CCVec2f::match( const CCVec2f& vec, float tolerance ) const {
    return (fabs(x - vec.x) < tolerance)
    && (fabs(y - vec.y) < tolerance);
}



inline CCVec2f CCVec2f::operator+( const CCVec2f& vec ) const {
    return CCVec2f( x+vec.x, y+vec.y);
}

inline CCVec2f& CCVec2f::operator+=( const CCVec2f& vec ) {
    x += vec.x;
    y += vec.y;
    return *this;
}
inline CCVec2f CCVec2f::operator+( const float f ) const {
    return CCVec2f( x+f, y+f);
}

inline CCVec2f& CCVec2f::operator+=( const float f ) {
    x += f;
    y += f;
    return *this;
}


inline CCVec2f& CCVec2f::operator-=( const float f ) {
    x -= f;
    y -= f;
    return *this;
}

inline CCVec2f& CCVec2f::operator-=( const CCVec2f& vec ) {
    x -= vec.x;
    y -= vec.y;
    return *this;
}


inline CCVec2f CCVec2f::operator-( const CCVec2f& vec ) const {
    return CCVec2f(x-vec.x, y-vec.y);
}


inline CCVec2f CCVec2f::operator-() const {
    return CCVec2f(-x, -y);
}


inline CCVec2f CCVec2f::operator*( const CCVec2f& vec ) const {
    return CCVec2f(x*vec.x, y*vec.y);
}

inline CCVec2f& CCVec2f::operator*=( const CCVec2f& vec ) {
    x*=vec.x;
    y*=vec.y;
    return *this;
}

inline CCVec2f CCVec2f::operator*( const float f ) const {
    return CCVec2f(x*f, y*f);
}

inline CCVec2f& CCVec2f::operator*=( const float f ) {
    x*=f;
    y*=f;
    return *this;
}


inline CCVec2f CCVec2f::operator/( const CCVec2f& vec ) const {
    return CCVec2f( vec.x!=0 ? x/vec.x : x , vec.y!=0 ? y/vec.y : y);
}

inline CCVec2f& CCVec2f::operator/=( const CCVec2f& vec ) {
    vec.x!=0 ? x/=vec.x : x;
    vec.y!=0 ? y/=vec.y : y;
    return *this;
}

inline CCVec2f CCVec2f::operator/( const float f ) const {
    if(f == 0) return CCVec2f(x, y);

    return CCVec2f(x/f, y/f);
}

inline CCVec2f& CCVec2f::operator/=( const float f ) {
    if(f == 0) return *this;

    x/=f;
    y/=f;
    return *this;
}

inline float CCVec2f::length() const {
    return (float)sqrt( x*x + y*y);
}


#endif // CCVEC2F_H
