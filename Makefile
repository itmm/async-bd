HEADERs := $(wildcard *.h)
SOURCEs := $(wildcard *.cpp)
APP := async-bd-tests

CPPFLAGS += -Wall -std=c++17

.PHONY: clean tests

tests: $(APP)
	@echo TESTS
	@./$(APP) && echo "  ALL TESTS PASSED"

$(SOURCEs): $(HEADERs)
	@touch $@

$(APP): $(SOURCEs)
	@echo C++ $@
	@$(CXX) $^ -o $@

clean:
	@echo RM
	@rm -f $(APP)
