/*
Project Title: Image Steganography using LSB technique

Project Overview:
This project hides a secret file inside a BMP image using the Least Significant Bit (LSB) technique. 
The hidden data is not visible to the human eye and can be extracted later using a decoder.

Working:
- Takes a BMP image and a secret file as input
- Checks image capacity
- Stores a magic string, file extension, and file size
- Hides secret data inside image pixels using LSB method
- Generates a stego image with hidden data

Encoding:
Hides secret data into image pixels.

Decoding:
Extracts hidden data from stego image and recovers original file.

Name   : Pragati Subhash Todakar.
Course : ECEP

*/


#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

/* Check operation type */
OperationType check_operation_type(char *argv[])
{
    if (strcmp(argv[1], "-e") == 0)
        return e_encode;
    if (strcmp(argv[1], "-d") == 0)
        return e_decode;
    else
        return e_unsupported;
}

int main(int argc,char* argv[])
{
    if (argc>=2)
    {
        EncodeInfo encodeInfo;
        DecodeInfo decodeInfo;
        int ret=check_operation_type(argv);
        if (ret == 0)
        {
            if(argc>=4)
            {
                if (read_and_validate_encode_args(argv,&encodeInfo) == 0)
                {
                    if (do_encoding(&encodeInfo) == 0)
                    {
                            return e_success;
                    }
                    else
                        return e_failure;
                }
            }
            else
            {
                printf("INFO: Encoding: ./a.out -e <.bmp file> <txt file>[output file]\n");
                return e_failure;
            }
        }
        else if (ret == 1)
        {
            if(argc>=3)
            {
                if (read_and_validate_decode_args(argv,&decodeInfo) == 0)
                {
                    
                    if (do_decoding(&decodeInfo) == 0)
                    {

                        return e_success;
                    }
                    else
                        return e_failure;
                }
            }
            else
            {
               printf("INFO: Decoding: ./a.out -d <.bmp file>[output file]\n");
                return e_failure;
            }
    
        }
        else
        {
            printf("ERROR! Invalid Operation Type \n");
        }
    }
    else
        printf("INFO: Encoding: -e <.bmp file> ‹txt file> [output file]\nINFO: Decoding: -d <.bmp file>[output file]\n");
    
}
