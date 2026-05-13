
// csv 파일 읽기 관련 라이브러리. 데이터를 파싱하고, 정제. 큰 버퍼나 파일을 만들어 내용 저장.
// 다른 라이브러리 함수에 입력할 수 있도록 큰 데이터셋을 처리하는게 목적.

# define _CRT_SECURE_NO_WARNINGS
# include <stdio.h>
# include <stdint.h>
# include <string.h>
# include <stdlib.h>
# include <iso646.h>
# include "connection.h"


uint32_t file_len_detector(char* input_str) {
	uint32_t file_size;
	FILE* filePointer = fopen(input_str, "r");
	// 파일 포인터 이동 명령. SEEK_END 로부터 0 만큼 띄워라.
	fseek(filePointer, 0, SEEK_END);
	//포인터가 가리키는 지점은, 파일 원점으로부터 얼마나 떨어저 있나?
	file_size = (uint32_t)ftell(filePointer);
	fclose(filePointer); 	return file_size;
}

// 두 문자열이 서로 같은지, 다른지 확인하는 함수. 길이 입력을 받자.
uint8_t txt_sameness_check(char* input_1, char* input_2, uint16_t len) {
	for (uint8_t i = 0; i < len; i++) {
		if (input_1[i] != input_2[i]) { return 0; }
		else { if (input_1[i] == input_2[i] == '\0'); return 1; }
	}
	return 2; // 종결어 나오기 전에 끝나는 경우. len 이 짧거나, 종결어가 없거나.
}

// 줄 공백을 제거할수 있어야지. 문서 전체를 스켄해서, 빈 공백을 없애는게 우선이겠다.
// 주석구간을 제거하는 알고리즘. 50% 이상의 데이터를 가지고 있는지? 조건을 만족하는 첫 줄 후보.
// 첫 줄이라면, 그 아래 데이터셋에는 같은 문자열이 없을 것이다.

// 첫 유효 줄의 번호를 출력하는 함수 uint16_t 길이. 그 이상이면, 나눠서.

// 첫 유효 줄이 목록명인지 확인하는 함수 . bool 출력. 만약 그렇다면, 명칭별로 데이터 파싱. 


//file test printing 예제를 함수롤 표현. 코딩도장 https://dojang.io/mod/page/view.php?id=619
void test_file_printing(char* input_str) {
	char buffer = 0;	uint32_t count = 0; // 파일 포인터 생성, 업데이트
	FILE* filePointer = fopen(input_str, "r");
	while (feof(filePointer) == 0) { // feof() : end of file 이면 1, 아니면 0 출력.
		count = count + fread(&buffer, 1, 1, filePointer); // 1 바이트씩 1개 읽기. 포인터 움직임도 포함되어있다.
		printf("(%u)%c ", buffer, buffer);
	} // fread() 는 읽은 메모리 길이만큼 출력.
	printf("\ntotal: %u\n", count); // 읽은 전체 길이 출력
	fclose(filePointer);
}

// quotation 처리해가며, 한 줄의 칸 수를 세는 함수.
uint16_t csv_comma_counter(FILE* filePointer) {// 읽기 모드로 열린 FILE pointer 받음
	uint8_t single_line_flag = 1; uint16_t comma_counter = 0; uint32_t total_len = 0; char buffer = 0;
	fseek(filePointer, 0, SEEK_SET);
	while (feof(filePointer) == 0) { // 파일의 끝까지 읽는 조건.
	CONTINUE:
		total_len = total_len + fread(&buffer, sizeof(char), 1, filePointer); // 한글자씩 끊어서 읽기. buffer 에 저장.
		if (buffer == '"')
		{
			for (uint16_t i = 0; i < 1000; i++)
			{
				total_len = total_len + fread(&buffer, sizeof(char), 1, filePointer);
				if (feof(filePointer)) goto BREAK; // feof : 파일 끝에 도달하면 1 이상값 출력
				if (buffer == '"') { goto CONTINUE; }
			}
		}
		if (buffer == (char)44)  comma_counter++;
		if (buffer == (char)10) { single_line_flag = 0; break; }
	} // 한줄짜리일 수도 있잖아?
BREAK:
	if (single_line_flag) { printf("error : single_line_file\n"); return 0; }
	return comma_counter + 1;
}

uint8_t txt_singleNum_reader(char single_char, uint8_t* nature_num) { // 부울 결과 출력. 반복확인을 위해.
	char num_key[10] = "0123456789"; uint8_t single_number = 255;
	for (uint8_t cnt = 0; cnt < 10; cnt++)
		if (num_key[cnt] == single_char) { single_number = cnt; break; }
	if (single_number > 9) { return 0; } //
	else *nature_num = single_number; return 1;
}

// 마지막 번호 읽는 함수. 동적할당 문자열 변수를 받을 것이므로, 개수는 내부에서 추정한다. 
void last_codeNum_reader(char* input_txt_array) { // missing mark code 번호 생성시 사용.
	uint8_t reset_lock = 1; // 읽은 single_char 가 숫자라면, 저장해야하므로, 1. 그 외엔 0.
	uint16_t codeNum = 0; 	uint8_t frame_loc = 0, buffer = 0, len_input = 0;
	for (uint8_t i = 0; i < 200; i++) {
		if (input_txt_array[i] == 0) { len_input = i; break; }
	}
	for (; frame_loc < len_input; frame_loc++) {
		reset_lock = txt_singleNum_reader(input_txt_array[frame_loc], &buffer);
		if (reset_lock) codeNum = codeNum * 10 + buffer;
		else codeNum = 0;
	}
	printf("\n%u\n", codeNum); // 별도 출력 변경이 필요하면, 바꿀것.
}

//realloc 함수와 같이가는, null 대응 코드 결합.
void Realloc(void** double_ptr, uint16_t len) { // 업데이트할 포인터, 새 할당 길이(비트단위)
	void* backup_ptr;
	//printf("input_ptr = %p\n", *double_ptr);
	backup_ptr = realloc(*double_ptr, len);
	//printf("realloced_ptr = %p\n", backup_ptr);
	if (backup_ptr == NULL)
	{
		printf("realloc_failed\n");
		return;
	}
	*double_ptr = backup_ptr;
}

/*	void* backup_ptr = *double_ptr; uint32_t backup_len = _msize(backup_ptr);
	*double_ptr = realloc(*double_ptr, len);
	if (*double_ptr == NULL) {
		*double_ptr = malloc(len); uint32_t i = 0;
		if (*double_ptr == NULL) { printf("reallocation_failed\n"); return; }
		if (backup_len >= len) {
			for (; i < len; i++) // 길이 감소
			{
				((uint8_t*)(*double_ptr))[i] = ((uint8_t*)backup_ptr)[i];
			}
		}
		else { // 길이 증가
			for (; i < backup_len; i++) ((uint8_t*)(*double_ptr))[i] = ((uint8_t*)backup_ptr)[i];
			for (; i < len; i++) ((uint8_t*)(*double_ptr))[i] = 0;
		}
		free(backup_ptr);
	}*/


void arr_initializer(alloc_arr** ptr, uint8_t unit_size) {
	(*ptr) = malloc(sizeof(alloc_arr));
	if ((*ptr) == NULL) printf("memory_allocation_failed\n");
	else {
		(*ptr)->len = 1; /*\\//*/ (*ptr)->unit_size = unit_size;
		(*ptr)->arr_ptr = malloc(unit_size * 1);
	}
}

// 1번째 버전. "차원수, 1번값, 2번값,,,," // 구조체 포인터 주소를 넣으면 malloc 해주는
void mtrx_initializer_0(alloc_mtrx** ptr, uint8_t unit_size, char* shape_order) { // shape order 를 문자열로 받는다
	//printf(shape_order); printf("\n");// 문자열로 받는 이유 : 길이, 요소 개수 자유롭게 바꿀 수 있기 때문.
	uint16_t i = 0; uint32_t buffer = 0; uint16_t* shape_arr;
	uint8_t num_counter = 0;
	// shape order 에서 첫번째 유효숫자, n 을 읽는다. 그만큼 malloc 할 예정.
	for (; i < 50000; i++) {
		printf("%c\n", shape_order[i]);
		if ((47 < shape_order[i]) and (shape_order[i] < 58)) {
			buffer = buffer * 10 + shape_order[i] - 48;
			printf("buffer = %u\n", buffer);
		}
		else if (shape_order[i] == ',') { i++; break; }
		if (shape_order[i] == 0) break;
	}// 이제 n 개만큼 할당. 다음 나오는 유효숫자들을 저장한다.
	if (buffer == 0) { printf("no valid numbers\n"); return; }
	shape_arr = malloc(2 * (buffer + 1)); printf("memory_size = %d\n", (int)((shape_arr == NULL) ? 0 : _msize(shape_arr)));
	if (shape_arr == NULL) { /*printf("malloc error");*/  return; }
	shape_arr[0] = buffer; printf("shape_arr[0] = %u\n", buffer); buffer = 0;
	for (; i < 50000; i++) {
		printf("shape_order[%u] = %c", i, shape_order[i]);
		if ((47 < shape_order[i]) and (shape_order[i] < 58)) {
			printf(" => %u", shape_order[i] - 48); buffer = buffer * 10 + shape_order[i] - 48;
		}
		else if ((shape_order[i] == 0) or (num_counter == shape_arr[0])) { printf("break at //%u\n", num_counter);  break; }
		else if (shape_order[i] == ',') {
			if (buffer != 0) { num_counter++; (shape_arr[num_counter]) = (uint16_t)buffer;	buffer = 0; }
			printf("shape_arr[%u] = %u\n", num_counter, (shape_arr[num_counter]));
		}	printf("\n");
	}
	if (buffer != 0) shape_arr[++num_counter] = buffer;
	printf("_shape_arr[%u] = %u\n", num_counter, shape_arr[num_counter]);
	if (num_counter < shape_arr[0]) { printf("valid_number_low_error\n"); }
	if (num_counter > shape_arr[0]) { printf("valid_number_high_error\n"); }
	printf("endofloop,, shape_arr[%u] = %u\n", num_counter, shape_arr[num_counter]);
	for (uint8_t i = 0; i < num_counter + 1; i++) { if (shape_arr != NULL) { printf("shape_arr[%u] = %u\n", i, shape_arr[i]); } }
	(*ptr) = malloc(sizeof(alloc_mtrx)); if ((*ptr) != NULL) { (*ptr)->unit_size = unit_size; }
	(*ptr)->shape_ptr = shape_arr; buffer = 1;  printf("buffer = %u\n", buffer);
	for (uint8_t i = 1; i < num_counter + 1; i++) { buffer = buffer * shape_arr[i]; }
	printf("after update,  buffer = %u\n", buffer);
	(*ptr)->mtrx_ptr = malloc(unit_size * buffer);
}

void mtrx_free(alloc_mtrx** ptr) {
	free((*ptr)->shape_ptr); free((*ptr)->mtrx_ptr); free(*ptr);
}

void arr_free(alloc_arr** ptr) { free((*ptr)->arr_ptr); free(*ptr); }

//원본 alloc_mtrx 의 크기-모양을 바꾸는 함수.
void mtrx_reshape_0(alloc_mtrx* ptr, uint16_t* shape_order) {
	// 원본 및 목표 shape 를 읽는다. 전채 용량을 추정한다.
	void* backup_ptr = ptr->mtrx_ptr;	uint32_t input_len = 1, output_len = 1;
	for (uint16_t i = 1; i < ptr->shape_ptr[0] + 1; i++) { input_len = input_len * (ptr->shape_ptr[i]); }
	for (uint16_t i = 1; i < shape_order[0] + 1; i++) { output_len = output_len * shape_order[i]; }
	free(ptr->shape_ptr); ptr->shape_ptr = malloc(2 * (shape_order[0] + 1));// shape_ptr 지정 메모리값 수정
	for (uint16_t i = 0; i < shape_order[0] + 1; i++) { ptr->shape_ptr[i] = shape_order[i]; }
	uint8_t unit_size = ptr->unit_size; void* mtrx_ptr_backup = ptr->mtrx_ptr;
	ptr->mtrx_ptr = realloc(ptr->mtrx_ptr, unit_size * output_len); //mtrx_ptr 에 realloc 시도.
	if (ptr->mtrx_ptr == NULL) {
		void* ptr_ = ptr->mtrx_ptr = malloc(unit_size * output_len); uint32_t i = 0;
		if (output_len > input_len) {
			for (; i < unit_size * input_len; i++) { ((uint8_t*)(ptr_))[i] = ((uint8_t*)(mtrx_ptr_backup))[i]; }
			for (; i < unit_size * output_len; i++) { ((uint8_t*)(ptr_))[i] = 0; }
		}
		else for (; i < unit_size * output_len; i++) { ((uint8_t*)(ptr_))[i] = ((uint8_t*)(mtrx_ptr_backup))[i]; }
		free(mtrx_ptr_backup);
	}
}

//배열형 자료의 길이 상대 수정. 
void arr_realloc_relative(alloc_arr* ptr, int add_len) { // 초기, 이전 길이 백업 및 새 길이 계산 
	int len[2] = { 0,0 }; uint8_t unit_size = (ptr)->unit_size; len[0] = ptr->len;
	len[1] = len[0] + add_len; printf("%u = arr_realloc_test_len[0]\n", len[0]);
	if (len[1] < 2) { printf("invalid_inputn\n"); return; } ptr->len = len[1];
	// realloc 시도. null 시를 대비해 backup.	
	void* pointer_backup = ptr->arr_ptr;	ptr->arr_ptr = realloc(ptr->arr_ptr, len[1] * (ptr->unit_size));
	if (ptr->arr_ptr == NULL) {// 만약 realloc 이 실패해서 malloc 을 해야한다면, 
		void* ptr_ = ptr->arr_ptr = malloc(len[1] * unit_size); uint16_t i = 0;
		if (len[1] > len[0]) {
			for (; i < unit_size * len[0]; i++) { ((uint8_t*)(ptr_))[i] = ((uint8_t*)(pointer_backup))[i]; }
			for (; i < unit_size * len[1]; i++) { ((uint8_t*)(ptr_))[i] = 0; }
		}
		else for (; i < unit_size * len[1]; i++) { ((uint8_t*)(ptr_))[i] = ((uint8_t*)(pointer_backup))[i]; }
		free(pointer_backup);
	}
}

//가변 메모리 txt 버퍼에 한글자 더 추가하는 함수.
void alloc_txt_stitcher(char single_txt, alloc_arr* alloc_txt_ptr) {
	arr_realloc_relative(alloc_txt_ptr, 1); uint16_t len = (alloc_txt_ptr)->len;
	((char*)(alloc_txt_ptr)->arr_ptr)[len - 1] = single_txt;
}


//// 버퍼를 반복적으로 재정의하더라도 버퍼 수명 관리 해야해.
//int main() {
//	// 구간 0. 가함수 입출력.
//	char* input_file_address = "C:\\Users\\wjdtj\\OneDrive\\Desktop\\test_00.csv";
//	void* output_ptr;// 가함수 출력
//
//	// 범구간 변수 선언. 함수 끝까지 살아갈 변수들. 이 부분 외에 선언된 변수들의 수명 제한을 확인할것.
//	FILE* filePointer = fopen(input_file_address, "r");  // 파일 주소 입력.
//	uint16_t numbers_of_dimension = csv_comma_counter(filePointer);
//	alloc_encoder* encoder_ptr; uint8_t* data_type_array;
//	uint16_t line_ctr = 0;
//
//	printf("numbers_of_dimension = %u\n", numbers_of_dimension);
//
//	{
//		fseek(filePointer, 0, SEEK_SET); // test_printing 전에 pointer 위치 초기화.// test_printing
//		uint16_t total_len = 0; char buffer;
//		for (uint32_t i = 0; i < 1000000000; i++)
//		{
//			if (feof(filePointer) != 0) break;
//			total_len = total_len + fread(&buffer, sizeof(char), 1, filePointer);
//			printf("%c", buffer);
//		}
//	}
//
//	// 불필요한 txt encoder structure 만들지 않기 위해, data_type 확인을 먼저 해야한다.
//	// data type array 선언, 0값으로 초기화. ##  0 -> 실수형 자료,   1 -> 라벨형 자료.
//	data_type_array = malloc(numbers_of_dimension);
//	for (uint16_t i = 0; i < numbers_of_dimension; i++)  data_type_array[i] = 0;
//
//	// data type 확인 stack. 
//	{
//		fseek(filePointer, 0, SEEK_SET); // test_printing 전에 pointer 위치 초기화.
//		uint32_t total_len = 0; char buffer; uint16_t position = 0;
//		for (uint32_t i = 0; i < 1000000000; i++) // 무한루프를 막기 위해 사용할 뿐, while 과 같은 용도. 약 1기가바이트 길이까지의 csv 를 받을 수 있다. 
//		{ // SKIP 인지 CHECK 인지 position 에 따라 결정해줘야 한다.
//			if (data_type_array[position] == 0) goto CHECK_LOOP;
//			//skip 작업.
//		SKIP_LOOP:
//			for (; i < 1000000000; i++)
//			{	// 포인터가 끝인가? 그렇다면, 스켄을 끝내라.
//				if (feof(filePointer) != 0) { i = 1000000000; goto SCAN_END; }
//				// 아니라면, 원래 계획대로 글자를 한개 읽는다.
//				total_len = total_len + fread(&buffer, sizeof(char), 1, filePointer);
//				// 글자가 컴마-엔터면, position shifting, break. 아니라면, 다시 반복.
//				if ((buffer == ',') or (buffer == 10)) goto SCAN_END;
//			}
//			// check 작업
//		CHECK_LOOP:
//			for (; i < 1000000000; i++)
//			{	// 포인터가 끝인가? 그렇다면, 스켄을 끝내라.
//				if (feof(filePointer) != 0) { i = 1000000000; goto SCAN_END; }
//				// 아니라면, 원래 계획대로 글자를 한개 읽는다.
//				total_len = total_len + fread(&buffer, sizeof(char), 1, filePointer);
//				// 글자가 컴마-엔터면, position shifting, break. 아니라면,
//				if ((buffer == ',') or (buffer == 10)) goto SCAN_END;
//				// 숫자-온점인지 확인. 만약 그렇다면 continue, 아니라면 data_type 수정 후 skip 작업.
//				if ((buffer >= '0') and (buffer <= '9')) continue;
//				if (buffer == '.') continue;
//				data_type_array[position] = 1; goto SKIP_LOOP;
//			}
//		SCAN_END: // 종결. break 또는 position shifting.
//			position = (++line_ctr) % numbers_of_dimension;
//			printf("position = %u\ni = %u\n", position, i);
//		}
//		if (line_ctr % numbers_of_dimension) buffer = 1;
//		else buffer = 0;
//		line_ctr = (line_ctr / numbers_of_dimension) + buffer;
//	}
//
//	// 결과 출력해보자.
//	printf("data_type_printing\n");
//	for (uint16_t i = 0; i < numbers_of_dimension; i++) { printf("%u\n", data_type_array[i]); }
//
//
//	printf("total_lines = %u\n", line_ctr);
//
//
//	free(data_type_array);
//
//	return 0;
//}
//
//
