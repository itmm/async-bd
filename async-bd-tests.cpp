#define async_bd_TESTS 1
#include "async-bd.h"

#include <cassert>
#include <iostream>

class Test_Callback: public Async_BD::Callback {
	unsigned step_ { 0 };
	static constexpr int test_blocks { 3 };
	Async_BD::Mem_Device mem_ { test_blocks };
	char page_[Async_BD::Block_Device::block_size];
public:
	Test_Callback() {
		assert(mem_.blocks() == test_blocks);
		success(0, page_, page_ + sizeof(page_));
	}
	void success(unsigned block, const char *begin, const char *end) override {
		unsigned step { step_++ };
		if (step < mem_.blocks()) {
			if (step > 0) {
				assert(block + 1 == step);
			}
			std::fill(page_, page_ + sizeof(page_), static_cast<char>(step));
			mem_.write(step, page_, page_ + sizeof(page_), *this);
		} else {
			if (step > mem_.blocks()) {
				assert(*begin == step - mem_.blocks() - 1);
			}
			if (step < 2 * mem_.blocks()) {
				mem_.read(step - mem_.blocks(), page_, page_ + sizeof(page_), *this);
			}
		}
	}
	void failed(unsigned block, const char *begin, const char *end, Async_BD::Error_Reason reason) override {
		std::cerr << "failed step " << step_ << '\n';
		std::exit(EXIT_FAILURE);
	}
};

int main() {
	Test_Callback cb;
}
