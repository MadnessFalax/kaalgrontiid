#pragma once
#include "common/stream/cFileStream.h"
#include "common/stream/cStream.h"
#include "../../container/pString.h"
#include "pBinaryException.h"

namespace nspFile {

	using String = nspString::pString;
	using FileStream = common::stream::cFileStream;
	constexpr size_t BUFFER_SIZE = 1 * 1024 * 1024;

	class pFileHandler {
	public:
		enum class ByteOrder {
			BE,			// BIG ENDIAN
			LE			// LITTLE ENDIAN
		};

	private:

		bool _is_open = false;
		
		size_t _file_size = 0;
		size_t _buffer_pos = 0;
		size_t _buffer_size = BUFFER_SIZE;	
		size_t _pos = 0;
		String _filename = "";
		String* _buffer = nullptr;
		bool _binary = false;
		char* _binary_buffer = nullptr;
		
		FileStream* _file = nullptr;

		void _set_buffer(size_t position = 0) {
			_buffer_pos = position;
			_file->Seek(_buffer_pos);
			if (_binary) {
				delete _binary_buffer;
			}
			else {
				delete _buffer;
			}
			char* tmp_buf = new char[_buffer_size + 1];
			memset(tmp_buf, '\0', _buffer_size + 1);
			_file->Read(tmp_buf, (int) _buffer_size);
			if (_binary) {
				_binary_buffer = tmp_buf;
			}
			else {
				_buffer = new String(tmp_buf);
				delete[] tmp_buf;
			}
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
			_binary = binary;
			_file = new FileStream();
			_is_open = _file->Open(reinterpret_cast<const char*>(_filename.c_str()), ACCESS_READ, FILE_OPEN);
			_file_size = (size_t)_file->GetSize();
			_set_buffer();
		}

		pFileHandler(const char* filename, size_t buf_size_override, bool binary = false) {
			_filename = String(filename);
			_binary = binary;
			_file = new FileStream();
			_is_open = _file->Open(reinterpret_cast<const char*>(_filename.c_str()), ACCESS_READ, FILE_OPEN);
			_file_size = (size_t)_file->GetSize();
			_buffer_size = buf_size_override;
			_set_buffer();
		}

		pFileHandler(pFileHandler& other) = delete;

		pFileHandler(pFileHandler&& other) noexcept
			: _is_open(other._is_open),
			_filename(other._filename),
			_file_size(other._file_size),
			_file(other._file),
			_buffer(other._buffer),
			_binary_buffer(other._binary_buffer),
			_binary(other._binary),
			_buffer_pos(other._buffer_pos)
		{
			other._file = nullptr;
			other._buffer = nullptr;
			other._binary_buffer = nullptr;
		}

		~pFileHandler() {
			_close();
			delete _binary_buffer;
			_binary_buffer = nullptr;
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
			delete _buffer;
			_buffer = nullptr;
			delete _binary_buffer;
			_binary_buffer = nullptr;
			
			_is_open = other._is_open;
			_filename = other._filename;
			_file_size = other._file_size;
			_file = other._file;
			_buffer = other._buffer;
			_binary_buffer = other._binary_buffer;
			_binary = other._binary;
			_buffer_pos = other._buffer_pos;

			other._buffer = nullptr;
			other._binary_buffer = nullptr;
			other._file = nullptr;
			return *this;
		}

		size_t position() const {
			return _pos;
		}

		bool set_position(size_t pos) {
			if (pos < 0) {
				pos = 0;
			}
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
				char read_val = 0;
				if (_binary) {
					read_val = _binary_buffer[_pos - _buffer_pos];
				}
				else {
					read_val = (*_buffer)[_pos - _buffer_pos];
				}
				_pos++;
				return read_val;
			}
			return '\0';
		}

		int get_int(ByteOrder byte_order = ByteOrder::LE) {
			if (!_binary) {
				throw pBinaryException();
			}

			if (_pos + 3 < _file_size) {
				if (_pos + 3 >= _buffer_pos + _buffer_size || _pos < _buffer_pos) {
					_set_buffer(_pos);
				}
				int read_val = 0;
				char* buf = new char[4];

				for (size_t i = 0; i < 4; i++) {
					buf[i] = _binary_buffer[_pos - _buffer_pos];
					_pos++;
				}

				if (byte_order == ByteOrder::BE) {
					for (size_t i = 0; i < 2; i++) {
						char tmp = buf[i];
						buf[i] = buf[3 - i];
						buf[3 - i] = tmp;
					}
					read_val = *reinterpret_cast<int*>(buf);
				}
				else {
					read_val = *reinterpret_cast<int*>(buf);
				}

				delete[] buf;
				buf = nullptr;

				return read_val;
			}
			return 0;
		}

		double get_double(ByteOrder byte_order = ByteOrder::LE) {
			if (!_binary) {
				throw pBinaryException();
			}

			if (_pos + 7 < _file_size) {
				if (_pos + 7 >= _buffer_pos + _buffer_size || _pos < _buffer_pos) {
					_set_buffer(_pos);
				}
				double read_val = 0;
				char* buf = new char[8];

				for (size_t i = 0; i < 8; i++) {
					buf[i] = _binary_buffer[_pos - _buffer_pos];
					_pos++;
				}

				if (byte_order == ByteOrder::BE) {
					for (size_t i = 0; i < 4; i++) {
						char tmp = buf[i];
						buf[i] = buf[7 - i];
						buf[7 - i] = tmp;
					}
					read_val = *reinterpret_cast<double*>(buf);
				}
				else {
					read_val = *reinterpret_cast<double*>(buf);
				}

				delete[] buf;
				buf = nullptr;

				return read_val;
			}
			return 0;
		}

		// shouldn't cause errors if called on first char as far as I know (I hope)
		void unget_char() {
			_pos--;
		}
	};
}