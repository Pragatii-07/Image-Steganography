# Image Steganography Using LSB Technique

## Description
A C programming project that hides secret data inside a BMP image using the Least Significant Bit (LSB) technique. The hidden data can later be extracted using the decoding process.

## Features
- Encode a secret file into a BMP image.
- Decode and recover the hidden file.
- Check image capacity before encoding.
- Store file information required for decoding.

## Technologies Used
- C Programming
- File Handling
- Bitwise Operators
- Structures and Functions
- BMP Image Format

## Project Files
- `encode.c` – Encoding implementation
- `encode.h` – Encoding declarations
- `decode.c` – Decoding implementation
- `decode.h` – Decoding declarations
- `common.h` – Common declarations
- `types.h` – Type definitions

## Compilation
Compile the C source files using GCC:

```bash
gcc *.c -o steganography
```

Run the program using the command-line arguments expected by the implementation.

## Concepts Learned
- Bitwise manipulation
- File input/output operations
- Data encoding and decoding
- Modular programming in C