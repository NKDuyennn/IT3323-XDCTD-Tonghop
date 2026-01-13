# HƯỚNG DẪN ĐỌC HIỂU COMPILER KPL

## 📚 TỔNG QUAN

Đây là một **compiler hoàn chỉnh** cho ngôn ngữ lập trình KPL (K Programming Language). Compiler này biên dịch code KPL thành mã máy ảo có thể chạy trên một máy ảo (Virtual Machine).

## 🏗️ KIẾN TRÚC COMPILER

Compiler được chia thành **6 giai đoạn chính**:

```
Source Code (.kpl)
       ↓
   [1. SCANNER] ────→ Token stream
       ↓
   [2. PARSER] ─────→ Abstract Syntax Tree (AST)
       ↓
   [3. SYMTAB] ─────→ Symbol Table (bảng ký hiệu)
       ↓
   [4. SEMANTICS] ──→ Kiểm tra ngữ nghĩa
       ↓
   [5. CODEGEN] ────→ Mã máy ảo
       ↓
   [6. VM] ─────────→ Thực thi
```

---

## 📂 CẤU TRÚC FILE

### **1. CÁC FILE CƠ SỞ** (Foundation)

#### **charcode.h / charcode.c**
- **Mục đích**: Phân loại các ký tự ASCII
- **Chức năng**: 
  - Định nghĩa enum `CharCode` (CHAR_LETTER, CHAR_DIGIT, CHAR_SPACE, ...)
  - Cung cấp bảng tra cứu `charCodes[256]` để map từ ASCII code sang CharCode
- **Ví dụ**: `charCodes['A']` → `CHAR_LETTER`, `charCodes['5']` → `CHAR_DIGIT`

#### **token.h / token.c**
- **Mục đích**: Định nghĩa các token (từ tố) trong ngôn ngữ
- **Các loại token**:
  - Keywords: `KW_PROGRAM`, `KW_IF`, `KW_WHILE`, ...
  - Identifiers: `TK_IDENT` (tên biến, hàm, ...)
  - Numbers: `TK_NUMBER` (hằng số nguyên)
  - Symbols: `SB_PLUS`, `SB_SEMICOLON`, `SB_ASSIGN` (`:=`), ...
- **Hàm quan trọng**:
  - `checkKeyword()`: Kiểm tra xem một chuỗi có phải keyword không
  - `makeToken()`: Tạo token mới
  - `tokenToString()`: Chuyển token thành chuỗi (để báo lỗi)

#### **error.h / error.c**
- **Mục đích**: Xử lý và báo lỗi
- **Các loại lỗi**:
  - Scanner errors: `ERR_IDENT_TOO_LONG`, `ERR_INVALID_SYMBOL`, ...
  - Parser errors: `ERR_INVALID_STATEMENT`, `ERR_INVALID_EXPRESSION`, ...
  - Semantic errors: `ERR_UNDECLARED_IDENT`, `ERR_TYPE_INCONSISTENCY`, ...
- **Hàm quan trọng**:
  - `error()`: Báo lỗi và dừng chương trình
  - `missingToken()`: Báo lỗi thiếu token mong đợi

#### **reader.h / reader.c**
- **Mục đích**: Đọc file nguồn từng ký tự
- **Chức năng**:
  - Mở file nguồn (`openInputStream()`)
  - Đọc từng ký tự (`readChar()`)
  - Theo dõi vị trí (lineNo, colNo) để báo lỗi chính xác
  - Đóng file (`closeInputStream()`)

---

### **2. SCANNER (Phân tích từ vựng)** - scanner.h / scanner.c

**Vai trò**: Chuyển đổi chuỗi ký tự thành chuỗi token

**Nguyên lý hoạt động**:
- Sử dụng **Finite State Machine (FSM)** - Máy trạng thái hữu hạn
- Đọc từng ký tự và chuyển đổi giữa các trạng thái
- Mỗi trạng thái xử lý một bước trong việc nhận diện token

**Các trạng thái chính**:
```
State 0:  Trạng thái khởi đầu, phân loại ký tự đầu tiên
State 1:  End of file
State 2:  Bỏ qua khoảng trắng
State 3-6: Nhận diện identifier/keyword
State 7-8: Nhận diện số nguyên
State 9-42: Nhận diện ký hiệu đặc biệt (+, -, *, /, <, >, :=, ...)
State 31-34: Nhận diện ký tự constant ('A')
State 35-40: Xử lý comment (* ... *) và ngoặc đơn
State 43: Ký tự không hợp lệ
```

**Ví dụ**:
```kpl
count := 123;
```
Scanner tạo ra:
- `TK_IDENT(count)` 
- `SB_ASSIGN` (`:=`)
- `TK_NUMBER(123)`
- `SB_SEMICOLON` (`;`)

**Hàm quan trọng**:
- `getToken()`: Lấy token tiếp theo
- `getValidToken()`: Lấy token hợp lệ (bỏ qua lỗi)

---

### **3. PARSER (Phân tích cú pháp)** - parser.h / parser.c

**Vai trò**: Kiểm tra cú pháp và xây dựng cây cú pháp trừu tượng (AST)

**Nguyên lý**:
- Sử dụng **Recursive Descent Parser** - Parser đệ quy xuống
- Mỗi quy tắc văn phạm → một hàm
- Các hàm gọi nhau để phân tích cấu trúc chương trình

**Cấu trúc chương trình KPL**:
```
Program := PROGRAM Ident ; Block .
Block := ConstDecl TypeDecl VarDecl SubDecl StatementBlock
ConstDecl := CONST ConstDecls | ε
VarDecl := VAR VarDecls | ε
SubDecl := FuncDecl ProcDecl | ε
StatementBlock := BEGIN Statements END
```

**Các hàm parsing quan trọng**:
- `compileProgram()`: Parse toàn bộ chương trình
- `compileBlock()`: Parse một block (khai báo + câu lệnh)
- `compileStatements()`: Parse các câu lệnh
- `compileExpression()`: Parse biểu thức
- `compileTerm()`: Parse term (số hạng)
- `compileFactor()`: Parse factor (nhân tử)

**Ví dụ parsing biểu thức**: `a + b * 3`
```
compileExpression()
  └─ compileTerm() → a
  └─ SB_PLUS
  └─ compileTerm()
       └─ compileFactor() → b
       └─ SB_TIMES
       └─ compileFactor() → 3
```

---

### **4. SYMBOL TABLE (Bảng ký hiệu)** - symtab.h / symtab.c

**Vai trò**: Lưu trữ thông tin về các identifier (biến, hàm, hằng, kiểu, ...)

**Các cấu trúc dữ liệu**:

#### **Object** - Đối tượng trong chương trình
```c
struct Object_ {
  char name[MAX_IDENT_LEN];    // Tên (VD: "count", "readInt")
  ObjectKind kind;              // Loại (biến, hàm, hằng, ...)
  union {                       // Thuộc tính tùy theo loại
    ConstantAttributes* constAttrs;
    VariableAttributes* varAttrs;
    FunctionAttributes* funcAttrs;
    ...
  };
};
```

**Các loại Object**:
- `OBJ_CONSTANT`: Hằng số (VD: `CONST PI = 3`)
- `OBJ_VARIABLE`: Biến (VD: `VAR count: INTEGER`)
- `OBJ_TYPE`: Kiểu dữ liệu (VD: `TYPE MyArray = ARRAY[10] OF INTEGER`)
- `OBJ_FUNCTION`: Hàm (có giá trị trả về)
- `OBJ_PROCEDURE`: Procedure (không trả về)
- `OBJ_PARAMETER`: Tham số của hàm/procedure
- `OBJ_PROGRAM`: Chương trình chính

#### **Scope** - Phạm vi
```c
struct Scope_ {
  ObjectNode *objList;     // Danh sách các object trong scope
  Object *owner;           // Chủ sở hữu (program, function, procedure)
  Scope *outer;            // Scope bên ngoài (nested scope)
  int frameSize;           // Kích thước stack frame
};
```

**Scope Management**:
- Chương trình có nhiều scope lồng nhau (nested)
- Mỗi function/procedure có scope riêng
- Khi tìm identifier, tìm từ scope trong ra scope ngoài

**Ví dụ**:
```kpl
PROGRAM Example;
VAR x: INTEGER;        (* x trong scope global *)

PROCEDURE Proc;
VAR y: INTEGER;        (* y trong scope của Proc *)
BEGIN
  x := 10;             (* Tìm x ở scope ngoài *)
  y := 20;             (* Tìm y ở scope hiện tại *)
END;

BEGIN
  CALL Proc;
END.
```

**Hàm quan trọng**:
- `declareObject()`: Khai báo object mới
- `findObject()`: Tìm object theo tên
- `enterBlock()`: Vào scope mới
- `exitBlock()`: Thoát scope

---

### **5. SEMANTICS (Phân tích ngữ nghĩa)** - semantics.h / semantics.c

**Vai trò**: Kiểm tra tính đúng đắn về mặt ngữ nghĩa

**Các kiểm tra**:
1. **Kiểm tra khai báo**:
   - Biến/hàm đã được khai báo chưa?
   - Có bị khai báo trùng không?

2. **Kiểm tra kiểu (Type Checking)**:
   - Phép gán: kiểu vế trái = kiểu vế phải?
   - Phép toán: `INTEGER + INTEGER` ✓, `INTEGER + CHAR` ✗
   - Tham số hàm: số lượng và kiểu có khớp không?

3. **Kiểm tra phạm vi (Scope)**:
   - Biến có được dùng đúng scope không?
   - Return của hàm có đúng owner không?

**Ví dụ lỗi semantic**:
```kpl
VAR x: INTEGER;
VAR y: CHAR;
BEGIN
  x := y;              (* Lỗi: TYPE_INCONSISTENCY *)
  z := 10;             (* Lỗi: UNDECLARED_VARIABLE *)
END.
```

**Hàm quan trọng**:
- `checkTypeEquality()`: Kiểm tra 2 kiểu có bằng nhau không
- `checkArrayType()`: Kiểm tra kiểu mảng
- `checkIntType()`: Kiểm tra kiểu INTEGER

---

### **6. CODE GENERATION (Sinh mã)** - codegen.h / codegen.c

**Vai trò**: Sinh mã máy ảo từ AST

**Instruction Set** (tập lệnh máy ảo):

#### **Lệnh load/store**:
- `LA addr`: Load Address - đưa địa chỉ lên stack
- `LV addr`: Load Value - đưa giá trị lên stack
- `LC value`: Load Constant - đưa hằng số lên stack
- `ST`: Store - lưu giá trị từ stack vào biến

#### **Lệnh toán học**:
- `OP_ADD`, `OP_SUB`, `OP_MUL`, `OP_DIV`: +, -, *, /
- `OP_NEG`: Phủ định
- `OP_EQ`, `OP_NE`, `OP_LT`, `OP_GT`, `OP_LE`, `OP_GE`: So sánh

#### **Lệnh nhảy**:
- `J addr`: Jump - nhảy vô điều kiện
- `FJ addr`: False Jump - nhảy nếu false
- `CALL addr`: Gọi hàm/procedure

#### **Lệnh I/O**:
- `RI`: Read Integer
- `RC`: Read Char
- `WI`: Write Integer
- `WC`: Write Char

**Ví dụ sinh mã**:
```kpl
x := a + b * 2;
```
Sinh mã:
```assembly
LV addr_a      ; Đưa giá trị a lên stack
LV addr_b      ; Đưa giá trị b lên stack
LC 2           ; Đưa hằng 2 lên stack
OP_MUL         ; b * 2
OP_ADD         ; a + (b * 2)
ST addr_x      ; Lưu kết quả vào x
```

**Hàm quan trọng**:
- `genLA()`, `genLV()`, `genLC()`: Sinh lệnh load
- `genST()`: Sinh lệnh store
- `genOP()`: Sinh lệnh toán học
- `genJ()`, `genFJ()`: Sinh lệnh nhảy

---

### **7. INSTRUCTIONS & DEBUG** - instructions.h / instructions.c, debug.h / debug.c

**instructions**: Định nghĩa các instruction của máy ảo

**debug**: Các hàm debug để in ra symbol table, AST, code, ...

---

## 🔄 QUY TRÌNH BIÊN DỊCH

```
1. main.c gọi openInputStream() → mở file .kpl
2. main.c gọi compile() 
3. compile() gọi compileProgram()
4. compileProgram():
   a. Gọi getToken() để lấy token
   b. Kiểm tra cú pháp
   c. Cập nhật symbol table
   d. Kiểm tra semantic
   e. Sinh mã
5. Lưu mã ra file
6. Máy ảo thực thi mã
```

---

## 🎯 CÁC KHÁI NIỆM QUAN TRỌNG

### **1. Token vs Lexeme**
- **Lexeme**: Chuỗi ký tự trong source code (VD: `"count"`, `"123"`)
- **Token**: Đại diện trừu tượng của lexeme (VD: `TK_IDENT`, `TK_NUMBER`)

### **2. Terminal vs Non-terminal**
- **Terminal**: Ký hiệu kết thúc (token) - VD: `PROGRAM`, `BEGIN`, `;`
- **Non-terminal**: Ký hiệu trung gian (phải phân tích tiếp) - VD: `Block`, `Statement`

### **3. Grammar Rule (Quy tắc văn phạm)**
```
Statement → AssignSt | CallSt | WhileSt | ForSt | IfSt
```
Đọc là: "Statement có thể là AssignSt hoặc CallSt hoặc..."

### **4. Stack Frame**
- Mỗi lần gọi hàm/procedure tạo một stack frame mới
- Frame chứa: local variables, parameters, return address
- `frameSize`: Tổng kích thước của frame

### **5. Pass-by-value vs Pass-by-reference**
- **Pass-by-value** (`PARAM_VALUE`): Truyền giá trị → không thay đổi biến gốc
- **Pass-by-reference** (`PARAM_REFERENCE`): Truyền địa chỉ → có thể thay đổi biến gốc

---

## 📖 LỜI KHUYÊN HỌC TẬP

### **Bước 1: Hiểu các file cơ sở** (đã comment đầy đủ)
1. Đọc `charcode.h/c` - Hiểu cách phân loại ký tự
2. Đọc `token.h/c` - Hiểu các token trong ngôn ngữ
3. Đọc `error.h/c` - Hiểu các loại lỗi
4. Đọc `reader.h/c` - Hiểu cách đọc file

### **Bước 2: Hiểu Scanner** (đã comment đầy đủ)
1. Đọc `scanner.h/c`
2. Vẽ sơ đồ FSM để hiểu rõ các trạng thái
3. Thử chạy debug với file test đơn giản

### **Bước 3: Hiểu Parser**
1. Đọc grammar của KPL (thường có trong tài liệu)
2. Đọc `parser.h/c` - hiểu từng hàm compile
3. Chạy debug để xem cây phân tích

### **Bước 4: Hiểu Symbol Table & Semantics**
1. Đọc `symtab.h/c` - hiểu cấu trúc dữ liệu
2. Vẽ sơ đồ scope cho ví dụ
3. Đọc `semantics.h/c` - hiểu các kiểm tra

### **Bước 5: Hiểu Code Generation**
1. Đọc `instructions.h/c` - hiểu tập lệnh máy ảo
2. Đọc `codegen.h/c` - hiểu cách sinh mã
3. Chạy debug để xem mã sinh ra

### **Bước 6: Thực hành**
1. Viết chương trình KPL đơn giản
2. Chạy compiler step-by-step
3. Xem output ở từng giai đoạn

---

## 🐛 DEBUG TIPS

1. **In token stream**: Dùng `printToken()` để xem scanner hoạt động
2. **In symbol table**: Dùng `printObject()` để xem các object đã khai báo
3. **In code**: Dùng `printInstruction()` để xem mã sinh ra
4. **Dùng breakpoint**: Đặt breakpoint trong các hàm compile để theo dõi

---

## 📚 TÀI LIỆU THAM KHẢO

1. **Compilers: Principles, Techniques, and Tools** (Dragon Book)
2. **Engineering a Compiler**
3. **Modern Compiler Implementation in C**

---

## ❓ CÂU HỎI THƯỜNG GẶP

**Q: Tại sao scanner dùng FSM?**
A: FSM hiệu quả cho việc nhận diện pattern, dễ implement và debug.

**Q: Tại sao cần symbol table?**
A: Để lưu thông tin về identifiers, kiểm tra khai báo trùng, kiểm tra kiểu.

**Q: Code generation sinh ra mã gì?**
A: Mã máy ảo (bytecode), không phải assembly thật.

**Q: Làm sao để học nhanh?**
A: Đọc code + vẽ sơ đồ + chạy debug + thực hành viết compiler nhỏ.

---

**Chúc bạn học tốt môn Compiler! 🚀**

*File này được tạo để giúp bạn hiểu tổng quan về compiler. Đọc kỹ từng phần, sau đó đi sâu vào code đã được comment chi tiết.*
