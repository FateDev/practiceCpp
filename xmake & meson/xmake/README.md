Easy enough to run:
Run `xmake` in both `test` and `test_lib`, then in `test` run the following:
`LD_LIBRARY_PATH=../test_lib/build/linux/x86_64/release ./build/linux/x86_64/release/test`
(It runs the program while adding the library to the runtime search path temporarily).