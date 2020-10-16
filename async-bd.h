#pragma once

#include <algorithm>

namespace Async_BD {
	enum class Error_Reason {
		not_implemented,
		invalid_buffer,
		out_of_range
	};
	class Callback {
	public:
		virtual ~Callback() {}
		virtual void success(unsigned block, const char *begin, const char *end) {}
		virtual void failed(unsigned block, const char *begin, const char *end, Error_Reason reason) {}
	};
	class Block_Device {
	protected:
		bool valid_args(unsigned block, const char *begin, const char *end, Callback &cb) {
			if (block >= blocks()) { cb.failed(block, begin, end, Error_Reason::out_of_range); return false; }
			if (! begin || end - begin != block_size) { cb.failed(block, begin, end, Error_Reason::invalid_buffer); return false; }
			return true;
		}
		void not_implemented(unsigned block, const char *begin, const char *end, Callback &cb) {
			if (valid_args(block, begin, end, cb)) {
				cb.failed(block, begin, end, Error_Reason::not_implemented);
			}
		}
	public:
		constexpr static int block_size { 512 };

		virtual ~Block_Device() {}
		virtual unsigned blocks() { return 0; }
		virtual void read(unsigned block, char *begin, const char *end, Callback &cb) { not_implemented(block, begin, end, cb); }
		virtual void write(unsigned block, const char *begin, const char *end, Callback &cb) { not_implemented(block, begin, end, cb); }
	};
	class Mem_Device: public Block_Device {
		unsigned blocks_;
		char *mem_;
	public:
		Mem_Device(unsigned blocks):
			blocks_ { blocks },
			mem_ { blocks > 0 ? new char[blocks * block_size] : nullptr }
		{}
		~Mem_Device() {
			if (mem_) { delete[] mem_; mem_ = nullptr; }
		}
		unsigned blocks() override { return blocks_; }
		void read(unsigned block, char *begin, const char *end, Callback &cb) override {
			if (valid_args(block, begin, end, cb)) {
				const char *block_begin { mem_ + block * block_size };
				const char *block_end { block_begin + block_size };
				std::copy(block_begin, block_end, begin);
				cb.success(block, begin, end);
			}
		}
		void write(unsigned block, const char *begin, const char *end, Callback &cb) override {
			if (valid_args(block, begin, end, cb)) {
				char *block_begin { mem_ + block * block_size };
				std::copy(begin, end, block_begin);
				cb.success(block, begin, end);
			}
		}
	};
}
