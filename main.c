#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "decode.h"


OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1], "-e") == 0)
    {
      
        return e_encode;
    }  
    
    else if(strcmp(argv[1], "-d") == 0)
    {
     
        return e_decode;
    }

    else
    {
        return e_unsupported;
    }
    
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("----------------------------------------------------------\n");
        printf("        IMAGE STEGANOGRAPHY PROJECT\n");
        printf("----------------------------------------------------------\n");
        printf("Usage:\n");
        printf("  To encode a secret file into a BMP image:\n");
        printf("    %s -e <source_image.bmp> <secret_file> [output_image.bmp]\n\n", argv[0]);
        printf("  To decode a secret file from a stego image:\n");
        printf("    %s -d <stego_image.bmp> [output_file]\n\n", argv[0]);
        printf("Examples:\n");
        printf("  %s -e flower.bmp message.txt secret.bmp\n", argv[0]);
        printf("  %s -d secret.bmp output.txt\n\n", argv[0]);
        
        return 0;
    }
   // check_operation_type(&argv[1]);
    
    EncodeInfo encInfo;   // to access structure member
    DecodeInfo decInfo;
    OperationType ret = check_operation_type(argv);
    if(ret == e_encode)
    {
        if(argc >= 4)
        {
            Status res = read_and_validate_encode_args(argv, &encInfo);
            if(res == e_failure)
            {
                return 0;  // terminate the program
            }
            else
            {
                do_encoding(&encInfo);
            }
        }
    }
    else if(ret == e_decode)
    { if(argc >= 3)
        {
            Status res = read_and_validate_decode_args(argv, &decInfo);
            if(res == e_failure)
            {
                return 0;  // terminate the program
            }
            else
            {
                do_decoding(&decInfo);
            }
        }
    }
    
    else
    {
        printf("ERROR: Invalid option '%s'\n", argv[1]);
        printf("Usage:\n");
        printf("  %s -e <source_image.bmp> <secret_file> [output_image.bmp]\n", argv[0]);
        printf("  %s -d <stego_image.bmp> [output_file]\n", argv[0]);
      
        return 0;
    }

}