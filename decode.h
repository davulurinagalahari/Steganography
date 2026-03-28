#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include "types.h"

typedef struct _DecodeInfo
{
    char *d_src_image_fname;
    FILE *fptr_d_src_image;

    int magic_string_size;
    char *magic_data;

    int extn_size;
    char *d_extn_secret_file;

    int size_secret_file;

    char *d_secret_fname;
    FILE *fptr_d_secret;

} DecodeInfo;

/* Prototypes */
Status_d read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);
Status_d do_decoding(DecodeInfo *decInfo);

Status_d open_files_dec(DecodeInfo *decInfo);
Status_d skip_bmp_header(FILE *fptr);

Status_d decode_magic_string_size(DecodeInfo *decInfo);
Status_d decode_size_from_lsb(char *buffer, int *size);

Status_d decode_magic_string(DecodeInfo *decInfo);
Status_d decode_data_from_image(int size, FILE *fptr, char *data);
Status_d decode_byte_from_lsb(char *data, char *image_buffer);

Status_d compare_magic_string(DecodeInfo *decInfo);

Status_d decode_file_extn_size(DecodeInfo *decInfo);
Status_d decode_secret_file_extn(DecodeInfo *decInfo);
Status_d concat_extension(DecodeInfo *decInfo);
Status_d open_decoded_file(DecodeInfo *decInfo);

Status_d decode_secret_file_size(DecodeInfo *decInfo);
Status_d decode_secret_file_data(DecodeInfo *decInfo);

#endif
