#pragma once
#include "common/stream/cFileStream.h"
#include "common/stream/cStream_win.h"
#include "../../container/pString.h"

namespace nspFile {

	using String = nspString::pString;
	using FileStream = common::stream::cFileStream;
	constexpr size_t BUFFER_SIZE = 1 * 1024 * 1024;

	class pFileHandler {

		bool _is_open = false;
		
		size_t _file_size = 0;
		size_t _buffer_pos = 0;
		size_t _buffer_size = BUFFER_SIZE;	
		size_t _pos = 0;
		String _filename = "";
		String* _buffer = nullptr;
		
		FileStream* _file = nullptr;

		void _set_buffer(size_t position = 0) {
			delete _buffer;
			_buffer_pos = position;
			char* tmp_buf = new char[_buffer_size + 1];
			memset(tmp_buf, '\0', _buffer_size + 1);
			_file->Seek(_buffer_pos);
			_file->Read(tmp_buf, (int) _buffer_size);
			_buffer = new String(tmp_buf);
			delete[] tmp_buf;
			tmp_buf = nullptr;
		}

		void _close() {
			if (_file && _is_open) {
				_file->Close();
			}
		}

	public:
		pFileHandler() = delete;

		pFileHandler(String filename, bool binary = false) {
			_filename = filename;
			_file = new FileStream();
			_is_open = _file->Open(_filename.c_str(), ACCESS_READ, FILE_OPEN);
			_file_size = (size_t)_file->GetSize();
			_set_buffer();
		}

		pFileHandler(const char* filename, size_t buf_size_override, bool binary = false) {
			_filename = String(filename);
			_file = new FileStream();
			_is_open = _file->Open(_filename.c_str(), ACCESS_READ, FILE_OPEN);
			_file_size = (size_t)_file->GetSize();
			_buffer_size = buf_size_override;
			_set_buffer();
		}

		pFileHandler(pFileHandler& other) = delete;

		pFileHandler(pFileHandler&& other) noexcept
			: _is_open(other._is_open),
			_filename(other._filename),
			_file_size(other._file_size),
			_file(other._file)
		{
			other._file = nullptr;
		}

		~pFileHandler() {
			_close();
			delete _buffer;
			_buffer = nullptr;
			delete _file;
			_file = nullptr;
		}

		pFileHandler& operator=(pFileHandler& other) = delete;
		
		pFileHandler& operator=(pFileHandler&& other) noexcept {
			_close();
			delete _file;
			_file = nullptr;
			_is_open = other._is_open;
			_filename = other._filename;
			_file_size = other._file_size;
			_file = other._file;
			other._file = nullptr;
			return *this;
		}

		size_t position() const {
			return _pos;
		}

		bool set_position(size_t pos) {
			if (pos < _file_size) {
				_pos = pos;
				if (_pos < _buffer_pos || _pos >= _buffer_pos + _buffer_size) {
					_set_buffer(_pos);
				}
				return true;
			}
			return false;
		}

		size_t size() const {
			return _file_size;
		}

		char get_char() {
			if (_pos < _file_size) {
				if (_pos >= _buffer_pos + _buffer_size || _pos < _buffer_pos) {
					_set_buffer(_pos);
				}
				char read_val = (*_buffer)[_pos - _buffer_pos];
				_pos++;
				return read_val;
			}
			return '\0';
		}

		void unget_char() {
			_pos--;
		}

		char operator[] (size_t index) {
			_pos = index;
			if (_pos < _file_size) {
				if (_pos >= _buffer_pos + _buffer_size || _pos < _buffer_pos) {
					_set_buffer(_pos);
				}
				return (*_buffer)[_pos - _buffer_pos];
			}
			return '\0';
		}
	};
}