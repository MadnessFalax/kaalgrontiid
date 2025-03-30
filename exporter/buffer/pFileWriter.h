#pragma once
#include "common/stream/cFileStream.h"
#include "common/stream/cStream.h"
#include "../../container/pString.h"

namespace nspFile {

	using String = nspString::pString;
	using FileStream = common::stream::cFileStream;
	constexpr size_t BUFFER_SIZE = 1 * 1024 * 1024;

	class pFileWriter {
		
	public:
		enum class ByteOrder {
			BE,			// BIG ENDIAN
			LE			// LITTLE ENDIAN
		};

	private:

		bool _is_open = false;
		bool _is_binary = false;

		size_t _buffer_size = BUFFER_SIZE;
		String _filename = "";
		String* _buffer = nullptr;
		char* _binary_buffer = nullptr;
		size_t _cur_buffer_len = 0;

		FileStream* _file = nullptr;

		bool _flush() {
			if (_is_binary) {
				bool result = _file->Write(_binary_buffer, (int)_cur_buffer_len, nullptr);
				_cur_buffer_len = 0;
				return result;
			}
			else {
				auto buf_siz = _buffer->length() + 1;
				char* buf_cpy = new char[buf_siz];
				nspString::memcpy(reinterpret_cast<unsigned char*>(buf_cpy), _buffer->c_str(), buf_siz);
				bool result = _file->Write(buf_cpy, (int) buf_siz - 1, nullptr);
				delete[] buf_cpy;
				buf_cpy = nullptr;
				delete _buffer;
				_buffer = new String();

				return result;
			}
		}

		void _close() {
			if (_file && _is_open) {
				_file->Close();
			}
		}

	public:
		pFileWriter() = delete;
		pFileWriter(String path, bool is_binary = false) {
			_filename = path;
			_is_binary = is_binary;
			_file = new FileStream();
			_is_open = _file->Open(reinterpret_cast<const char*>(_filename.c_str()), ACCESS_WRITE, FILE_CREATE);
			if (_is_binary) {
				_binary_buffer = new char[_buffer_size];
				_cur_buffer_len = 0;
			}
			else {
				_buffer = new String();
			}
		}

		pFileWriter(const pFileWriter&) = delete;
		pFileWriter(pFileWriter&& other) noexcept {
			_filename = other._filename;
			_file = other._file;
			_is_open = other._is_open;
			_buffer = other._buffer;
			_is_binary = other._is_binary;
			_binary_buffer = other._binary_buffer;
			_cur_buffer_len = other._cur_buffer_len;
			
			other._binary_buffer = nullptr;
			other._filename = "";
			other._file = nullptr;
			other._buffer = nullptr;
		}
		
		pFileWriter& operator=(const pFileWriter&) = delete;
		pFileWriter& operator=(pFileWriter&& other) noexcept {
			if (this != &other) {
				_filename = other._filename;
				_file = other._file;
				_is_open = other._is_open;
				_buffer = other._buffer;
				_is_binary = other._is_binary;
				_binary_buffer = other._binary_buffer;
				_cur_buffer_len = other._cur_buffer_len;
				
				other._binary_buffer = nullptr;
				other._filename = "";
				other._file = nullptr;
				other._buffer = nullptr;
			}
		}


		bool write(String string) {
			bool status = false;
			(*_buffer) += string;
			if (_buffer->length() >= BUFFER_SIZE) {
				status = _flush();
			}
			return status;
		}

		bool write(unsigned char* buffer, size_t size) {
			bool status = false;
			if (_is_binary) {
				if (_cur_buffer_len + size >= _buffer_size) {
					_flush();
				}
				memcpy(&(_binary_buffer[_cur_buffer_len]), buffer, size);
				_cur_buffer_len += size;
			}
			return status;
		}

		bool write_int(int value, ByteOrder byte_order = ByteOrder::LE) {
			bool status = false;
			if (_is_binary) {
				if (_cur_buffer_len + 4 >= _buffer_size) {
					_flush();
				}

				auto* buf = reinterpret_cast<unsigned char*>(&value);

				if (byte_order == ByteOrder::BE) {
					for (size_t i = 0; i < 2; i++) {
						char tmp = buf[i];
						buf[i] = buf[3 - i];
						buf[3 - i] = tmp;
					}
				}
				memcpy(&(_binary_buffer[_cur_buffer_len]), buf, 4);
				_cur_buffer_len += 4;
			}
			return status;
		}

		bool write_double(double value, ByteOrder byte_order = ByteOrder::LE) {
			bool status = false;
			if (_is_binary) {
				if (_cur_buffer_len + 8 >= _buffer_size) {
					_flush();
				}

				auto buf = reinterpret_cast<unsigned char*>(&value);

				if (byte_order == ByteOrder::BE) {
					for (size_t i = 0; i < 4; i++) {
						char tmp = buf[i];
						buf[i] = buf[7 - i];
						buf[7 - i] = tmp;
					}
				}
				memcpy(&(_binary_buffer[_cur_buffer_len]), buf, 8);
				_cur_buffer_len += 8;
			}
			return status;
		}

		bool set_position(size_t pos = -1) {
			bool status = false;
			status &= _flush();
			if (pos == -1) {
				status &= _file->Seek(0, SEEK_END);
			}
			else {
				status &= _file->Seek(pos);
			}

			return status;
		}	

		~pFileWriter() {
			_flush();
			_close();
			delete _buffer;
			_buffer = nullptr;
			delete _file;
			_file = nullptr;
			if (_binary_buffer) {
				delete[] _binary_buffer;
				_binary_buffer = nullptr;
			}
		}
		
	};

}