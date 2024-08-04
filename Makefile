.PHONY: init init.release build test format format.check lint lint.check docs docs.deploy clean
init:
	meson setup build --warnlevel=2 --wipe --werror

init.release:
	meson setup build --warnlevel=2 --wipe --werror --buildtype release

build:
	meson compile -C build

test:
	meson test -C build

format:
	ninja -C build clang-format

format.check:
	ninja -C build clang-format-check

lint:
	ninja -C build clang-tidy-fix

lint.check:
	ninja -C build clang-tidy

docs:
	doxygen 

docs.deploy: docs
	npx wrangler pages deploy ./html 

clean:
	rm -rf ./build
	rm -rf ./html
