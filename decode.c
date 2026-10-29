#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

/* Global arrays to store output filename and magic string */
char out_name[100];
char new_magic[100];

/* Function to open stego image file */
Status open_decode_image_files(char *file_name, DecodeInfo *decInfo)
{
    /* Open stego image in read mode */
    decInfo->fptr_output_image = fopen(file_name, "r");

    /* Error handling */
    if (decInfo->fptr_output_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->output_image_fname);

        return e_failure;
    }

    printf("Opened stego image file\n");

    return e_success;
}

/* Function to open decoded output file */
Status open_decode_output_file(char *file_name, DecodeInfo *decInfo)
{
    /* Open output file in write mode */
    decInfo->fptr_output = fopen(file_name, "w");

    /* Error handling */
    if (decInfo->fptr_output == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->output_fname);

        return e_failure;
    }

    /* If user did not provide output file name */
    if (strcmp(decInfo->output_fname, "output.txt") == 0 ||
        strcmp(decInfo->output_fname, "output.c") == 0 ||
        strcmp(decInfo->output_fname, "output.sh") == 0 ||
        strcmp(decInfo->output_fname, "output.h") == 0)
    {
        printf("Output file not mentioned. Creating decoded file\n");
    }

    printf("Opened decoded file\n");

    return e_success;
}

/* Function to validate command line arguments */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    /* Check BMP file extension */
    if (argv[2][0] != '.')
    {
        char *str3 = strstr(argv[2], ".bmp");

        if (str3 != NULL && strcmp(str3, ".bmp") == 0)
        {
            decInfo->output_image_fname = argv[2];
        }
        else
        {
            printf("Decoding: ./a.out -d <.bmp file> [output file]\n");
            return e_failure;
        }
    }
    else
    {
        printf("Decoding: ./a.out -d <.bmp file> [output file]\n");
        return e_failure;
    }

    /* Default output file */
    if (argv[3] == NULL)
        decInfo->output_fname = "output";
    else
        decInfo->output_fname = argv[3];

    return e_success;
}

/* Function to skip BMP header */
Status skip_bmp_header(FILE *fptr_output_image)
{
    /* BMP header size = 54 bytes */
    fseek(fptr_output_image, 54, SEEK_SET);

    return e_success;
}

/* Function to decode one byte from image LSB */
Status decode_byte_from_lsb(char *byte_data, char *image_buffer)
{
    *byte_data = 0;

    /* Extract 1 bit from each image byte */
    for (int i = 7; i >= 0; i--)
    {
        image_buffer[7 - i] = (image_buffer[7 - i] & 1) << i;

        *byte_data = *byte_data | image_buffer[7 - i];
    }

    return e_success;
}

/* Function to decode magic string */
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    char magic[8];

    /* Decode magic string characters */
    for (int i = 0; i < strlen(magic_string); i++)
    {
        fread(magic, 8, 1, decInfo->fptr_output_image);

        decode_byte_from_lsb(new_magic + i, magic);
    }

    /* Compare decoded string with original magic string */
    if (strcasecmp(new_magic, MAGIC_STRING) == 0)
    {
        printf("Decoded Magic String Successfully\n");

        return e_success;
    }
    else
        return e_failure;
}

/* Function to decode integer from LSB */
Status decode_int_from_lsb(long *size, char *image_buffer)
{
    *size = 0;

    /* Decode 32-bit integer */
    for (int i = 31; i >= 0; i--)
    {
        image_buffer[31 - i] = (image_buffer[31 - i] & 1) << i;

        *size = *size | image_buffer[31 - i];
    }

    return e_success;
}

/* Function to decode extension size */
Status decode_output_file_extn_size(DecodeInfo *decInfo)
{
    /* Array to store encoded extension size */
    char extn[32];

    /* Read 32 bytes from image */
    fread(extn, 32, 1, decInfo->fptr_output_image);

    /* Decode extension size */
    decode_int_from_lsb(&decInfo->output_extn_size, extn);

    printf("Decoded secret file extension size Successfully\n");

    return e_success;
}

/* Function to decode secret file extension */
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    unsigned char extn1[8];
    int i;

    /* Decode extension characters */
    for (i = 0; i < decInfo->output_extn_size; i++)
    {
        fread(extn1, 8, 1, decInfo->fptr_output_image);

        decode_byte_from_lsb(&decInfo->extn_output_file[i], extn1);
    }

    /* Add null character */
    decInfo->extn_output_file[i] = '\0';

    i = 0;

    /* Copy output filename */
    while (decInfo->output_fname[i])
    {
        if (decInfo->output_fname[i] != '.')
        {
            out_name[i] = decInfo->output_fname[i];
        }
        else
            break;

        i++;
    }

    out_name[i] = '\0';

    /* Append decoded extension */
    decInfo->output_fname = strcat(out_name, decInfo->extn_output_file);

    printf("Decoded secret file extension Successfully\n");

    return e_success;
}

/* Function to decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char file[32];

    /* Read encoded size */
    fread(file, 32, 1, decInfo->fptr_output_image);

    /* Decode file size */
    decode_int_from_lsb(&decInfo->size_secret_file, file);

    printf("Decoded secret file size Successfully\n");

    return e_success;
}

/* Function to decode secret file data */
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char file1[8];

    /* Decode actual secret data */
    for (int i = 0; i < decInfo->size_secret_file; i++)
    {
        fread(file1, 8, 1, decInfo->fptr_output_image);

        decode_byte_from_lsb(&decInfo->output_data[0], file1);

        fwrite(&decInfo->output_data[0], 1, 1, decInfo->fptr_output);
    }

    printf("Decoded secret file data Successfully\n");

    return e_success;
}

/* Main decoding function */
Status do_decoding(DecodeInfo *decInfo)
{
    printf("----------- Decoding Started -----------\n");

    printf("Opening required files\n");

    /* Open stego image */
    if (open_decode_image_files(decInfo->output_image_fname, decInfo) == 0)
    {
        /* Skip BMP header */
        if (skip_bmp_header(decInfo->fptr_output_image) == 0)
        {
            /* Decode magic string */
            if (decode_magic_string(MAGIC_STRING, decInfo) == 0)
            {
                /* Decode extension size */
                if (decode_output_file_extn_size(decInfo) == 0)
                {
                    /* Decode extension */
                    if (decode_secret_file_extn(decInfo) == 0)
                    {
                        /* Open decoded output file */
                        if (open_decode_output_file(decInfo->output_fname, decInfo) == 0)
                        {
                            printf("All required files opened successfully\n");

                            /* Decode secret file size */
                            if (decode_secret_file_size(decInfo) == 0)
                            {
                                /* Decode secret file data */
                                if (decode_secret_file_data(decInfo) == 0)
                                {
                                    printf("----------- Decoding is Successfully Done -----------\n");

                                    return e_success;
                                }
                                else
                                    return e_failure;
                            }
                            else
                                return e_failure;
                        }
                        else
                        {
                            printf("Decoding unsuccessful\n");

                            return e_failure;
                        }
                    }
                    else
                        return e_failure;
                }
                else
                    return e_failure;
            }
            else
                return e_failure;
        }
        else
            return e_failure;
    }
    else
        return e_failure;
}

