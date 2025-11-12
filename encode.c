#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}


//read and validate
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
  // validate source file
    if(argv[2][0] != '.')
    {
        if(strstr(argv[2], ".bmp"))  // check if source file has .bmp ext
        {
          encInfo->src_image_fname = argv[2];
        }
        else
        {
            return e_failure;
        }
    }
    else
    {
        return e_failure;
    }

    //validate secret file
     if(argv[3][0] != '.')   //atleast 1 char should be there before .
    {
        if (strstr(argv[3], ".txt") != NULL || strstr(argv[3], ".c")   != NULL || strstr(argv[3], ".h")   != NULL ||
         strstr(argv[3], ".sh")  != NULL) // check for secret file ext
        {
           encInfo->secret_fname = argv[3];
        }
        else
        {
            return e_failure;
        }
    }
    else
    {
        return e_failure;
    }

    //if argv[4] has NULL store default file
    if(argv[4] == NULL)
    {
        encInfo-> stego_image_fname = "default.bmp";
    }
    else
    {
            if(argv[4][0] != '.')
            {
                if(strstr(argv[4], ".bmp"))  // check if source file has .bmp ext
                {
                encInfo->stego_image_fname = argv[4];
                }
                else
                {
                    return e_failure;
                }
            }
            else
            {
                return e_failure;
            }
    }

    return e_success;
}


    //to check whether source file .bmp has capacity to store data or not
Status check_capacity(EncodeInfo *encInfo)
{
   
    //encode magic string-> 16 bits  2 byte = 2*8 bits  (#*)
    int magic_string_size = strlen(MAGIC_STRING);     //len -> 2
    int magic_string_size_bits = 32;   //fixed
    int magic_string_bits = magic_string_size * 8;    //each char is 1 byte so convert it into bits

    //extn_size -> 32 bits  (4 byte * 8) , used to store how long the extension is
      int extn_size_bits = 32;    // always fixed
                                                                                 //ext size alwways 4 bytes , ext-> it depends on extension size 4, 3 or 2
    //store extn -> (2 * 8) for ext size 2  ,  (3 * 8) for ext size 3
    char *extn = strrchr(encInfo->secret_fname, '.');   // find file extension
    if (extn == NULL)
    {
        return e_failure;
    }
    int extn_len = strlen(extn);   // to get extn size depends on extn for .txt it is 4
    int extn_bits = extn_len * 8;

    //secret file_size -> How many bytes of secret data should needs to be extracted     
    int file_size_bits = 32; // fixed

    // secret _dara -> (24 * 8) 
    // Encode secret data -> (secret_file_size * 8)
    fseek(encInfo->fptr_secret, 0, SEEK_END);         //take offset to end of file
    encInfo->size_secret_file = ftell(encInfo->fptr_secret);  //since offset is at last it returns size(bytes)
    rewind(encInfo->fptr_secret);               //take offset again at start for encoding
    long secret_data_bits = encInfo->size_secret_file * 8;

    //add all bits 
    long total_bits_needed = magic_string_bits + magic_string_size_bits + extn_size_bits + extn_bits + file_size_bits + secret_data_bits + 54;

     //total image capacity (width * height * 3 bytes[rgb])
    fseek(encInfo->fptr_src_image, 0, SEEK_SET);    
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);  //to get complete image size  //23,3490 bytes

     // Compare total bits required with image capacity 
    if (encInfo->image_capacity >= total_bits_needed)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }

}

 //copy bmp header 54 bytes
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char buffer[54];
    fseek(fptr_src_image, 0, SEEK_SET);
    fread(buffer, 1, 54, fptr_src_image);
    fwrite(buffer, 1, 54, fptr_stego_image);   //output file

    if(ftell(fptr_src_image) == ftell(fptr_stego_image))
    {
         return e_success;
    }
    else
    {
        return e_failure;
    }
}

//byte to lsb
Status encode_byte_to_lsb(char data, char *buffer)    //data-> magic string
{
     for(int i = 7; i >= 0; i--)              //we ll not return e_fail bcz loop stops upto 8 
     {
        if((data >> i) & 1)
        {
            buffer[7-i] = buffer[7-i] | 1;
        } 
        else
        {
            buffer[7-i] = buffer[7-i] & ~1;
        }
     }
   return e_success;  
}

//encode magic string
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    int magic_len = strlen(MAGIC_STRING);
    char buffer[8];   //get 8 byte of data from source file
    for(int i = 0; i < magic_len; i++)
    {  
        fread(buffer, 8, 1, encInfo->fptr_src_image); //  // Read 8 bytes from the source image 
        encode_byte_to_lsb(MAGIC_STRING[i], buffer);   
        fwrite(buffer, 8, 1, encInfo->fptr_stego_image);  //write 8 bytes into the stego image
    }
    return e_success;
}

//encode screte file extn for decoding we need to know How many characters need to be encoded and decoded
Status encode_secret_file_extn_size(int extn_size, EncodeInfo *encInfo)
{
    char buffer[32]; 
    fread(buffer, 32, 1, encInfo->fptr_src_image); //  Read 32 bytes from the source image 
    encode_int_to_lsb(extn_size, buffer);      //encode
    fwrite(buffer, 32, 1, encInfo->fptr_stego_image);  //write 32 bytes into the stego image
    
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    char buffer[8];
    for (int i = 0; i < strlen(file_extn); i++)
    {
        fread(buffer, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(file_extn[i], buffer);   // Encode one byte (character) of file extension into 8 bytes of image data
        fwrite(buffer, 8, 1, encInfo->fptr_stego_image);
    }

    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    char buffer[32];
    fread(buffer, 32, 1, encInfo->fptr_src_image);
    encode_int_to_lsb(file_size, buffer);
    fwrite(buffer, 32, 1, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    rewind(encInfo->fptr_secret);
    char buffer[8];
    for (long i = 0; i < encInfo->size_secret_file; i++)
    {
        fread(encInfo->secret_data, 1, 1, encInfo->fptr_secret);
        fread(buffer, 8, 1, encInfo->fptr_src_image);
        encode_byte_to_lsb(encInfo->secret_data[0], buffer);
        fwrite(buffer, 8, 1, encInfo->fptr_stego_image);
    }
    return e_success;
}

Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
   
    char buffer[1024];
    size_t bytes;

    while ((bytes = fread(buffer, 1, sizeof(buffer), fptr_src)) > 0)
    {
        fwrite(buffer, 1, bytes, fptr_dest);
    }
    return e_success;
}


Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo) == e_failure)  //file pointer so dont mention & it is base address
    {
        return e_failure;
    }
        
    if(check_capacity(encInfo) == e_failure)// validate if file has capcity to store the data OR NOT
    {
      return e_failure;   
    }

    // Copy BMP header 
    if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }

    if(encode_magic_string(MAGIC_STRING, encInfo) == e_failure)
    {
        return e_failure;
    }
 
    int extn_size = strlen(strchr(encInfo->secret_fname, '.'));
    if(encode_secret_file_extn_size(extn_size, encInfo) ==  e_failure )
    {
        return e_failure;
    }

    strcpy(encInfo->extn_secret_file,strchr(encInfo->secret_fname, '.'));
    if(encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_failure)
    {
        return e_failure;
    }

    if(encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_failure)
    {
        return e_failure;
    }

    if(encode_secret_file_data(encInfo) == e_failure)
    {
        return e_failure;
    }

    if(copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        return e_failure;
    }

     return e_success;
}



Status encode_int_to_lsb(int size, char *buffer)
{
     for(int i = 31; i >= 0; i--)              
     {
        if((size >> i) & 1)
        {
            buffer[31-i] = buffer[31-i] | 1;
        } 
        else
        {
            buffer[31-i] = buffer[31-i] &~ 1;
        }
     }
   return e_success; 
}