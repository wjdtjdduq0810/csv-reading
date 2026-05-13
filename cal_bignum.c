//2026_03 초반쯤 완성된 mersenn_twister 파일 내용을 2026_03_22 즈음 옮김.  

// 수의 연산에 관한 라이브러리. 기존 연산 규칙의 확장, txt 값을 읽고 변수로 변환하거나 변수를 txt로 변환 (출력)

# define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
# include <string.h>
# include <iso646.h>
# include "connection.h"


void input_binary_byTXT_32bit(char* txt, uint32_t* ptr)
{
	uint8_t i = 0; uint8_t x; // 부호는 확인하지 않는다. 2진 입력만 있을 뿐.
	uint8_t nonvalid_cnt = 0;
	// 첫번째 스켄. 종결문자 또는 제한까지, 읽고 입력이 정상형인지 확인.
	for (; i < 200; i++)
	{
		x = txt[i]; // 종결문자 확인하면 나간다.
		if (x == 0) break; // 입력이 0 또는 1 이 아니면 나간다.
		if (x == ' ') { nonvalid_cnt++; continue; }
		if (x != '1' and x != '0') { printf("NAN!!\n"); return; }
		printf("%c", (char)x);
	}
	printf("\n");

	// 2. 32 bit 고정이므로, 천천히 입력해보자.
	uint8_t j = 0; uint32_t output = 0; uint32_t slider = 1;
	slider = slider << (i - 1 - nonvalid_cnt);
	for (; j < i; j++)
	{
		if (txt[j] == '1') output = output + slider;
		if (txt[j] == ' ') continue;
		slider = slider >> 1;
	}
	*ptr = output;
}

void print_uint32_t_binary(uint32_t target)
{// %u 형식지정자를 통해 2진수를 출력할 수 있을까?
	//printf("target = %u\n", target);
	//printf("target_in_binary, \n");
	uint32_t frame_loc = 1 << (8 * 4 - 1); // 128, 1000 0000
	for (uint32_t i = 0; i < 8 * 4; i++) {
		uint32_t j = 0;
		j = target & frame_loc;
		//printf("frame_loc = %u\n", frame_loc);
		//printf("j = %u\n", j);
		printf("%u", (uint32_t)(j / frame_loc));
		frame_loc = frame_loc >> 1;
	} printf("\n");
}

void print_float_binary(void* target_0) // 띄어쓰기 말곤 차이가 없다.
{// %u 형식지정자를 통해 2진수를 출력할 수 있을까?
	uint32_t target = *(uint32_t*)target_0; uint32_t i = 0;
	uint32_t frame_loc = 1 << (8 * 4 - 1); // 128, 1000 0000
	uint32_t j = target & frame_loc;
	printf("%u", (uint32_t)(j / frame_loc));
	frame_loc = frame_loc >> 1; i++; printf("   ");
	for (; i < 9; i++) {
		uint32_t j = target & frame_loc;
		printf("%u", (uint32_t)(j / frame_loc));
		frame_loc = frame_loc >> 1;
	} printf("   ");
	for (; i < 8 * 4; i++) {
		uint32_t j = target & frame_loc;
		printf("%u", (uint32_t)(j / frame_loc));
		frame_loc = frame_loc >> 1;
	}
	printf("\n");
}
void test_print_underdot(uint16_t lim)
{
	if (lim == 0) printf("1.0\n");
	if (lim == 1) printf("0.5\n");
	char* txt_buffer = malloc(lim + 1);
	uint8_t remain[2] = { 0, 0 };
	for (uint16_t i = 0; i < lim + 1; i++) txt_buffer[i] = 0;
	txt_buffer[0] = 5;
	for (uint16_t i = 2; i < lim + 1; i++) // i 는 최종 출력의 소수 digit 개수와 같다.
	{
		//printf("0.");
		for (uint16_t j = 0; j < i; j++) // j 는 업데이트 할 txt 버퍼의 인덱스 번호. 1개씩 업데이트 한다.
		{
			//printf("txt_buffer[%u] = %u\n", j, txt_buffer[j]);
			remain[1] = txt_buffer[j] % 2;
			//printf("remain[1] = % u\n", remain[1]);
			txt_buffer[j] = (txt_buffer[j] / 2) + remain[0] * 5;
			//printf("%u", txt_buffer[j]);
			remain[0] = remain[1];
		}
		*((uint16_t*)remain) = 0;
		//printf(" = 2^-%u", i);	printf("\n");
		//printf("remains = %u, % u\n", remain[0], remain[1]);
	}
	for (uint16_t i = 0; i < lim; i++) txt_buffer[i] = txt_buffer[i] + '0';
	printf("0."); printf("%s = 2^-%u\n", txt_buffer, lim);
	free(txt_buffer);
}


uint8_t manual_add(uint32_t A, uint32_t B, uint32_t* output_buffer)
{	*output_buffer = A + B;
	if (~A < B) return 1;
	return 0;}


void manual_mult(uint32_t A, uint32_t B, uint32_t* output_buffer) {
	uint32_t frame_loc = 1, adder = 0;
	for (uint32_t i = 0; i < 8 * 4; i++) {
		if ((uint32_t)(B & ((uint32_t)frame_loc << i)) > 0) {
			// 아래비트 연산, 올림수 저장
			adder = adder + manual_add(output_buffer[0], (uint32_t)(A << i), &output_buffer[0]);
			// 위비트 연산, 올림수 제외한 덧셈
			if (i > 0)
				output_buffer[1] = output_buffer[1] + (uint32_t)(A >> (8 * 4 - i));
		}
	} // 위비트 + 올림수 연산
	output_buffer[1] = output_buffer[1] + adder;
}

// 큰 수 표현 함수. 직관성-범용성 확보를 위해 str 을 입력으로 사용한다. (보류)
//void print_heavynum(uint32_t input_array, uint32_t len_array)

// 큰 수가 0보다 큰지 확인하는 함수. 1번째 판.
uint8_t check_existency_bigNum_0(uint32_t* big_num, uint16_t len_big_num) {
	for (uint16_t i = 0; i < len_big_num; i++)
		if (big_num[i] > 0) return 1;
	return 0;
}

// 큰 수가 0보다 큰지 확인하는 함수. 1 이상이라면 1 출력, 아니면 0
uint8_t check_existency_bigNum(big_uint input) {
	if (input.len > 2) return 1;
	if (input.num_ptr[0] > 0) return 1;
	return 0;
}

// 큰 수와 작은 수의 덧셈. 주의! 입력 포인터의 주소에 결과 출력함.
void manual_add_bigNum(uint32_t* big_num, \
	uint32_t small_num, uint16_t len_big_num, uint16_t starting_index) {
	uint8_t adder = 0;
	adder = manual_add(big_num[starting_index], small_num, &big_num[starting_index]);
	for (uint16_t i = starting_index + 1; i < len_big_num; i++) {
		if (adder == 0) break;
		adder = manual_add(big_num[i], (uint32_t)adder, &big_num[i]);
	}
}

// 큰 수의 shifting. + 방향 shifting 만 가능하게끔. 단순화를 위해. shifting 은 1 이상이여야 함.
void manual_bit_shifting_bigNum(uint32_t* big_num, uint8_t shifting, uint16_t len_big_num, uint32_t* output_array) {
	// 초기화. 시작 바이트 부분 shifting
	uint32_t return_delay_buffer[2] = { 0 , 0 };
	return_delay_buffer[0] = (uint32_t)(big_num[0] << shifting);
	for (uint16_t i = 1; i < len_big_num; i++) {
		return_delay_buffer[1] = (uint32_t)(big_num[i] << shifting) + (uint32_t)(big_num[i - 1] >> (32 - shifting));
		output_array[i - 1] = return_delay_buffer[0];
		return_delay_buffer[0] = return_delay_buffer[1];
	}
	output_array[len_big_num - 1] = return_delay_buffer[0];
}

// 큰 수와 작은 수의 곱, 초기판. 비효율적이며, 메모리 가변성 없다.
void  manual_mult_bigNum_0(uint32_t* big_num, uint32_t small_num, uint16_t len_big_num, uint32_t* output_array) {
	//1. 동적 메모리 할당. 나중에 출력할 내용 저장
	//2. 작은 수의 1값 위치 읽기, (반복). 1번째 loop
	//3. 단위 메모리 길이의 덧셈 버퍼 생성
	//4. manual_add_bigNum 를 이용해 덧셈 (반복). 2번째 loop
	//5. 목표 주소에 순차적으로 출력.
	// 변수선언
	uint32_t* buffer_array = malloc(sizeof(uint32_t) * len_big_num);
	uint32_t buffer = 0;
	uint32_t frame_loc = 1;
	// buffer_array 초기화. 작은 수의 첫번째 비트에 대해. 
	if ((uint32_t)(small_num & frame_loc) > 0) {
		for (uint16_t i = 0; i < len_big_num; i++)
			buffer_array[i] = big_num[i];
	}
	else {
		for (uint16_t i = 0; i < len_big_num; i++)
			buffer_array[i] = 0;
	}
	// 작은 수의 두번째 비트에 대해. 
	for (uint32_t j = 1; j < 32; j++) {// 1~31 번의 shifting 지시자. 초기값 1.
		frame_loc = frame_loc << 1;
		if ((uint32_t)(small_num & frame_loc) > 0) {
			buffer = big_num[0] << j;
			//printf("buffer = \n");
			//print_uint32_t_binary(buffer);
			manual_add_bigNum(buffer_array, buffer, len_big_num, 0); // big_num 의 첫번째 단위. 단위길이 shifting 없음
			// buffer_array 초기화 후 첫번째 frame loc 에 대한 루프.
			for (uint16_t i = 1; i < len_big_num; i++) {
				buffer = (big_num[i] << j) + (big_num[i - 1] >> (32 - j));
				manual_add_bigNum(buffer_array, buffer, len_big_num, i); // big_num 의 첫번째 단위. 단위길이 shifting 없음
			}
		}
	}
	//종결 전 데이터 순차 저장, memory allocation 해제
	for (uint16_t i = 0; i < len_big_num; i++)
		output_array[i] = buffer_array[i];
	free(buffer_array);
}

// 구조체 형태의 큰 수에  32bit 작은 수 덧셈.
void add_bigNum(big_uint* big_uint_ptr, uint32_t add)
{
	uint8_t adder = 0;
	uint32_t* num_ptr = big_uint_ptr->num_ptr;
	//printf("testprinting_ \n bignum[0] = %u\n", num_ptr[0]);
	adder = manual_add(num_ptr[0], add, num_ptr);
	//printf("%u\n", num_ptr[0]);
	//printf("%u\n", adder);
	//printf("--------------------------\n");

	//printf("%u\n", (uint32_t)num_ptr);
	//printf("%u\n", (uint32_t)(&num_ptr[1]));

	for (uint16_t i = 1; i < big_uint_ptr->len; i++) {
		if (adder == 0) break;
		//printf("i = %u\n", i);
		adder = manual_add(num_ptr[i], adder, (num_ptr + 4 * i));
	}
	if (adder) {
		//printf("pop!!\n");
		//printf("len = %u\n", big_uint_ptr->len);
		big_uint_ptr->len = big_uint_ptr->len + 1;
		Realloc(&big_uint_ptr->num_ptr, 4 * big_uint_ptr->len);
		big_uint_ptr->num_ptr[big_uint_ptr->len - 1] = adder;
	}
}
// 큰 수 포인터를 입력받고, 큰수에서 작은 수를 뺀 값을 큰 수 구조체에 저장하는 함수.
// 오류 검출기능은 성능 향상을 위해 주석처리. 입력 전 필터링하고 입력할 것.
void subtrac_bigNum(big_uint* big_num, uint32_t small_num, uint16_t frame_offset)
{
	uint16_t len_bignum = big_num->len;
	uint32_t* num_ptr = big_num->num_ptr;
	// 오류 검출
	//if (frame_offset > len_bignum) { printf("too big_offset\n"); return; }
	//if (frame_offset == len_bignum )
	//{	uint32_t sample = big_num->num_ptr[len_bignum - 1];
	//	if (small_num > sample) { printf("too big small_num\n"); return; }
	//}
		// 본격적인 수 계산.
	// offset 에 맞는 big num 파트를 불러와서 기본적인 뺄셈을 수행한다.
	num_ptr[frame_offset] = num_ptr[frame_offset] - small_num;
	// 내림 연산 여부 확인을 위해, 뺄 수가 더 큰지 확인해야지.
	if (num_ptr[frame_offset] >= small_num) // 만약 내림연산이 필요없는 간단한 뺄셈이라면, 연산 종료
	{
		goto END;
	}
	// 만약 내림연산이 필요하다면, 그 다음 칸부터 연속되는 0 값은 -1 언더플로우 값으로 바꾸고 1 이상이면 1 빼고 종결.
	for (uint16_t i = frame_offset + 1; i < len_bignum; i++)
	{
		if (num_ptr[i] != 0)
		{
			num_ptr[i] = num_ptr[i] - 1;	break;
		}
		num_ptr[i] = (uint32_t)(0 - 1);
	}
END: 	// 내림계산에 의해 0 값이 될 수 있는 칸수는 1개가 최대 --> loop 기능 필요 없음.
	if ((num_ptr[len_bignum - 1] == 0) and (len_bignum > 1))
	{
		len_bignum--;
		big_num->len = len_bignum;
		Realloc(&(big_num->num_ptr), len_bignum * 4);
	}
}

// 큰 수에 그보다 작은 큰 수를 빼고 결과를 큰 수 구조체에 저장하는 함수.
// 메모리 관리 기능은 하위함수가 이미 가지고 있으므로, 고려하지 않아도 됨.
void subtrac_bigNums(big_uint* big_num, big_uint* small_num)
{
	// 오류 조건. 2번째 입력이 1번째 입력보다 더 클 경우.
	// 1. 입력 길이가 더 큰 경우
	// 2. 입력 길이가 같고 가장 큰 자릿수가 더 큰 경우.
	uint16_t lim = small_num->len;
	if ((big_num->len < lim) or (big_num->len == lim and big_num->num_ptr[lim - 1] < small_num->num_ptr[lim - 1]))
	{
		printf("wrong_intput_set\n"); return;
	}
	// 반복계산. 
	for (uint16_t i = 0; i < lim; i++)
	{
		subtrac_bigNum(big_num, small_num->num_ptr[i], i);
	}
}

// 큰 수와 uint16 의 곱을 큰 수 포인터에 저장하고, 필요하면 메모리 연장하는 함수.
// 32비트 이상의 작은 수 적용은 필요할 때 추가하기로.
void mult_bigNum(big_uint* big_uint_ptr, uint16_t mult)
{	uint32_t* big_num = big_uint_ptr->num_ptr;
	uint16_t len = big_uint_ptr->len;
	uint32_t cal_ = 0;
	uint16_t adder = 0;
	for (uint16_t i = 0; i < 2 * len; i++)
	{
		cal_ = (uint32_t)(mult * ((uint16_t*)(big_num))[i]);
		cal_ = cal_ + adder;
		((uint16_t*)(big_num))[i] = (uint16_t)(cal_);
		adder = ((uint16_t*)(&cal_))[1];
	}
	if (adder) {
		Realloc(&(big_uint_ptr->num_ptr), ++(big_uint_ptr->len) * 4);
		big_uint_ptr->num_ptr[len] = adder;
	}
}

// 큰 수를 uint16 로 나눈 나머지를 출력하고, 몫을 큰 수에 저장하는 함수.
uint16_t divid_bigNum(big_uint* big_uint_ptr, uint16_t divid) 
{	uint32_t* big_num = big_uint_ptr->num_ptr;
	uint16_t len = big_uint_ptr->len;
	uint32_t cal_ = 0;
	for (int i = 2 * len - 2 ; i >= 0 ; i--)
	{	cal_ = cal_ << 16; // shifting
		cal_ = cal_ + ((uint16_t*)(big_num))[i]; // reading
		((uint16_t*)(big_num))[i] = cal_ / divid; // 몫 저장
		cal_ = cal_ % divid; // 나머지 저장
	}
	if (((uint16_t*)(big_num))[len-1] == 0)
		Realloc(&(big_uint_ptr->num_ptr), --(big_uint_ptr->len) * 4);
	return cal_;
}

//  큰 수를 uint16 로 나눈 나머지 출력만 하고 종료되는 함수.
uint16_t remain_bigNum(big_uint* big_uint_ptr, uint16_t divid)
{
	uint32_t* big_num = big_uint_ptr->num_ptr;
	uint16_t len = big_uint_ptr->len;
	uint32_t cal_ = 0;
	for (int i = 2 * len - 2; i >= 0; i--)
	{
		cal_ = cal_ << 16; // shifting
		cal_ = cal_ + ((uint16_t*)(big_num))[i]; // reading
		cal_ = cal_ % divid; // 나머지 저장
	}
	return cal_;
}

// 큰 수간 크기 비교. 1번째 값이 크면 1, 2번째 값이 크면 -1 반환. 같으면 0
char compare_bigNum(big_uint no_1 , big_uint no_2)
{	uint8_t flag = 1;
	uint16_t len = no_1.len;
	// 길이가 다르면, 바로 결정난다.
	//printf("compareing_len/  %u   ---   %u\n", no_1.len, no_2.len);
	if (len < no_2.len) return -1;
	if (len > no_2.len) return 1;
	// 길이가 같다면, 윗 자리부터 비교한다.
	for (int i = (int)(len - 1); i >= (int)0; i--)
	{
		if (no_1.num_ptr[i] < no_2.num_ptr[i]) return -1;
		if (no_1.num_ptr[i] > no_2.num_ptr[i]) return 1;
	}// 같으면 continue 겠지. 끝까지 같으면 그냥 같은거지.
	return 0;
}


// 큰 수를 10진수로 표현하는 함수. 1 번째 판.
void print_bigNum_Decimal_0(uint32_t* big_num, uint16_t len_big_num) {
	//  유효 단위 자리 찾기. 버퍼 길이 결정.
	uint16_t len_buffer = 0;
	uint32_t buffer_for_debug = 0;
	for (uint16_t i = 1; i < len_big_num + 1; i++) {
		if (big_num[len_big_num - i] > 0) {
			len_buffer = len_big_num - i + 1;
			break;
		}
	}
	if (len_buffer < 2) {		// 유효 단위 길이가 너무 짫다 --> 일반적인 printing 이 가능하다.
		printf("=>> %u\n", big_num[0]);
		return;
	}

	// 출력 문자열 길이 계산, 출력 문자열 및 문자열 번호 counter 선언.
	float log_scaling = 0.30103f;
	log_scaling = log_scaling * len_buffer * 32 + 1;  // +2 는, 올림자리 + 종단문자.
	// 출력 문자열 길이 계산, 출력 문자열 및 문자열 번호 counter 선언.
	char* txt_array = malloc(sizeof(char) * (int)log_scaling); // 1개씩 출력하므로 종단은 의미없다.
	uint16_t txt_loc = 0;  // + 문자 틀 선언
	char str_num[10] = "0123456789";
	// 입력보다 작거나 같은 길이의 버퍼 선언. 초기값, 몫을 저장하는곳.
	uint32_t* buffer_array = malloc(sizeof(uint32_t) * len_buffer);
	// 버퍼 배열에 입력값 복사
	for (uint16_t i = 0; i < len_buffer; i++) {
		buffer_array[i] = big_num[i];
	}
	// 나머지, 내림몫 버퍼 선언
	uint32_t loop_counter = 0;
	uint8_t remain; /* 10 으로 나눴을 때 나머지.*/
	uint32_t Quotient_drop;

G:	// 나누기 반복 시작점
	remain = 0, Quotient_drop = 0;
	loop_counter++;
	if (loop_counter > (int)log_scaling * 10) {
		printf("something_go_wrong. infinite loop block activated");
		return;
	}
	for (uint16_t i = 1; i < len_buffer + 1; i++) {
		//1.나머지 업데이트
		remain = buffer_array[len_buffer - i] % 10;
		//2. 현재 단위에 몫 저장, 몫이 저장된 현재 단위에 내림몫 더하기.
		buffer_array[len_buffer - i] = (buffer_array[len_buffer - i] / 10) + Quotient_drop;
		//3. 업데이트된 나머지 처리.  
		if (len_buffer == i) { /*마지막 단위의 나머지를 문자로 변환.*/
			txt_array[txt_loc] = str_num[remain];
			txt_loc++;
		}
		else {
			if (remain) { // 마지막 단위 나머지가 아니라면, 이를 내림몫으로 가공
				if (buffer_array[len_buffer - i - 1] > 36) {
					Quotient_drop = remain * 429496730;
					buffer_array[len_buffer - i - 1] = buffer_array[len_buffer - i - 1] - remain * 4;
				}
				else {
					Quotient_drop = remain * 429496729;
					buffer_array[len_buffer - i - 1] = buffer_array[len_buffer - i - 1] + remain * 6;
				}
			}
			else Quotient_drop = 0;
		} // 0 도 예외일순 없지! 이거 빼먹고 찾느라 고생했다.
	}
	//  유효 단위 자리 찾기. 최종 출력 전 검사
	uint16_t valid_test = 0;
	for (uint16_t i = 1; i < len_buffer; i++) {
		if (buffer_array[len_buffer - i] > 0) {
			valid_test = len_buffer - i + 1;
			break;
		}
	}

	if (valid_test < 2) {
		printf("=>> %u", buffer_array[0]);
		for (; txt_loc > 0; txt_loc--)
			printf("%c", txt_array[txt_loc - 1]);
		printf("\n");
	}
	else goto G;
	//종료 전 buffer 동적 할당 해제
	free(buffer_array);
	free(txt_array);
}

// 큰 수를 10진수로 표현하는 함수. 2 번째 판.
void print_bigNum_Decimal(big_uint input) {
	//  유효 단위 자리 찾기. 버퍼 길이 결정.
	uint32_t* big_num = input.num_ptr;
	uint16_t len_big_num = input.len;
	uint16_t len_buffer = 0;
	uint32_t buffer_for_debug = 0;
	for (uint16_t i = 1; i < len_big_num + 1; i++) {
		if (big_num[len_big_num - i] > 0) {
			len_buffer = len_big_num - i + 1;
			break;
		}
	}
	if (len_buffer < 2) {		// 유효 단위 길이가 너무 짫다 --> 일반적인 printing 이 가능하다.
		printf("=>> %u\n", big_num[0]);
		return;
	}

	// 출력 문자열 길이 계산, 출력 문자열 및 문자열 번호 counter 선언.
	float log_scaling = 0.30103f;
	log_scaling = log_scaling * len_buffer * 32 + 1;  // +2 는, 올림자리 + 종단문자.
	// 출력 문자열 길이 계산, 출력 문자열 및 문자열 번호 counter 선언.
	char* txt_array = malloc(sizeof(char) * (int)log_scaling); // 1개씩 출력하므로 종단은 의미없다.
	uint16_t txt_loc = 0;  // + 문자 틀 선언
	char str_num[10] = "0123456789";
	// 입력보다 작거나 같은 길이의 버퍼 선언. 초기값, 몫을 저장하는곳.
	uint32_t* buffer_array = malloc(sizeof(uint32_t) * len_buffer);
	// 버퍼 배열에 입력값 복사
	for (uint16_t i = 0; i < len_buffer; i++) {
		buffer_array[i] = big_num[i];
	}
	// 나머지, 내림몫 버퍼 선언
	uint32_t loop_counter = 0;
	uint8_t remain; /* 10 으로 나눴을 때 나머지.*/
	uint32_t Quotient_drop;

G:	// 나누기 반복 시작점
	remain = 0, Quotient_drop = 0;
	loop_counter++;
	if (loop_counter > (int)log_scaling * 10) {
		printf("something_go_wrong. infinite loop block activated");
		return;
	}
	for (uint16_t i = 1; i < len_buffer + 1; i++) {
		//1.나머지 업데이트
		remain = buffer_array[len_buffer - i] % 10;
		//2. 현재 단위에 몫 저장, 몫이 저장된 현재 단위에 내림몫 더하기.
		buffer_array[len_buffer - i] = (buffer_array[len_buffer - i] / 10) + Quotient_drop;
		//3. 업데이트된 나머지 처리.  
		if (len_buffer == i) { /*마지막 단위의 나머지를 문자로 변환.*/
			txt_array[txt_loc] = str_num[remain];
			txt_loc++;
		}
		else {
			if (remain) { // 마지막 단위 나머지가 아니라면, 이를 내림몫으로 가공
				if (buffer_array[len_buffer - i - 1] > 36) {
					Quotient_drop = remain * 429496730;
					buffer_array[len_buffer - i - 1] = buffer_array[len_buffer - i - 1] - remain * 4;
				}
				else {
					Quotient_drop = remain * 429496729;
					buffer_array[len_buffer - i - 1] = buffer_array[len_buffer - i - 1] + remain * 6;
				}
			}
			else Quotient_drop = 0;
		} // 0 도 예외일순 없지! 이거 빼먹고 찾느라 고생했다.
	}
	//  유효 단위 자리 찾기. 최종 출력 전 검사
	uint16_t valid_test = 0;
	for (uint16_t i = 1; i < len_buffer; i++) {
		if (buffer_array[len_buffer - i] > 0) {
			valid_test = len_buffer - i + 1;
			break;
		}
	}

	if (valid_test < 2) {
		printf("=>> %u", buffer_array[0]);
		for (; txt_loc > 0; txt_loc--)
			printf("%c", txt_array[txt_loc - 1]);
		printf("\n");
	}
	else goto G;
	//종료 전 buffer 동적 할당 해제
	free(buffer_array);
	free(txt_array);
}

void mercenn_twister(uint32_t seed_num, uint32_t output_len_order, uint32_t* output_ptr) {
	//상수 선언	
	uint8_t w = 32, r = 31, u = 11, s = 7, t = 15, l = 18, tac = 0;
	uint16_t n = 624, m = 397, loc_ = 0;
	uint32_t a = 2567483615, b = 2636928640, c = 4022730752, cal_ = 0; /*계산용 버퍼, cal_*/
	uint32_t lower = 2147483647, upper = 2147483648, d = 4294967295; // 01(31) , 10(31) , 1(32)
	uint32_t output_counter = 0;

	// seed growing part
	uint32_t key_array[2][624];
	uint32_t* pin_array = key_array[0], * twist_array = key_array[1], * ptr_buffer; // 초기화
	for (uint16_t i = 1; i < 52; i++) seed_num = seed_num * 69069 + 1;
	for (uint16_t i = 0; i < 624; i++) {  // 위의 확인용 출력과 내용이 같다. 맞는 indexing.
		seed_num = seed_num * 69069 + 1; pin_array[i] = seed_num;
	}

TWISTING:  // twisting part
	for (uint16_t i = 0; i < 624; i++) {
		//cal_ = (pin_array[i] & upper) + ((pin_array[(i + 1) % n]) & d);
		cal_ = (pin_array[i] & upper) + ((pin_array[(i + 1) % n]) & lower);
		tac = cal_ % 2; /* */ cal_ = cal_ >> 1;
		if (tac) cal_ = cal_ ^ a;
		twist_array[i] = pin_array[(i + m) % n] ^ cal_;
	}

TEMPERING: //tempering part.
	cal_ = twist_array[loc_]; //step_1
	cal_ = cal_ ^ ((cal_ >> u) & d); //step_2
	cal_ = cal_ ^ ((cal_ << s) & b); //step_3
	cal_ = cal_ ^ ((cal_ << t) & c); //step_4
	cal_ = cal_ ^ (cal_ >> l); //step_5

	output_ptr[loc_++] = cal_;

	// 반복제어
	output_counter++;
	if (output_counter < output_len_order) {
		if (loc_ < 624) goto TEMPERING;
		else {
			loc_ = 0;
			ptr_buffer = pin_array;
			pin_array = twist_array;
			twist_array = ptr_buffer;
			printf("ptr_buffer = %p\n", ptr_buffer);
			printf("pin_array = %p\n", pin_array);
			printf("twist_array = %p\n", twist_array);
			goto TWISTING;
		}
	}
}

//
//void main()
//{
//	// 참고영상 : https://www.youtube.com/watch?v=TF4PLUcJO5w&t=1526s
//
//	// 영상의 내용을 따라하는, 메르센 트위스터 가함수 시작.
//
//	uint32_t mercenn_buffer[624];
//	mercenn_twister(1958, 624, mercenn_buffer);
//
//	for (uint16_t i = 0; i < 624; i++)
//		printf("%u\n", mercenn_buffer[i]);
//
//}




