# NamuGardener
나무위키 목차 크기 조정 프로그램 "namu정원사" 입니다.

빈틈없이 작동하지만 현재 코드가 복잡합니다.

CodeBlocks로 만들었습니다.

## 기능
* 목차 크기 조정
  * 자동 조정
  * 사용자 조정
* 잘못된 목차 문법 검출

## 사용 방법
1. 입력 파일 input.txt를 만든다.
2. 프로그램을 실행하고 조정할 갯수를 입력한다.
   * 양수는 '='갯수를 더해주고, 음수는 '='갯수를 덜어준다.
   * 0을 입력하면 추천 크기로 자동조정된다.(가장 큰 목차가 '=' 2개)
3. 조정된 파일 output.txt가 생성된다.

## TODO
* 변수 이름 수정
* 함수 다듬기

----------------------------------------------------------------
# English
namu.wiki headings size adjustment tool "namuGardener"

The code is complicated, but it works flawlessly.

Made with CodeBlocks.


## Features
* Headings size adjustment
  * Auto adj.
  * manual adj.
* faulty markup detection

## How to use
1. Make input.txt
2. Launch the program and type adjustment number
   * Positive number adds '=', Negative number subtracts '='
   * Type '0' to adjust automatically (two '='s on the largest headings)
3. Adjusted file output.txt will be created
