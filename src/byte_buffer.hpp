#ifndef BYTE_BUFFER
#define BYTE_BUFFER

#include "list.hpp"
#include <stdio.h>
#include <string.h>

class ByteBuffer {
public:
    ByteBuffer();
    ByteBuffer(const ByteBuffer & copy);
    ByteBuffer(const char * str);
    ByteBuffer(const char * str, int length);
    ~ByteBuffer() {}
    ByteBuffer& operator= (const ByteBuffer& other);

    const char & at(int index) const {
        return _buffer.at(index);
    }
    char & at(int index) {
        return _buffer.at(index);
    }

    static ByteBuffer format(const char *format, ...) __attribute__ ((format (printf, 1, 2)));

    int length() const {
        return _buffer.length() - 1;
    }
    void resize(int length) {
        _buffer.resize(length + 1);
    }
    void append(const ByteBuffer &other);
    void append(const char *str);
    void append(const char *str, int length);

    int index_of_rev(char c) const;
    int index_of_rev(char c, int start) const;

    ByteBuffer substring(int start, int end) const;

    char *raw() const {
        return _buffer.raw();
    }

    void fill(char value) {
        memset(_buffer.raw(), value, length());
    }
private:
    List<char> _buffer;
};

#endif