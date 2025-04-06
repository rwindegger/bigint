# bigint
A header-only C++ library provides a fixed-width arbitrary-precision integer type called `bigint`. It supports both signed and unsigned numbers with a customizable bit-width. The class implements a wide range of arithmetic, bitwise, and comparison operators, making it suitable for tasks that require precise control over large integers.

## Features
- **Customizable Bit-Width:**  
  Define your `bigint` with a specific number of bits (must be a multiple of 8).
- **Signed and Unsigned Support:**  
  Use the template parameter `is_signed` to select between signed (two’s complement) and unsigned behavior.
- **Arithmetic Operators:**
    - Addition (`+`, `+=`)
    - Subtraction (`-`, `-=`)
    - Multiplication (`*`, `*=`)
    - Division (`/`, `/=`)
    - Modulus (`%`, `%=`)
- **Unary Operators:**
    - Unary minus (`-`) which computes two’s complement for signed values
    - Increment and Decrement (prefix and postfix)
- **Bitwise Operators:**
    - AND (`&`, `&=`)
    - OR (`|`, `|=`)
    - XOR (`^`, `^=`)
    - NOT (`~`)
- **Shift Operators:**
    - Left Shift (`<<`, `<<=`)
    - Right Shift (`>>`, `>>=`)
- **Comparison Operators:**  
  Supports three-way comparisons (`<=>`) for both `bigint`s and built-in integral types.
- **String Initialization:**  
  Initialize from strings representing decimal, hexadecimal (prefix `0x`), binary (prefix `0b`), and octal numbers. Negative decimal values are supported for signed types.

## Usage
### Including the Library
Simply include the header in your project:

```cpp
#include "bigint.h"
```

### Creating bigints
Construct a `bigint` from an integral type or a string:

```cpp
// Create a 128-bit signed bigint from an integer:
bigint::bigint<128, true> a(123456789);

// Create a 128-bit unsigned bigint from a decimal string:
bigint::bigint<128, false> b("9876543210");

// Create a 128-bit signed bigint from a hexadecimal string:
bigint::bigint<128, true> c("0x1A2B3C4D");
```

### Performing Arithmetic and Bitwise Operations
The `bigint` class overloads many operators, allowing you to use familiar C++ syntax:

```cpp
auto sum   = a + b;
auto diff  = a - c;
auto prod  = a * c;
auto quot  = b / 12345;
auto mod   = b % a;

a += 100;
a -= 50;
a *= -3;
a /= 7;

bigint::bigint<128, true> d = ~a;   // Bitwise NOT
bigint::bigint<128, true> e = a & c;  // Bitwise AND
bigint::bigint<128, true> f = a | c;  // Bitwise OR
bigint::bigint<128, true> g = a ^ c;  // Bitwise XOR

a <<= 4;  // Left shift
b >>= 8;  // Right shift

if (a < b) {
    // Comparison
}
```

### Comparison
Compare `bigint`s or built-in integers using the three-way comparison operator:

```cpp
if (a < 1000) {
    // ...
}

if (a == b) {
    // ...
}
```

## Implementation Details
- **Internal Representation:** The number is stored as an array of bytes (`std::array<std::uint8_t, bits / CHAR_BIT>`) in native endianness. Operators are implemented to be endianness-aware.
- **Arithmetic Algorithms:**
  - **Multiplication:** Uses a school-book algorithm with proper carry propagation. 
  - **Division and Modulus:** Use a binary long-division algorithm that operates on each bit.
- **Overflow Handling:** Some helper operations (like multiplication and addition) throw `std::overflow_error` if an operation produces a result that exceeds the fixed width.
- **Two's Complement:** For signed `bigint`s, negative numbers are stored in two's complement form. The unary minus operator (`operator-()`) computes this by inverting the bits and adding one.

## Compilation and Requirements
This is a header-only library that requires a C++20-compliant compiler (for features like concepts, `<=>` and `std::endian`). To compile your project use a command such as:

```bash
g++ -std=c++20 -O2 -Wall your_program.cpp -o your_program
```

## Contributing

Contributions, bug reports, and feature requests are welcome! Feel free to open an [issue](https://github.com/rwindegger/bigint23/issues) or submit a pull request.

1. Fork it!
2. Create your feature branch: `git checkout -b feature/my-new-feature`
3. Commit your changes: `git commit -am 'Add some feature'`
4. Push to the branch: `git push origin feature/my-new-feature`
5. Submit a pull request

## License

This project is licensed under the [MIT License](LICENSE).