# define _CRT_SECURE_NO_WARNINGS
# include <stdio.h>
# include <stdint.h>
# include <string.h>
# include <stdlib.h>
# include <iso646.h>
# include "connection.h"





int main()
{
	// float 문자 입력 규칙에 맞는지 아닌지 판단하여, 3개 유형으로 정리.
	// 1. 정수 2. 실수 3.문자열
	// 
	float buffer = 0;
	float comparator = 15.31f;
	//buffer = txt_to_float_converter("11.305e-2");
	printf("00001531.0e-2\n");
	buffer = txt_to_float_converter("00001531.0e-2");
	

	//buffer = txt_to_float_converter("e3");

	print_float_binary(&buffer);
	print_float_binary(&comparator);


	
	printf("%f\n", buffer);
	printf("%f\n", comparator);


}

