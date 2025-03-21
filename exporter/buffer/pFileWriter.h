#pragma once
#include "common/stream/cFileStream.h"
#include "common/stream/cStream.h"
#include "../../container/pString.h"

namespace nspFile {

	using String = nspString::pString;
	using FileStream = common::stream::cFileStream;
	constexpr size_t BUFFER_SIZE = 1 * 1024 * 1024;

	class pFileWriter {
		
		bool _is_open = false;

		size_t _buffer_size = BUFFER_SIZE;
		String _filename = "";
		String* _buffer = nullptr;

		FileStream* _file = nullptr;

		bool _flush() {
			auto buf_siz = _buffer->length() + 1;
			char* buf_cpy = new char[buf_siz];
			memcpy(buf_cpy, _buffer->c_str(), buf_siz);
			bool result = _file->Write(buf_cpy, (int) buf_siz, nullptr);
			delete[] buf_cpy;
			buf_cpy = nullptr;

			return result;
		}

		void _close() {
			if (_file && _is_open) {
				_file->Close();
			}
		}

	public:
		pFileWriter() = delete;
		pFileWriter(String string) {
			_filename = string;
			_file = new FileStream();
			_is_open = _file->Open(reinterpret_cast<const char*>(_filename.c_str()), ACCESS_WRITE, FILE_CREATE);
			_buffer = new String();
		}

		pFileWriter(const pFileWriter&) = delete;
		pFileWriter(pFileWriter&& other) noexcept {
			_filename = other._filename;
			_file = other._file;
			_is_open = other._is_open;
			_buffer = other._buffer;
			
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

		~pFileWriter() {
			_flush();
			_close();
			delete _buffer;
			_buffer = nullptr;
			delete _file;
			_file = nullptr;
		}
		
	};

}