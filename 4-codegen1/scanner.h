/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#ifndef __SCANNER_H__
#define __SCANNER_H__

#include "token.h"

/**
 * Lấy token tiếp theo từ input stream
 * 
 * Hàm này đọc ký tự từ file và nhận diện token dựa trên
 * finite state machine (máy trạng thái hữu hạn).
 * 
 * @return Con trỏ đến token vừa nhận diện, có thể là:
 *         - Token hợp lệ (keyword, identifier, number, symbol, ...)
 *         - TK_NONE nếu gặp lỗi (invalid symbol, comment không đóng, ...)
 *         - TK_EOF nếu hết file
 */
Token* getToken(void);

/**
 * Lấy token hợp lệ tiếp theo
 * 
 * Hàm này gọi getToken() liên tục cho đến khi gặp một token
 * hợp lệ (khác TK_NONE). Bỏ qua các token lỗi và tiếp tục đọc.
 * 
 * @return Con trỏ đến token hợp lệ đầu tiên
 */
Token* getValidToken(void);

/**
 * In thông tin token ra màn hình (dùng để debug/test)
 * 
 * Format: lineNo-colNo:TokenType(value)
 * Ví dụ: 
 *   5-10:TK_IDENT(count)
 *   8-3:KW_BEGIN
 *   12-7:TK_NUMBER(123)
 * 
 * @param token - Token cần in
 */
void printToken(Token *token);

#endif
