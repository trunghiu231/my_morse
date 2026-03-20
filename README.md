# my_morse — Morse Code Encoder & Decoder

Chương trình mã hóa và giải mã mã Morse viết bằng ngôn ngữ C, chạy trên môi trường Linux.

> **Bài thi tuyển dụng:** Newbie Enrollment Examination 2026 — Phòng PMHT - VTBR – VHT

---

## Mục lục

- [Yêu cầu hệ thống](#yêu-cầu-hệ-thống)
- [Cài đặt & Build](#cài-đặt--build)
- [Cách sử dụng](#cách-sử-dụng)
- [Quy ước ký hiệu Morse](#quy-ước-ký-hiệu-morse)
- [Cấu trúc thư mục](#cấu-trúc-thư-mục)
- [Cấu trúc mã nguồn](#cấu-trúc-mã-nguồn)
- [Ví dụ](#ví-dụ)

---

## Yêu cầu hệ thống

- Hệ điều hành: Linux (Ubuntu 24.04 hoặc tương đương)
- Compiler: `gcc`
- Build tool: `make`

```bash
sudo apt install gcc make -y
```

---

## Cài đặt & Build

```bash
# Clone repository
git clone https://github.com/<your-username>/my_morse.git
cd my_morse

# Build chương trình
make

# Dọn dẹp file object sau khi build
make clean
```

---

## Cách sử dụng

```bash
# Mã hóa (Encode): đọc file văn bản, xuất ra mã Morse
./my_morse -e <input_file>

# Giải mã (Decode): đọc file mã Morse, xuất ra văn bản
./my_morse -d <input_file>
```

**Kết quả:**
- In tối đa **20 ký tự đầu tiên** ra màn hình
- Lưu toàn bộ kết quả vào file:
  - Encode: `<tên_file>_encoded.txt`
  - Decode: `<tên_file>_decoded.txt`

---

## Quy ước ký hiệu Morse

| Ký hiệu Morse | Biểu diễn trong chương trình |
|---|---|
| Dot (chấm `.`) | `+` |
| Dash (gạch `-`) | `===` |
| Phân cách ký tự | ` ` (một dấu cách) |

**Ví dụ theo đề bài:**

```
+=== ===+++ ===+===+   →   A B C
```

Chi tiết bảng mã:

```
A: +===        B: ===+++      C: ===+===+    D: ===++
E: +           F: ++===+      G: ======+     H: ++++
I: ++          J: +=========  K: ===+===     L: +===++
M: ======      N: ===+        O: =========   P: +======+
Q: ======+===  R: +===+       S: +++         T: ===
U: ++===       V: +++===      W: +======     X: ===++===
Y: ===+======  Z: ======++

0: ===============   1: +============   2: ++=========
3: +++======         4: ++++===         5: +++++
6: ===++++           7: ======+++       8: =========++
9: =============+
```

---

## Cấu trúc thư mục

```
my_morse/
│
├── main.c                  # Điểm vào chương trình, xử lý tham số -e/-d
├── morse.c                 # Logic encode, decode, validate và bảng mã Morse
├── morse.h                 # Header khai báo hàm và hằng số
├── file_utils.c            # Tiện ích đọc/ghi file, in giới hạn 20 ký tự
├── file_utils.h            # Header file_utils
├── Makefile                # Build system
├── .gitignore
├── README.md
│
├── Detailed Test Evidence/ # Các file sinh ra trong quá trình kiểm thử
│   ├── alpha.txt
│   ├── alpha_encoded.txt
│   ├── alpha_encoded_decoded.txt
│   ├── bad.txt
│   ├── hw.txt
│   ├── hw_encoded.txt
│   ├── hw_encoded_decoded.txt
│   ├── input.txt
│   ├── t.txt
│   └── t_encoded.txt
│
└── docs/                   # Tài liệu dự án
    ├── my_morse_design_document.pdf    # Tài liệu mô tả thiết kế
    ├── my_morse_test_report.pdf        # Kết quả kiểm thử
    ├── my_morse_test_scenarios.pdf     # Kịch bản kiểm thử
    └── Newbie_Enrollment_Examination_Morse_Code_2026.md  # Đề bài gốc
```

---

## Cấu trúc mã nguồn

### `main.c`
Điểm vào chương trình. Phân tích tham số dòng lệnh (`-e` hoặc `-d`), điều phối luồng encode/decode, in kết quả ra màn hình và lưu vào file.

### `morse.c` / `morse.h`
Module core chứa toàn bộ logic xử lý:

| Hàm | Mô tả |
|---|---|
| `char_to_morse(c)` | Tra bảng: ký tự → chuỗi Morse tùy chỉnh |
| `morse_to_char(token)` | Tra bảng: token Morse → ký tự |
| `parse_token_to_std(token, out)` | Chuyển token tùy chỉnh (`+`, `===`) sang Morse chuẩn (`.`, `-`) |
| `validate_morse(input)` | Kiểm tra chuỗi Morse đầu vào hợp lệ |
| `encode(input, output, size)` | Mã hóa văn bản → Morse |
| `decode(input, output, size)` | Giải mã Morse → văn bản |

### `file_utils.c` / `file_utils.h`
Tiện ích I/O:

| Hàm | Mô tả |
|---|---|
| `read_file(path, buf, size)` | Đọc nội dung file vào buffer |
| `write_file(path, content)` | Ghi chuỗi ra file |
| `print_limited(str, max)` | In tối đa `max` ký tự, hiển thị thông báo nếu bị cắt |

---

## Ví dụ

### Encode

```bash
echo "Hello World" > input.txt
./my_morse -e input.txt
```

```
[INFO]   Mode       : ENCODE
[INFO]   Input file : input.txt
[INFO]   Input text : Hello World
[RESULT] Morse output (first 20 chars):
++++ + +===++ +===++
[... truncated. Full result saved to output file.]
[INFO]   Output saved: input_encoded.txt
```

### Decode

```bash
./my_morse -d input_encoded.txt
```

```
[INFO]   Mode        : DECODE
[INFO]   Input file  : input_encoded.txt
[INFO]   Validation  : PASSED
[RESULT] Decoded text (first 20 chars):
HELLOWORLD
[INFO]   Output saved: input_encoded_decoded.txt
```

### Validate lỗi

```bash
echo "abc!!!" > bad.txt
./my_morse -d bad.txt
```

```
[VALIDATOR] Ky tu khong hop le 'a' tai vi tri 0.
[ERROR] Input is NOT a valid Morse string.
```