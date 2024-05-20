#include <stdio.h>
#include <stdlib.h>
#include "decode.h"
#include "types.h"
#include<string.h>

Status do_decoding(DecodeInfo *decInfo)
{
    if(open_decode_files(decInfo)==e_success)
    {
	printf("INFO: Open files Successfully\n");
	char *magic;
	printf("INFO: Enter magic string to Validate : ");
	scanf("%s",magic);
	if(decode_magic_string(magic,decInfo)==e_success)
	{
	    printf("INFO: Magic code decoded Successfully\n");
	    if(decode_secret_file_ext_size(0,decInfo)==e_success)
	    {
		printf("INFO: Secret file ext size found successfully\n");
		if(decode_secret_file_ext(decInfo->secret_ext_size,decInfo)==e_success)
		{
		    printf("INFO: Secret file ext found successfully\n");
		    if(decode_secret_file_size(0,decInfo)==e_success)
		    {
			printf("INFO: Secret file size found successfully\n");
			if(decode_secret_file_data(decInfo)==e_success)
			{
			    printf("INFO: secret file data found successfully\n");
			    return e_success;
			}
			else{
			    printf("secret file data not found\n");
			}
		    }
		    else{
			printf("secret file size not found\n");
		    }
		}
		else{
		    printf("INFO: secret file ext not correct\n");
		}
	    }
	    else{
		printf("INFO: secret file ext size not found\n");
	    }
	}
	else{
	    printf("INFO: Magic string is not matched. No need to decode \n");
	    return e_failure;
	}
    }
    else{
	printf("INFO: open files failed !!\n");
	return e_failure;
    }
}

Status decode_magic_string(char *magic_str,DecodeInfo *decInfo)
{

    char buffer[32];
    //take a buffer to store 32 bytes from image and read from source image
    fread(buffer,1,32,decInfo->fptr_src_image);
    int val=0;
    //get the value of the LSB's of those 32 bytes
    decode_lsb_to_size(buffer,&val,decInfo);
    int magic_string_size=val;
    //pass magic string and its length to verify the bytes of image contain these bits
    if(decode_magic_to_data(magic_str,magic_string_size,decInfo->fptr_src_image)==e_success)
    {
	//printf("Magic string Image to data decoded successfully\n");
	return e_success;
    }
    else{
	//printf("ERROR: Magic string Image to data not decoded \n");
	return e_failure;
    }
}
Status decode_magic_to_data(char *data, long size, FILE *fptr_src_image)
{
    char buffer[8];
    int val=0;
    for(int i=0;i<size;i++)
    {
	//data_val+=data[i];
	fread(buffer,1,8,fptr_src_image);
	decode_lsb_to_byte(buffer,&val);
	if(val != data[i])
	{
	    return e_failure;
	}
	val=0;
    }
    return e_success;
}

Status open_decode_files(DecodeInfo *decInfo)
{
    //open the source file in read mode
    decInfo->fptr_src_image=fopen(decInfo->src_image_fname,"r");
    if(decInfo->fptr_src_image==NULL)
    {
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->src_image_fname);
	return e_failure;
    }
    //set the file indicator next to 54th position, 
    //because upto 54 bytes it contain image data
    //but we want to decode from 55th byte 
    fseek(decInfo->fptr_src_image,54,SEEK_SET);
    return e_success;
}

Status read_and_validate_decode_args(char *argv[],DecodeInfo *decInfo)
{
    //the arg-3 should be .bmp file only, if it was .bmp, then add it to structure
    if(strstr(argv[2],".bmp") != NULL)
    {
	//printf("Arg 3 is passed\n");
	//pass the file name to structure
	decInfo->src_image_fname=argv[2];
	//check the arg-4 is present without ext
	if(argv[3]!=NULL)
	{
	    //the arg-4 must not contain ext, if present, print error
	    if(strchr(argv[3],'.')==NULL)
	    {
		//if ext not present, add thatname to structure
	    	decInfo->output_fname=argv[3];
	    	//printf("file name: %s\n",decInfo->output_fname);
	    	return e_success;
	    }
	    else
	    {
		printf("INFO: Please pass argument 4 without extension\n");
		printf("INFO: don't enter the extension for the output file\nPlease provide formal like below:\nDecoding: ./a.out -d <.bmp file> [output file]\n");
		return e_failure;
	    }
	}
	else
	{
	    //if arg-4 is not passed, ask the user to give file name without extenion
	    char *file_name;
	    printf("Enter the file name(without ext) to store the decoded data : ");
	    scanf("%s",file_name);
	    //if given file name contain extension, then print error
	    //else pass the file name to the structure
	    if(strchr(file_name,'.')==NULL){
	    	decInfo->output_fname=file_name;
	    	//printf("file name: without ext %s\n",decInfo->output_fname);
	    	return e_success;
	    }
	    else
	    {
		printf("INFO: Please pass argument 4 without extension\n");
		printf("INFO: don't enter the extension for the output file\nPlease provide formal like below:\nDecoding: ./a.out -d <.bmp file> [output file]\n");
		return e_failure;
	    }
	}
    }
    else
    {
	printf("INFO: Please pass argument 3 with .bmp file name\n");
	printf("INFO: Please provide formal like below:\nDecoding: ./a.out -d <.bmp file> [output file]\n");
	return e_failure;
    }
}

Status decode_image_to_data(char *data,long size, FILE *fptr_src_image)
{
    //take buffer and read 8 bytes from file ptr and decode those 8bytes to lsb to get 1 byte of data
    char buffer[8];
    int val=0,char_value=0,data_val=0;
    for(int i=0;i<size;i++){
	//data_val+=data[i];
	fread(buffer,1,8,fptr_src_image);
	decode_lsb_to_byte(buffer,&val);
	//1 byte data contain 8 bits so add every bit to get value
	//char_value+=val;
	//this data can be used to only extension while after return from this function
	data[i]=val;
	//printf("%c ",val); 
	val=0;
    }
    
    //printf("\nTotal value : %d  %d\n",char_value,data_val);
    return e_success;
}

Status decode_lsb_to_byte(char *buffer, int *int_ptr)
{
    //take only LSB from byte of image and shift to correct place of bit
    for(int i=0;i<8;i++)
    {
	*int_ptr=*int_ptr | (1&buffer[i])<<(7-i); 
    }
    return e_success;
}
Status decode_secret_file_ext(int secret_ext_size, DecodeInfo *decInfo)
{
    char buffer[10];
    //take one buffer to store the extension obtained after converting to data
    if(decode_image_to_data(buffer,secret_ext_size,decInfo->fptr_src_image)==e_success){
	//at last add null character to extension
	buffer[secret_ext_size]='\0';
	//printf("ext buffer: %s\n",buffer);
	//add the buffer(extension) to output file name
	strcat(decInfo->output_fname,buffer);
	//printf("str extension: %s\n",decInfo->output_fname);
        //printf("image to data of secret file extension Decoded successfully\n");
        return e_success;
    }
    else{
        //printf("image to data of secret file ext is not decoded\n");
        return e_failure;
    }
}

Status decode_lsb_to_size(char *buffer, int *size, DecodeInfo *decInfo)
{
    //size is an integer, to get it we want to process 32 bytes of image data
    //we take only LSB from each byte
    for(int i=0;i<32;i++){
        *size = *size | (1 & buffer[i]) <<(31-i);
    }
}
Status decode_secret_file_ext_size(long file_size,DecodeInfo *decInfo)
{
    char buffer[32];
    //take a buffer to store 32 bytes from image and read from source image
    fread(buffer,1,32,decInfo->fptr_src_image);
    int val=0;
    //get the value of the LSB's of those 32 bytes
    decode_lsb_to_size(buffer,&val,decInfo);
    //pass the size value to structure as secret ext size
    decInfo->secret_ext_size=val;
    //printf("size of secret file ext : %d\n",val);
    return e_success;
}

Status decode_secret_file_size(long file_size,DecodeInfo *decInfo)
{
    int val=0;
    //take a buffer of size 32 to store 32 bytes from file by reading
    char buffer_data[32];
    fread(buffer_data,1,32,decInfo->fptr_src_image);
    //then convert the LSB's of 32 bytes to value(integer)
    decode_lsb_to_size(buffer_data,&val,decInfo);
    //pass the value to the structure as secret file size
    decInfo->secret_file_size=val;
    //printf("size of secret file : %d\n",val);
    return e_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    //open the output file in write mode
    decInfo->fptr_output_file=fopen(decInfo->output_fname,"w");
    if(decInfo->fptr_output_file==NULL){
	//if file not found, print error
	perror("fopen");
	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->output_fname);
	return e_failure;
    }
    //decode the data 
    if(decode_secret_data(decInfo->secret_file_size,decInfo->fptr_src_image,decInfo->fptr_output_file)==e_success){
        //printf("image to data of secret file data decoded successfully\n");
        return e_success;
    }
    else{
        //printf("image to data of secret file data is not decoded\n");
        return e_failure;
    }
}
Status decode_secret_data(long size, FILE *fptr_src, FILE *fptr_out)
{
    char buffer[8];
    //take a buffer of size 8(bytes), because we use this for every byte
    int val=0;
	//printf("size : %ld\n",size);
    for(int i=0;i<size;i++){
	//read the 8 bytes from file, get value by converting LSB's into byte
	fread(buffer,1,8,fptr_src);
	decode_lsb_to_byte(buffer,&val);
	//write the value into fptr_output file
	fwrite(&val,1,1,fptr_out);
	//printf("%c ",val); 
        val=0;
    }
    //printf("\n");
    return e_success;
}
    
