# 📚 DỰ ÁN COMPILER - TỔNG QUAN

## 🎯 MỤC ĐÍCH DỰ ÁN

Dự án này là **bài tập lớn môn Compiler** (Nguyên lý ngôn ngữ lập trình), bao gồm việc xây dựng từng giai đoạn của một compiler hoàn chỉnh cho ngôn ngữ KPL.

---

## 📂 CẤU TRÚC THƯ MỤC

### **1-pttuvung-scanner/** - SCANNER (Phân tích từ vựng)
**Mục tiêu**: Xây dựng Scanner để chuyển source code thành token stream

**Files chính**:
- `scanner.c/h`: Chứa FSM để nhận diện token
- `charcode.c/h`: Phân loại ký tự
- `token.c/h`: Định nghĩa các token
- `reader.c/h`: Đọc file từng ký tự
- `error.c/h`: Xử lý lỗi

**Kiến thức cần nắm**:
- Finite State Machine (FSM)
- Regular Expression
- Token vs Lexeme
- Các loại token (keyword, identifier, number, symbol)

**Test**: Chạy với các file trong `test/`
```bash
./scanner test/example1.kpl
```

---

### **2-ptcuphap-parser/** - PARSER (Phân tích cú pháp)
**Mục tiêu**: Xây dựng Parser để kiểm tra cú pháp và xây dựng cây phân tích

**Files chính**:
- `parser.c/h`: Chứa các hàm parse (compile)
- Kế thừa tất cả files từ bài 1

**Kiến thức cần nắm**:
- Context-Free Grammar (CFG)
- Recursive Descent Parser
- LL(1) Parsing
- Abstract Syntax Tree (AST)
- Các quy tắc văn phạm của KPL

**Ví dụ quy tắc**:
```
Program := PROGRAM Ident ; Block .
Block := ConstDecl TypeDecl VarDecl SubDecl StatementBlock
Statement := AssignSt | CallSt | WhileSt | ...
```

**Test**: Các file `error*.kpl` để test các trường hợp lỗi cú pháp

---

### **3-1-ptnn-symtab/** - SYMBOL TABLE (Bảng ký hiệu)
**Mục tiêu**: Quản lý thông tin các identifier (biến, hàm, hằng, ...)

**Files chính**:
- `symtab.c/h`: Quản lý symbol table
- `debug.c/h`: In debug info

**Kiến thức cần nắm**:
- Symbol Table data structure
- Object: Constant, Variable, Function, Procedure, Type, Parameter
- Scope management (chưa có trong bài này)
- Hash table / Linked list

**Các thao tác**:
- `declareObject()`: Khai báo identifier mới
- `findObject()`: Tìm identifier
- `checkDeclared()`: Kiểm tra đã khai báo chưa

---

### **3-2-ptnn-scopemanage/** - SCOPE MANAGEMENT (Quản lý phạm vi)
**Mục tiêu**: Quản lý các scope lồng nhau (nested scope)

**Thêm vào symtab**: 
- `Scope` structure với pointer `outer` để link scope
- `enterBlock()`: Vào scope mới (function/procedure body)
- `exitBlock()`: Thoát scope

**Kiến thức cần nắm**:
- Nested scope
- Scope chain
- Static scoping vs Dynamic scoping
- Block structure

**Ví dụ**:
```kpl
PROGRAM Example;
VAR x: INTEGER;           (* scope level 0 - global *)

PROCEDURE Proc;
VAR y: INTEGER;           (* scope level 1 - local to Proc *)
BEGIN
  x := 10;                (* tìm x ở scope ngoài *)
  y := 20;                (* tìm y ở scope hiện tại *)
END;
```

---

### **3-3-ptnn-typecheck1/** - TYPE CHECKING 1 (Kiểm tra kiểu - cơ bản)
**Mục tiêu**: Kiểm tra tính đúng đắn của kiểu dữ liệu

**Files chính**:
- `semantics.c/h`: Các hàm kiểm tra semantic

**Kiểm tra**:
1. Kiểu cơ bản: INTEGER, CHAR
2. Phép gán: kiểu trái = kiểu phải?
3. Phép toán: INTEGER + INTEGER ✓, INTEGER + CHAR ✗
4. Hằng số: đúng kiểu không?

**Kiến thức cần nắm**:
- Type system
- Type compatibility
- Type coercion (ép kiểu)
- Static typing vs Dynamic typing

---

### **3-4-ptnn-typecheck2/** - TYPE CHECKING 2 (Kiểm tra kiểu - nâng cao)
**Mục tiêu**: Kiểm tra kiểu cho mảng, hàm, procedure

**Kiểm tra thêm**:
1. **Array type**: 
   - `arr[.(i).]` có đúng kiểu không?
   - Index có phải INTEGER không?
2. **Function call**:
   - Số lượng tham số có khớp không?
   - Kiểu từng tham số có đúng không?
   - Pass-by-value vs Pass-by-reference
3. **Procedure call**: Tương tự function nhưng không có return

**Ví dụ lỗi**:
```kpl
FUNCTION Add(a: INTEGER; b: INTEGER): INTEGER;
BEGIN
  RETURN a + b;
END;

VAR x: CHAR;
BEGIN
  x := Add(1, 2);        (* Lỗi: gán INTEGER cho CHAR *)
  Add(1);                (* Lỗi: thiếu tham số *)
  Add('A', 2);           (* Lỗi: tham số 1 sai kiểu *)
END.
```

---

### **4-sinhmacoban/** - CODE GENERATION (Sinh mã)
**Mục tiêu**: Sinh mã máy ảo từ AST

**Files chính**:
- `codegen.c/h`: Sinh các instruction
- `instructions.c/h`: Định nghĩa instruction set
- Tất cả files từ bài trước

**Instruction Set**:

| Nhóm | Lệnh | Mô tả |
|------|------|-------|
| **Load** | LA, LV, LC | Load address/value/constant |
| **Store** | ST | Store value to memory |
| **Arithmetic** | ADD, SUB, MUL, DIV, NEG | Toán học |
| **Comparison** | EQ, NE, LT, GT, LE, GE | So sánh |
| **Jump** | J, FJ | Jump (vô điều kiện/có điều kiện) |
| **Call** | CALL, RETURN | Gọi hàm/procedure |
| **I/O** | RI, RC, WI, WC | Read/Write Integer/Char |
| **Stack** | HALT | Dừng chương trình |

**Ví dụ sinh mã**:
```kpl
x := (a + b) * 2;
```
↓
```assembly
LV <addr_a>    ; Load value of a
LV <addr_b>    ; Load value of b
OP ADD         ; a + b
LC 2           ; Load constant 2
OP MUL         ; (a+b) * 2
ST <addr_x>    ; Store to x
```

**Kiến thức cần nắm**:
- Stack machine
- Three-address code
- Bytecode
- Virtual machine
- Instruction encoding
- Stack frame layout

---

## 🔄 TRÌNH TỰ HỌC

### **Tuần 1-2: Nền tảng**
1. Đọc kỹ lý thuyết về compiler
2. Hiểu rõ các giai đoạn của compiler
3. Học Regular Expression và FSM

### **Tuần 3-4: Scanner**
1. Làm bài 1 (Scanner)
2. Test kỹ với nhiều test case
3. Hiểu rõ cách FSM hoạt động

### **Tuần 5-6: Parser**
1. Học Context-Free Grammar
2. Hiểu Recursive Descent Parser
3. Làm bài 2 (Parser)

### **Tuần 7-8: Symbol Table**
1. Học về symbol table
2. Làm bài 3-1 (Symbol Table)
3. Làm bài 3-2 (Scope Management)

### **Tuần 9-10: Semantic Analysis**
1. Học về type system
2. Làm bài 3-3 (Type Checking cơ bản)
3. Làm bài 3-4 (Type Checking nâng cao)

### **Tuần 11-12: Code Generation**
1. Học về máy ảo và instruction set
2. Hiểu stack machine
3. Làm bài 4 (Code Generation)

---

## 🛠️ TOOLS CẦN THIẾT

1. **Compiler C**: GCC hoặc MinGW
2. **IDE**: VS Code, Code::Blocks, Visual Studio
3. **Debugger**: GDB
4. **Git**: Để quản lý code

---

## 📝 CÁCH COMPILE & RUN

### **Compile**:
```bash
gcc -o compiler *.c
```

### **Run**:
```bash
./compiler input.kpl
```

### **Debug**:
```bash
gcc -g -o compiler *.c
gdb ./compiler
```

---

## 🎓 TÀI LIỆU THAM KHẢO

### **Sách**:
1. **Compilers: Principles, Techniques, and Tools** (Aho, Lam, Sethi, Ullman) - Dragon Book
2. **Engineering a Compiler** (Cooper, Torczon)
3. **Modern Compiler Implementation in C** (Appel)

### **Online**:
1. [CS143 Stanford - Compilers](https://web.stanford.edu/class/cs143/)
2. [Crafting Interpreters](https://craftinginterpreters.com/)
3. [LLVM Tutorial](https://llvm.org/docs/tutorial/)

---

## ❓ FAQ - CÂU HỎI THƯỜNG GẶP

### **Q1: Tôi nên bắt đầu từ đâu?**
A: Bắt đầu từ bài 1 (Scanner), đọc kỹ comment trong code, vẽ FSM diagram.

### **Q2: Làm sao để debug hiệu quả?**
A: 
- In ra token stream để test scanner
- In ra symbol table để test parser
- Dùng GDB để trace từng bước
- Viết test case nhỏ trước khi test phức tạp

### **Q3: Tôi không hiểu về FSM?**
A: 
- Vẽ sơ đồ trạng thái trên giấy
- Thử chạy từng ví dụ bằng tay
- Xem video tutorial về FSM

### **Q4: Parser quá khó hiểu?**
A:
- Học kỹ về CFG và derivation
- Vẽ parse tree cho ví dụ đơn giản
- Hiểu rõ từng production rule

### **Q5: Làm sao để hiểu code generation?**
A:
- Học về stack machine
- Viết code đơn giản và xem mã sinh ra
- Trace stack khi thực thi

### **Q6: Tôi gặp lỗi compile, làm gì?**
A:
- Đọc kỹ thông báo lỗi
- Kiểm tra syntax C
- Kiểm tra #include và dependencies
- Dùng `-Wall` để xem warning

### **Q7: Test case nào quan trọng nhất?**
A:
- Test cả trường hợp đúng và sai
- Test edge case (rỗng, quá dài, ký tự đặc biệt)
- Test nested structure (scope lồng nhau, biểu thức phức tạp)

---

## 🚀 CHECKLIST HỌC TẬP

- [ ] Đọc hết file `HUONG_DAN_DOC_HIEU.md` trong thư mục `4-sinhmacoban`
- [ ] Hiểu rõ các file cơ sở (charcode, token, error, reader)
- [ ] Vẽ được FSM diagram cho Scanner
- [ ] Viết được parse tree cho một chương trình đơn giản
- [ ] Hiểu được cấu trúc Symbol Table
- [ ] Hiểu được scope chain
- [ ] Liệt kê được các loại type checking cần làm
- [ ] Hiểu được instruction set của máy ảo
- [ ] Trace được quá trình sinh mã cho một biểu thức
- [ ] Hoàn thành tất cả 6 bài tập

---

## 💡 LỜI KHUYÊN

1. **Đừng copy code**: Hiểu từng dòng code, tự viết lại
2. **Debug thường xuyên**: Đặt breakpoint và trace
3. **Vẽ diagram**: FSM, parse tree, scope chain, ...
4. **Test kỹ**: Viết nhiều test case
5. **Hỏi khi cần**: Đừng ngại hỏi giảng viên/bạn bè
6. **Code sạch**: Comment rõ ràng, format đẹp
7. **Version control**: Dùng Git để track changes

---

## 🌟 MỤC TIÊU CUỐI KHÓA

Sau khi hoàn thành dự án, bạn sẽ:
- ✅ Hiểu rõ các giai đoạn của compiler
- ✅ Biết cách thiết kế và implement Scanner
- ✅ Biết cách viết Recursive Descent Parser
- ✅ Hiểu về Symbol Table và Scope Management
- ✅ Biết cách kiểm tra kiểu (Type Checking)
- ✅ Biết cách sinh mã máy ảo
- ✅ Có nền tảng vững để học compiler theory nâng cao

---

**Chúc bạn thành công! 🎉**

*Nếu có thắc mắc, hãy đọc kỹ comment trong code (đã được comment chi tiết) và file HUONG_DAN_DOC_HIEU.md*
