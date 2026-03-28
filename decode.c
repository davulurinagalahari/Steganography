#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "decode.h"
#include "common.h"
/* Read & validate decode arguments */
Status_d read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if (argv[2] && strstr(argv[2], ".bmp"))
        decInfo->d_src_image_fname = argv[2];
    else
        return d_failure;
    decInfo->d_secret_fname = malloc(100);
    if (decInfo->d_secret_fname == NULL)
        return d_failure;
    if (argv[3])
        strcpy(decInfo->d_secret_fname, argv[3]);
    else
        strcpy(decInfo->d_secret_fname, "decoded");
    return d_success;
}
/* Open stego image */
Status_d open_files_dec(DecodeInfo *decInfo)
{
    decInfo->fptr_d_src_image = fopen(decInfo->d_src_image_fname, "rb");
    if (decInfo->fptr_d_src_image == NULL)
        return d_failure;
    return d_success;
}
/* Skip BMP header */
Status_d skip_bmp_header(FILE *fptr)
{
    fseek(fptr, 54, SEEK_SET);
    return d_success;
}
/* Decode 32-bit size (MSB-first) */
Status_d decode_size_from_lsb(char *buffer, int *size)
{
    int num = 0;

    for (int i = 0; i < 32; i++)
    {
        num |= (buffer[i] & 1) << (31 - i);
    }

    *size = num;
    return d_success;
}
/* Decode one byte (MSB-first) */
Status_d decode_byte_from_lsb(char *data, char *image_buffer)
{
    unsigned char ch = 0;

    for (int i = 0; i < 8; i++)
    {
        ch |= (image_buffer[i] & 1) << (7 - i);
    }

    *data = ch;
    return d_success;
}
/* Decode data from image */
Status_d decode_data_from_image(int size, FILE *fptr, char *data)
{
    char buffer[8];

    for (int i = 0; i < size; i++)
    {
        fread(buffer, 1, 8, fptr);
        decode_byte_from_lsb(&data[i], buffer);
    }

    return d_success;
}
/* Decode magic string size */
Status_d decode_magic_string_size(DecodeInfo *decInfo)
{
    char buffer[32];
    fread(buffer, 1, 32, decInfo->fptr_d_src_image);
    decode_size_from_lsb(buffer, &decInfo->magic_string_size);
    return d_success;
}
/* Decode magic string */
Status_d decode_magic_string(DecodeInfo *decInfo)
{
    decInfo->magic_data = malloc(decInfo->magic_string_size + 1);
    if (decInfo->magic_data == NULL)
        return d_failure;
    decode_data_from_image(decInfo->magic_string_size,decInfo->fptr_d_src_image,decInfo->magic_data);
    decInfo->magic_data[decInfo->magic_string_size] = '\0';
    return d_success;
}
/* Compare magic string with USER input */
Status_d compare_magic_string(DecodeInfo *decInfo)
{
    char user_magic[100];
    printf("Enter magic string: ");
    scanf("%s", user_magic);
    if (strcmp(user_magic, decInfo->magic_data) == 0)
        return d_success;
    else
        return d_failure;
}
/* Decode extension size */
Status_d decode_file_extn_size(DecodeInfo *decInfo)
{
    char buffer[32];
    fread(buffer, 1, 32, decInfo->fptr_d_src_image);
    decode_size_from_lsb(buffer, &decInfo->extn_size);
    return d_success;
}
/* Decode extension */
Status_d decode_secret_file_extn(DecodeInfo *decInfo)
{
    decInfo->d_extn_secret_file = malloc(decInfo->extn_size + 1);
    if (decInfo->d_extn_secret_file == NULL)
        return d_failure;
    decode_data_from_image(decInfo->extn_size,decInfo->fptr_d_src_image,decInfo->d_extn_secret_file);
    decInfo->d_extn_secret_file[decInfo->extn_size] = '\0';
    return d_success;
}
/* Append extension */
Status_d concat_extension(DecodeInfo *decInfo)
{
    strcat(decInfo->d_secret_fname, decInfo->d_extn_secret_file);
    return d_success;
}
/* Open decoded output file */
Status_d open_decoded_file(DecodeInfo *decInfo)
{
    decInfo->fptr_d_secret = fopen(decInfo->d_secret_fname, "wb");
    if (decInfo->fptr_d_secret == NULL)
        return d_failure;
    return d_success;
}
/* Decode secret file size */
Status_d decode_secret_file_size(DecodeInfo *decInfo)
{
    char buffer[32];
    fread(buffer, 1, 32, decInfo->fptr_d_src_image);
    decode_size_from_lsb(buffer, &decInfo->size_secret_file);
    return d_success;
}
/* Decode secret file data */
Status_d decode_secret_file_data(DecodeInfo *decInfo)
{
    char buffer[8];
    char ch;
    for (int i = 0; i < decInfo->size_secret_file; i++)
    {
        fread(buffer, 1, 8, decInfo->fptr_d_src_image);
        decode_byte_from_lsb(&ch, buffer);
        fputc(ch, decInfo->fptr_d_secret);
    }
    return d_success;
}
/* Main decoding flow */
Status_d do_decoding(DecodeInfo *decInfo)
{
    if (open_files_dec(decInfo) == d_failure)
    {
        printf("Error: Unable to open stego image\n");
        return d_failure;
    }
    skip_bmp_header(decInfo->fptr_d_src_image);
    decode_magic_string_size(decInfo);
    if (decInfo->magic_string_size <= 0 || decInfo->magic_string_size > 50)
    {
        printf("Error: Invalid magic string size\n");
        return d_failure;
    }
    decode_magic_string(decInfo);
    if (compare_magic_string(decInfo) == d_failure)
    {
        printf("Magic string mismatch\n");
        return d_failure;
    }
    decode_file_extn_size(decInfo);
    decode_secret_file_extn(decInfo);
    concat_extension(decInfo);
    if (open_decoded_file(decInfo) == d_failure)
    {
        printf("Error: Unable to open output file\n");
        return d_failure;
    }
    decode_secret_file_size(decInfo);
    decode_secret_file_data(decInfo);
    printf("Decoding completed successfully\n");
    return d_success;
}
