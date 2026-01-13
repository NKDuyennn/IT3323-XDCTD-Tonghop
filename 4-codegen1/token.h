/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#ifndef __TOKEN_H__
#define __TOKEN_H__

// Độ dài tối đa của một identifier (tên biến, hàm, ...)
#define MAX_IDENT_LEN 15

// Số lượng từ khóa trong ngôn ngữ
#define KEYWORDS_COUNT 20

/**
 * TokenType - Định nghĩa các loại token (từ tố) trong ngôn ngữ KPL
 * 
 * Token là đơn vị nhỏ nhất có ý nghĩa trong chương trình.
 * Gồm 3 nhóm chính:
 *   1. Token đặc biệt: TK_NONE, TK_IDENT, TK_NUMBER, TK_CHAR, TK_EOF
 *   2. Keywords (từ khóa): KW_PROGRAM, KW_CONST, ...
 *   3. Special symbols (ký hiệu đặc biệt): SB_SEMICOLON, SB_PLUS, ...
 */
typedef enum {
  TK_NONE,              // Token không hợp lệ hoặc chưa xác định
  TK_IDENT,             // Identifier (tên biến, hàm, procedure, ...)
  TK_NUMBER,            // Hằng số nguyên
  TK_CHAR,              // Hằng ký tự (ví dụ: 'A', 'b')
  TK_EOF,               // End of file - kết thúc file

  // ===== Keywords (Từ khóa) - 20 từ khóa trong ngôn ngữ KPL =====
  KW_PROGRAM,           // Từ khóa PROGRAM - khai báo chương trình
  KW_CONST,             // Từ khóa CONST - khai báo hằng số
  KW_TYPE,              // Từ khóa TYPE - khai báo kiểu dữ liệu
  KW_VAR,               // Từ khóa VAR - khai báo biến
  
  KW_INTEGER,           // Từ khóa INTEGER - kiểu số nguyên
  KW_CHAR,              // Từ khóa CHAR - kiểu ký tự
  KW_ARRAY,             // Từ khóa ARRAY - kiểu mảng
  KW_OF,                // Từ khóa OF - dùng trong ARRAY ... OF ...
  
  KW_FUNCTION,          // Từ khóa FUNCTION - khai báo hàm (có giá trị trả về)
  KW_PROCEDURE,         // Từ khóa PROCEDURE - khai báo procedure (không trả về giá trị)
  
  KW_BEGIN,             // Từ khóa BEGIN - bắt đầu block câu lệnh
  KW_END,               // Từ khóa END - kết thúc block câu lệnh
  KW_CALL,              // Từ khóa CALL - gọi procedure
  
  KW_IF,                // Từ khóa IF - câu lệnh điều kiện
  KW_THEN,              // Từ khóa THEN - phần thực hiện khi IF đúng
  KW_ELSE,              // Từ khóa ELSE - phần thực hiện khi IF sai
  
  KW_WHILE,             // Từ khóa WHILE - vòng lặp while
  KW_DO,                // Từ khóa DO - phần thân của while hoặc for
  KW_FOR,               // Từ khóa FOR - vòng lặp for
  KW_TO,                // Từ khóa TO - dùng trong FOR i := 1 TO 10 DO

  // ===== Special Symbols (Ký hiệu đặc biệt) =====
  // --- Dấu ngăn cách ---
  SB_SEMICOLON,         // Dấu ';' - kết thúc câu lệnh
  SB_COLON,             // Dấu ':' - phân cách trong khai báo
  SB_PERIOD,            // Dấu '.' - kết thúc chương trình
  SB_COMMA,             // Dấu ',' - phân cách các phần tử trong danh sách
  
  // --- Toán tử gán và so sánh ---
  SB_ASSIGN,            // Dấu ':=' - gán giá trị
  SB_EQ,                // Dấu '=' - so sánh bằng
  SB_NEQ,               // Dấu '!=' - so sánh khác
  SB_LT,                // Dấu '<' - nhỏ hơn
  SB_LE,                // Dấu '<=' - nhỏ hơn hoặc bằng
  SB_GT,                // Dấu '>' - lớn hơn
  SB_GE,                // Dấu '>=' - lớn hơn hoặc bằng
  
  // --- Toán tử số học ---
  SB_PLUS,              // Dấu '+' - cộng
  SB_MINUS,             // Dấu '-' - trừ
  SB_TIMES,             // Dấu '*' - nhân
  SB_SLASH,             // Dấu '/' - chia
  
  // --- Dấu ngoặc ---
  SB_LPAR,              // Dấu '(' - mở ngoặc đơn
  SB_RPAR,              // Dấu ')' - đóng ngoặc đơn
  SB_LSEL,              // Dấu '(.' - mở ngoặc vuông (truy xuất mảng)
  SB_RSEL               // Dấu '.)' - đóng ngoặc vuông
} TokenType; 

/**
 * Cấu trúc Token - đại diện cho một token trong chương trình
 * 
 * Mỗi token chứa thông tin về:
 *   - string: Chuỗi ký tự của token (cho identifier và number)
 *   - lineNo, colNo: Vị trí của token trong file (dùng để báo lỗi)
 *   - tokenType: Loại token (keyword, identifier, number, ...)
 *   - value: Giá trị số nguyên (chỉ dùng cho TK_NUMBER)
 */
typedef struct {
  char string[MAX_IDENT_LEN + 1];  // Chuỗi chứa nội dung token (VD: "count", "123", "A")
  int lineNo, colNo;                // Vị trí token trong file nguồn
  TokenType tokenType;              // Loại của token
  int value;                        // Giá trị số (chỉ dùng cho TK_NUMBER)
} Token;

/**
 * Kiểm tra xem một chuỗi có phải là từ khóa hay không
 * @param string - Chuỗi cần kiểm tra
 * @return TokenType của từ khóa tương ứng, hoặc TK_NONE nếu không phải từ khóa
 */
TokenType checkKeyword(char *string);

/**
 * Tạo một token mới với loại và vị trí cho trước
 * @param tokenType - Loại token cần tạo
 * @param lineNo - Số dòng của token
 * @param colNo - Số cột của token
 * @return Con trỏ đến token mới được cấp phát
 */
Token* makeToken(TokenType tokenType, int lineNo, int colNo);

/**
 * Chuyển TokenType thành chuỗi mô tả (để in ra màn hình)
 * @param tokenType - Loại token cần chuyển đổi
 * @return Chuỗi mô tả token (VD: "keyword PROGRAM", "a number")
 */
char *tokenToString(TokenType tokenType);


#endif
