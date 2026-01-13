/*
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include "error.h"

#define NUM_OF_ERRORS 31  // Tổng số loại lỗi

/**
 * Cấu trúc lưu thông tin về một loại lỗi
 */
struct ErrorMessage {
	ErrorCode errorCode;    // Mã lỗi
	char* message;          // Thông điệp mô tả lỗi
};

/**
 * Bảng tra cứu thông điệp lỗi
 * 
 * Mỗi ErrorCode được ánh xạ tới một chuỗi mô tả lỗi.
 * Khi gặp lỗi, compiler sẽ in ra thông điệp tương ứng
 * cùng với vị trí (lineNo-colNo) để giúp người dùng 
 * dễ dàng tìm và sửa lỗi.
 */
struct ErrorMessage errors[31] = {
  {ERR_END_OF_COMMENT, "End of comment expected."},
  {ERR_IDENT_TOO_LONG, "Identifier too long."},
  {ERR_NUMBER_TOO_LONG,"Value of integer number exceeds the range!"},
  {ERR_INVALID_CONSTANT_CHAR, "Invalid char constant."},
  {ERR_INVALID_SYMBOL, "Invalid symbol."},
  {ERR_INVALID_IDENT, "An identifier expected."},
  {ERR_INVALID_CONSTANT, "A constant expected."},
  {ERR_INVALID_TYPE, "A type expected."},
  {ERR_INVALID_BASICTYPE, "A basic type expected."},
  {ERR_INVALID_VARIABLE, "A variable expected."},
  {ERR_INVALID_FUNCTION, "A function identifier expected."},
  {ERR_INVALID_PROCEDURE, "A procedure identifier expected."},
  {ERR_INVALID_PARAMETER, "A parameter expected."},
  {ERR_INVALID_STATEMENT, "Invalid statement."},
  {ERR_INVALID_COMPARATOR, "A comparator expected."},
  {ERR_INVALID_EXPRESSION, "Invalid expression."},
  {ERR_INVALID_TERM, "Invalid term."},
  {ERR_INVALID_FACTOR, "Invalid factor."},
  {ERR_INVALID_LVALUE, "Invalid lvalue in assignment."},
  {ERR_INVALID_ARGUMENTS, "Wrong arguments."},
  {ERR_UNDECLARED_IDENT, "Undeclared identifier."},
  {ERR_UNDECLARED_CONSTANT, "Undeclared constant."},
  {ERR_UNDECLARED_INT_CONSTANT, "Undeclared integer constant."},
  {ERR_UNDECLARED_TYPE, "Undeclared type."},
  {ERR_UNDECLARED_VARIABLE, "Undeclared variable."},
  {ERR_UNDECLARED_FUNCTION, "Undeclared function."},
  {ERR_INVALID_RETURN, "Expect the owner of the current scope."},
  {ERR_UNDECLARED_PROCEDURE, "Undeclared procedure."},
  {ERR_DUPLICATE_IDENT, "Duplicate identifier."},
  {ERR_TYPE_INCONSISTENCY, "Type inconsistency"},
  {ERR_PARAMETERS_ARGUMENTS_INCONSISTENCY, "The number of arguments and the number of parameters are inconsistent."}
};

/**
 * Báo lỗi và kết thúc chương trình
 * 
 * Hàm này:
 * 1. Tìm thông điệp lỗi tương ứng với mã lỗi
 * 2. In ra dòng-cột và thông điệp lỗi
 * 3. Gọi exit(0) để dừng chương trình ngay lập tức
 * 
 * Format: lineNo-colNo:message
 * Ví dụ: 5-10:Identifier too long.
 * 
 * @param err - Mã lỗi cần báo
 * @param lineNo - Dòng xảy ra lỗi
 * @param colNo - Cột xảy ra lỗi
 */
void error(ErrorCode err, int lineNo, int colNo) {
	int i;
	for (i = 0; i < NUM_OF_ERRORS; i++)
		if (errors[i].errorCode == err) {
			printf("%d-%d:%s\n", lineNo, colNo, errors[i].message);
			exit(0);
		}
}

/**
 * Báo lỗi thiếu token và kết thúc chương trình
 * 
 * Hàm này được gọi khi parser mong đợi một token cụ thể
 * nhưng không tìm thấy (VD: thiếu dấu ';', thiếu 'END')
 * 
 * Format: lineNo-colNo:Missing <tokenDescription>
 * Ví dụ: 10-5:Missing ';'
 * 
 * @param tokenType - Loại token bị thiếu
 * @param lineNo - Dòng xảy ra lỗi
 * @param colNo - Cột xảy ra lỗi
 */
void missingToken(TokenType tokenType, int lineNo, int colNo) {
	printf("%d-%d:Missing %s\n", lineNo, colNo, tokenToString(tokenType));
	exit(0);
}

/**
 * In thông báo assertion (dùng để debug)
 * 
 * Đơn giản in ra thông điệp, không dừng chương trình
 * 
 * @param msg - Thông điệp cần in
 */
void assert(char* msg) {
	printf("%s\n", msg);
}
