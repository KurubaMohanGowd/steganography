#include <stdio.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include<string.h>
int main(int argc,char *argv[])
{
    EncodeInfo encInfo;
    DecodeInfo decInfo;
    uint img_size;
    //check no of command arguments(4 or 5) present or not
    if(argc >= 2)
    {
        int oper_type;
        //take value from function, which is enum value(encode(0) or decode(1) or unsupport(2))
        oper_type=check_operation_type(argv);
        if(oper_type==e_encode)
        {
            if(argc ==4 || argc ==5){
                if(read_and_validate_encode_args(argv,&encInfo)==e_success){
                    printf("INFO: Encode Validation Successful\n");
                    if(do_encoding(&encInfo)==e_success){
			printf("\n************  ENCODING DONE SUCCESSFULLY  *************\n\n");
		    }
		    else{
			printf("INFO: Encoding Failed\n");
		    }
                }
                else{
                    printf("INFO: validation of args failed\n");
                    //printf("Please provide format like below:\nEncoding: ./a.out -e <.bmp file> <.txt file> [output file]\n");
                }
            }
            else{
		printf("INFO: Please provide 4 or 5 arguments for encoding\n");
                printf("Please provide format like below:\nEncoding: ./a.out -e <.bmp file> <.txt file> [output file]\n");
            }
        }
        else if(oper_type==e_decode)
        {
            if(argc==3 || argc == 4){
                if(read_and_validate_decode_args(argv,&decInfo)==e_success){
                    printf("INFO: Decode Validation Successful\n");
                    if(do_decoding(&decInfo)==e_success){
			printf("\n************ Decoding Done Successfully ***********\n\n");
			return 0;
		    }
		    else{
			printf("INFO: Decoding Failed\n");
			return 0;
		    }

                }
                else{
                    printf("INFO: validation of args failed\n");
                    //printf("Please provide format like below:\nDecoding: ./a.out -d <.bmp file> [output file]\n");
                }
            }
            else{
		printf("INFO: Please provide 3 or 4 arguments for decoding\n");
                printf("Please provide format like below:\nDecoding: ./a.out -d <.bmp file> [output file]\n");
            }
        }
        else
        {
            printf("ERROR: Invalid option. \nFormat for  \n\tEncoding: ./a.out -e <.bmp file> <.txt file> [output file]\n\tDecoding: ./a.out -d <.bmp file> [output file]\n");
        }
    }
    else
    {
       printf("INFO: Argument count is very less\n");
       printf("Please provide format like below:\nEncoding: ./a.out -e <.bmp file> <.txt file> [output file]\nDecoding: ./a.out -d <.bmp file> [output file]\n");
    }


    return 0;
}

OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1],"-e")==0)
    {
        printf("INFO: Encode selected\n");
        return e_encode;
    }
    else if(strcmp(argv[1],"-d")==0)
    {
        printf("INFO: Decode selected\n");
        return e_decode;
    }
    else
    {
        printf("ERROR: Unsupported code its neither Encode nor Decode \n");
        return e_unsupported;
    }
}
        
