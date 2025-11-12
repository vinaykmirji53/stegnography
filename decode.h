#ifndef DECODE_H
#define DECODE_H
#include<stdio.h>
#include "types.h" 

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX_DECODE 4

typedef struct _DecodeInfo
{
    // Destination Image info  
    char *dest_image_fname;
    FILE *fptr_dest_image;

    // output File Info        //output.txt
    char *output_fname;  //store the secrete file name
    FILE *fptr_output;
    char extn_output_file[MAX_FILE_SUFFIX_DECODE]; 
    long size_output_file;

    int extn_size;

} DecodeInfo;


Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);
Status do_decoding(DecodeInfo *decInfo);

Status open_files_for_decoding(DecodeInfo *decInfo);
Status skip_bmp_header(FILE *fptr_dest_image);
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);
Status decode_secret_file_extn_size(int *size, DecodeInfo *decInfo); 
Status decode_secret_file_extn(char *file_extn, DecodeInfo *decInfo);
Status decode_secret_file_size(long *file_size, DecodeInfo *decInfo);
Status decode_secret_file_data(DecodeInfo *decInfo);
Status decode_int_from_lsb(int *size, char *image_buffer); //collecting 32 bytes of data
Status decode_byte_from_lsb(char *data, char *image_buffer); // collecting 8 bytes of data  // Fixed: char *data

#endif