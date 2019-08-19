//나무위키 목차크기 조정코드 ver2 by yclee126 2018/10/15
//https://github.com/yclee126/NamuGardener
//
//  뒤 <- 커서 -> 앞
//
//<예제>
//목차 제목 : 가나다
//목차 형식 : == 가나다 ==
//           ^  ^      ^  ^
//           1  2      3  4
//
//1과 4에는 아무 문자도 없어야 하며 2와 3에는 반드시 공백이 있어야 한다.
//1-2, 3-4의 부분만 문법적 기능으로 인식하고 그 사이의 문자들은 전부 텍스트로서 표시된다.
//'='의 갯수는 최대 각 6개까지 가능하며, 최소 하나씩은 있어야 한다.

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int lfcount = 0; //줄바꿈 카운터
int userreq = 0; //사용자 요청
int charcount = 0; //counteq - writeeq 함수간 공동변수
bool errorflag = false; // 오류 발생시 "작업 완료!" 메시지 표시 안함
char enterflush; //엔터키 더미??
char yesorno; //선택지 판별용
char rbuf[2] = { 0, }; //메인 버퍼
char *eqchar = "=";
char *lfchar = "\n";
char *spcchar = " ";

void backstep(FILE *input){ //뒤로 한칸 가기
	if(rbuf[0] == '\n') fseek(input, -2, SEEK_CUR); //줄바꿈 문자인 경우에는 두칸 뒤로
	else fseek(input, -1, SEEK_CUR); //일반 문자는 한칸 뒤로
}

void forwardstep(FILE *input){ //앞으로 한칸 가기
	if(rbuf[0] == '\n') fseek(input, 2, SEEK_CUR); //줄바꿈 문자인 경우에는 두칸 앞으로
	else fseek(input, 1, SEEK_CUR); //일반 문자는 한칸 앞으로
}

bool fsee(FILE *input){ //한번읽고 다시 원위치로

	char sbuf[2] = { 0, }; //임시 버퍼 선언

	if(fread(sbuf, 1, 1, input)){ //파일의 끝이 아니면
		rbuf[0] = sbuf[0]; //읽은 문자 버퍼에 복사하고
		backstep(input); //한칸 뒤로 가기
		return true; //파일이 끝나지 않음
	}
	else return false; //파일의 끝인 경우

}

bool copyline(FILE *input, FILE *output){ //한줄 복사 함수
	while(fread(rbuf, 1, 1, input) && rbuf[0] != '\n'){ //다음 줄바꿈 문자까지 복사 (도중에 파일이 끝나도 탈출)
		fwrite(rbuf, 1, 1, output);
	}

	if(feof(input) == 0){ //파일의 끝이 아니면
		fwrite(rbuf, 1, 1, output); //마지막으로 읽은 줄바꿈 문자까지 복사
	}
	else{ //파일의 끝이면
		return false; //false 리턴
	}
	return true; //성공적으로 완료했으면 true리턴
}

int findtill(FILE *input, char thechar, bool tillwhen, bool direction){ //문자 찾는 함수 (특정 문자가 보일때까지/보이지 않을 때까지, 앞쪽으로/뒤쪽으로)
	int count = 0;
	if(direction){ //앞쪽으로 찾기 (앞으로 문자를 찾고 찾은 바로 앞에 커서를 위치)

		if(tillwhen){ //문자가 보일 때까지
			while(fread(rbuf, 1, 1, input) && rbuf[0] != thechar) count++; //문자가 보일때까지 탐색 (도중에 파일이 끝나도 탈출)
			if(rbuf[0] == thechar){ //문자를 정상적으로 찾았으면
				backstep(input); //한칸 뒤로 이동후
				return count; //이동한 횟수 리턴
			}
			else return -count; //문자를 찾지 못하고 파일이 끝나면 음수 이동횟수 반환

		}

		else{ //문자가 보이지 않을 때까지
			while(fread(rbuf, 1, 1, input) && rbuf[0] == thechar) count++;//문자가 안보일때까지 탐색 (도중에 파일이 끝나도 탈출)
			if(rbuf[0] != thechar){ //다른 문자를 찾았으면
				backstep(input); //마지막 문자 위치 앞으로 이동
				return count; //이동횟수 반환
			}
			else return count; //도중에 파일이 끝나 다른 문자를 찾지 못해도 이동횟수 반환
		}

	}

	else{ //뒤쪽으로 찾기 (뒤로 문자를 찾고 찾은 바로 뒤에 커서를 위치)

		if(tillwhen){ //문자가 보일 때까지
			if(fseek(input, -1, SEEK_CUR) < 0) return count; //먼저 한칸 뒤로 간 후 (도중에 파일이 끝나면 이동횟수 반환)
			while(fread(rbuf, 1, 1, input) && rbuf[0] != thechar){ //문자가 보일때까지 탐색 (도중에 파일이 끝나도 탈출)
				count++;
				if(fseek(input, -2, SEEK_CUR) < 0) return -count; //한칸 앞으로 읽었으므로 두칸 뒤로 이동(문자를 찾지 못하고 파일이 끝나면 음수 이동횟수 반환)
			}
			backstep(input); //찾은 문자 뒤쪽으로 이동
			return count; //이동횟수 반환
		}

		else{ //문자가 보이지 않을 때까지
			if(fseek(input, -1, SEEK_CUR) < 0) return count; //먼저 한칸 뒤로 간 후 (도중에 파일이 끝나면 이동횟수 반환)
			while(fread(rbuf, 1, 1, input) && rbuf[0] == thechar){ //문자가 보이지 않을 때까지 탐색 (도중에 파일이 끝나도 탈출)
				count++;
				if(fseek(input, -2, SEEK_CUR) < 0) return count; //한칸 앞으로 읽었으므로 두칸 뒤로 이동(다른 문자를 찾지 못하고 파일이 끝나도 이동횟수 반환)
			}
			return count; //다른 문자를 찾았을때는 이미 마지막 문자 뒤로 커서가 이동해 있으므로 이동횟수 반환
		}
	}
}

int counteq(FILE *input){ //목차 형식이 맞는지 체크, 올바른 형식이면 '='의 갯수를 반환, 올바르지 않은 형식이면 체크하며 이동한 횟수 음수를 반환
	int eq1 = 0; //앞쪽 '='갯수
	int eq2 = 0; //뒤쪽 '='갯수

	//1. 앞쪽 '=' 문법검사
	eq1 = findtill(input, '=', false, true); //'='가 끊길 때까지 갯수세기
	if(rbuf[0] != ' ') return -eq1; //'='바로 앞이 공백이 아니면 오류!

	//2. 줄끝 문법검사
	charcount = findtill(input, '\n', true, true); //줄바꿈 문자 찾기 (경우에 따라서는 파일끝도 허용함)
	if(charcount  < 0) charcount = -charcount; //(math.h의 abs 함수 사용 안함)
	//줄바꿈 문자 바로 뒤쪽 문자가 뭔지 알아봄
	fseek(input, -1, SEEK_CUR);
	fread(rbuf, 1, 1, input);
	if(rbuf[0] != '=') return -(eq1 + charcount); //바로 뒤에 '='가 없으면 오류!

	//3. 뒤쪽 '=' 문법검사
	eq2 = findtill(input, '=', false, false); //뒤쪽으로 '='가 끊길때까지 찾기
	if(rbuf[0] != ' ') return -(eq1 + charcount - eq2); //뒤쪽으로 '='가 끊기는 지점이 공백이 아니면 오류!

	//4. 양쪽 '='갯수 확인
	if(eq1 != eq2) return -(eq1 + charcount - eq2); //양쪽의 '=' 갯수가 다르면 오류!

	//5. 검사 통과.
	fseek(input, eq2, SEEK_CUR); //줄바꿈 바로 뒤로 커서 이동 (검사 완료)
	return eq1; //'='갯수 반환
}

bool writeeq(FILE *input, FILE *output, int userreq){ //목차 형식이 맞는지 체크하고 output 파일에 조정한 길이만큼 출력
	int eq = counteq(input); //counteq 함수 실행
	if(eq > 0){ //올바른 목차이면

		fseek(input, -(eq + charcount), SEEK_CUR); //목차의 처음으로 돌아가

		for(int i = 0;i < eq + userreq;i++) fwrite(eqchar, 1, 1, output); //파일에 조정한 길이만큼 '=' 출력 (앞부분)
		fseek(input, eq, SEEK_CUR); //입력 파일도 똑같이 커서이동

		for(int i = 0;i < charcount - eq;i++){ //앞쪽 '='과 뒤쪽 '=' 사이의 문자 출력
			fread(rbuf, 1, 1, input);
			fwrite(rbuf, 1, 1, output);
		}
		for(int i = 0;i < eq + userreq;i++) fwrite(eqchar, 1, 1, output); //파일에 조정한 길이만큼 '=' 출력 (뒷부분)
		fseek(input, eq, SEEK_CUR); //입력 파일도 똑같이 커서이동

		return true; //성공 반환
	}
	else{ //올바르지 않은 목차이면
		fseek(input, eq, SEEK_CUR); // 줄의 처음으로 돌아가기
		return false; //실패 반환
	}
}

int main() //메인 함수
{
start:; //시작
	errorflag = false; //나중에 다시 돌아왔을때 대비
	FILE *input = fopen("input.txt", "r"); //입력 파일
	FILE *output = fopen("output.txt", "w"); //출력 파일
    fseek(input, 0, SEEK_SET); //입력 파일 커서 시작점으로 이동

    int topcount = 0; //최대 목차 '=' 갯수
    int lowcount = 7; //최소 목차 '=' 갯수 (함수가 무엇이 더 작은지 비교하여 값을 바꾸므로 가장 높은 값으로 초기설정함)
    int eqcount = 0; //함숫값 저장용 변수
	int spccount = 0; //함숫값 저장용 변수

	printf("==============================================================\n");
	printf("나무위키 목차 크기 조정 프로그램 \"namu정원사\" v2.1 by yclee126\n");
	printf("==============================================================\n\n");
	printf("!!주의 : 반드시 UTF-8 형식의 텍스트 파일을 사용해 주세요!\n\n");
	if(input == 0){ //입력 파일 오류
		printf("오류 : 입력 파일을 찾지 못했습니다!\n\n현재 폴더에 input.txt파일을 만들고 위키 글을 붙여넣어 주세요.");
		errorflag = true;
		goto end;
	}
	if(output == 0){ //출력 파일 오류
		printf("오류 : 출력 파일을 기록할 수 없습니다!\n\n기록 가능한 디스크인지 확인해 주세요.");
		errorflag = true;
		goto end;
	}
	printf(" > 조정할 \"=\"의 갯수를 정수로 입력해 주세요. 음수도 가능합니다.\n\n");
	printf(" > 0을 입력시에는 파일분석 후 추천값으로 자동조정 됩니다.\n\n");
	printf("잘못된 목차 문법에는 조정이 적용되지 않습니다!\n");
	printf("파일은 현재 폴더의 output.txt로 저장됩니다.\n");
	printf("\n--------------------------------------------------------------\n");
	printf(" + - 갯수 : ");
    scanf("%d", &userreq); //조정할 갯수 입력받기
    scanf("%c",&enterflush); //마지막 엔터키 입력 버퍼에서 비우기
    printf("\n--------------------------------------------------------------\n");

	//오류검사
    while(fsee(input)){ //줄바꿈 후 첫 문자가 파일의 끝이 아닐동안 반복, 파일의 끝이면 문법 검사로 넘어가기
		if(rbuf[0] == '='){ //줄의 첫 문자가 '='이면 목차검사
			if( (eqcount = counteq(input)) > 0){ //목차검사후 목차가 맞으면
				//목차'='갯수 최대최소 판별후 변수에 대입
				if(topcount < eqcount) topcount = eqcount;
				if(lowcount > eqcount) lowcount = eqcount;
			}
			//목차가 아니면 커서 위치 상관없이 다음 줄로 이동v
		}
		findtill(input, '\n', true, true); //줄바꿈 문자 뒤로 이동 (파일의 끝이면 파일의 끝으로 커서 이동)
		forwardstep(input); //줄바꿈 문자 앞으로 이동
	}

	int autovalue = topcount - lowcount; //최대차 값

	if(autovalue > 5){ //최대 갯수와 최소 갯수의 차가 5 초과인 경우 문법 오류
		printf("\n해당 문서의 \"=\" 갯수 집합이 잘못되었습니다.\n\n가장 \"=\"이 많은 곳과 적은 곳의 차가 %d입니다.\n\n정상 문법은 차가 5이하 입니다.", (topcount - lowcount));
		errorflag = true;
		goto end;
	}

	if(userreq != 0){ //수동조정일 경우 오류메시지 출력 가능
		if(topcount + userreq > 6){ //나무위키 문법 검사(최대'='갯수)
			printf("\n요청한 조정 갯수가 너무 큽니다!!\n\n가장 \"=\"이 많은 곳에서 %d개를 초과했습니다.\n\n나무위키의 최대 \"=\"갯수는 6개입니다.", (topcount + userreq - 6));
			errorflag = true;
			goto end;
		}
		if(lowcount + userreq < 1){ //나무위키 문법 검사(최소'='갯수)
			printf("\n요청한 조정 갯수가 너무 작습니다!!\n\n가장 \"=\"이 적은 곳에서 %d개를 미달했습니다.\n\n나무위키의 최소 \"=\"갯수는 1개입니다.", -(lowcount + userreq - 1));
			errorflag = true;
			goto end;
		}
		if(topcount + userreq == 6 && autovalue != 5){ //나무위키 문법 검사(최대'='갯수 추천)
			printf("\n요청한 조정 갯수는 \"=\"이 가장 많은 곳에서 6개가 되어 문법상 문제가 없지만\n\n변경하면 더이상 하위 목차를 만들 수 없게 됩니다.\n\n계속하려면 y를, 처음으로 돌아갈려면 n을 입력해 주세요.\n\n");
			scanf("%c",&yesorno); //선택지 받기
			scanf("%c",&enterflush); //마지막 엔터키 입력 버퍼에서 비우기
			if(yesorno == 'n'){ //처음으로 돌아가기 선택시
				fclose(input); //메모리 누수 방지
				fclose(output);
				goto start;
			}
		}
		if(lowcount + userreq == 1 && autovalue != 5){ //나무위키 문법 검사(최소'='갯수 추천)
			printf("\n요청한 조정 갯수는 \"=\"이 가장 적은 곳에서 1개가 되어 문법상 문제가 없지만\n\n변경하면 목차의 크기가 너무 커져 좋지 않습니다.\n\n추천하는 최소 \"=\" 갯수는 2개 입니다.\n\n계속하려면 y를, 처음으로 돌아갈려면 n을 입력해 주세요.\n\n");
			scanf("%c",&yesorno); //선택지 받기
			scanf("%c",&enterflush); //마지막 엔터키 입력 버퍼에서 비우기
			if(yesorno == 'n'){ //처음으로 돌아가기 선택시
				fclose(input); //메모리 누수 방지
				fclose(output);
				goto start;
			}
		}
	}
	else{ //자동조정일 때
		if(autovalue == 5) userreq = 6 - topcount; //6 //모든 사이즈를 사용하는 목차이면 현상유지 또는 전체 줄이기
		else if(autovalue == 4){ //5 //더이상 하위 목차 생성이 불가 or 가장 큰 사이즈 방지 -> 사용자에게 질문
			printf("\n가장 \"=\"이 많은 곳과 적은 곳의 차이가 4로 사용자의 선택이 필요합니다.\n\n");
			printf("a) 가장 작은 \"=\"의 갯수를 2개로 하여 보기 좋게 합니다.\n   단, 더이상 하위 목차를 만들 수 없게 됩니다.\n");
			printf("b) 가장 큰 \"=\"의 갯수를 5개로 하여 하위 목차를 위한 공간을 남겨 둡니다.\n   단, 제일 상위 목차의 크기가 커져 보기 좋지 않습니다.\n\n");
			printf("a 또는 b를 입력해 주세요. r 입력시 작성자의 추천값으로 선택됩니다.\n\n");
			scanf("%c",&yesorno); //선택지 받기
			scanf("%c",&enterflush); //마지막 엔터키 입력 버퍼에서 비우기
			if(yesorno == 'b') userreq = 5 - topcount; //하위목차를 선택시
			else userreq = 6 - topcount; //미관을 선택시
		}
		else userreq = autovalue + 2 - topcount; //나머지도 최소 갯수가 2가 되도록 조정
	}

	//파일복사 시작
	printf("\n--------------------------------------------------------------\n");
	fseek(input, 0, SEEK_SET); //입력파일 다시 초기 위치로

	while(fsee(input)){ //줄바꿈 후 첫 문자가 파일의 끝이 아닐동안 반복, 파일의 끝이면 end로 이동
		lfcount++; //줄바꿈 카운터 증가
		if(rbuf[0] == '='){ //줄의 첫 문자가 '='이면 목차검사
			if(writeeq(input, output, userreq) == false) printf("\n%d번째 줄에서 목차 문법 오류가 발견되었습니다.", lfcount); //writeeq함수로 목차검사 및 조정된 '=' 출력, 목차가 아니면 유사 문법 경고
		}

		else if(rbuf[0] == ' '){ //목차앞 공백을 넣는 실수를 저질렀을 때 판별
			spccount = findtill(input, ' ',false, true); //공백 갯수 세기(출력파일에 갯수만큼 출력하기 위함)
			for(int i = 0;i < spccount;i++) fwrite(spcchar, 1, 1, output); //출력파일에도 센 공백만큼 출력
			//오늘의 교훈 ------>                     ^     ^  ^여기
			//                                        |     |   이건 반복횟수가 아니라
			//                                        |     |   파일에 쓰여질 요소 갯수를 말하는 것이다
			//                                        |     |
			//                                  즉, 이것을 이 사이즈 만큼 나누었을 때의 갯수가 저 숫자이다. 명심하자!!
			if(rbuf[0] == '=') printf("\n%d번째 줄에서 목차 문법 오류가 발견되었습니다.", lfcount); //공백앞 첫 글자가 '='이면 유사 문법 경고
		}

		if(copyline(input, output) == false) goto end; //줄 복사시 줄바꿈 문자 전에 파일이 끝나면 end로 이동
	}

end:; //끝내기 구간
	if(errorflag == false) printf("(오류 없음)");
	printf("\n--------------------------------------------------------------\n");
	//메모리 누수 방지
    fclose(input);
    fclose(output);
	if(errorflag == false) printf("\n\n작업 완료!\n"); //심각한 오류 없이 완료시 완료메시지 출력
	printf("\n\n프로그램을 끝내려면 엔터키를, 처음으로 돌아갈려면 n을 입력해 주세요.\n\n");
	/*
	scanf("%c",&yesorno); //선택지 받기
	if(yesorno == 'n'){
		scanf("%c",&enterflush); //마지막 엔터키 입력 버퍼에서 비우기
		goto start; //처음으로 돌아가기 선택시
	}
	*/
	yesorno = fgetc(stdin);
	if(yesorno == 'n') goto start;
	//그외 아무키는 끝내기

    return 0;
}
