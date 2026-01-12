/* 
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @version 1.0
 * 
 * PARSER - BO PHAN TICH CU PHAP
 * Nhiem vu: Kiem tra xem cac token tu Scanner co tuan theo cu phap cua ngon ngu KPL hay khong
 */

#include <stdlib.h>
#include <stdio.h>
#include "reader.h"
#include "scanner.h"
#include "parser.h"
#include "error.h"

// ============= BIEN TOAN CUC =============
// currentToken: token hien tai dang xu ly
// lookAhead: token ke tiep (xem truoc 1 buoc de quyet dinh lam gi tiep theo)
Token *currentToken;
Token *lookAhead;

// ============= HAM CO BAN =============

/* 
 * Ham scan() - TIEN LEN TOKEN KE TIEP
 * Chuc nang: Di chuyen sang token tiep theo trong chuoi tokens
 * Vi du: 
 *   Ban dau: current=[PROGRAM], lookAhead=[Example1]
 *   Sau scan(): current=[Example1], lookAhead=[;]
 */
void scan(void) { 
  Token* tmp = currentToken;        // Luu token hien tai de giai phong sau
  currentToken = lookAhead;         // Token ke tiep tro thanh token hien tai
  lookAhead = getValidToken();      // Lay token moi tu scanner
  free(tmp);                        // Giai phong bo nho cua token cu
}

/* 
 * Ham eat() - "AN" MOT TOKEN
 * Chuc nang: Kiem tra xem token ke tiep co dung voi token mong doi khong
 * Tham so: tokenType - loai token mong doi (VD: KW_PROGRAM, SB_SEMICOLON,...)
 * 
 * Hoat dong:
 *   - Neu lookAhead DUNG la tokenType mong doi -> in ra va tien len token tiep
 *   - Neu lookAhead SAI -> bao loi thieu token
 * 
 * Vi du:
 *   Code: PROGRAM Example1;
 *   eat(KW_PROGRAM) -> thanh cong, tien len
 *   eat(TK_IDENT) -> thanh cong, tien len  
 *   eat(SB_SEMICOLON) -> thanh cong, tien len
 */
void eat(TokenType tokenType) {
  // Kiem tra xem token ke tiep co khop voi token mong doi khong
  if (lookAhead->tokenType == tokenType) {
    printToken(lookAhead);           // In token ra console (de debug)
    scan();                          // Tien len token ke tiep
  } else {
    // Neu khong khop -> bao loi thieu token
    missingToken(tokenType, lookAhead->lineNo, lookAhead->colNo);
  }
}

// ============= HAM COMPILE CHUONG TRINH CHINH =============

/*
 * Ham compileProgram() - BIEN DICH TOAN BO CHUONG TRINH
 * Cu phap: PROGRAM <ten_chuong_trinh> ; <block> .
 * 
 * Vi du:
 *   PROGRAM Example1;
 *   BEGIN
 *     x := 5;
 *   END.
 */
void compileProgram(void) {
  assert("Parsing a Program ....");   // In thong bao bat dau
  eat(KW_PROGRAM);                    // Bat buoc phai co tu khoa PROGRAM
  eat(TK_IDENT);                      // Bat buoc phai co ten chuong trinh
  eat(SB_SEMICOLON);                  // Bat buoc phai co dau ;
  compileBlock();                     // Bien dich phan than chuong trinh
  eat(SB_PERIOD);                     // Bat buoc ket thuc bang dau .
  assert("Program parsed!");          // In thong bao hoan thanh
}

// ============= HAM COMPILE BLOCK (KHOI CODE) =============

/*
 * Ham compileBlock() - BIEN DICH MOT KHOI CODE
 * Block co cau truc: [CONST] [TYPE] [VAR] [Function/Procedure] BEGIN...END
 * Moi phan la tuy chon (co the co hoac khong)
 * 
 * Vi du block day du:
 *   CONST MAX = 10;
 *   TYPE T = INTEGER;
 *   VAR x : INTEGER;
 *   FUNCTION f() : INTEGER;
 *   BEGIN
 *     x := 5;
 *   END;
 */
void compileBlock(void) {
  assert("Parsing a Block ....");
  
  // Kiem tra xem co phan CONST khong
  if (lookAhead->tokenType == KW_CONST) {
    eat(KW_CONST);                   // An tu khoa CONST
    compileConstDecl();              // Bien dich khai bao hang dau tien
    compileConstDecls();             // Bien dich cac khai bao hang con lai
    compileBlock2();                 // Chuyen sang phan TYPE
  } 
  else compileBlock2();              // Khong co CONST -> nhay thang sang TYPE
  
  assert("Block parsed!");
}

/*
 * Ham compileBlock2() - XU LY PHAN TYPE
 * Tiep tuc xu ly phan TYPE cua block
 */
void compileBlock2(void) {
  assert("Parsing Block 2 ....");
  
  // Kiem tra xem co phan TYPE khong
  if (lookAhead->tokenType == KW_TYPE) {
    eat(KW_TYPE);                    // An tu khoa TYPE
    compileTypeDecl();               // Bien dich khai bao kieu dau tien
    compileTypeDecls();              // Bien dich cac khai bao kieu con lai
    compileBlock3();                 // Chuyen sang phan VAR
  } 
  else compileBlock3();              // Khong co TYPE -> nhay sang VAR
}

/*
 * Ham compileBlock3() - XU LY PHAN VAR
 * Tiep tuc xu ly phan VAR (khai bao bien)
 */
void compileBlock3(void) {
  assert("Parsing Block 3 ....");
  
  // Kiem tra xem co phan VAR khong
  if (lookAhead->tokenType == KW_VAR) {
    eat(KW_VAR);                     // An tu khoa VAR
    compileVarDecl();                // Bien dich khai bao bien dau tien
    compileVarDecls();               // Bien dich cac khai bao bien con lai
    compileBlock4();                 // Chuyen sang phan Function/Procedure
  } 
  else compileBlock4();              // Khong co VAR -> nhay sang Function/Procedure
}

/*
 * Ham compileBlock4() - XU LY PHAN FUNCTION/PROCEDURE
 * Tiep tuc xu ly phan khai bao ham va thu tuc
 */
void compileBlock4(void) {
  assert("Parsing Block 4 ....");
  compileSubDecls();                 // Bien dich cac Function/Procedure
  compileBlock5();                   // Chuyen sang phan BEGIN...END
}

/*
 * Ham compileBlock5() - XU LY PHAN BEGIN...END
 * Day la phan than chinh cua block chua cac cau lenh thuc thi
 */
void compileBlock5(void) {
  eat(KW_BEGIN);                     // Bat buoc bat dau bang BEGIN
  compileStatements();               // Bien dich cac cau lenh ben trong
  eat(KW_END);                       // Bat buoc ket thuc bang END
}

// ============= HAM XU LY KHAI BAO HANG SO (CONST) =============

/*
 * Ham compileConstDecls() - BIEN DICH NHIEU KHAI BAO HANG
 * Duyet qua tat ca cac khai bao hang lien tiep
 * Dung lai khi gap token khong phai la TK_IDENT
 * 
 * Vi du:
 *   CONST 
 *     MAX = 10;
 *     MIN = 1;
 *     PI = 3.14;
 */
void compileConstDecls(void) {
  // Lap lai trong khi con gap TK_IDENT (ten hang)
  while (lookAhead->tokenType == TK_IDENT) 
    compileConstDecl();              // Bien dich tung khai bao hang
}

/*
 * Ham compileConstDecl() - BIEN DICH MOT KHAI BAO HANG
 * Cu phap: <ten_hang> = <gia_tri> ;
 * 
 * Vi du:
 *   MAX = 10;
 *   NAME = 'John';
 */
void compileConstDecl(void) {
  eat(TK_IDENT);                     // Ten hang (VD: MAX, MIN,...)
  eat(SB_EQ);                        // Dau =
  compileConstant();                 // Gia tri hang (so, ky tu, hoac hang khac)
  eat(SB_SEMICOLON);                 // Dau ; ket thuc
}

// ============= HAM XU LY KHAI BAO KIEU (TYPE) =============

/*
 * Ham compileTypeDecls() - BIEN DICH NHIEU KHAI BAO KIEU
 * Tuong tu compileConstDecls() nhung cho TYPE
 * 
 * Vi du:
 *   TYPE
 *     T1 = INTEGER;
 *     T2 = ARRAY[10] OF CHAR;
 */
void compileTypeDecls(void) {
  // Lap lai trong khi con gap TK_IDENT (ten kieu)
  while (lookAhead->tokenType == TK_IDENT)
    compileTypeDecl();               // Bien dich tung khai bao kieu
}

/*
 * Ham compileTypeDecl() - BIEN DICH MOT KHAI BAO KIEU
 * Cu phap: <ten_kieu> = <kieu_du_lieu> ;
 * 
 * Vi du:
 *   T = INTEGER;
 *   MyArray = ARRAY[5] OF CHAR;
 */
void compileTypeDecl(void) {
  eat(TK_IDENT);                     // Ten kieu moi
  eat(SB_EQ);                        // Dau =
  compileType();                     // Dinh nghia kieu (INTEGER, CHAR, ARRAY,...)
  eat(SB_SEMICOLON);                 // Dau ; ket thuc
}

// ============= HAM XU LY KHAI BAO BIEN (VAR) =============

/*
 * Ham compileVarDecls() - BIEN DICH NHIEU KHAI BAO BIEN
 * 
 * Vi du:
 *   VAR
 *     x : INTEGER;
 *     y : CHAR;
 *     arr : ARRAY[10] OF INTEGER;
 */
void compileVarDecls(void) {
  // Lap lai trong khi con gap TK_IDENT (ten bien)
  while (lookAhead->tokenType == TK_IDENT)
    compileVarDecl();                // Bien dich tung khai bao bien
}

/*
 * Ham compileVarDecl() - BIEN DICH MOT KHAI BAO BIEN
 * Cu phap: <ten_bien> : <kieu_du_lieu> ;
 * 
 * Vi du:
 *   x : INTEGER;
 *   name : CHAR;
 */
void compileVarDecl(void) {
  eat(TK_IDENT);                     // Ten bien
  eat(SB_COLON);                     // Dau :
  compileType();                     // Kieu du lieu cua bien
  eat(SB_SEMICOLON);                 // Dau ; ket thuc
}

// ============= HAM XU LY FUNCTION VA PROCEDURE =============

/*
 * Ham compileSubDecls() - BIEN DICH CAC HAM VA THU TUC
 * "Sub" la viet tat cua Subroutine (chuong trinh con)
 * 
 * Vi du:
 *   FUNCTION f(x: INTEGER): INTEGER;
 *   BEGIN...END;
 *   
 *   PROCEDURE p(y: CHAR);
 *   BEGIN...END;
 */
void compileSubDecls(void) {
  assert("Parsing subroutines ....");
  
  // Lap lai trong khi gap FUNCTION hoac PROCEDURE
  while (lookAhead->tokenType == KW_FUNCTION || lookAhead->tokenType == KW_PROCEDURE) {
    if (lookAhead->tokenType == KW_FUNCTION)
      compileFuncDecl();             // Bien dich function
    else
      compileProcDecl();             // Bien dich procedure
  }
  
  assert("Subroutines parsed ....");
}

/*
 * Ham compileFuncDecl() - BIEN DICH FUNCTION
 * Cu phap: FUNCTION <ten>(<tham_so>) : <kieu_tra_ve> ; <block> ;
 * Function CO gia tri tra ve
 * 
 * Vi du:
 *   FUNCTION Sum(a: INTEGER; b: INTEGER) : INTEGER;
 *   BEGIN
 *     Sum := a + b;
 *   END;
 */
void compileFuncDecl(void) {
  assert("Parsing a function ....");
  eat(KW_FUNCTION);                  // Tu khoa FUNCTION
  eat(TK_IDENT);                     // Ten function
  compileParams();                   // Danh sach tham so (co the rong)
  eat(SB_COLON);                     // Dau :
  compileBasicType();                // Kieu tra ve (INTEGER hoac CHAR)
  eat(SB_SEMICOLON);                 // Dau ;
  compileBlock();                    // Than function
  eat(SB_SEMICOLON);                 // Dau ; ket thuc
  assert("Function parsed ....");
}

/*
 * Ham compileProcDecl() - BIEN DICH PROCEDURE
 * Cu phap: PROCEDURE <ten>(<tham_so>) ; <block> ;
 * Procedure KHONG CO gia tri tra ve
 * 
 * Vi du:
 *   PROCEDURE Print(x: INTEGER);
 *   BEGIN
 *     CALL WriteI(x);
 *   END;
 */
void compileProcDecl(void) {
  assert("Parsing a procedure ....");
  eat(KW_PROCEDURE);                 // Tu khoa PROCEDURE
  eat(TK_IDENT);                     // Ten procedure
  compileParams();                   // Danh sach tham so
  eat(SB_SEMICOLON);                 // Dau ;
  compileBlock();                    // Than procedure
  eat(SB_SEMICOLON);                 // Dau ; ket thuc
  assert("Procedure parsed ....");
}

// ============= HAM XU LY HANG SO (CONSTANT) =============

/*
 * Ham compileUnsignedConstant() - BIEN DICH HANG KHONG DAU
 * Hang khong dau co the la: so, ten hang da dinh nghia, hoac ky tu
 * 
 * Vi du:
 *   10
 *   MAX (neu MAX da dinh nghia truoc do)
 *   'a'
 */
void compileUnsignedConstant(void) {
  switch (lookAhead->tokenType) {
  case TK_NUMBER:                    // So nguyen: 10, 255,...
    eat(TK_NUMBER);
    break;
  case TK_IDENT:                     // Ten hang: MAX, MIN,...
    eat(TK_IDENT);
    break;
  case TK_CHAR:                      // Ky tu: 'a', 'Z',...
    eat(TK_CHAR);
    break;
  default:                           // Khong phai hang hop le
    error(ERR_INVALID_CONSTANT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

/*
 * Ham compileConstant() - BIEN DICH HANG CO DAU
 * Hang co the co dau +/- o dau (chi cho so)
 * 
 * Vi du:
 *   +10
 *   -5
 *   'a'  (ky tu khong co dau)
 *   MAX
 */
void compileConstant(void) {
  switch (lookAhead->tokenType) {
  case SB_PLUS:                      // Dau + o dau
    eat(SB_PLUS);
    compileConstant2();              // Xu ly phan con lai (so hoac ten hang)
    break;
  case SB_MINUS:                     // Dau - o dau
    eat(SB_MINUS);
    compileConstant2();              // Xu ly phan con lai
    break;
  case TK_CHAR:                      // Ky tu dac biet (khong co dau)
    eat(TK_CHAR);
    break;
  default:                           // Truong hop con lai
    compileConstant2();
    break;
  }
}

/*
 * Ham compileConstant2() - XU LY PHAN SAU DAU +/-
 * Chi xu ly so hoac ten hang (khong xu ly ky tu)
 * 
 * Vi du:
 *   10
 *   MAX
 */
void compileConstant2(void) {
  switch (lookAhead->tokenType) {
  case TK_NUMBER:                    // So nguyen
    eat(TK_NUMBER);
    break;
  case TK_IDENT:                     // Ten hang
    eat(TK_IDENT);
    break;
  default:                           // Loi: sau +/- phai la so hoac ten hang
    error(ERR_INVALID_CONSTANT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

// ============= HAM XU LY KIEU DU LIEU (TYPE) =============

/*
 * Ham compileType() - BIEN DICH KIEU DU LIEU
 * Co 4 loai kieu:
 *   1. INTEGER - so nguyen
 *   2. CHAR - ky tu
 *   3. ARRAY[n] OF <kieu> - mang
 *   4. <ten_kieu> - kieu da dinh nghia truoc
 * 
 * Vi du:
 *   INTEGER
 *   CHAR
 *   ARRAY[10] OF INTEGER
 *   ARRAY[5] OF ARRAY[3] OF CHAR  (mang 2 chieu)
 *   MyType  (kieu da dinh nghia)
 */
void compileType(void) {
  switch (lookAhead->tokenType) {
  case KW_INTEGER:                   // Kieu so nguyen
    eat(KW_INTEGER); 
    break;
    
  case KW_CHAR:                      // Kieu ky tu
    eat(KW_CHAR); 
    break;
    
  case KW_ARRAY:                     // Kieu mang
    eat(KW_ARRAY);                   // Tu khoa ARRAY
    eat(SB_LSEL);                    // Dau mo [
    eat(TK_NUMBER);                  // Kich thuoc mang (VD: 10)
    eat(SB_RSEL);                    // Dau dong ]
    eat(KW_OF);                      // Tu khoa OF
    compileType();                   // Kieu phan tu (de quy cho mang nhieu chieu)
    break;
    
  case TK_IDENT:                     // Kieu tu dinh nghia
    eat(TK_IDENT);
    break;
    
  default:                           // Kieu khong hop le
    error(ERR_INVALIDTYPE, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

/*
 * Ham compileBasicType() - BIEN DICH KIEU CO BAN
 * Chi chap nhan INTEGER hoac CHAR (dung cho kieu tra ve va tham so)
 * 
 * Vi du:
 *   INTEGER
 *   CHAR
 */
void compileBasicType(void) {
  switch (lookAhead->tokenType) {
  case KW_INTEGER:                   // Kieu INTEGER
    eat(KW_INTEGER);
    break;
  case KW_CHAR:                      // Kieu CHAR
    eat(KW_CHAR);
    break;
  default:                           // Loi: phai la INTEGER hoac CHAR
    error(ERR_INVALIDBASICTYPE, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

// ============= HAM XU LY THAM SO (PARAMETERS) =============

/*
 * Ham compileParams() - BIEN DICH DANH SACH THAM SO
 * Cu phap: ( <tham_so_1> ; <tham_so_2> ; ... )
 * Danh sach tham so co the rong ()
 * 
 * Vi du:
 *   (x: INTEGER; y: CHAR)
 *   (VAR a: INTEGER)
 *   ()  <- rong
 */
void compileParams(void) {
  // Neu co dau mo ngoac ( thi co tham so
  if (lookAhead->tokenType == SB_LPAR) {
    eat(SB_LPAR);                    // Dau (
    compileParam();                  // Tham so dau tien
    compileParams2();                // Cac tham so con lai (neu co)
    eat(SB_RPAR);                    // Dau )
  }
  // Neu khong co ( thi khong co tham so (de trong)
}

/*
 * Ham compileParams2() - XU LY CAC THAM SO TIEP THEO
 * Lap lai trong khi con gap dau ;
 * 
 * Vi du:
 *   ; y: CHAR ; z: INTEGER
 */
void compileParams2(void) {
  // Lap lai trong khi con gap dau ; (phan cach giua cac tham so)
  while (lookAhead->tokenType == SB_SEMICOLON) {
    eat(SB_SEMICOLON);               // Dau ;
    compileParam();                  // Tham so ke tiep
  }
}

/*
 * Ham compileParam() - BIEN DICH MOT THAM SO
 * Co 2 loai tham so:
 *   1. Tham tri: <ten> : <kieu>
 *   2. Tham chieu: VAR <ten> : <kieu>
 * 
 * Vi du:
 *   x : INTEGER  (tham tri - truyen gia tri)
 *   VAR y : CHAR  (tham chieu - truyen dia chi)
 */
void compileParam(void) {
  switch (lookAhead->tokenType) {
  case TK_IDENT:                     // Tham tri (truyen gia tri)
    eat(TK_IDENT);                   // Ten tham so
    eat(SB_COLON);                   // Dau :
    compileBasicType();              // Kieu tham so
    break;
    
  case KW_VAR:                       // Tham chieu (truyen dia chi)
    eat(KW_VAR);                     // Tu khoa VAR
    eat(TK_IDENT);                   // Ten tham so
    eat(SB_COLON);                   // Dau :
    compileBasicType();              // Kieu tham so
    break;
    
  default:                           // Loi tham so
    error(ERR_INVALIDPARAM, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

// ============= HAM XU LY CAU LENH (STATEMENTS) =============

/*
 * Ham compileStatements() - BIEN DICH NHIEU CAU LENH
 * Xu ly danh sach cac cau lenh cach nhau boi dau ;
 * 
 * Vi du:
 *   x := 5;
 *   CALL WriteLn;
 *   IF x > 0 THEN y := 1;
 */
void compileStatements(void) {
  compileStatement();                // Bien dich cau lenh dau tien
  compileStatements2();              // Bien dich cac cau lenh con lai
}

/*
 * Ham compileStatements2() - XU LY CAC CAU LENH TIEP THEO
 * Lap lai trong khi con gap dau ;
 */
void compileStatements2(void) {
  // Neu gap dau ; thi co cau lenh tiep theo
  if (lookAhead->tokenType == SB_SEMICOLON) {
    eat(SB_SEMICOLON);               // An dau ;
    compileStatement();              // Xu ly cau lenh ke tiep
    compileStatements2();            // De quy xu ly cau lenh tiep theo (neu con)
  }
  // Khong gap ; thi dung lai (het cau lenh)
}

/*
 * Ham compileStatement() - BIEN DICH MOT CAU LENH
 * Co 6 loai cau lenh:
 *   1. Gan gia tri: x := 5
 *   2. Goi procedure: CALL WriteLn
 *   3. Nhom lenh: BEGIN...END
 *   4. If: IF...THEN...ELSE
 *   5. While: WHILE...DO
 *   6. For: FOR...TO...DO
 * 
 * Con co cau lenh rong (empty statement) khi gap: ; hoac END hoac ELSE
 */
void compileStatement(void) {
  switch (lookAhead->tokenType) {
  case TK_IDENT:                     // Cau lenh gan: x := 5
    compileAssignSt();
    break;
    
  case KW_CALL:                      // Goi procedure: CALL WriteLn
    compileCallSt();
    break;
    
  case KW_BEGIN:                     // Nhom lenh: BEGIN...END
    compileGroupSt();
    break;
    
  case KW_IF:                        // Cau lenh dieu kien: IF...THEN...ELSE
    compileIfSt();
    break;
    
  case KW_WHILE:                     // Vong lap While: WHILE...DO
    compileWhileSt();
    break;
    
  case KW_FOR:                       // Vong lap For: FOR...TO...DO
    compileForSt();
    break;
    
  // Cac truong hop duoi day la empty statement (cau lenh rong)
  // Khong lam gi ca, chi de kiem tra FOLLOW set
  case SB_SEMICOLON:                 // ; (cau lenh rong truoc ;)
  case KW_END:                       // END (cau lenh rong truoc END)
  case KW_ELSE:                      // ELSE (cau lenh rong truoc ELSE)
    break;
    
  default:                           // Loi: cau lenh khong hop le
    error(ERR_INVALIDSTATEMENT, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

// ============= CAC LOAI CAU LENH CU THE =============

/*
 * Ham compileAssignSt() - BIEN DICH CAU LENH GAN
 * Cu phap: <bien> := <bieu_thuc>
 * Bien co the la bien don hoac phan tu mang
 * 
 * Vi du:
 *   x := 5
 *   y := x + 3
 *   a[i] := 10
 *   matrix[i][j] := 0
 */
void compileAssignSt(void) {
  assert("Parsing an assign statement ....");
  eat(TK_IDENT);                     // Ten bien
  compileIndexes();                  // Neu la mang thi co indexes: [i][j]...
  eat(SB_ASSIGN);                    // Dau gan :=
  compileExpression();               // Bieu thuc ben phai
  assert("Assign statement parsed ....");
}

/*
 * Ham compileCallSt() - BIEN DICH CAU LENH GOI PROCEDURE
 * Cu phap: CALL <ten_procedure>(<doi_so>)
 * 
 * Vi du:
 *   CALL WriteLn
 *   CALL WriteI(x)
 *   CALL Print(a, b, c)
 */
void compileCallSt(void) {
  assert("Parsing a call statement ....");
  eat(KW_CALL);                      // Tu khoa CALL
  eat(TK_IDENT);                     // Ten procedure
  compileArguments();                // Danh sach doi so (co the rong)
  assert("Call statement parsed ....");
}

/*
 * Ham compileGroupSt() - BIEN DICH NHOM LENH
 * Cu phap: BEGIN <cac_cau_lenh> END
 * Dung de nhom nhieu cau lenh thanh 1 khoi
 * 
 * Vi du:
 *   BEGIN
 *     x := 5;
 *     CALL WriteLn;
 *     y := x + 1
 *   END
 */
void compileGroupSt(void) {
  assert("Parsing a group statement ....");
  eat(KW_BEGIN);                     // Tu khoa BEGIN
  compileStatements();               // Cac cau lenh ben trong
  eat(KW_END);                       // Tu khoa END
  assert("Group statement parsed ....");
}

/*
 * Ham compileIfSt() - BIEN DICH CAU LENH IF
 * Cu phap: IF <dieu_kien> THEN <cau_lenh> [ELSE <cau_lenh>]
 * Phan ELSE la tuy chon (co the co hoac khong)
 * 
 * Vi du:
 *   IF x > 0 THEN y := 1
 *   IF n = 0 THEN F := 1 ELSE F := n * F(n-1)
 */
void compileIfSt(void) {
  assert("Parsing an if statement ....");
  eat(KW_IF);                        // Tu khoa IF
  compileCondition();                // Dieu kien (VD: x > 0)
  eat(KW_THEN);                      // Tu khoa THEN
  compileStatement();                // Cau lenh khi dieu kien dung
  
  // Kiem tra xem co phan ELSE khong
  if (lookAhead->tokenType == KW_ELSE) 
    compileElseSt();                 // Xu ly phan ELSE
    
  assert("If statement parsed ....");
}

/*
 * Ham compileElseSt() - XU LY PHAN ELSE
 * Cu phap: ELSE <cau_lenh>
 */
void compileElseSt(void) {
  eat(KW_ELSE);                      // Tu khoa ELSE
  compileStatement();                // Cau lenh khi dieu kien sai
}

/*
 * Ham compileWhileSt() - BIEN DICH VONG LAP WHILE
 * Cu phap: WHILE <dieu_kien> DO <cau_lenh>
 * Lap lai cau lenh trong khi dieu kien con dung
 * 
 * Vi du:
 *   WHILE i < 10 DO i := i + 1
 *   WHILE x > 0 DO BEGIN
 *     CALL WriteI(x);
 *     x := x - 1
 *   END
 */
void compileWhileSt(void) {
  assert("Parsing a while statement ....");
  eat(KW_WHILE);                     // Tu khoa WHILE
  compileCondition();                // Dieu kien lap
  eat(KW_DO);                        // Tu khoa DO
  compileStatement();                // Cau lenh can lap lai
  assert("While statement parsed ....");
}

/*
 * Ham compileForSt() - BIEN DICH VONG LAP FOR
 * Cu phap: FOR <bien> := <gia_tri_dau> TO <gia_tri_cuoi> DO <cau_lenh>
 * Lap tu gia tri dau den gia tri cuoi (tang dan)
 * 
 * Vi du:
 *   FOR i := 1 TO 10 DO CALL WriteI(i)
 *   FOR n := 1 TO 7 DO BEGIN
 *     CALL WriteLn;
 *     CALL WriteI(F(n))
 *   END
 */
void compileForSt(void) {
  assert("Parsing a for statement ....");
  eat(KW_FOR);                       // Tu khoa FOR
  eat(TK_IDENT);                     // Bien dem (VD: i, n)
  eat(SB_ASSIGN);                    // Dau :=
  compileExpression();               // Gia tri bat dau (VD: 1)
  eat(KW_TO);                        // Tu khoa TO
  compileExpression();               // Gia tri ket thuc (VD: 10)
  eat(KW_DO);                        // Tu khoa DO
  compileStatement();                // Cau lenh can lap lai
  assert("For statement parsed ....");
}

// ============= HAM XU LY DOI SO (ARGUMENTS) =============

/*
 * Ham compileArguments() - BIEN DICH DANH SACH DOI SO
 * Cu phap: ( <bieu_thuc_1> , <bieu_thuc_2> , ... )
 * Dung khi goi function hoac procedure
 * Danh sach co the rong ()
 * 
 * Vi du:
 *   ()  <- rong
 *   (x)  <- 1 doi so
 *   (a, b, c+1)  <- 3 doi so
 */
void compileArguments(void) {
  // Neu co dau ( thi co doi so
  if (lookAhead->tokenType == SB_LPAR) {
    eat(SB_LPAR);                    // Dau (
    
    // Kiem tra xem co phai la danh sach rong () khong
    if (lookAhead->tokenType == SB_RPAR) {
      // Danh sach rong - OK
      eat(SB_RPAR);
      return;
    }
    
    // Kiem tra token dau tien co hop le khong
    switch (lookAhead->tokenType) {
    case TK_IDENT:
    case TK_NUMBER:
    case TK_CHAR:
    case SB_LPAR:
    case SB_PLUS:
    case SB_MINUS:
      // Hop le - tiep tuc
      compileExpression();           // Doi so dau tien
      compileArguments2();           // Cac doi so con lai
      eat(SB_RPAR);                  // Dau )
      break;
      
    default:
      // Token khong hop le - bao loi
      error(ERR_INVALIDARGUMENTS, lookAhead->lineNo, lookAhead->colNo);
      break;
    }
  }
  // Neu khong co ( thi khong co doi so
}

/*
 * Ham compileArguments2() - XU LY CAC DOI SO TIEP THEO
 * Lap lai trong khi con gap dau ,
 * 
 * Vi du:
 *   , y , z+1
 */
void compileArguments2(void) {
  // Lap lai trong khi con gap dau , (phan cach giua cac doi so)
  while (lookAhead->tokenType == SB_COMMA) {
    eat(SB_COMMA);                   // Dau ,
    
    // Kiem tra token tiep theo co hop le khong
    switch (lookAhead->tokenType) {
    case TK_IDENT:
    case TK_NUMBER:
    case TK_CHAR:
    case SB_LPAR:
    case SB_PLUS:
    case SB_MINUS:
      // Hop le - tiep tuc
      compileExpression();           // Doi so ke tiep
      break;
      
    default:
      // Token khong hop le sau dau ,
      // Vi du: f(a, ), f(a, BEGIN), f(a, ;)
      error(ERR_INVALIDARGUMENTS, lookAhead->lineNo, lookAhead->colNo);
      return;  // Dung lai de tranh loi cascade
    }
  }
}

// ============= HAM XU LY DIEU KIEN (CONDITION) =============

/*
 * Ham compileCondition() - BIEN DICH DIEU KIEN
 * Cu phap: <bieu_thuc_1> <toan_tu_so_sanh> <bieu_thuc_2>
 * Toan tu so sanh: = != < <= > >=
 * 
 * Vi du:
 *   x = 0
 *   y != 5
 *   a + b > 10
 *   i <= n
 */
void compileCondition(void) {
  compileExpression();               // Bieu thuc ben trai
  compileCondition2();               // Toan tu so sanh + bieu thuc ben phai
}

/*
 * Ham compileCondition2() - XU LY TOAN TU SO SANH
 * Kiem tra va xu ly 6 loai toan tu so sanh
 */
void compileCondition2(void) {
  switch (lookAhead->tokenType) {
  case SB_EQ:                        // Dau = (bang)
    eat(SB_EQ);
    compileExpression();             // Bieu thuc ben phai
    break;
    
  case SB_NEQ:                       // Dau != (khac)
    eat(SB_NEQ);
    compileExpression();
    break;
    
  case SB_LE:                        // Dau <= (nho hon hoac bang)
    eat(SB_LE);
    compileExpression();
    break;
    
  case SB_LT:                        // Dau < (nho hon)
    eat(SB_LT);
    compileExpression();
    break;
    
  case SB_GE:                        // Dau >= (lon hon hoac bang)
    eat(SB_GE);
    compileExpression();
    break;
    
  case SB_GT:                        // Dau > (lon hon)
    eat(SB_GT);
    compileExpression();
    break;
    
  default:                           // Loi: toan tu so sanh khong hop le
    error(ERR_INVALIDCOMPARATOR, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}



// ============= HAM XU LY BIEU THUC (EXPRESSION) =============

/*
 * LOGIC BAT LOI CUOI CUNG (CORRECT):
 * 
 * 1. ERR_INVALIDEXPRESSION:
 *    - CHI BAT 1 LAN DUY NHAT o compileExpression2()
 *    - Khi token DAU TIEN khong phai: IDENT, NUMBER, CHAR, LPAR, PLUS, MINUS
 *    Vi du: x := FOR
 * 
 * 2. ERR_INVALIDTERM:
 *    - CHI BAT SAU * hoac /
 *    - Vi * va / thuoc Term
 *    Vi du: x := 5 * FOR
 *          x := 10 / WHILE
 * 
 * 3. ERR_INVALIDFACTOR:
 *    - TAT CA cac truong hop con lai
 *    - Sau + hoac -
 *    - Ben trong ()
 *    - Token khong thuoc FOLLOW set
 *    Vi du: x := 5 + FOR
 *          x := (FOR)
 *          x := 5 WHILE
 */

void compileExpression(void) {
  assert("Parsing an expression");
  
  switch (lookAhead->tokenType) {
  case SB_PLUS:
  case SB_MINUS:
  case TK_IDENT:
  case TK_NUMBER:
  case TK_CHAR:
  case SB_LPAR:
    // Token đầu hợp lệ -> tiếp tục
    compileExpression2();
    break;
    
  default:
    // Token đầu KHÔNG hợp lệ -> ERR_INVALIDEXPRESSION
    error(ERR_INVALIDEXPRESSION, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
  
  assert("Expression parsed");
}

/*
 * compileExpression2() - Xử lý dấu +/- ở đầu
 * SAU khi qua hàm này thì ĐÃ KHÔNG còn bắt ERR_INVALIDEXPRESSION nữa
 */
void compileExpression2(void) {
  switch (lookAhead->tokenType) {
  case SB_PLUS:
    eat(SB_PLUS);
    // SAU + -> nếu sai thì phải là INVALIDFACTOR (đã qua token đầu)
    compileTerm();
    compileExpression3();
    break;
    
  case SB_MINUS:
    eat(SB_MINUS);
    // SAU - -> nếu sai thì phải là INVALIDFACTOR (đã qua token đầu)
    compileTerm();
    compileExpression3();
    break;
    
  default:
    // Không có dấu +/- -> bắt đầu bằng term luôn
    compileTerm();
    compileExpression3();
    break;
  }
}

/*
 * compileExpression3() - Xu ly + -
 * Sau + hoac - ma sai -> BAT ERR_INVALIDFACTOR
 */
void compileExpression3(void) {
  switch (lookAhead->tokenType) {
  case SB_PLUS:
    eat(SB_PLUS);
    // SAU + -> goi thang compileTerm()
    // Neu sai -> de Term/Factor tu bat
    compileTerm();
    compileExpression3();
    break;
    
  case SB_MINUS:
    eat(SB_MINUS);
    // SAU - -> goi thang compileTerm()
    compileTerm();
    compileExpression3();
    break;
    
  // FOLLOW set cua Expression
  case KW_TO:
  case KW_DO:
  case SB_RPAR:
  case SB_COMMA:
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:
  case SB_RSEL:
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
  case KW_THEN:
    break;
    
  default:
    // Token khong thuoc FOLLOW -> INVALIDFACTOR
    error(ERR_INVALIDFACTOR, lookAhead->lineNo, lookAhead->colNo);
  }
}

// ============= HAM XU LY TERM =============

/*
 * compileTerm() - Kiem tra token bat dau term
 * Neu sai -> bat INVALIDFACTOR (vi da qua Expression roi)
 */
void compileTerm(void) {
  switch (lookAhead->tokenType) {
  case TK_IDENT:
  case TK_NUMBER:
  case TK_CHAR:
  case SB_LPAR:
    compileFactor();
    compileTerm2();
    break;
    
  default:
    // Token khong the bat dau term -> INVALIDFACTOR
    // (KHONG phai INVALIDTERM vi chua vao * / gi ca)
    error(ERR_INVALIDFACTOR, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}

/*
 * compileTerm2() - Xu ly * /
 * SAU * hoac / ma sai -> BAT ERR_INVALIDTERM
 * Day la noi DUY NHAT bat INVALIDTERM
 */
void compileTerm2(void) {
  switch (lookAhead->tokenType) {
  case SB_TIMES:
    eat(SB_TIMES);
    // SAU * -> kiem tra token
    switch (lookAhead->tokenType) {
    case TK_IDENT:
    case TK_NUMBER:
    case TK_CHAR:
    case SB_LPAR:
      compileFactor();
      compileTerm2();
      break;
    default:
      // SAU * sai -> BAT INVALIDTERM
      error(ERR_INVALIDTERM, lookAhead->lineNo, lookAhead->colNo);
      break;
    }
    break;
    
  case SB_SLASH:
    eat(SB_SLASH);
    // SAU / -> kiem tra token
    switch (lookAhead->tokenType) {
    case TK_IDENT:
    case TK_NUMBER:
    case TK_CHAR:
    case SB_LPAR:
      compileFactor();
      compileTerm2();
      break;
    default:
      // SAU / sai -> BAT INVALIDTERM
      error(ERR_INVALIDTERM, lookAhead->lineNo, lookAhead->colNo);
      break;
    }
    break;
    
  // FOLLOW set cua Term
  case SB_PLUS:
  case SB_MINUS:
  case KW_TO:
  case KW_DO:
  case SB_RPAR:
  case SB_COMMA:
  case SB_EQ:
  case SB_NEQ:
  case SB_LE:
  case SB_LT:
  case SB_GE:
  case SB_GT:
  case SB_RSEL:
  case SB_SEMICOLON:
  case KW_END:
  case KW_ELSE:
  case KW_THEN:
    break;
    
  default:
    // Token khong thuoc FOLLOW -> INVALIDFACTOR
    error(ERR_INVALIDFACTOR, lookAhead->lineNo, lookAhead->colNo);
  }
}

// ============= HAM XU LY FACTOR =============

void compileFactor(void) {
  switch (lookAhead->tokenType) {
    
  case TK_NUMBER:
    eat(TK_NUMBER);
    break;
    
  case TK_CHAR:
    eat(TK_CHAR);
    break;
    
  case TK_IDENT:
    eat(TK_IDENT);
    
    switch (lookAhead->tokenType) {
    case SB_LSEL:
      compileIndexes();
      break;
      
    case SB_LPAR:
      compileArguments();
      break;
      
    default:
      break;
    }
    break;
    
  case SB_LPAR:
    eat(SB_LPAR);
    compileExpression();
    // // Goi THANG compileTerm() de day het xuong INVALIDFACTOR
    // compileTerm();
    // compileExpression3();
    eat(SB_RPAR);
    break;
    
  default:
    // Token khong hop le -> INVALIDFACTOR
    error(ERR_INVALIDFACTOR, lookAhead->lineNo, lookAhead->colNo);
    break;
  }
}


// ============= HAM XU LY INDEXES (CHI SO MANG) =============

/*
 * Ham compileIndexes() - BIEN DICH CHI SO MANG
 * Xu ly 1 hoac nhieu chi so cho mang nhieu chieu
 * Moi chi so la 1 bieu thuc nam trong [ ]
 * 
 * Vi du:
 *   a[5]            <- mang 1 chieu, chi so co dinh
 *   a[i]            <- mang 1 chieu, chi so bien
 *   a[i+1]          <- mang 1 chieu, chi so la bieu thuc
 *   matrix[i][j]    <- mang 2 chieu
 *   cube[x][y][z]   <- mang 3 chieu
 *   a[b[i]]         <- chi so phuc tap (mang trong mang)
 */
void compileIndexes(void) {
  // Lap lai trong khi con gap dau [ (bat dau cua index)
  while (lookAhead->tokenType == SB_LSEL) {
    eat(SB_LSEL);                    // Dau [ (mo)
    compileExpression();             // Chi so (co the la bieu thuc phuc tap)
    eat(SB_RSEL);                    // Dau ] (dong)
  }
  // Khi khong con gap [ thi dung lai
}

// ============= HAM CHINH - KHOI DONG PARSER =============

/*
 * Ham compile() - HAM KHOI DONG PARSER
 * Day la ham duoc goi tu main() de bat dau qua trinh bien dich
 * 
 * Cac buoc:
 *   1. Mo file nguon
 *   2. Lay token dau tien
 *   3. Bat dau parse tu compileProgram()
 *   4. Giai phong bo nho
 *   5. Dong file
 * 
 * Tham so:
 *   fileName - ten file can bien dich (VD: "example1.kpl")
 * 
 * Gia tri tra ve:
 *   IO_SUCCESS - thanh cong
 *   IO_ERROR - loi (khong mo duoc file)
 */
int compile(char *fileName) {
  // Buoc 1: Mo file nguon de doc
  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;                 // Loi: khong mo duoc file

  // Buoc 2: Khoi tao cac token
  currentToken = NULL;               // Chua co token nao
  lookAhead = getValidToken();       // Lay token dau tien tu scanner
  
  // Buoc 3: Bat dau parse chuong trinh
  compileProgram();                  // Parse toan bo chuong trinh
  
  // Buoc 4: Giai phong bo nho
  free(currentToken);                // Giai phong token cuoi cung
  free(lookAhead);                   // Giai phong lookAhead
  
  // Buoc 5: Dong file
  closeInputStream();                // Dong file nguon
  
  return IO_SUCCESS;                 // Thanh cong!
}

/*
 * ============= TOM TAT QUA TRINH PARSER =============
 * 
 * 1. Scanner tao ra chuoi tokens tu source code
 *    Source: "PROGRAM Example1;"
 *    Tokens: [PROGRAM] [Example1] [;]
 * 
 * 2. Parser kiem tra cu phap bang cach:
 *    - Dung lookAhead de xem token ke tiep
 *    - Dung eat() de "an" token neu dung
 *    - Goi cac ham compile...() tuong ung voi cu phap
 * 
 * 3. Cau truc de quy:
 *    compileProgram()
 *      -> compileBlock()
 *          -> compileBlock2()
 *              -> compileBlock3()
 *                  -> compileBlock4()
 *                      -> compileBlock5()
 *                          -> compileStatements()
 *                              -> compileStatement()
 *                                  -> compileExpression()
 *                                      -> compileTerm()
 *                                          -> compileFactor()
 * 
 * 4. Khi gap loi:
 *    - Thieu token: missingToken()
 *    - Token khong hop le: error(ERR_...)
 * 
 * 5. Ket qua:
 *    - Thanh cong: "Program parsed!"
 *    - That bai: In loi va thoat
 * 
 * ============= CAC KHAI NIEM QUAN TRONG =============
 * 
 * - Token: Don vi tu vung nho nhat (keyword, identifier, symbol,...)
 * - eat(): Kiem tra va tieu thu token
 * - scan(): Di chuyen sang token ke tiep
 * - lookAhead: Xem truoc token ke tiep de quyet dinh
 * - Recursive Descent: Ky thuat parse de quy tu tren xuong
 * - FOLLOW set: Tap cac token co the xuat hien sau mot cau truc
 * 
 * ============= VI DU PHAN TICH =============
 * 
 * Code: x := 5 + 3;
 * 
 * Tokens: [x] [:=] [5] [+] [3] [;]
 * 
 * Parse:
 *   compileStatement()
 *     -> compileAssignSt()
 *         eat(x)                   // An "x"
 *         eat(:=)                  // An ":="
 *         -> compileExpression()
 *             -> compileExpression2()
 *                 -> compileTerm()
 *                     -> compileFactor()
 *                         eat(5)   // An "5"
 *                 -> compileExpression3()
 *                     eat(+)       // An "+"
 *                     -> compileTerm()
 *                         -> compileFactor()
 *                             eat(3)  // An "3"
 * 
 * Ket qua: Cu phap dung!
 */