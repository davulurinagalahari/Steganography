
📔 LSB Image Steganography

A command-line based application written in C that enables secure communication by embedding secret text inside image files using the Least Significant Bit (LSB) technique.

🚀 Overview

This project demonstrates how data can be hidden within an image without noticeably changing its appearance. By altering only the least significant bits of pixel values, the application ensures that the visual quality of the image remains intact while carrying hidden information.

It supports both encoding (hiding data) and decoding (retrieving data) operations, making it a complete steganography solution.

💻 Tech Stack
C Programming
File I/O Operations
Bitwise Manipulation
CLI (Command Line Interface)

🔧 Functionalities
Hide a secret message inside an image file
Extract hidden data from a stego-image
Check whether the image has enough capacity before encoding
Provide error messages for invalid inputs or operations
Maintain original image quality after embedding data

⚙️ Working Principle
🔐 Encoding Process

The program reads each pixel of the input image and replaces the least significant bit with bits from the secret message. This process continues until the entire message is embedded into the image.

🔓 Decoding Process

The application extracts the least significant bits from the modified image and reconstructs the original message.

⚡ Challenges
Handling data at the bit level accurately
Ensuring the image file structure is not corrupted
Designing proper validation for input size and format

📚 Learning Outcomes
Gained hands-on experience with bitwise operations
Understood practical implementation of steganography techniques
Improved skills in file handling in C
Built a structured and user-friendly CLI tool

