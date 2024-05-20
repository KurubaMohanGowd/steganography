#ifndef DECODE_H
#define DECODE_H

#include "types.h"

typedef struct _DecodeInfo
{
    char *src_image_fname;
    FILE *fptr_src_image;

    char magic_str[10];
    char secret_file_ext[5];
    char secret_data[40];
    int secret_ext_size;
    int secret_file_size;

    char *output_fname;
    FILE *fptr_output_file;

}DecodeInfo;

Status do_decoding(DecodeInfo *decInfo);

Status decode_magic_string(char *magic_str,DecodeInfo *decInfo);

Status decode_image_to_data(char *magic_str,long size, FILE *fptr_src_image);

Status decode_magic_to_data(char *magic_str,long size, FILE *fptr_src_image);

Status decode_lsb_to_byte(char *buffer, int *int_ptr);

Status decode_lsb_to_size(char *buffer, int *size, DecodeInfo *decInfo);

Status decode_secret_file_ext_size(long file_size,DecodeInfo *decInfo);

Status decode_secret_file_ext(int secret_ext_size, DecodeInfo *decInfo);

Status open_decode_files(DecodeInfo *decInfo);

Status decode_secret_data(long size, FILE *fptr_src, FILE *fptr_out);

Status decode_secret_file_data(DecodeInfo *decInfo);

Status read_and_validate_decode_args(char *argv[],DecodeInfo *decInfo);

Status decode_secret_file_size(long file_size,DecodeInfo *decInfo);

void display_content(DecodeInfo *decInfo);


#endif
