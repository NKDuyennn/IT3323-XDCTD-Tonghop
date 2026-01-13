/*
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#ifndef __ERROR_H__
#define __ERROR_H__
#include "token.h"

/**
 * ErrorCode - Định nghĩa các mã lỗi trong compiler
 * 
 * Các lỗi được phân thành nhiều loại:
 * 1. Lỗi Scanner (phân tích từ vựng): ERR_END_OF_COMMENT, ERR_IDENT_TOO_LONG, ...
 * 2. Lỗi Parser (phân tích cú pháp): ERR_INVALID_STATEMENT, ERR_INVALID_EXPRESSION, ...
 * 3. Lỗi Semantic (ngữ nghĩa): ERR_UNDECLARED_IDENT, ERR_TYPE_INCONSISTENCY, ...
 */
typedef enum {
	// ===== Lỗi Scanner (Lexical Analysis) =====
	ERR_END_OF_COMMENT,                    // Comment không được đóng (thiếu *)
	ERR_IDENT_TOO_LONG,                    // Identifier dài quá 15 ký tự
	ERR_NUMBER_TOO_LONG,                   // Số vượt quá giới hạn INT_MAX
	ERR_INVALID_CONSTANT_CHAR,             // Hằng ký tự không hợp lệ (VD: 'AB', '')
	ERR_INVALID_SYMBOL,                    // Ký hiệu không hợp lệ (ký tự không có trong ngôn ngữ)
	
	// ===== Lỗi Parser (Syntax Analysis) =====
	ERR_INVALID_IDENT,                     // Mong đợi identifier nhưng không phải
	ERR_INVALID_CONSTANT,                  // Mong đợi constant nhưng không phải
	ERR_INVALID_TYPE,                      // Mong đợi khai báo kiểu nhưng không hợp lệ
	ERR_INVALID_BASICTYPE,                 // Mong đợi kiểu cơ bản (INTEGER, CHAR)
	ERR_INVALID_VARIABLE,                  // Mong đợi biến nhưng không phải
	ERR_INVALID_FUNCTION,                  // Mong đợi hàm nhưng không phải
	ERR_INVALID_PROCEDURE,                 // Mong đợi procedure nhưng không phải
	ERR_INVALID_PARAMETER,                 // Tham số không hợp lệ
	ERR_INVALID_STATEMENT,                 // Câu lệnh không hợp lệ
	ERR_INVALID_COMPARATOR,                // Toán tử so sánh không hợp lệ
	ERR_INVALID_EXPRESSION,                // Biểu thức không hợp lệ
	ERR_INVALID_TERM,                      // Term (số hạng) không hợp lệ
	ERR_INVALID_FACTOR,                    // Factor (nhân tử) không hợp lệ
	ERR_INVALID_LVALUE,                    // Vế trái phép gán không hợp lệ (VD: 5 := x)
	ERR_INVALID_ARGUMENTS,                 // Tham số truyền vào không đúng
	
	// ===== Lỗi Semantic (Ngữ nghĩa) =====
	ERR_UNDECLARED_IDENT,                  // Identifier chưa được khai báo
	ERR_UNDECLARED_CONSTANT,               // Hằng chưa được khai báo
	ERR_UNDECLARED_INT_CONSTANT,           // Hằng số nguyên chưa được khai báo
	ERR_UNDECLARED_TYPE,                   // Kiểu dữ liệu chưa được khai báo
	ERR_UNDECLARED_VARIABLE,               // Biến chưa được khai báo
	ERR_UNDECLARED_FUNCTION,               // Hàm chưa được khai báo
	ERR_INVALID_RETURN,                    // Return không hợp lệ (không khớp với hàm owner)
	ERR_UNDECLARED_PROCEDURE,              // Procedure chưa được khai báo
	ERR_DUPLICATE_IDENT,                   // Identifier bị trùng lặp (khai báo 2 lần)
	ERR_TYPE_INCONSISTENCY,                // Kiểu dữ liệu không khớp (VD: gán CHAR cho INTEGER)
	ERR_PARAMETERS_ARGUMENTS_INCONSISTENCY // Số lượng tham số không khớp với số argument khi gọi
} ErrorCode;

/**
 * In lỗi ra màn hình và kết thúc chương trình
 * 
 * @param err - Mã lỗi
 * @param lineNo - Số dòng xảy ra lỗi
 * @param colNo - Số cột xảy ra lỗi
 */
void error(ErrorCode err, int lineNo, int colNo);

/**
 * Báo lỗi thiếu token mong đợi
 * 
 * Ví dụ: thiếu dấu ';', thiếu 'END', ...
 * 
 * @param tokenType - Loại token bị thiếu
 * @param lineNo - Số dòng
 * @param colNo - Số cột
 */
void missingToken(TokenType tokenType, int lineNo, int colNo);

/**
 * In thông báo assertion (dùng để debug)
 * 
 * @param msg - Thông báo cần in
 */
void assert(char* msg);

#endif
