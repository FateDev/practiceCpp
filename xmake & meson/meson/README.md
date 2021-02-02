Just go into the `test_lib` and `test` and run `meson init --name (proj name) --build`.
That'll generate the `build` directory, then go into those and run `meson compile`.
Then to run the `testing` program, go into `test` and run:
`LD_LIBRARY_PATH=../test_lib/build ./build/testing`
(It runs the program while adding the library to the runtime search path temporarily).