#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) : _output(capacity), _capacity(capacity) {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    // 需要维护：_unassembled_num, _next_index, _eof_flag, end_input()
    
    // 1. 先尽可能拼到 _output 中；
    // 2. 消除 _unassembled_bytes 中对应的值；
    // 3. 将超出部分放入 _unassembled_bytes 中；
    if (index == _next_index) { // 恰好能接上
        const size_t writtenBytes = _output.write(data);

        _next_index += writtenBytes;

        if (eof && writtenBytes == data.length()) {
            _output.end_input();
        }

        for (size_t i = index; i < index + writtenBytes; i++) {
            if (_unassembled_bytes.count(i)) {
                _unassembled_bytes.erase(i);
                _unassembled_num--;
            }
        }

        if (writtenBytes < data.length()) {
            for (size_t i = writtenBytes; i < data.length(); i++) {
                if (!_unassembled_bytes.count(i + index) && _unassembled_num < _capacity) {
                    _unassembled_bytes.insert(std::make_pair(i + index, data[i]));
                    _unassembled_num++;
                    if (i == data.length() - 1 && eof) {
                        _eof_flag = true;
                        _eof_index = i + index;
                    }
                }
            }
        }
    }
    else if (index > _next_index) { // 中间断开
        for (size_t i = 0; i < data.length(); i++) {
            if (!_unassembled_bytes.count(i + index) && _unassembled_num < _capacity) {
                _unassembled_bytes.insert(std::make_pair(i + index, data[i]));
                _unassembled_num++;
                if (i == data.length() - 1 && eof) {
                    _eof_flag = true;
                    _eof_index = i + index;
                }
            }
        }
    }
    else if (index < _next_index && index + data.length() > _next_index) { // 有一部分能接上
        string postString;
        const size_t offset = _next_index - index;
        postString.assign(data.begin() + offset, data.end());

        const size_t writtenBytes = _output.write(postString);
        const size_t origin_index = _next_index;
        _next_index += writtenBytes;

        if (eof && writtenBytes == postString.length()) {
            _output.end_input();
        }

        for (size_t i = origin_index; i < origin_index + writtenBytes; i++) {
            if (_unassembled_bytes.count(i)) {
                _unassembled_bytes.erase(i);
                _unassembled_num--;
            }
        }

        if (writtenBytes < postString.length()) {
            for (size_t i = writtenBytes; i < postString.length(); i++) {
                if (!_unassembled_bytes.count(origin_index + i) && _unassembled_num < _capacity) {
                    _unassembled_bytes.insert(std::make_pair(origin_index + i, postString[i]));
                    _unassembled_num++;
                    if (i == postString.length() - 1 && eof) {
                        _eof_flag = true;
                        _eof_index = i + origin_index;
                    }
                }
            }
        }
    }
    else { // 完全被覆盖
        return;
    }

    // 4. 拼接 _unassemble_bytes 中的值
    string fix;
    while (_unassembled_bytes.count(_next_index)) {
        if (fix.length() >= _output.remaining_capacity()) {
            break;
        }

        char item = _unassembled_bytes[_next_index];
        _unassembled_bytes.erase(_next_index);
        _unassembled_num--;
        fix += item;

        if (_eof_flag && _eof_index == _next_index) {
            _output.end_input();
        }

        _next_index++;
    }
    _output.write(fix);
}

size_t StreamReassembler::unassembled_bytes() const { return _unassembled_num; }

bool StreamReassembler::empty() const { return _output.input_ended(); }
