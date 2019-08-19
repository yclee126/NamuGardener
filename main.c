//������Ű ����ũ�� �����ڵ� ver2 by yclee126 2018/10/15
//https://github.com/yclee126/NamuGardener
//
//  �� <- Ŀ�� -> ��
//
//<����>
//���� ���� : ������
//���� ���� : == ������ ==
//           ^  ^      ^  ^
//           1  2      3  4
//
//1�� 4���� �ƹ� ���ڵ� ����� �ϸ� 2�� 3���� �ݵ�� ������ �־�� �Ѵ�.
//1-2, 3-4�� �κи� ������ ������� �ν��ϰ� �� ������ ���ڵ��� ���� �ؽ�Ʈ�μ� ǥ�õȴ�.
//'='�� ������ �ִ� �� 6������ �����ϸ�, �ּ� �ϳ����� �־�� �Ѵ�.

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int lfcount = 0; //�ٹٲ� ī����
int userreq = 0; //����� ��û
int charcount = 0; //counteq - writeeq �Լ��� ��������
bool errorflag = false; // ���� �߻��� "�۾� �Ϸ�!" �޽��� ǥ�� ����
char enterflush; //����Ű ����??
char yesorno; //������ �Ǻ���
char rbuf[2] = { 0, }; //���� ����
char *eqchar = "=";
char *lfchar = "\n";
char *spcchar = " ";

void backstep(FILE *input){ //�ڷ� ��ĭ ����
	if(rbuf[0] == '\n') fseek(input, -2, SEEK_CUR); //�ٹٲ� ������ ��쿡�� ��ĭ �ڷ�
	else fseek(input, -1, SEEK_CUR); //�Ϲ� ���ڴ� ��ĭ �ڷ�
}

void forwardstep(FILE *input){ //������ ��ĭ ����
	if(rbuf[0] == '\n') fseek(input, 2, SEEK_CUR); //�ٹٲ� ������ ��쿡�� ��ĭ ������
	else fseek(input, 1, SEEK_CUR); //�Ϲ� ���ڴ� ��ĭ ������
}

bool fsee(FILE *input){ //�ѹ��а� �ٽ� ����ġ��

	char sbuf[2] = { 0, }; //�ӽ� ���� ����

	if(fread(sbuf, 1, 1, input)){ //������ ���� �ƴϸ�
		rbuf[0] = sbuf[0]; //���� ���� ���ۿ� �����ϰ�
		backstep(input); //��ĭ �ڷ� ����
		return true; //������ ������ ����
	}
	else return false; //������ ���� ���

}

bool copyline(FILE *input, FILE *output){ //���� ���� �Լ�
	while(fread(rbuf, 1, 1, input) && rbuf[0] != '\n'){ //���� �ٹٲ� ���ڱ��� ���� (���߿� ������ ������ Ż��)
		fwrite(rbuf, 1, 1, output);
	}

	if(feof(input) == 0){ //������ ���� �ƴϸ�
		fwrite(rbuf, 1, 1, output); //���������� ���� �ٹٲ� ���ڱ��� ����
	}
	else{ //������ ���̸�
		return false; //false ����
	}
	return true; //���������� �Ϸ������� true����
}

int findtill(FILE *input, char thechar, bool tillwhen, bool direction){ //���� ã�� �Լ� (Ư�� ���ڰ� ���϶�����/������ ���� ������, ��������/��������)
	int count = 0;
	if(direction){ //�������� ã�� (������ ���ڸ� ã�� ã�� �ٷ� �տ� Ŀ���� ��ġ)

		if(tillwhen){ //���ڰ� ���� ������
			while(fread(rbuf, 1, 1, input) && rbuf[0] != thechar) count++; //���ڰ� ���϶����� Ž�� (���߿� ������ ������ Ż��)
			if(rbuf[0] == thechar){ //���ڸ� ���������� ã������
				backstep(input); //��ĭ �ڷ� �̵���
				return count; //�̵��� Ƚ�� ����
			}
			else return -count; //���ڸ� ã�� ���ϰ� ������ ������ ���� �̵�Ƚ�� ��ȯ

		}

		else{ //���ڰ� ������ ���� ������
			while(fread(rbuf, 1, 1, input) && rbuf[0] == thechar) count++;//���ڰ� �Ⱥ��϶����� Ž�� (���߿� ������ ������ Ż��)
			if(rbuf[0] != thechar){ //�ٸ� ���ڸ� ã������
				backstep(input); //������ ���� ��ġ ������ �̵�
				return count; //�̵�Ƚ�� ��ȯ
			}
			else return count; //���߿� ������ ���� �ٸ� ���ڸ� ã�� ���ص� �̵�Ƚ�� ��ȯ
		}

	}

	else{ //�������� ã�� (�ڷ� ���ڸ� ã�� ã�� �ٷ� �ڿ� Ŀ���� ��ġ)

		if(tillwhen){ //���ڰ� ���� ������
			if(fseek(input, -1, SEEK_CUR) < 0) return count; //���� ��ĭ �ڷ� �� �� (���߿� ������ ������ �̵�Ƚ�� ��ȯ)
			while(fread(rbuf, 1, 1, input) && rbuf[0] != thechar){ //���ڰ� ���϶����� Ž�� (���߿� ������ ������ Ż��)
				count++;
				if(fseek(input, -2, SEEK_CUR) < 0) return -count; //��ĭ ������ �о����Ƿ� ��ĭ �ڷ� �̵�(���ڸ� ã�� ���ϰ� ������ ������ ���� �̵�Ƚ�� ��ȯ)
			}
			backstep(input); //ã�� ���� �������� �̵�
			return count; //�̵�Ƚ�� ��ȯ
		}

		else{ //���ڰ� ������ ���� ������
			if(fseek(input, -1, SEEK_CUR) < 0) return count; //���� ��ĭ �ڷ� �� �� (���߿� ������ ������ �̵�Ƚ�� ��ȯ)
			while(fread(rbuf, 1, 1, input) && rbuf[0] == thechar){ //���ڰ� ������ ���� ������ Ž�� (���߿� ������ ������ Ż��)
				count++;
				if(fseek(input, -2, SEEK_CUR) < 0) return count; //��ĭ ������ �о����Ƿ� ��ĭ �ڷ� �̵�(�ٸ� ���ڸ� ã�� ���ϰ� ������ ������ �̵�Ƚ�� ��ȯ)
			}
			return count; //�ٸ� ���ڸ� ã�������� �̹� ������ ���� �ڷ� Ŀ���� �̵��� �����Ƿ� �̵�Ƚ�� ��ȯ
		}
	}
}

int counteq(FILE *input){ //���� ������ �´��� üũ, �ùٸ� �����̸� '='�� ������ ��ȯ, �ùٸ��� ���� �����̸� üũ�ϸ� �̵��� Ƚ�� ������ ��ȯ
	int eq1 = 0; //���� '='����
	int eq2 = 0; //���� '='����

	//1. ���� '=' �����˻�
	eq1 = findtill(input, '=', false, true); //'='�� ���� ������ ��������
	if(rbuf[0] != ' ') return -eq1; //'='�ٷ� ���� ������ �ƴϸ� ����!

	//2. �ٳ� �����˻�
	charcount = findtill(input, '\n', true, true); //�ٹٲ� ���� ã�� (��쿡 ���󼭴� ���ϳ��� �����)
	if(charcount  < 0) charcount = -charcount; //(math.h�� abs �Լ� ��� ����)
	//�ٹٲ� ���� �ٷ� ���� ���ڰ� ���� �˾ƺ�
	fseek(input, -1, SEEK_CUR);
	fread(rbuf, 1, 1, input);
	if(rbuf[0] != '=') return -(eq1 + charcount); //�ٷ� �ڿ� '='�� ������ ����!

	//3. ���� '=' �����˻�
	eq2 = findtill(input, '=', false, false); //�������� '='�� ���涧���� ã��
	if(rbuf[0] != ' ') return -(eq1 + charcount - eq2); //�������� '='�� ����� ������ ������ �ƴϸ� ����!

	//4. ���� '='���� Ȯ��
	if(eq1 != eq2) return -(eq1 + charcount - eq2); //������ '=' ������ �ٸ��� ����!

	//5. �˻� ���.
	fseek(input, eq2, SEEK_CUR); //�ٹٲ� �ٷ� �ڷ� Ŀ�� �̵� (�˻� �Ϸ�)
	return eq1; //'='���� ��ȯ
}

bool writeeq(FILE *input, FILE *output, int userreq){ //���� ������ �´��� üũ�ϰ� output ���Ͽ� ������ ���̸�ŭ ���
	int eq = counteq(input); //counteq �Լ� ����
	if(eq > 0){ //�ùٸ� �����̸�

		fseek(input, -(eq + charcount), SEEK_CUR); //������ ó������ ���ư�

		for(int i = 0;i < eq + userreq;i++) fwrite(eqchar, 1, 1, output); //���Ͽ� ������ ���̸�ŭ '=' ��� (�պκ�)
		fseek(input, eq, SEEK_CUR); //�Է� ���ϵ� �Ȱ��� Ŀ���̵�

		for(int i = 0;i < charcount - eq;i++){ //���� '='�� ���� '=' ������ ���� ���
			fread(rbuf, 1, 1, input);
			fwrite(rbuf, 1, 1, output);
		}
		for(int i = 0;i < eq + userreq;i++) fwrite(eqchar, 1, 1, output); //���Ͽ� ������ ���̸�ŭ '=' ��� (�޺κ�)
		fseek(input, eq, SEEK_CUR); //�Է� ���ϵ� �Ȱ��� Ŀ���̵�

		return true; //���� ��ȯ
	}
	else{ //�ùٸ��� ���� �����̸�
		fseek(input, eq, SEEK_CUR); // ���� ó������ ���ư���
		return false; //���� ��ȯ
	}
}

int main() //���� �Լ�
{
start:; //����
	errorflag = false; //���߿� �ٽ� ���ƿ����� ���
	FILE *input = fopen("input.txt", "r"); //�Է� ����
	FILE *output = fopen("output.txt", "w"); //��� ����
    fseek(input, 0, SEEK_SET); //�Է� ���� Ŀ�� ���������� �̵�

    int topcount = 0; //�ִ� ���� '=' ����
    int lowcount = 7; //�ּ� ���� '=' ���� (�Լ��� ������ �� ������ ���Ͽ� ���� �ٲٹǷ� ���� ���� ������ �ʱ⼳����)
    int eqcount = 0; //�Լ��� ����� ����
	int spccount = 0; //�Լ��� ����� ����

	printf("==============================================================\n");
	printf("������Ű ���� ũ�� ���� ���α׷� \"namu������\" v2.1 by yclee126\n");
	printf("==============================================================\n\n");
	printf("!!���� : �ݵ�� UTF-8 ������ �ؽ�Ʈ ������ ����� �ּ���!\n\n");
	if(input == 0){ //�Է� ���� ����
		printf("���� : �Է� ������ ã�� ���߽��ϴ�!\n\n���� ������ input.txt������ ����� ��Ű ���� �ٿ��־� �ּ���.");
		errorflag = true;
		goto end;
	}
	if(output == 0){ //��� ���� ����
		printf("���� : ��� ������ ����� �� �����ϴ�!\n\n��� ������ ��ũ���� Ȯ���� �ּ���.");
		errorflag = true;
		goto end;
	}
	printf(" > ������ \"=\"�� ������ ������ �Է��� �ּ���. ������ �����մϴ�.\n\n");
	printf(" > 0�� �Է½ÿ��� ���Ϻм� �� ��õ������ �ڵ����� �˴ϴ�.\n\n");
	printf("�߸��� ���� �������� ������ ������� �ʽ��ϴ�!\n");
	printf("������ ���� ������ output.txt�� ����˴ϴ�.\n");
	printf("\n--------------------------------------------------------------\n");
	printf(" + - ���� : ");
    scanf("%d", &userreq); //������ ���� �Է¹ޱ�
    scanf("%c",&enterflush); //������ ����Ű �Է� ���ۿ��� ����
    printf("\n--------------------------------------------------------------\n");

	//�����˻�
    while(fsee(input)){ //�ٹٲ� �� ù ���ڰ� ������ ���� �ƴҵ��� �ݺ�, ������ ���̸� ���� �˻�� �Ѿ��
		if(rbuf[0] == '='){ //���� ù ���ڰ� '='�̸� �����˻�
			if( (eqcount = counteq(input)) > 0){ //�����˻��� ������ ������
				//����'='���� �ִ��ּ� �Ǻ��� ������ ����
				if(topcount < eqcount) topcount = eqcount;
				if(lowcount > eqcount) lowcount = eqcount;
			}
			//������ �ƴϸ� Ŀ�� ��ġ ������� ���� �ٷ� �̵�v
		}
		findtill(input, '\n', true, true); //�ٹٲ� ���� �ڷ� �̵� (������ ���̸� ������ ������ Ŀ�� �̵�)
		forwardstep(input); //�ٹٲ� ���� ������ �̵�
	}

	int autovalue = topcount - lowcount; //�ִ��� ��

	if(autovalue > 5){ //�ִ� ������ �ּ� ������ ���� 5 �ʰ��� ��� ���� ����
		printf("\n�ش� ������ \"=\" ���� ������ �߸��Ǿ����ϴ�.\n\n���� \"=\"�� ���� ���� ���� ���� ���� %d�Դϴ�.\n\n���� ������ ���� 5���� �Դϴ�.", (topcount - lowcount));
		errorflag = true;
		goto end;
	}

	if(userreq != 0){ //���������� ��� �����޽��� ��� ����
		if(topcount + userreq > 6){ //������Ű ���� �˻�(�ִ�'='����)
			printf("\n��û�� ���� ������ �ʹ� Ů�ϴ�!!\n\n���� \"=\"�� ���� ������ %d���� �ʰ��߽��ϴ�.\n\n������Ű�� �ִ� \"=\"������ 6���Դϴ�.", (topcount + userreq - 6));
			errorflag = true;
			goto end;
		}
		if(lowcount + userreq < 1){ //������Ű ���� �˻�(�ּ�'='����)
			printf("\n��û�� ���� ������ �ʹ� �۽��ϴ�!!\n\n���� \"=\"�� ���� ������ %d���� �̴��߽��ϴ�.\n\n������Ű�� �ּ� \"=\"������ 1���Դϴ�.", -(lowcount + userreq - 1));
			errorflag = true;
			goto end;
		}
		if(topcount + userreq == 6 && autovalue != 5){ //������Ű ���� �˻�(�ִ�'='���� ��õ)
			printf("\n��û�� ���� ������ \"=\"�� ���� ���� ������ 6���� �Ǿ� ������ ������ ������\n\n�����ϸ� ���̻� ���� ������ ���� �� ���� �˴ϴ�.\n\n����Ϸ��� y��, ó������ ���ư����� n�� �Է��� �ּ���.\n\n");
			scanf("%c",&yesorno); //������ �ޱ�
			scanf("%c",&enterflush); //������ ����Ű �Է� ���ۿ��� ����
			if(yesorno == 'n'){ //ó������ ���ư��� ���ý�
				fclose(input); //�޸� ���� ����
				fclose(output);
				goto start;
			}
		}
		if(lowcount + userreq == 1 && autovalue != 5){ //������Ű ���� �˻�(�ּ�'='���� ��õ)
			printf("\n��û�� ���� ������ \"=\"�� ���� ���� ������ 1���� �Ǿ� ������ ������ ������\n\n�����ϸ� ������ ũ�Ⱑ �ʹ� Ŀ�� ���� �ʽ��ϴ�.\n\n��õ�ϴ� �ּ� \"=\" ������ 2�� �Դϴ�.\n\n����Ϸ��� y��, ó������ ���ư����� n�� �Է��� �ּ���.\n\n");
			scanf("%c",&yesorno); //������ �ޱ�
			scanf("%c",&enterflush); //������ ����Ű �Է� ���ۿ��� ����
			if(yesorno == 'n'){ //ó������ ���ư��� ���ý�
				fclose(input); //�޸� ���� ����
				fclose(output);
				goto start;
			}
		}
	}
	else{ //�ڵ������� ��
		if(autovalue == 5) userreq = 6 - topcount; //6 //��� ����� ����ϴ� �����̸� �������� �Ǵ� ��ü ���̱�
		else if(autovalue == 4){ //5 //���̻� ���� ���� ������ �Ұ� or ���� ū ������ ���� -> ����ڿ��� ����
			printf("\n���� \"=\"�� ���� ���� ���� ���� ���̰� 4�� ������� ������ �ʿ��մϴ�.\n\n");
			printf("a) ���� ���� \"=\"�� ������ 2���� �Ͽ� ���� ���� �մϴ�.\n   ��, ���̻� ���� ������ ���� �� ���� �˴ϴ�.\n");
			printf("b) ���� ū \"=\"�� ������ 5���� �Ͽ� ���� ������ ���� ������ ���� �Ӵϴ�.\n   ��, ���� ���� ������ ũ�Ⱑ Ŀ�� ���� ���� �ʽ��ϴ�.\n\n");
			printf("a �Ǵ� b�� �Է��� �ּ���. r �Է½� �ۼ����� ��õ������ ���õ˴ϴ�.\n\n");
			scanf("%c",&yesorno); //������ �ޱ�
			scanf("%c",&enterflush); //������ ����Ű �Է� ���ۿ��� ����
			if(yesorno == 'b') userreq = 5 - topcount; //���������� ���ý�
			else userreq = 6 - topcount; //�̰��� ���ý�
		}
		else userreq = autovalue + 2 - topcount; //�������� �ּ� ������ 2�� �ǵ��� ����
	}

	//���Ϻ��� ����
	printf("\n--------------------------------------------------------------\n");
	fseek(input, 0, SEEK_SET); //�Է����� �ٽ� �ʱ� ��ġ��

	while(fsee(input)){ //�ٹٲ� �� ù ���ڰ� ������ ���� �ƴҵ��� �ݺ�, ������ ���̸� end�� �̵�
		lfcount++; //�ٹٲ� ī���� ����
		if(rbuf[0] == '='){ //���� ù ���ڰ� '='�̸� �����˻�
			if(writeeq(input, output, userreq) == false) printf("\n%d��° �ٿ��� ���� ���� ������ �߰ߵǾ����ϴ�.", lfcount); //writeeq�Լ��� �����˻� �� ������ '=' ���, ������ �ƴϸ� ���� ���� ���
		}

		else if(rbuf[0] == ' '){ //������ ������ �ִ� �Ǽ��� �������� �� �Ǻ�
			spccount = findtill(input, ' ',false, true); //���� ���� ����(������Ͽ� ������ŭ ����ϱ� ����)
			for(int i = 0;i < spccount;i++) fwrite(spcchar, 1, 1, output); //������Ͽ��� �� ���鸸ŭ ���
			//������ ���� ------>                     ^     ^  ^����
			//                                        |     |   �̰� �ݺ�Ƚ���� �ƴ϶�
			//                                        |     |   ���Ͽ� ������ ��� ������ ���ϴ� ���̴�
			//                                        |     |
			//                                  ��, �̰��� �� ������ ��ŭ �������� ���� ������ �� �����̴�. �������!!
			if(rbuf[0] == '=') printf("\n%d��° �ٿ��� ���� ���� ������ �߰ߵǾ����ϴ�.", lfcount); //����� ù ���ڰ� '='�̸� ���� ���� ���
		}

		if(copyline(input, output) == false) goto end; //�� ����� �ٹٲ� ���� ���� ������ ������ end�� �̵�
	}

end:; //������ ����
	if(errorflag == false) printf("(���� ����)");
	printf("\n--------------------------------------------------------------\n");
	//�޸� ���� ����
    fclose(input);
    fclose(output);
	if(errorflag == false) printf("\n\n�۾� �Ϸ�!\n"); //�ɰ��� ���� ���� �Ϸ�� �Ϸ�޽��� ���
	printf("\n\n���α׷��� �������� ����Ű��, ó������ ���ư����� n�� �Է��� �ּ���.\n\n");
	/*
	scanf("%c",&yesorno); //������ �ޱ�
	if(yesorno == 'n'){
		scanf("%c",&enterflush); //������ ����Ű �Է� ���ۿ��� ����
		goto start; //ó������ ���ư��� ���ý�
	}
	*/
	yesorno = fgetc(stdin);
	if(yesorno == 'n') goto start;
	//�׿� �ƹ�Ű�� ������

    return 0;
}
