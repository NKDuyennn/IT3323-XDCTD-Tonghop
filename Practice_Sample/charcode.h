/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#ifndef __CHARCODE_H__
#define __CHARCODE_H__

/**
 * CharCode - Định nghĩa các loại ký tự trong ngôn ngữ lập trình
 * 
 * Enum này phân loại các ký tự ASCII thành các nhóm khác nhau
 * để Scanner có thể nhận diện và xử lý chúng một cách hiệu quả
 */
typedef enum {
  CHAR_SPACE,          // Ký tự khoảng trắng: space, tab, newline
  CHAR_LETTER,         // Chữ cái: A-Z, a-z (dùng cho identifier)
  CHAR_DIGIT,          // Chữ số: 0-9 (dùng cho số)
  CHAR_PLUS,           // Ký tự '+' (toán tử cộng)
  CHAR_MINUS,          // Ký tự '-' (toán tử trừ)
  CHAR_TIMES,          // Ký tự '*' (toán tử nhân, cũng dùng trong comment)
  CHAR_SLASH,          // Ký tự '/' (toán tử chia)
  CHAR_LT,             // Ký tự '<' (toán tử nhỏ hơn)
  CHAR_GT,             // Ký tự '>' (toán tử lớn hơn)
  CHAR_EXCLAIMATION,   // Ký tự '!' (dùng trong toán tử !=)
  CHAR_EQ,             // Ký tự '=' (toán tử gán hoặc so sánh)
  CHAR_COMMA,          // Ký tự ',' (dấu phẩy, phân cách tham số)
  CHAR_PERIOD,         // Ký tự '.' (dấu chấm, kết thúc chương trình hoặc truy xuất mảng)
  CHAR_COLON,          // Ký tự ':' (dùng trong ':=' gán giá trị)
  CHAR_SEMICOLON,      // Ký tự ';' (dấu chấm phẩy, kết thúc câu lệnh)
  CHAR_SINGLEQUOTE,    // Ký tự ''' (nháy đơn, bao quanh ký tự constant)
  CHAR_LPAR,           // Ký tự '(' (mở ngoặc đơn)
  CHAR_RPAR,           // Ký tự ')' (đóng ngoặc đơn)
  CHAR_UNKNOWN         // Ký tự không xác định (ký tự không hợp lệ)
} CharCode;

#endif
