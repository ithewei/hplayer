#ifndef HBUF_H
#define HBUF_H

#include "hdef.h"
#include <stdlib.h>
#include <string.h>
#include <mutex>

typedef struct hbuf_s{
    uint8* base;
    uint64 len;

    hbuf_s(){
        base = NULL;
        len  = 0;
    }

    hbuf_s(uint8* base, uint64 len){
        this->base = base;
        this->len  = len;
    }

    void init(uint64 cap){
        len = cap;
        if (base){
            base = (uint8*)realloc(base, len);
        }else{
            base = (uint8*)malloc(len);
        }
    }

    void cleanup(){
        SAFE_FREE(base);
        len = 0;
    }

    bool isNull(){
        return base == NULL || len == 0;
    }
}hbuf_t;

class HBuf : public hbuf_t{
public:
    HBuf() : hbuf_t(){}
    HBuf(uint64 cap) {init(cap);}
    virtual ~HBuf() {cleanup();}

    std::mutex mutex; // used in multi-thread
};

class HStreamBuf : public HBuf{
public:
    HStreamBuf() : HBuf() {_offset = _size = 0;}
    HStreamBuf(uint64 cap) : HBuf(cap) { _offset = _size = 0;}

    void insert(void* buf, uint64 len){
        if (len > this->len - _size){
            this->len = MAX(this->len, len)*2;
            base = (uint8*)realloc(base, this->len);
        }else if (len > this->len - _offset - _size){
            memmove(base, data(), _size);
            _offset = 0;
        }
        memcpy(data()+_size, buf, len);
        _size += len;
    }
    void remove(uint64 len){
        if (_size >= len){
            _offset += len;
            if (_offset == this->len) _offset = 0;
            _size   -= len;
        }
    }
    void clear(){
        _offset = _size = 0;
    }
    uint8* data() {return base+_offset;}
    uint64 size()  {return _size;}

private:
    uint64 _offset;
    uint64 _size;
};

class HRingBuf : public HBuf{
public:
    HRingBuf() : HBuf() {_head = _tail = _size = 0;}
    HRingBuf(uint64 cap) : HBuf(cap) {_head = _tail = _size = 0;}

    uint8* alloc(uint64 len){
        uint8* ret = NULL;
        if (_head < _tail || _size == 0){
            // [_tail, this->len) && [0, _head)
            if (this->len - _tail >= len){
                ret = base + _tail;
                _tail += len;
                if (_tail == this->len) _tail = 0;
            }else if(_head >= len){
                ret = base;
                _tail = len;
            }
        }else{
            // [_tail, _head)
            if (_head - _tail >= len){
                ret = base + _tail;
                _tail += len;
            }
        }
        _size += ret ? len : 0;
        return ret;
    }
    void   free(uint64 len){
        _size -= len;
        if (len <= this->len - _head){
            _head += len;
            if (_head == this->len) _head = 0;
        }else{
            _head = len;
        }
    }
private:
    uint64 _head;
    uint64 _tail;
    uint64 _size;
};

#endif // HBUF_H
