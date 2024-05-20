#include <stdio.h>
#include <stdlib.h>
#include "encode.h"
#include "types.h"
#include<string.h>
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
    rewind(fptr_image);

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
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    //if argument-3 i.e, source image is .bmp ext, then only we will encode
    char *bmp_ext;
    char *out_bmp;
    bmp_ext=strchr(argv[2],'.');
    if(bmp_ext != NULL && strcmp(bmp_ext,".bmp") == 0)
    {
	//printf("Arg 3 is passed\n");
	//add arg-2 to structure as src_image_fname
	encInfo->src_image_fname=argv[2];
	char *ext;
	//if arg-4 contain '.' character, then strore that extension into struct as extn_secret_file
	ext=strchr(argv[3],'.');
	if(ext != NULL)
	{
	    //printf("Arg 4 is passed\n");
	    strcpy(encInfo->extn_secret_file,ext);
	    //printf("ext = %s\n",ext);
	    //store the arg-4 into struct as secret_fname
	    encInfo->secret_fname=argv[3];
	    //if arg-5 is given in cmd, then check it is.bmp or not
	    if(argv[4]!=NULL)
	    {
		//printf("Arg 5 is passed\n");
		out_bmp=strchr(argv[4],'.');
		if(out_bmp != NULL && strcmp(out_bmp,".bmp") == 0)
		{
		    encInfo->stego_image_fname=argv[4];
		    return e_success;
		}
		else
		{
		    printf("ERROR: Please pass argument 5 with .bmp file\n"); 
		    printf("INFO: Please provide format like below :\nEncoding: ./a.out  -e  <.bmp file> <.txt or .sh or .c file> [output file]\n");
		    return e_failure;
		}
	    }
	    else
	    {
		//if arg-5 not given in cmd, then give output filename default
		printf("INFO: Output file not mentioned. Creating steged_img.bmp as default\n");
		encInfo->stego_image_fname="steged_img.bmp";
		return e_success;
	    }
	}
	else
	{
	    printf("ERROR: Please pass argument 4 with .txt or .c or .sh file\n"); 
	    printf("INFO: Please provide format like below:\nEncoding: ./a.out -e <.bmp file> <.txt file> [output file]\n");
	    return e_failure;
	}
    }
    else
    {
	printf("ERROR: Please pass argument 3 with .bmp file\n"); 
	printf("INFO: Please provide format like below:\nEncoding: ./a.out -e <.bmp file> <.txt file> [output file]\n");
	return e_failure;
    }
}
Status do_encoding(EncodeInfo *encInfo)
{
    if(open_files(encInfo)==e_success){
	printf("INFO: Open files Successful\n");
	if(check_capacity(encInfo)==e_success){
	    printf("INFO: Check capacity Successful\n");
	    if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success){
		printf("INFO: BMP Header Copied successfully\n");
		if(encode_magic_string(encInfo->magic_str,encInfo)==e_success){
		    printf("INFO: Magic String Encoded Successfully\n");
		    if(encode_secret_file_ext_size(strlen(encInfo->extn_secret_file),encInfo)==e_success){
			printf("INFO: Secret file extension size encoded Successfully\n");
			if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)==e_success){
			    printf("INFO: Secret file Extension encoded successfully\n");
			    if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_success){
				printf("INFO: Secret file size Encoded Successfully\n");
				if(encode_secret_file_data(encInfo)==e_success){
				    printf("INFO: Secret file data Encoded Successfully\n");
				    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success){
					printf("INFO: Remaining data copied successfully\n");
					return e_success;
				    }
				    else{
					printf("ERROR: Remaining data not copied\n");
				    }
				}
				else{
				    printf("ERROR: Secret file data not encoded\n");
				}
			    }
			    else{
				printf("ERROR: Secret file size is not encoded\n");
				return e_failure;
			    }
			}
			else{
			    printf("ERROR: Secret extension file not encoded\n");
			    return e_failure;
			}
		    }
		    else{
			printf("ERROR: Secret file extension size not encoded\n");
			return e_failure;
		    }
		}
		else{
		    printf("ERROR: Magic string Not encoded\n");
		    return e_failure;
		}
	    }
	    else{
		printf("ERROR: Header not copied\n");
		return e_failure;
	    }

	}
    }
    else{
	printf("No such file name to open\n");
	return e_failure;
    }

}
Status check_capacity(EncodeInfo *encInfo)
{
    //get file size of source image. After getting size, set file pointer to beginning
    encInfo->image_capacity=get_file_size(encInfo->fptr_src_image);
    rewind(encInfo->fptr_src_image);
    //printf("size of bmp file(image capacity : %d\n",encInfo->image_capacity);
    printf("      Enter magic string : ");
    scanf("%s",encInfo->magic_str);
    //get file size of secret file. After getting size, set file pointer to beginning
    encInfo->size_secret_file=get_file_size(encInfo->fptr_secret)-1;
    rewind(encInfo->fptr_secret);
    //printf("size of secret file data : %ld\n",encInfo->size_secret_file);
    //if the encoded bits size must be lesser than image capacity(total bytes of source image)
    //then only we will encode, else we can't !!
    int len=(strlen(encInfo->magic_str)+sizeof(int)+strlen(encInfo->extn_secret_file)+sizeof(int)+encInfo->size_secret_file)*8;
    if(len<encInfo->image_capacity){
	return e_success;
    }
    else{
	printf("ERROR: capacity is less. i.e, source image size is lesser than secret file size\n");
	return e_failure;
    }
}
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    //read beginning 54 bytes of source image because it contain details related to image
    //write it as it is in output image
    char buffer[54];
    fread(buffer,54,1,fptr_src_image);
    fwrite(buffer,54,1,fptr_dest_image);
    return e_success;
}

Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    //encode magic string size
    encode_magic_string_size(strlen(magic_string),encInfo);
    //encode the magic string data
    if(encode_data_to_image(magic_string,strlen(magic_string),encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success){
	//printf("INFO: data to image Encoded successfully\n");
	return e_success;
    }
    else{
	printf("ERROR: Data to image not Encoded\n");
	return e_failure;
    }
}
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    //take a buffer of size 8 because in data a character hold 8 bits
    char image_buffer[8];
    //printf("size of %s = %d\n", data,size);
    //take every character of data and read the bytes of source image
    for(int i=0;i<size;i++){
	fread(image_buffer,1,8,fptr_src_image);
	//encode that bits of data to LSB of buffer bytes
	if(encode_byte_to_lsb(data[i],image_buffer)==e_success){
	    fwrite(image_buffer,1,8,fptr_stego_image);
	}
    }
    return e_success;
}
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    //here loop run 8 times because take every bit from character(data) =>1 byte
    for(int i=0;i<8;i++){
	//printf("byte = %hhx\n", image_buffer[i]);
	image_buffer[i]=(image_buffer[i]&0xfe) | (unsigned)(data&(1<<(7-i)))>>(7-i);
    }
    return e_success;
}
Status encode_magic_string_size(int magic_size, EncodeInfo *encInfo)
{
    char buffer[32];
    //create a buffer of size 32 to store 32 bits of a integer number
    fread(buffer,1,32,encInfo->fptr_src_image);
    //encode size (integer) 32 bits to LSB's of 32 Bytes
    if(encode_size_to_lsb(buffer,magic_size,encInfo)==e_success){
	//fwrite the buffer into output image file
	fwrite(buffer,1,32,encInfo->fptr_stego_image);
	//printf("magic_size size: %d\n",magic_size);
	return e_success;
    }
    return e_failure;

}
Status encode_secret_file_ext_size(long file_size, EncodeInfo *encInfo)
{
    char buffer[32];
    //create a buffer of size 32 to store 32 bits of a integer number
    fread(buffer,1,32,encInfo->fptr_src_image);
    //encode size (integer) 32 bits to LSB's of 32 Bytes
    if(encode_size_to_lsb(buffer,file_size,encInfo)==e_success){
	//fwrite the buffer into output image file
	fwrite(buffer,1,32,encInfo->fptr_stego_image);
	//printf("encode_secret_file_ext_siz secretfile extension size: %ld\n",file_size);
	//printf("secret file data to image encoded successfully\n");
	return e_success;
    }
    printf("Secret file data to image is not encoded \n");
    return e_failure;
}

Status encode_size_to_lsb(char *buffer,long ext_size, EncodeInfo *encInfo)
{
    //here loop run 32 times because size of integer is 32 bits
    for(int i=0;i<32;i++){
	buffer[i]=buffer[i]&0xfe | (unsigned)(ext_size&(1<<(31-i)))>>(31-i);
    }
    return e_success;
}
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    //encode extention data (ex: ".txt") all bits of bytes to LSB's of source image
    if(encode_data_to_image(file_extn,strlen(file_extn),encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success){
	//printf("data to image of secret file extension Encoded successfully\n");
	return e_success;
    }
    else{
	printf("data to image of secret file extension not Encoded\n");
	return e_failure;
    }
}
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    //we take buff size 32 because, we encode an integer (32 bits)
    char buffer[32];
    //printf("secret file size: %ld\n",file_size);
    //read 32 bits from source image into buffer
    fread(buffer,1,32,encInfo->fptr_src_image);
    if(encode_size_to_lsb(buffer,file_size,encInfo)==e_success){
	//fwrite the buffer into output image
	fwrite(buffer,1,32,encInfo->fptr_stego_image);
	//printf("encode_secret_file_size successfully\n");
	return e_success;
    }
    printf("encode_secret_file_size not encoded \n");
    return e_failure;
}
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    //take a buffer of size of secret file data size and read those no of bytes into buffer
    char buff_data_secret[encInfo->size_secret_file+1];
    fread(buff_data_secret,1,encInfo->size_secret_file,encInfo->fptr_secret);
    buff_data_secret[encInfo->size_secret_file]='\0';
    strcpy(encInfo->secret_data,buff_data_secret);
    //printf("data --------> : %s\n",encInfo->secret_data);
    //encode the data to output image LSB's
    if(encode_data_to_image(buff_data_secret,strlen(buff_data_secret),encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success){
	//printf("data to image of secret file data Encoded successfully\n");
	return e_success;
    }
    else{
	printf("data to image of secret file data not Encoded\n");
	return e_failure;
    }
}
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    //here take any size buffer because we dont know how much remaining
    char buffer[10];
    int size;
    //until fptr_src is false, it will read from src F and write into dest F of size it read.
    while(!feof(fptr_src)){
	size=fread(buffer,1,sizeof(buffer),fptr_src);
	fwrite(buffer,1,size,fptr_dest);
    }
    return e_success;
}
uint get_file_size(FILE *fptr)
{
    //set the file pointer to last of file, to get file size
    fseek(fptr,0,SEEK_END);
    return ftell(fptr);
}

