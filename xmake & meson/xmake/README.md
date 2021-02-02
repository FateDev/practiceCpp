Easy enough to run:<br>
Run `xmake` in both `test` and `test_lib`, then in `test` run the following:<br>
`LD_LIBRARY_PATH=../test_lib/build/linux/x86_64/release ./build/linux/x86_64/release/test`<br>
(It runs the program while adding the library to the runtime search path temporarily).
