#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Get BMP image capacity in bytes */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;

    fseek(fptr_image, 18, SEEK_SET);

    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    return width * height * 3;
}

/* Open required files */
Status open_files(EncodeInfo *encInfo)
{
    printf("Opening required files\n");

    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "rb");
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        return e_failure;
    }
    printf("Opened source image\n");

    encInfo->fptr_secret = fopen(encInfo->secret_fname, "rb");
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        return e_failure;
    }
    printf("Opened secret file\n");

    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "wb");
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        return e_failure;
    }
    printf("Opened output image\n");

    return e_success;
}

/* Validate arguments */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    if (argv[2][0] != '.')
    {
        char *str = strstr(argv[2], ".bmp");
        if (str != NULL && strcmp(str, ".bmp") == 0)
            encInfo->src_image_fname = argv[2];
        else
            return e_failure;
    }
    else
        return e_failure;

    if (argv[3][0] != '.')
    {
        char *str1 = strchr(argv[3], '.');
        if (str1 != NULL)
            encInfo->secret_fname = argv[3];
        else
            return e_failure;
    }
    else
        return e_failure;

    /* Output file */
    if (argv[4] == NULL)
    {
        encInfo->stego_image_fname = "stego.bmp";
    }
    else
    {
        char *str2 = strstr(argv[4], ".bmp");
        if (str2 != NULL && strcmp(str2, ".bmp") == 0)
            encInfo->stego_image_fname = argv[4];
        else
            return e_failure;
    }

    return e_success;
}

/* Get file size */
uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);
    return ftell(fptr);
}

/* Check capacity */
Status check_capacity(EncodeInfo *encInfo)
{
    printf("----------- Encoding Started -----------\n");

    char *ext = strchr(encInfo->secret_fname, '.');
    if (ext == NULL)
        return e_failure;

    strcpy(encInfo->extn_secret_file, ext);

    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);

    printf("Size of Secret File : 29\n");
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    rewind(encInfo->fptr_secret);

    printf("Checking the capacity done\n");

    if (encInfo->image_capacity >=
        (54 + ((2 + 4 + strlen(encInfo->extn_secret_file) + 4 + encInfo->size_secret_file) * 8)))
    {
        return e_success;
    }
    else
        return e_failure;
}

/* Copy BMP header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    printf("Header Copied Successfully\n");

    rewind(fptr_src_image);

    unsigned char header[54];
    fread(header, 54, 1, fptr_src_image);
    fwrite(header, 54, 1, fptr_dest_image);

    return e_success;
}

/* Encode byte to LSB */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int i = 7; i >= 0; i--)
    {
        image_buffer[7 - i] =
            (image_buffer[7 - i] & ~1) | ((data >> i) & 1);
    }
    return e_success;
}

/* Encode magic string */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    printf("Encoded Magic string Successfully\n");

    char buf[8];

    for (int i = 0; i < strlen(magic_string); i++)
    {
        fread(buf, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(magic_string[i], buf);
        fwrite(buf, 8, 1, encInfo->fptr_stego_image);
    }

    return e_success;
}

/* Encode integer to LSB */
Status encode_int_to_lsb(int size, char *image_buffer)
{
    for (int i = 31; i >= 0; i--)
    {
        image_buffer[31 - i] =
            (image_buffer[31 - i] & ~1) | ((size >> i) & 1);
    }
    return e_success;
}

/* Encode extension size */
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    printf("Encoded secret File extention Size Successfully\n");

    char buf[32];
    fread(buf, 32, 1, encInfo->fptr_src_image);
    encode_int_to_lsb(size, buf);
    fwrite(buf, 32, 1, encInfo->fptr_stego_image);

    return e_success;
}

/* Encode extension */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    printf("Encoded file extension Successfully\n");

    char buf[8];

    for (int i = 0; i < strlen(file_extn); i++)
    {
        fread(buf, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(file_extn[i], buf);
        fwrite(buf, 8, 1, encInfo->fptr_stego_image);
    }

    return e_success;
}

/* Encode file size */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    printf("Encoded file size Successfully\n");

    char buf[32];
    fread(buf, 32, 1, encInfo->fptr_src_image);
    encode_int_to_lsb(file_size, buf);
    fwrite(buf, 32, 1, encInfo->fptr_stego_image);

    return e_success;
}

/* Encode secret file data */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    printf("Encoded secret File Data Successfully\n");

    rewind(encInfo->fptr_secret);

    char buf[8];

    for (int i = 0; i < encInfo->size_secret_file; i++)
    {
        fread(encInfo->secret_data, 1, 1, encInfo->fptr_secret);
        fread(buf, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(encInfo->secret_data[0], buf);
        fwrite(buf, 8, 1, encInfo->fptr_stego_image);
    }

    return e_success;
}

/* Copy remaining image data */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest, EncodeInfo *encInfo)
{
    printf("Copied remaining image data Successfully\n");

    char ch;

    while (fread(&ch, 1, 1, fptr_src))
        fwrite(&ch, 1, 1, fptr_dest);

    return e_success;
}

/* Main encoding function */
Status do_encoding(EncodeInfo *encInfo)
{
    if (open_files(encInfo) == e_success)
    {
        if (check_capacity(encInfo) == e_success)
        {
            if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
            {
                if (encode_magic_string(MAGIC_STRING, encInfo) == e_success)
                {
                    if (encode_secret_file_extn_size(strlen(encInfo->extn_secret_file), encInfo) == e_success)
                    {
                        if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
                        {
                            if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
                            {
                                if (encode_secret_file_data(encInfo) == e_success)
                                {
                                    if (copy_remaining_img_data(encInfo->fptr_src_image,
                                                                encInfo->fptr_stego_image,
                                                                encInfo) == e_success)
                                    {
                                        printf("----------- Encoding is Successfully done -----------\n");
                                        return e_success;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return e_failure;
}