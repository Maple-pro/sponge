#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity): _myqueue({}), _capacity(capacity), _input_ended(false), _bytes_written(0), _bytes_read(0) { }

size_t ByteStream::write(const string &data) {
    const size_t len = data.length();
    size_t res = 0;
    for (size_t i = 0; i < len; i++) {
        if (_myqueue.size() < _capacity) {
            _myqueue.push_back(data[i]);
            res++;
            _bytes_written++;
        }
        else {
            break;
        }
    }
    return res;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    string res;
    size_t realPeekLen = len;
    if (_myqueue.size() < realPeekLen) {
        realPeekLen = _myqueue.size();
    }
    res.assign(_myqueue.begin(), _myqueue.begin() + realPeekLen);

    return res;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    size_t realPopLen = len;
    if (_myqueue.size() < realPopLen) {
        realPopLen = _myqueue.size();
    }
    _bytes_read += realPopLen;
    for (size_t i = 0; i < realPopLen; i++) {
        _myqueue.pop_front();
    }
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    string res = peek_output(len);
    pop_output(len);

    return res;
}

void ByteStream::end_input() { _input_ended = true; }

bool ByteStream::input_ended() const { return _input_ended; }

size_t ByteStream::buffer_size() const { return _myqueue.size(); }

bool ByteStream::buffer_empty() const { return _myqueue.size() == 0; }

bool ByteStream::eof() const {
    if (_input_ended && _myqueue.size() == 0) {
        return true;
    }
    else {
        return false;
    }
}

size_t ByteStream::bytes_written() const { return _bytes_written; }

size_t ByteStream::bytes_read() const { return _bytes_read; }

size_t ByteStream::remaining_capacity() const { return _capacity - _myqueue.size(); }
