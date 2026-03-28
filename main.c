/* Documentation
    ROLL NO:25031A_006
    START DATE:30-01-2026
    END DATE:08-02-2026
    BATCH:31A
    Description :This project implements LSB (Least Significant Bit) Image Steganography to securely hide secret data inside digital images without visibly changing the image quality.
                 It uses pixel-level bitwise manipulation and BMP image structure knowledge to encode and decode hidden information safely.
                 The system also supports multiple file formats and reconstructs the original file during decoding using embedded metadata and authentication.
*/
#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"
#include "decode.h"


OperationType check_operation_type(char *argv[])
{
    if (strcmp(argv[1], "-e") == 0)
        return e_encode;
    else if (strcmp(argv[1], "-d") == 0)
        return e_decode;
    else
        return e_unsupported;
}

/* Passing arguments through command line arguments */
int main(int argc, char *argv[])
{
    if(argc < 4)
    {
        // error msg
        printf("Insufficient arguments\n");
        printf("Encoding: ./a.out -e beautiful.bmp secret.txt stego.bmp\n");
        printf("Decoding: ./a.out -d stego.bmp decode.txt\n");
        return 1;
    }  
    // Function call for check operation type
    if (check_operation_type(argv) == e_encode)
    {
        printf("Selected encoding\n");
        // Declare structure variable
        EncodeInfo encInfo;
        // Read and validate encode arguments
        if (read_and_validate_encode_args(argv, &encInfo) == e_success)
        {
            printf("Read and validate encode arguments is a success\n");
            printf(" Started Encoding \n");
            // Function call for encoding
            if (do_encoding(&encInfo) == e_success)
            {
                printf("<--------Completed encoding-------->\n");
            }
            else
            {
                printf("Failed to encode\n");
                return e_failure;
            }
        }
        else
        {
            printf("Read and validate encode arguments is a failure\n");
            return e_failure;
        }
    }
    // Function call for check operation type
    else if (check_operation_type(argv) == e_decode)
    {
        printf("Selected decoding\n");
        // Declare structure variables
        DecodeInfo decInfo;
        if (read_and_validate_decode_args(argv, &decInfo) == d_success)
        {
            printf("Read and validate decode arguments is a success\n");
            printf("Started Decoding \n");
            // Function call for do decoding
            if (do_decoding(&decInfo) == d_success)
            {
                printf("<---------Completed decoding--------->\n");
            }
            else
            {
                printf("Failed to decode\n");
                return e_failure;
            }
        }
        else
        {
            printf("Read and validate decode arguments is a failure\n");
            return e_failure;
        }
    }
    else
    {
        printf("Invalid option\nKindly pass for\nEncoding: ./a.out -e beautiful.bmp secret.txt stego.bmp\nDecoding: ./a.out -d stego.bmp decode.txt\n");
    }
    return 0;
}