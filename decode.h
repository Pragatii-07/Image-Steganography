#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* Source Image info */
    char *output_image_fname;
    FILE *fptr_output_image;
    //char image_data[MAX_IMAGE_BUF_SIZE];

    /* Secret File Info */
    char *output_fname;
    FILE *fptr_output;
    char extn_output_file[MAX_FILE_SUFFIX];
    char output_data[MAX_SECRET_BUF_SIZE];
    long output_extn_size;
    long size_secret_file;

} DecodeInfo;


/* Encoding function prototype */

/* Check operation type */
//OperationType check_operation_type(char *argv[]);

/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the encoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
Status open_dceode_files(char* file_name,DecodeInfo *decInfo);

/* check capacity */
//Status check_capacity(EncodeInfo *encInfo);

/* Get image size */
//uint get_image_size_for_bmp(FILE *fptr_image);

/* Get file size */
//uint get_file_size(FILE *fptr);

/* Copy bmp image header */
Status skip_bmp_header(FILE *fptr_output_image);

/* Store Magic String */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

/* Encode secret file extenstion size */
Status decode_secret_file_extn_size(DecodeInfo *decInfo);

/* Encode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Encode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Encode secret file data*/
Status decode_secret_file_data(DecodeInfo *decInfo);

/* Encode function, which does the real encoding */
Status decode_int_from_lsb(long *size,char *image_buffer);//32 byte

/* Encode a byte into LSB of image data array */
Status decode_byte_from_lsb(char* byte_data,char *image_buffer);

#endif
