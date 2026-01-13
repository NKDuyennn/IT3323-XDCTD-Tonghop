/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdlib.h>
#include <ctype.h>
#include "token.h"

/**
 * Bảng tra cứu các từ khóa của ngôn ngữ KPL
 * 
 * Mảng này chứa 20 từ khóa, mỗi từ khóa gồm:
 *   - string: Chuỗi ký tự của từ khóa (viết HOA)
 *   - tokenType: Loại token tương ứng
 * 
 * Dùng để tra cứu nhanh: khi đọc được một identifier,
 * ta kiểm tra xem nó có phải từ khóa không bằng cách
 * so sánh với bảng này
 */
struct {
  char string[MAX_IDENT_LEN + 1];
  TokenType tokenType;
} keywords[KEYWORDS_COUNT] = {
  {"PROGRAM", KW_PROGRAM},
  {"CONST", KW_CONST},
  {"TYPE", KW_TYPE},
  {"VAR", KW_VAR},
  {"INTEGER", KW_INTEGER},
  {"CHAR", KW_CHAR},
  {"ARRAY", KW_ARRAY},
  {"OF", KW_OF},
  {"FUNCTION", KW_FUNCTION},
  {"PROCEDURE", KW_PROCEDURE},
  {"BEGIN", KW_BEGIN},
  {"END", KW_END},
  {"CALL", KW_CALL},
  {"IF", KW_IF},
  {"THEN", KW_THEN},
  {"ELSE", KW_ELSE},
  {"WHILE", KW_WHILE},
  {"DO", KW_DO},
  {"FOR", KW_FOR},
  {"TO", KW_TO}
};

/**
 * So sánh một từ khóa với một chuỗi
 * 
 * Hàm này so sánh từng ký tự một cho đến khi:
 *   - Gặp ký tự khác nhau -> trả về 0 (không bằng)
 *   - Cả hai chuỗi đều kết thúc '\0' -> trả về 1 (bằng nhau)
 * 
 * @param kw - Từ khóa cần so sánh
 * @param string - Chuỗi cần kiểm tra
 * @return 1 nếu bằng nhau, 0 nếu khác nhau
 */
int keywordEq(char *kw, char *string) {
  while ((*kw != '\0') && (*string != '\0')) {
    if (*kw != *string) break;
    kw ++; string ++;
  }
  return ((*kw == '\0') && (*string == '\0'));
}

/**
 * Kiểm tra xem một chuỗi có phải là từ khóa không
 * 
 * Hàm này duyệt qua bảng keywords và so sánh chuỗi
 * với từng từ khóa. Nếu tìm thấy, trả về TokenType
 * của từ khóa đó, ngược lại trả về TK_NONE
 * 
 * @param string - Chuỗi cần kiểm tra
 * @return TokenType của từ khóa nếu tìm thấy, TK_NONE nếu không
 */
TokenType checkKeyword(char *string) {
  int i;
  for (i = 0; i < KEYWORDS_COUNT; i++)
    if (keywordEq(keywords[i].string, string)) 
      return keywords[i].tokenType;
  return TK_NONE;
}

/**
 * Tạo một token mới
 * 
 * Hàm này cấp phát bộ nhớ cho một token mới và
 * khởi tạo các trường cơ bản (type, lineNo, colNo)
 * 
 * @param tokenType - Loại token
 * @param lineNo - Số dòng
 * @param colNo - Số cột
 * @return Con trỏ đến token mới được tạo
 */
Token* makeToken(TokenType tokenType, int lineNo, int colNo) {
  Token *token = (Token*)malloc(sizeof(Token));
  token->tokenType = tokenType;
  token->lineNo = lineNo;
  token->colNo = colNo;
  return token;
}

/**
 * Chuyển đổi TokenType thành chuỗi mô tả
 * 
 * Hàm này dùng để in ra thông tin token một cách dễ đọc
 * cho người dùng, đặc biệt hữu ích khi báo lỗi.
 * 
 * Ví dụ:
 *   - KW_PROGRAM -> "keyword PROGRAM"
 *   - TK_NUMBER -> "a number"
 *   - SB_SEMICOLON -> "';'"
 * 
 * @param tokenType - Loại token cần chuyển đổi
 * @return Chuỗi mô tả token
 */
char *tokenToString(TokenType tokenType) {
  switch (tokenType) {
  case TK_NONE: return "None";
  case TK_IDENT: return "an identification";
  case TK_NUMBER: return "a number";
  case TK_CHAR: return "a constant char";
  case TK_EOF: return "end of file";

  case KW_PROGRAM: return "keyword PROGRAM";
  case KW_CONST: return "keyword CONST";
  case KW_TYPE: return "keyword TYPE";
  case KW_VAR: return "keyword VAR";
  case KW_INTEGER: return "keyword INTEGER";
  case KW_CHAR: return "keyword CHAR";
  case KW_ARRAY: return "keyword ARRAY";
  case KW_OF: return "keyword OF";
  case KW_FUNCTION: return "keyword FUNCTION";
  case KW_PROCEDURE: return "keyword PROCEDURE";
  case KW_BEGIN: return "keyword BEGIN";
  case KW_END: return "keyword END";
  case KW_CALL: return "keyword CALL";
  case KW_IF: return "keyword IF";
  case KW_THEN: return "keyword THEN";
  case KW_ELSE: return "keyword ELSE";
  case KW_WHILE: return "keyword WHILE";
  case KW_DO: return "keyword DO";
  case KW_FOR: return "keyword FOR";
  case KW_TO: return "keyword TO";

  case SB_SEMICOLON: return "\';\'";
  case SB_COLON: return "\':\'";
  case SB_PERIOD: return "\'.\'";
  case SB_COMMA: return "\',\'";
  case SB_ASSIGN: return "\':=\'";
  case SB_EQ: return "\'=\'";
  case SB_NEQ: return "\'!=\'";
  case SB_LT: return "\'<\'";
  case SB_LE: return "\'<=\'";
  case SB_GT: return "\'>\'";
  case SB_GE: return "\'>=\'";
  case SB_PLUS: return "\'+\'";
  case SB_MINUS: return "\'-\'";
  case SB_TIMES: return "\'*\'";
  case SB_SLASH: return "\'/\'";
  case SB_LPAR: return "\'(\'";
  case SB_RPAR: return "\')\'";
  case SB_LSEL: return "\'(.\'";
  case SB_RSEL: return "\'.)\'";
  default: return "";
  }
}
