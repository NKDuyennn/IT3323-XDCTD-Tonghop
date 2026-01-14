/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#ifndef __READER_H__
#define __READER_H__

// Các mã trạng thái I/O
#define IO_ERROR 0      // Lỗi khi mở file
#define IO_SUCCESS 1    // Mở file thành công

/**
 * Đọc một ký tự từ file nguồn
 * 
 * Hàm này đọc ký tự tiếp theo từ inputStream và
 * cập nhật các biến lineNo, colNo để theo dõi vị trí
 * 
 * @return Ký tự vừa đọc, hoặc EOF nếu hết file
 */
int readChar(void);

/**
 * Mở file nguồn để đọc
 * 
 * Hàm này mở file với tên cho trước ở chế độ đọc text,
 * khởi tạo lineNo = 1, colNo = 0, và đọc ký tự đầu tiên
 * 
 * @param fileName - Đường dẫn đến file cần đọc
 * @return IO_SUCCESS nếu mở thành công, IO_ERROR nếu lỗi
 */
int openInputStream(char *fileName);

/**
 * Đóng file nguồn
 * 
 * Giải phóng tài nguyên file đã mở
 */
void closeInputStream(void);

#endif
