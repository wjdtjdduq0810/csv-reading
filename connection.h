#pragma once
# include <stdio.h>
# include <stdint.h>
# include <string.h>
# include <stdlib.h>
# include <iso646.h>

// 구조체-----------------------------------

// 큰 정수
typedef struct _big_uint
{
	uint32_t* num_ptr;
	uint16_t len;
}big_uint;

typedef struct _alloc_arr {
	uint16_t len;  // 종결문자 대신 길이 저장. 
	void* arr_ptr; // 동적할당 포인터 변수 저장. 관련내용 : 이미 사용된 코드번호(정수)
	uint8_t unit_size;
} alloc_arr;

typedef struct _alloc_encoder { // 라벨타입 엔코더에 사용할 고정길이 문자열 structure.
	uint16_t len;  // 종결문자 대신 길이 저장. 전체 길이로 한다.  
	char* arr_ptr; // 동적할당 포인터 변수 저장. 
	uint8_t block_size; // 고정길이 단위 크기
} alloc_encoder;

//2차원 이상의 배열.
typedef struct _alloc_mtrx {
	uint16_t* shape_ptr; // 동적 할당 배열의 포인터. 첫째 항에는 차원수, 그 다음은 각 차원 길이. ex) 2,2,3. 2차원, 가로 2 세로 3
	void* mtrx_ptr; // 목표로 하는 행렬과 같은 크기의 백터. 어떻게 읽을지는 shape_ptr 지정 메모리에 저장되있음.
	uint8_t unit_size; // 데이터타입 지정
} alloc_mtrx;


// cal_bignum-------------------------------------

void input_binary_byTXT_32bit(char* txt, uint32_t* ptr);
void print_uint32_t_binary(uint32_t target);
void print_float_binary(void* target_0);
void test_print_underdot(uint16_t lim);

uint8_t manual_add(uint32_t A, uint32_t B, uint32_t* output_buffer);
void manual_mult(uint32_t A, uint32_t B, uint32_t* output_buffer);

// 큰 수 표현 함수. 직관성-범용성 확보를 위해 str 을 입력으로 사용한다. (보류)
//void print_heavynum(uint32_t input_array, uint32_t len_array)

// 큰 수가 0보다 큰지 확인하는 함수. 1번째 판.
uint8_t check_existency_bigNum_0(uint32_t* big_num, uint16_t len_big_num);

// 큰 수가 0보다 큰지 확인하는 함수. 1 이상이라면 1 출력, 아니면 0
uint8_t check_existency_bigNum(big_uint input);

// 큰 수와 작은 수의 덧셈. 주의! 입력 포인터의 주소에 결과 출력함.
void manual_add_bigNum(uint32_t* big_num, \
	uint32_t small_num, uint16_t len_big_num, uint16_t starting_index);

// 큰 수의 shifting. + 방향 shifting 만 가능하게끔. 단순화를 위해. shifting 은 1 이상이여야 함.
void manual_bit_shifting_bigNum(uint32_t* big_num, uint8_t shifting, uint16_t len_big_num, uint32_t* output_array);

// 큰 수와 작은 수의 곱, 초기판. 비효율적이며, 메모리 가변성 없다.
void  manual_mult_bigNum_0(uint32_t* big_num, uint32_t small_num, uint16_t len_big_num, uint32_t* output_array);

// cal_bignum, ------------ 큰 수 구조체 사용 함수 -------------------

// 구조체 형태의 큰 수에  32bit 작은 수 덧셈.
void add_bigNum(big_uint* big_uint_ptr, uint32_t add);

// 큰 수에 그보다 작은 큰 수를 빼고 결과를 큰 수 구조체에 저장하는 함수.
void subtrac_bigNum(big_uint* big_num, uint32_t small_num, uint16_t frame_offset);

// 큰 수에 그보다 작은 큰 수를 빼고 결과를 큰 수 구조체에 저장하는 함수.
void subtrac_bigNums(big_uint* big_num, big_uint* small_num);

// 큰 수와 uint16 의 곱을 큰 수 포인터에 저장하고, 필요하면 메모리 연장하는 함수.
// 32비트 이상의 작은 수 적용은 필요할 때 추가하기로.
void mult_bigNum(big_uint* big_uint_ptr, uint16_t mult);

// 큰 수를 uint16 로 나눈 나머지를 출력하고, 몫을 큰 수에 저장하는 함수.
uint16_t divid_bigNum(big_uint* big_uint_ptr, uint16_t divid);

//  큰 수를 uint16 로 나눈 나머지 출력만 하고 종료되는 함수.
uint16_t remain_bigNum(big_uint* big_uint_ptr, uint16_t divid);

// 큰 수간 크기 비교. 1번째 값이 크면 1, 2번째 값이 크면 -1 반환. 같으면 0
char compare_bigNum(big_uint no_1, big_uint no_2);

// 큰 수를 10진수로 표현하는 함수. 첫번째 판.
void print_bigNum_Decimal_0(uint32_t* big_num, uint16_t len_big_num);

// 큰 수를 10진수로 표현하는 함수. 2 번째 판.
void print_bigNum_Decimal(big_uint input);

// 메르센 트위스터
void mercenn_twister(uint32_t seed_num, uint32_t output_len_order, uint32_t* output_ptr);

// float encoder 의 하위함수. 개별적 사용시 주의 필요.
uint32_t underdot_bitencoder(char* txt, uint16_t remain_digit, uint16_t len_to_go);

// 표기규칙대로 나열된 문자열을 읽고 float 변수화 하는 converter. 
float txt_to_float_converter(char* txt);


// csv_reader -------------------------------------

uint32_t file_len_detector(char* input_str);

// 두 문자열이 서로 같은지, 다른지 확인하는 함수. 길이 입력을 받자.
uint8_t txt_sameness_check(char* input_1, char* input_2, uint16_t len);

//file test printing 예제를 함수로 표현. 코딩도장 https://dojang.io/mod/page/view.php?id=619
void test_file_printing(char* input_str);

// quotation 처리해가며, 한 줄의 칸 수를 세는 함수.
uint16_t csv_comma_counter(FILE* filePointer);

// 부울 결과 출력. 반복확인을 위해.
uint8_t txt_singleNum_reader(char single_char, uint8_t* nature_num);

// 마지막 번호 읽는 함수. 동적할당 문자열 변수를 받을 것이므로, 개수는 내부에서 추정한다. 
void last_codeNum_reader(char* input_txt_array);

//realloc 함수와 같이가는, null 대응 코드 결합.
void Realloc(void** double_ptr, uint16_t len);

void arr_initializer(alloc_arr** ptr, uint8_t unit_size);

// 1번째 버전. "차원수, 1번값, 2번값,,,," // 구조체 포인터 주소를 넣으면 malloc 해주는
void mtrx_initializer_0(alloc_mtrx** ptr, uint8_t unit_size, char* shape_order);

void mtrx_free(alloc_mtrx** ptr);

void arr_free(alloc_arr** ptr);

//원본 alloc_mtrx 의 크기-모양을 바꾸는 함수.
void mtrx_reshape_0(alloc_mtrx* ptr, uint16_t* shape_order);

//배열형 자료의 길이 상대 수정. 
void arr_realloc_relative(alloc_arr* ptr, int add_len);

//가변 메모리 txt 버퍼에 한글자 더 추가하는 함수.
void alloc_txt_stitcher(char single_txt, alloc_arr* alloc_txt_ptr);


// testing_ (empty_source) --------------------------------------------

float txt_to_float_converter(char* txt);

