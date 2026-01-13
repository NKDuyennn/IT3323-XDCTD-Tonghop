/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include "reader.h"

FILE *inputStream;      // Con trỏ đến file nguồn đang đọc
int lineNo, colNo;      // Vị trí hiện tại trong file (dòng, cột)
int currentChar;        // Ký tự hiện tại vừa đọc được

/**
 * Đọc một ký tự từ input stream
 * 
 * Hàm này:
 * 1. Đọc một ký tự từ file bằng getc()
 * 2. Tăng colNo (số cột) lên 1
 * 3. Nếu gặp xuống dòng '\n':
 *    - Tăng lineNo (số dòng) lên 1
 *    - Reset colNo về 0
 * 
 * Điều này giúp theo dõi chính xác vị trí trong file
 * để báo lỗi đúng dòng/cột khi cần
 * 
 * @return Ký tự vừa đọc, hoặc EOF nếu hết file
 */
int readChar(void) {
  currentChar = getc(inputStream);
  colNo ++;
  if (currentChar == '\n') {
    lineNo ++;
    colNo = 0;
  }
  return currentChar;
}

/**
 * Mở file nguồn để đọc
 * 
 * Hàm này:
 * 1. Mở file ở chế độ "rt" (read text)
 * 2. Khởi tạo lineNo = 1, colNo = 0 (bắt đầu dòng 1)
 * 3. Gọi readChar() để đọc ký tự đầu tiên
 * 
 * @param fileName - Tên/đường dẫn file cần mở
 * @return IO_SUCCESS nếu mở thành công, IO_ERROR nếu file không tồn tại
 */
int openInputStream(char *fileName) {
  inputStream = fopen(fileName, "rt");
  if (inputStream == NULL)
    return IO_ERROR;
  lineNo = 1;
  colNo = 0;
  readChar();
  return IO_SUCCESS;
}

/**
 * Đóng input stream
 * 
 * Giải phóng tài nguyên file
 */
void closeInputStream() {
  fclose(inputStream);
}

