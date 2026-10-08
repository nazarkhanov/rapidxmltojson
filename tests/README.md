# Regression tests

Build the extension and run the Python API tests:

```sh
python -m pip install .
python -m unittest discover -s tests -v
```

Run the deterministic native allocation regression test (requires a C++11 compiler):

```sh
c++ -std=c++11 -Iinclude tests/test_memory.cpp -o /tmp/rapidxmltojson-test-memory
/tmp/rapidxmltojson-test-memory
```

The native test compares outstanding `new`/`new[]` allocations before and after
1,000 calls per case, avoiding noisy process-RSS measurements. It checks both
successful namespace conversion and malformed input, including valid and invalid
XML documents large enough to grow RapidXML's pool. Each case also checks the exact output or
error text. The Python tests check wrapper exceptions and recovery after errors.

For additional memory-safety checks, run the native test with sanitizers on a
host that supports LeakSanitizer:

```sh
c++ -std=c++11 -g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer \
  -Iinclude tests/test_memory.cpp -o /tmp/rapidxmltojson-test-memory-asan
ASAN_OPTIONS=detect_leaks=1 /tmp/rapidxmltojson-test-memory-asan
```
