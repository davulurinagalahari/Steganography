#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;

    fseek(fptr_image, 18, SEEK_SET);
    fread(&width, sizeof(int), 1, fptr_image);
    fread(&height, sizeof(int), 1, fptr_image);

    printf("width = %u\n", width);
    printf("height = %u\n", height);

    return width * height * 3;
}

/* Open files */
Status open_files(EncodeInfo *encInfo)
{
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        return e_failure;
    }

    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        return e_failure;
    }

    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        return e_failure;
    }

    return e_success;
}

/* Read and validate encode arguments */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    // ./a.out -e beautiful.bmp secret.txt stego.bmp

    if (strcmp(strstr(argv[2], "."), ".bmp") == 0)
    {

        encInfo->src_image_fname = argv[2];
    }
    else
    {
        return e_failure;
    }

    encInfo->secret_fname = argv[3];

    if (argv[4] != NULL)
    {
        encInfo->stego_image_fname = argv[4];
    }
    else
    {
        encInfo->stego_image_fname = "stego.bmp";
    }

    printf("Read and validate encode arguments is a success\n");
    return e_success;
}

/* Check capacity */
Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    printf("Enter the magic string : ");
    scanf("%s", encInfo->magic_string);
    fseek(encInfo->fptr_secret, 0, SEEK_END);
    encInfo->size_secret_file = ftell(encInfo->fptr_secret);
    rewind(encInfo->fptr_secret);
    strcpy(encInfo->extn_secret_file, strstr(encInfo->secret_fname, "."));
    int size_of_info =(4 + strlen(encInfo->magic_string) + 4 + strlen(encInfo->extn_secret_file) + 4 + encInfo->size_secret_file);
    if (encInfo->image_capacity > size_of_info * 8)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

/* Copy BMP header */
Status copy_bmp_header(FILE *fptr_src_image,FILE *fptr_dest_image)
{
    char arr[54];
    fread(arr, 54, 1, fptr_src_image);
    fwrite(arr, 54, 1, fptr_dest_image);
    return e_success;
}

/* Encode magic string size */
Status encode_magic_string_size(EncodeInfo *encInfo)
{
    char arr[32];
    fread(arr, 32, 1, encInfo->fptr_src_image);
    encode_size_to_lsb(strlen(encInfo->magic_string), arr);
    fwrite(arr, 32, 1, encInfo->fptr_stego_image);
    return e_success;
}

/* Encode magic string */
Status encode_magic_string(char *magic_string,EncodeInfo *encInfo)
{
    encode_data_to_image(magic_string,strlen(magic_string),encInfo->fptr_src_image,encInfo->fptr_stego_image,encInfo);
    return e_success;
}
/* Encode secret file extension size */
Status encode_secret_file_extn_size(int size,FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char arr[32];
    fread(arr, 32, 1, fptr_src_image);
    encode_size_to_lsb(size, arr);
    fwrite(arr, 32, 1, fptr_stego_image);
    return e_success;
}
Status encode_size_to_lsb(int size, char *image_buffer)
{
    for (int i = 0; i < 32; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((size >> (31 - i)) & 1);
    }
    return e_success;
}
/* Encode secret file extension */
Status encode_secret_file_extn(char *file_extn,EncodeInfo *encInfo)
{
    encode_data_to_image(file_extn,strlen(file_extn), encInfo->fptr_src_image,encInfo->fptr_stego_image,encInfo);
    return e_success;
}
/* Encode data to image */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image,FILE *fptr_stego_image,EncodeInfo *encInfo)
{
    for (int i = 0; i < size; i++)
    {
        fread(encInfo->image_data, 8, 1, fptr_src_image);
        encode_byte_to_lsb(data[i], encInfo->image_data);
        fwrite(encInfo->image_data, 8, 1, fptr_stego_image);
    }
    return e_success;
}
/* Encode secret file size into LSB */
Status encode_secret_file_size(int size, EncodeInfo *encInfo)
{
    char image_buffer[32];

    fread(image_buffer, 32, 1, encInfo->fptr_src_image);

    encode_size_to_lsb(size, image_buffer);

    fwrite(image_buffer, 32, 1, encInfo->fptr_stego_image);

    return e_success;
}

Status encode_data_size(int size,FILE *fptr_src_image,FILE *fptr_stego_image)
{
    char image_buffer[32];
    fread(image_buffer, 32, 1, fptr_src_image);
    encode_size_to_lsb(size, image_buffer);
    fwrite(image_buffer, 32, 1, fptr_stego_image);
    return e_success;
}
// Function definition for encode secret file data
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    int size = ftell(encInfo->fptr_secret);
    char data[size];
    rewind(encInfo->fptr_secret);
    fread(data, size, 1, encInfo->fptr_secret);
    encode_data_to_image(data, size,encInfo->fptr_src_image,encInfo->fptr_stego_image,encInfo);
    return e_success;
}

/* Copy remaining image data */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    char ch;
    while (fread(&ch, 1, 1, fptr_src))
    {
        fwrite(&ch, 1, 1, fptr_dest);
    }
    return e_success;
}


/* Encode size to LSB */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int i = 0; i < 8; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((data >> (7 - i)) & 1);
    }
    return e_success;
}

Status do_encoding(EncodeInfo *encInfo)
{
    if (open_files(encInfo) != e_success)
    {
        printf("Open files is a failure\n");
        return e_failure;
    }
    printf("Open files is successfully\n");

    if (check_capacity(encInfo) != e_success)
    {
        printf("Check capacity is a failure\n");
        return e_failure;
    }
    printf("Check capacity is successfully\n");

    if (copy_bmp_header(encInfo->fptr_src_image,
                        encInfo->fptr_stego_image) != e_success)
    {
        printf("Failed to copy bmp header\n");
        return e_failure;
    }
    printf("Copied bmp header successfully\n");

    if (encode_magic_string_size(encInfo) != e_success)
    {
        printf("Failed to encode magic string size\n");
        return e_failure;
    }
    printf("Encoded magic string size successfully\n");

    if (encode_magic_string(MAGIC_STRING, encInfo) != e_success)
    {
        printf("Failed to encode magic string\n");
        return e_failure;
    }
    printf("Encoded magic string successfully\n");

    strcpy(encInfo->extn_secret_file,strstr(encInfo->secret_fname, "."));

    if (encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo->fptr_src_image,encInfo->fptr_stego_image) != e_success)
    {
        printf("Failed to encode secret file extn size\n");
        return e_failure;
    }
    printf("Encoded secret file extn size successfully\n");

    if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) != e_success)
    {
        printf("Failed to encode secret file extn\n");
        return e_failure;
    }
    printf("Encoded secret file extn successfully\n");
    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) != e_success)
    {
        printf("Failed to encode secret file size\n");
        return e_failure;
    }
    printf("Encoded secret file size successfully\n");

    if (encode_secret_file_data(encInfo) != e_success)
    {
        printf("Failed to encode secret file data\n");
        return e_failure;
    }
    printf("Encoded secret file data successfully\n");
    if (copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image) != e_success)
    {
        printf("Failed to copy remaining data\n");
        return e_failure;
    }
    printf("Copied remaining data successfully\n");

    return e_success;
}
