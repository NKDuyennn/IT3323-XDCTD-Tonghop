/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
*/
#pragma warning(disable: 6001)
#pragma warning(disable: 6011)

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"

// Các biến extern được khai báo trong reader.c
extern int lineNo;           // Dòng hiện tại
extern int colNo;            // Cột hiện tại
extern int currentChar;      // Ký tự hiện tại

extern CharCode charCodes[]; // Bảng tra cứu loại ký tự

// Biến toàn cục của scanner
int state;                   // Trạng thái hiện tại của FSM (Finite State Machine)
int ln, cn;                  // Lưu vị trí bắt đầu của token (line number, column number)
char str[MAX_IDENT_LEN];     // Buffer lưu chuỗi của identifier/number
char c;                      // Lưu ký tự tạm thời

/***************************************************************/

/**
 * ===== HÀM CHÍNH - SCANNER (PHÂN TÍCH TỪ VỰNG) =====
 * 
 * Đây là hàm chính của Scanner, sử dụng Finite State Machine (FSM)
 * để nhận diện các token từ input stream.
 * 
 * NGUYÊN LÝ HOẠT ĐỘNG:
 * - Scanner hoạt động như một FSM với nhiều trạng thái (state 0, 1, 2, ...)
 * - Mỗi trạng thái xử lý một bước trong quá trình nhận diện token
 * - Đọc từng ký tự và chuyển đổi giữa các trạng thái
 * - Khi nhận diện xong token, trả về token đó
 * 
 * CÁC TRẠNG THÁI CHÍNH:
 * - State 0: Trạng thái khởi đầu, phân loại ký tự đầu tiên
 * - State 1: End of file
 * - State 2: Skip khoảng trắng
 * - State 3-6: Nhận diện identifier và keyword
 * - State 7-8: Nhận diện number
 * - State 9-42: Nhận diện các ký hiệu đặc biệt (+, -, *, /, <, >, =, !=, :=, ...)
 * - State 31-34: Nhận diện ký tự constant ('A')
 * - State 35-40: Xử lý comment và ngoặc đơn
 * - State 43: Ký tự không hợp lệ
 * 
 * @return Token vừa nhận diện được
 */
Token* getToken(void)
{
	Token* token = (Token*)malloc(sizeof(Token));
	int index;
	
	switch (state)
	{
	// ===== STATE 0: TRẠNG THÁI KHỞI ĐẦU =====
	// Đây là trạng thái bắt đầu, phân loại ký tự đầu tiên
	// và chuyển sang trạng thái tương ứng
	case 0:
		if (currentChar == EOF) state = 1;  // Hết file
		else
			switch (charCodes[currentChar])
			{
			case CHAR_SPACE:       // Khoảng trắng -> bỏ qua
				state = 2; break;
			case CHAR_LETTER:      // Chữ cái -> identifier hoặc keyword
				ln = lineNo;
				cn = colNo;
				state = 3;
				break;
			case CHAR_DIGIT:       // Chữ số -> number
				state = 7;
				break;
			case CHAR_PLUS:        // Dấu '+'
				state = 9;
				break;
			case CHAR_MINUS:       // Dấu '-'
				state = 10;
				break;
			case CHAR_TIMES:       // Dấu '*' (nhân hoặc comment)
				state = 11;
				break;
			case CHAR_SLASH:       // Dấu '/'
				state = 12;
				break;
			case CHAR_LT:          // Dấu '<' hoặc '<='
				state = 13;
				break;
			case CHAR_GT:          // Dấu '>' hoặc '>='
				state = 16;
				break;
			case CHAR_EQ:          // Dấu '='
				state = 19;
				break;
			case CHAR_EXCLAIMATION:// Dấu '!' (phải theo sau bởi '=' tạo thành '!=')
				state = 20;
				break;
			case CHAR_COMMA:       // Dấu ','
				state = 23;
				break;
			case CHAR_PERIOD:      // Dấu '.' hoặc '.)'
				state = 24;
				break;
			case CHAR_SEMICOLON:   // Dấu ';'
				state = 27;
				break;
			case CHAR_COLON:       // Dấu ':' hoặc ':='
				state = 28;
				break;
			case CHAR_SINGLEQUOTE: // Nháy đơn ''' (bắt đầu char constant)
				state = 31;
				break;
			case CHAR_LPAR:        // Dấu '(' (hoặc bắt đầu comment '(*')
				state = 35;
				break;
			case CHAR_RPAR:        // Dấu ')'
				state = 42;
				break;
			default:               // Ký tự không hợp lệ
				state = 43;
			}
		return getToken();  // Gọi đệ quy để tiếp tục xử lý
		
	// ===== STATE 1: END OF FILE =====
	case 1:
		return makeToken(TK_EOF, lineNo, colNo);
		
	// ===== STATE 2: BỎ QUA KHOẢNG TRẮNG =====
	case 2:
		readChar();      // Đọc ký tự tiếp theo
		state = 0;       // Quay về trạng thái ban đầu
		return getToken();
		
	// ===== STATE 3: ĐỌC IDENTIFIER HOẶC KEYWORD =====
	// Đọc chuỗi ký tự gồm chữ cái và chữ số
	// ===== STATE 3: ĐỌC IDENTIFIER HOẶC KEYWORD =====
	// Đọc chuỗi ký tự gồm chữ cái và chữ số
	case 3:
	{
		ln = lineNo;     // Lưu vị trí bắt đầu
		cn = colNo;
		int count = 1;
		str[0] = (char)currentChar;  // Lưu ký tự đầu tiên
		readChar();
		
		// Tiếp tục đọc chữ cái và chữ số
		while ((currentChar != EOF) &&
			((charCodes[currentChar] == CHAR_LETTER) || (charCodes[currentChar] == CHAR_DIGIT)))
		{
			if (count <= MAX_IDENT_LEN)
			{
				str[count++] = (char)currentChar;
			}
			readChar();
		}
		
		// Kiểm tra độ dài identifier
		if (count > MAX_IDENT_LEN)
		{
			error(ERR_IDENT_TOO_LONG, ln, cn);
			return makeToken(TK_NONE, ln, cn);
		}
		
		str[count] = '\0';  // Kết thúc chuỗi
		state = 4;          // Chuyển sang state kiểm tra keyword
		return getToken();
	}
	
	// ===== STATE 4: KIỂM TRA KEYWORD =====
	// Kiểm tra xem chuỗi vừa đọc có phải keyword không
	case 4:
		token->tokenType = checkKeyword(str);
		if (token->tokenType == TK_NONE) 
			state = 5;  // Không phải keyword -> identifier
		else 
			state = 6;  // Là keyword
		return getToken();
		
	// ===== STATE 5: TẠO TOKEN IDENTIFIER =====
	case 5:
		token = makeToken(TK_IDENT, ln, cn);
		strcpy(token->string, str);
		return token;
		
	// ===== STATE 6: TẠO TOKEN KEYWORD =====
	case 6:
		token = makeToken(checkKeyword(str), ln, cn);
		return token;
		
	// ===== STATE 7: ĐỌC SỐ NGUYÊN =====
	// Đọc chuỗi chữ số và kiểm tra giới hạn INT_MAX
	// ===== STATE 7: ĐỌC SỐ NGUYÊN =====
	// Đọc chuỗi chữ số và kiểm tra giới hạn INT_MAX
	case 7:
		token = makeToken(TK_NONE, lineNo, colNo);
		index = 0;
		char string[11];  // Đủ chứa INT_MAX (10 chữ số)
		ln = lineNo;
		cn = colNo;
		
		// Bỏ qua các số 0 đứng đầu
		while (currentChar == '0')
			readChar();
			
		// Đọc các chữ số
		while (currentChar != EOF && charCodes[currentChar] == CHAR_DIGIT)
		{
			// Kiểm tra độ dài (INT_MAX có 10 chữ số)
			if (index >= 10)
			{
				// Đọc hết phần còn lại
				while (currentChar != EOF && charCodes[currentChar] == CHAR_DIGIT)
				{
					index++;
					readChar();
				}
				error(ERR_NUMBER_TOO_LONG, ln, cn);
				return token;
				break;
			}
			string[index] = currentChar;
			index++;
			readChar();
		}
		
		// Nếu toàn bộ là số 0
		if (index == 0)
		{
			token->tokenType = TK_NUMBER;
			token->value = 0;
			token->string[0] = '0';
			token->string[1] = '\0';
			return token;
		}

		string[index] = '\0';
		
		// So sánh với INT_MAX để đảm bảo không vượt quá giới hạn
		char strNumber[11];
		sprintf(strNumber, "%d", INT_MAX);
		if (strlen(string) == strlen(strNumber) && strcmp(string, strNumber) > 0)
		{
			error(ERR_NUMBER_TOO_LONG, ln, cn);
			return token;
		}
		
		// Tạo token số hợp lệ
		token->tokenType = TK_NUMBER;
		strcpy(token->string, string);
		token->value = atoi(string);
		return token;
		
	// ===== STATE 9-12: CÁC TOÁN TỬ SỐ HỌC ĐƠN GIẢN =====
	case 9:  // Dấu '+'
		readChar();
		return makeToken(SB_PLUS, lineNo, colNo - 1);
	case 10: // Dấu '-'
		readChar();
		return makeToken(SB_MINUS, lineNo, colNo - 1);
	case 11: // Dấu '*'
		readChar();
		return makeToken(SB_TIMES, lineNo, colNo - 1);
	case 12: // Dấu '/'
		readChar();
		return makeToken(SB_SLASH, lineNo, colNo - 1);
		
	// ===== STATE 13-15: TOÁN TỬ '<' VÀ '<=' =====
	case 13:
		readChar();
		if (charCodes[currentChar] == CHAR_EQ) 
			state = 14;  // '<=' 
		else 
			state = 15;  // '<'
		return getToken();
	case 14:  // Dấu '<='
		readChar();
		return makeToken(SB_LE, lineNo, colNo - 1);
	case 15:  // Dấu '<'
		return makeToken(SB_LT, lineNo, colNo - 1);
		
	// ===== STATE 16-18: TOÁN TỬ '>' VÀ '>=' =====
	case 16:
		readChar();
		if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ))
			state = 17;  // '>='
		else
			state = 18;  // '>'
		return getToken();
	case 17:  // Dấu '>='
		readChar();
		return makeToken(SB_GE, lineNo, colNo - 1);
	case 18:  // Dấu '>'
		return makeToken(SB_GT, lineNo, colNo - 1);
		
	// ===== STATE 19: TOÁN TỬ '=' =====
	case 19:
		readChar();
		return makeToken(SB_EQ, lineNo, colNo - 1);
		
	// ===== STATE 20-22: TOÁN TỬ '!=' =====
	case 20:
		readChar();
		if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ))
			state = 21;  // '!='
		else
			state = 22;  // '!' đơn lẻ (lỗi)
		return getToken();
	case 21:  // Dấu '!='
		readChar();
		return makeToken(SB_NEQ, lineNo, colNo - 1);
	case 22:  // Lỗi: '!' phải đi với '='
		token = makeToken(TK_NONE, lineNo, colNo - 1);
		error(ERR_INVALID_SYMBOL, token->lineNo, token->colNo);
		return token;
		
	// ===== STATE 23: DẤU PHẨY =====
	case 23:
		readChar();
		return makeToken(SB_COMMA, lineNo, colNo - 1);
		
	// ===== STATE 24-26: DẤU '.' VÀ '.)' =====
	case 24:
		readChar();
		if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_RPAR))
			state = 25;  // '.)'
		else
			state = 26;  // '.'
		return getToken();
	case 25:  // Dấu '.)'  (đóng ngoặc vuông)
		readChar();
		return makeToken(SB_RSEL, lineNo, colNo - 1);
	case 26:  // Dấu '.'
		readChar();
		return makeToken(SB_PERIOD, lineNo, colNo - 1);
		
	// ===== STATE 27: DẤU CHẤM PHẨY =====
	case 27:
		ln = lineNo;
		cn = colNo;
		readChar();
		// Xử lý trường hợp ';' ở cuối dòng
		if (ln != lineNo)
			return makeToken(SB_SEMICOLON, ln, cn);
		return makeToken(SB_SEMICOLON, lineNo, colNo - 1);
		
	// ===== STATE 28-30: DẤU ':' VÀ ':=' =====
	case 28:
		readChar();
		if ((currentChar != EOF) && (charCodes[currentChar] == CHAR_EQ))
			state = 29;  // ':='
		else
			state = 30;  // ':'
		return getToken();
	case 29:  // Dấu ':=' (phép gán)
		readChar();
		return makeToken(SB_ASSIGN, lineNo, colNo - 1);
	case 30:  // Dấu ':'
		return makeToken(SB_COLON, lineNo, colNo - 1);
		
	// ===== STATE 31-34: HẰNG KÝ TỰ ('A') =====
	case 31:  // Đã đọc nháy đơn mở '
		readChar();
		if (currentChar == EOF)
			state = 34;  // Lỗi: thiếu ký tự
		else
			if (isprint(currentChar))  // Ký tự có thể in được
				state = 32;
			else 
				state = 34;  // Lỗi: ký tự không hợp lệ
		return getToken();
	case 32:  // Đã đọc ký tự bên trong
		c = currentChar;  // Lưu ký tự
		readChar();
		if (charCodes[currentChar] == CHAR_SINGLEQUOTE)
			state = 33;  // Có nháy đơn đóng -> hợp lệ
		else
			state = 34;  // Lỗi: thiếu nháy đơn đóng
		return getToken();
	case 33:  // Hoàn thành char constant hợp lệ
		token = makeToken(TK_CHAR, lineNo, colNo - 1);
		token->string[0] = c;
		token->string[1] = '\0';
		readChar();
		return token;
	case 34:  // Lỗi: char constant không hợp lệ
		error(ERR_INVALID_CONSTANT, lineNo, colNo - 2);
		
	// ===== STATE 35-41: NGOẶC ĐƠN VÀ COMMENT =====
	case 35:  // Đã đọc '('
		ln = lineNo;
		cn = colNo;
		readChar();
		if (currentChar == EOF)
			state = 41;  // Chỉ là '('
		else
			switch (charCodes[currentChar])
			{
			case CHAR_PERIOD:  // '(.' -> mở ngoặc vuông
				state = 36;
				break;
			case CHAR_TIMES:   // '(*' -> bắt đầu comment
				state = 38;
				break;
			default:
				state = 41;    // Chỉ là '('
			}
		return getToken();
		
	case 36:  // '(.' -> mở ngoặc vuông
		readChar();
		return makeToken(SB_LSEL, ln, cn);
		
	// ===== STATE 37-40: XỬ LÝ COMMENT (* ... *) =====
	case 37:  // Đang trong comment, tìm '*'
		while ((currentChar != EOF) && (charCodes[currentChar] != CHAR_TIMES))
		{
			state = 37;
			readChar();
		}
		if (currentChar == EOF)
			state = 40;  // Lỗi: comment không đóng
		else 
			state = 38;  // Tìm thấy '*', kiểm tra tiếp
		return getToken();
		
	case 38:  // Đã gặp '(* hoặc đang kiểm tra '*'
		while ((currentChar != EOF) && (charCodes[currentChar] == CHAR_TIMES))
		{
			state = 38;
			readChar();
		}
		if (currentChar == EOF)
			state = 40;  // Lỗi: comment không đóng
		else
			if (charCodes[currentChar] == CHAR_RPAR) 
				state = 39;  // Gặp *)' -> kết thúc comment
			else 
				state = 37;  // Tiếp tục đọc comment
		return getToken();
		
	case 39:  // Kết thúc comment thành công
		state = 0;   // Quay về trạng thái ban đầu
		readChar();
		return getToken();
		
	case 40:  // Lỗi: comment không được đóng
		error(ERR_END_OF_COMMENT, lineNo, colNo);
		
	case 41:  // Chỉ là dấu '(' thông thường
		return makeToken(SB_LPAR, ln, cn);
		
	// ===== STATE 42: DẤU NGOẶC ĐÓNG ')' =====
	case 42:
		ln = lineNo;
		cn = colNo;
		readChar();
		// Xử lý trường hợp ')' ở cuối dòng
		if (ln != lineNo)
			return makeToken(SB_RPAR, ln, cn);
		return makeToken(SB_RPAR, lineNo, colNo - 1);
		
	// ===== STATE 43: KÝ TỰ KHÔNG HỢP LỆ =====
	case 43:
		token = makeToken(TK_NONE, lineNo, colNo);
		error(ERR_INVALID_SYMBOL, lineNo, colNo);
		readChar();
		return token;
	}
}

/**
 * Lấy token hợp lệ tiếp theo
 * 
 * Hàm này gọi getToken() liên tục và bỏ qua các token lỗi (TK_NONE)
 * cho đến khi tìm thấy token hợp lệ. Giải phóng bộ nhớ các token lỗi.
 * 
 * @return Token hợp lệ đầu tiên tìm được
 */
Token* getValidToken(void) {
	state = 0;  // Reset trạng thái về ban đầu
	Token* token = getToken();
	
	// Lặp cho đến khi gặp token hợp lệ
	while (token->tokenType == TK_NONE) {
		free(token);  // Giải phóng token lỗi
		token = getToken();
	}
	return token;
}

/******************************************************************/

/**
 * In thông tin token ra màn hình (dùng để debug)
 * 
 * Format: lineNo-colNo:TokenType(value)
 * Ví dụ:
 *   5-10:TK_IDENT(count)
 *   8-3:KW_BEGIN
 *   12-7:TK_NUMBER(123)
 */
void printToken(Token* token) {

	printf("%d-%d:", token->lineNo, token->colNo);

	switch (token->tokenType) {
	case TK_NONE: printf("TK_NONE\n"); break;
	case TK_IDENT: printf("TK_IDENT(%s)\n", token->string); break;
	case TK_NUMBER: printf("TK_NUMBER(%s)\n", token->string); break;
	case TK_CHAR: printf("TK_CHAR(\'%s\')\n", token->string); break;
	case TK_EOF: printf("TK_EOF\n"); break;
	case KW_PROGRAM: printf("KW_PROGRAM\n"); break;
	case KW_CONST: printf("KW_CONST\n"); break;
	case KW_TYPE: printf("KW_TYPE\n"); break;
	case KW_VAR: printf("KW_VAR\n"); break;
	case KW_INTEGER: printf("KW_INTEGER\n"); break;
	case KW_CHAR: printf("KW_CHAR\n"); break;
	case KW_ARRAY: printf("KW_ARRAY\n"); break;
	case KW_OF: printf("KW_OF\n"); break;
	case KW_FUNCTION: printf("KW_FUNCTION\n"); break;
	case KW_PROCEDURE: printf("KW_PROCEDURE\n"); break;
	case KW_BEGIN: printf("KW_BEGIN\n"); break;
	case KW_END: printf("KW_END\n"); break;
	case KW_CALL: printf("KW_CALL\n"); break;
	case KW_IF: printf("KW_IF\n"); break;
	case KW_THEN: printf("KW_THEN\n"); break;
	case KW_ELSE: printf("KW_ELSE\n"); break;
	case KW_WHILE: printf("KW_WHILE\n"); break;
	case KW_DO: printf("KW_DO\n"); break;
	case KW_FOR: printf("KW_FOR\n"); break;
	case KW_TO: printf("KW_TO\n"); break;
	case SB_SEMICOLON: printf("SB_SEMICOLON\n"); break;
	case SB_COLON: printf("SB_COLON\n"); break;
	case SB_PERIOD: printf("SB_PERIOD\n"); break;
	case SB_COMMA: printf("SB_COMMA\n"); break;
	case SB_ASSIGN: printf("SB_ASSIGN\n"); break;
	case SB_EQ: printf("SB_EQ\n"); break;
	case SB_NEQ: printf("SB_NEQ\n"); break;
	case SB_LT: printf("SB_LT\n"); break;
	case SB_LE: printf("SB_LE\n"); break;
	case SB_GT: printf("SB_GT\n"); break;
	case SB_GE: printf("SB_GE\n"); break;
	case SB_PLUS: printf("SB_PLUS\n"); break;
	case SB_MINUS: printf("SB_MINUS\n"); break;
	case SB_TIMES: printf("SB_TIMES\n"); break;
	case SB_SLASH: printf("SB_SLASH\n"); break;
	case SB_LPAR: printf("SB_LPAR\n"); break;
	case SB_RPAR: printf("SB_RPAR\n"); break;
	case SB_LSEL: printf("SB_LSEL\n"); break;
	case SB_RSEL: printf("SB_RSEL\n"); break;
	}
}