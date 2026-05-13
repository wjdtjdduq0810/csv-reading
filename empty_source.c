# define _CRT_SECURE_NO_WARNINGS
# include <stdio.h>
# include <stdint.h>
# include <string.h>
# include <stdlib.h>
# include <iso646.h>
# include "connection.h"



// float 문자 입력 규칙에 맞는지 아닌지 판단하여, 3개 유형으로 정리.
// 1. 정수 2. 실수 3.문자열
// 

// 그 전에, floatconverter 검사


// 표기규칙대로 나열된 문자열을 읽고 float 변수화 하는 converter. 
float txt_to_float_converter(char* txt)
{ // 띄어쓰기에 의한 반복을 줄이려면, 복사해.
	uint16_t len_digit = 0; uint16_t len_input = 0;	char* valid_digit;
	int degree_ = 0;	char sign_of_degree = 1;	uint32_t output = 0;
	// 유효 숫자만 개수 새기.
	for (uint16_t i = 0; i < 10000; i++)
	{
		len_input++;
		if (txt[i] >= '0' and txt[i] <= '9') { len_digit++; continue; }
		if (txt[i] == 0) { break; }
	}
	if (len_digit++ == 0) return  *(float*)(&output);
	// 샌 개수만큼 메모리 할당.
	valid_digit = malloc(len_digit);
	valid_digit[len_digit - 1] = 0;
	uint16_t partition[2] = { 0, 0 };
	uint8_t E_key = 0; uint8_t dot_key = 0;
	// 본격적인 파싱. 
	{ // 띄어쓰기 넘기고 첫 글자 찾기. 부호인지 판단.
		uint16_t i = 0; uint16_t slider = 0;
		for (; i < len_input; i++)
		{
			if (txt[i] == '+') { i++; break; }
			if (txt[i] == '-') { output = output | (1 << 31); i++; break; }
			if (txt[i] == ' ') continue;
			break;
		}
		// front digit
		for (; i < len_input; i++)
		{
			if (txt[i] >= '0' and txt[i] <= '9') { valid_digit[slider++] = txt[i]; continue; }
			if (txt[i] == '.') { partition[0] = slider; i++; dot_key = 2; break; }
			if (txt[i] == ' ') continue;
			if (txt[i] == 'e' or txt[i] == 'E') { partition[1] = slider; i++; E_key = 3; goto SCAN_END; }
		}

		//	// backside digit
		for (; i < len_input; i++)
		{
			if (txt[i] >= '0' and txt[i] <= '9') { valid_digit[slider++] = txt[i]; continue; }
			if (txt[i] == 'e' or txt[i] == 'E') { partition[1] = slider; i++; E_key = 3; break; }
			if (txt[i] == ' ') continue;
			break;
		}
	SCAN_END:

		// e 차수 부호 읽기.
		for (; i < len_input; i++)
		{
			if (txt[i] == ' ') continue;
			if (txt[i] == '+') { i++; break; }
			if (txt[i] == '-') { sign_of_degree = -1; i++; break; }
			break;
		}

		// e 차수 읽기.
		for (; i < len_input; i++)
		{
			if (txt[i] == ' ') continue;
			if (txt[i] >= '0' and txt[i] <= '9') { valid_digit[slider++] = txt[i]; continue; }
			break;
		}
	}

	// 유효 숫자가 전부 0 인지 확인.
	uint16_t check = 0;
	{
		uint16_t lim = 0;
		if (partition[1] == 0) lim = len_digit;
		else { lim = partition[1]; }
		for (uint16_t i = 0; i < lim; i++)
		{
			if (valid_digit[i] == '0') { check++; continue; }
			break;
		}
	}
	if (check == len_digit) { free(valid_digit); return *(float*)(&output); }

	// e 차수값 encoding.
	if (E_key) { for (uint16_t i = partition[1]; i < len_digit - 1; i++) { degree_ = degree_ * 10 + (valid_digit[i] - '0'); } }
	else { partition[1] = len_digit - 1; }
	if (dot_key == 0) { partition[0] = partition[1]; }
	degree_ = sign_of_degree * degree_ + partition[0] - check;
	//printf("degree = %d\n", degree_);
	if (degree_ > 38) // - 값은 고려할 필요 없다. 
	{
		output = output | (255 << 23); free(valid_digit); return *(float*)(&output);
	}

	// 비교작업에 사용할 큰 수 선언, 초기화
	big_uint top_num; uint32_t significand = 0;
	top_num.len = 1; top_num.num_ptr = malloc(4);
	top_num.num_ptr[0] = 0;

	// degree_ 부호값에 따라 정수부분이 살았는지 죽었는지 결정난다. 또, len_digit 보다
	// degree_ 값이 더 클 때 계산 방향이 다르다. 나눠서 할 필요가 있다. 
	// 정수 - 소수부분을 끊어읽는 이유 :
	// 10 을 거듭제곱하며 모든 digit 들을 읽는 대신, 구간을 나눠 개별 연산하는게 불필요한 메모리 점유 - 연산을 줄일 수 있음.
	// 특히 소수 부분은 1글자씩 끊어읽을 수 있음 - > 조기종료도 훨씬 편해짐.

	// 정수부분이 살아있는 경우, degree_ >= 0 조건.
	if (degree_ > 0)
	{
		uint16_t int_len = 0;
		uint16_t underdot_len = 0;	// 둘 다 남아있으면 
		uint16_t remain_degree_ = 0; // 관련 연산을 한다.

		switch((partition[1] - check) >= degree_)
		{	case 1: // 정수부분 개수 : degree_ 만큼, 나머진 소수 부분.
				int_len = degree_; underdot_len = partition[1] - check - degree_; break;
			case 0: // 정수부분 개수 : 원래 길이 + a
				int_len = partition[1] - check; remain_degree_ = degree_ - int_len; break;
		}
		for (uint16_t i = check; i < check + int_len ; i++)
		{
			mult_bigNum(&top_num, 10);
			add_bigNum(&top_num, valid_digit[i] - '0');
		}
		for (uint16_t i = 0 ; i < remain_degree_; i++){ mult_bigNum(&top_num, 10); }		

	
		// 정수부분 구하고, 소수부분 digit 수 세고
	
		// 무한대 여부 확인. 무한대가 아니라면, 읽은 값보다 살짝 작은 2 제곱수를 찾기.
		{
			if (top_num.len < 4) goto PASS;
			if (top_num.len == 4 and top_num.num_ptr[3] < 4294967040) goto PASS;
			if (top_num.len == 4 and top_num.num_ptr[3] == 4294967040)
			{
				for (uint8_t inf_check = 0; inf_check < 3; inf_check++) {
					if (top_num.num_ptr[inf_check] > 0) { output = output | (255 << 23); goto END; }
				}  // 아래 3블록의 모든 값이 0 임을 확인한 후
				goto PASS;
			}  // 지저분해 보이지만, 대부분 값이 pass 일 거라는 생각에..
			output = output | (255 << 23); goto END;
		PASS:;
		}

		// 찾은 제곱수를 기반으로 float 지수 값 결정, output 버퍼에 비트단위 입력.
		// 이전 알고리즘은 일일히 제곱수와 비교 했지만, 정수부분은 그러지 말자고.
		// 자리가 있으니 그걸 읽으면 되겠다.
		{
			uint32_t screen = 1; uint8_t i = 0; // 가수부분에 채울수 있는 비트 수를 의미. (맨 앞 생략된 1 비트 제외)
			// 1번째 1값의 위치를 찾아야지.
			for (; i < 32; i++)
			{
				screen = screen << 1;
				if (top_num.num_ptr[top_num.len - 1] < screen) { break; }
			}
			output = output | ((127 + i + 32 * (top_num.len - 1)) << 23); // return *(float*)(&output);
			//screen = screen >> 2;
			char remain = 24 - i; // 기존 목표 23 비트 + 반올림 고려한 1 비트 추가.
			significand = top_num.num_ptr[top_num.len - 1] - (screen >> 1);
			if (remain <= 0)  // 정수 블록 1개만 읽어 출력을 끝낼 수 있는 조건. 반올림을 고려한다.
			{
				significand = significand >> (-1 * remain);
				if (significand & 1) significand = (significand >> 1) + 1;
				else significand = significand >> 1;
				goto END;//output = output | significand;  //return *(float*)(&output);
			}
			// remain > 0, 반올림을 고려했을 때 다음 정수 블록 또는 소수점 밑 TXT 를 읽어야 하는 상황.
			if (top_num.len > 1)
			{
				significand = significand << remain;
				significand = significand + (top_num.num_ptr[top_num.len - 2] >> (32 - remain));
				if (significand & 1) significand = (significand >> 1) + 1;  //-- 예의주시하라. 중복.
				else significand = significand >> 1;
				goto END;//output = output | significand;  //return *(float*)(&output);
			}
			// remain 값만큼 소수부분을 더 읽어야 하는 상황.
			// 만약 소수점이 없었다면, (정수만)
			if (underdot_len == 0 ) {
				significand = significand << (remain -1);  // -1 연산은 반올림을 고려한 1비트 자리만큼 보정하는 역할
				goto END;//output = output | significand;  //return *(float*)(&output);
			}
			// 소수점도 있다면,
			free(top_num.num_ptr);
			top_num.len = 1; top_num.num_ptr = malloc(4);
			top_num.num_ptr[0] = 0;
			big_uint comparator; comparator.num_ptr = malloc(4);
			comparator.len = 1; comparator.num_ptr[0] = 1;

			uint8_t k = 0;
			uint8_t l = 0;

			if (remain > underdot_len) l = k + underdot_len; // 여기선 상대좌표 활용. 
			else l = k + remain; // 체계적이지 않다... 

			//printf("remain = %d\n", remain);
			//printf("underdot_len == %u\n", underdot_len);
			//for (uint8_t t = check + degree_; t < partition[1]; t++) printf("valid_digit[%u] = %c\n", t, valid_digit[t]);

			//printf("l = %u\n", l);

			for (;k  < l;k++)
			{	// digit 처리, 크기비교, bit update, shifting
				mult_bigNum(&comparator, 5);
				mult_bigNum(&top_num, 10);
				add_bigNum(&top_num, valid_digit[k+check+degree_] - '0');
				//print_bigNum_Decimal(comparator);
				//print_bigNum_Decimal(top_num);
				
				significand = significand << 1;
				if (compare_bigNum(top_num, comparator) >= 0)
				{	subtrac_bigNums(&top_num, &comparator); 	significand++; }	
				//print_float_binary(&significand);
			}

			for (;k < remain; k++)
			{	// digit 없이 2배수만, 크기비교, bit update, shifting
				mult_bigNum(&top_num, 2);
				//print_bigNum_Decimal(comparator);
				//print_bigNum_Decimal(top_num);

				significand = significand << 1;
				if (compare_bigNum(top_num, comparator) >= 0)
				{	subtrac_bigNums(&top_num, &comparator); 	significand++;}
				//print_float_binary(&significand);
			}

			

			
			if (significand & 1) significand = (significand >> 1) + 1;
			else significand = significand >> 1;
			goto END;//output = output | significand;  //return *(float*)(&output);
		}
		

	}
	// 소수만 있다면,
	degree_ = -1 * (degree_ + 1);
	big_uint comparator; comparator.num_ptr = malloc(4);
	comparator.len = 1; comparator.num_ptr[0] = 1;
	uint16_t i = check; uint16_t exp = 0;  uint8_t remain_position = 24; // 반올림 고려
	for (uint16_t j = 0; j < degree_; j++)
	{	// 소수점 아래 0 의 개수만큼, comparator 에 5 를 곱한다.
		mult_bigNum(&comparator, 5);
		exp++; // comparator 에 포함되지 않은 *2값을 저장.		
	}
	for (; i < (partition[1]); )
	{  // comparator 와 비교하기 시작. 지수 결정.
		mult_bigNum(&comparator, 5);
		mult_bigNum(&top_num, 10);
		add_bigNum(&top_num, valid_digit[i] - '0');
		exp++; i++;

		if (compare_bigNum(top_num, comparator) >= (char)0)  // - 값이 있는 변수 비교, 혹시나 해서..
		{
			subtrac_bigNums(&top_num, &comparator);
			goto SECTION;
		}
	}
	for (; exp < 129; )
	{
		mult_bigNum(&top_num, 2);
		exp++;
		if (compare_bigNum(top_num, comparator) >= 0)
		{
			subtrac_bigNums(&top_num, &comparator);
			goto SECTION;
		}
	}	goto END;

SECTION:
	output = output | ((127 - exp) << 23);
	for (; (i < partition[1]) and remain_position > 0; )
	{
		significand << 1;
		mult_bigNum(&comparator, 5);
		mult_bigNum(&top_num, 10);
		add_bigNum(&top_num, valid_digit[i] - '0');
		if (compare_bigNum(top_num, comparator) >= 0)
		{
			subtrac_bigNums(&top_num, &comparator);
			significand++;
		}
		remain_position--; i++;
	}

	for (; remain_position > 0;)
	{
		significand = significand << 1;
		mult_bigNum(&top_num, 2);
		if (compare_bigNum(top_num, comparator) >= 0)
		{
			subtrac_bigNums(&top_num, &comparator);
			significand++;
		}	remain_position--;
	}

	if (significand & 1) { significand = (significand >> 1) + 1; }
	else { significand = significand >> 1; }
	free(comparator.num_ptr);
END:
	free(top_num.num_ptr);
	free(valid_digit);
	output = output | significand;
	return *(float*)(&output);
}



