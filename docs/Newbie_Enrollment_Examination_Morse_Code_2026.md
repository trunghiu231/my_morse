# Morse Code: Encoding & Decoding

```
Phòng PMHT - VTBR – VHT
```
## Đề bài:

Viết một chương trình mã hóa và giải mã mã Morse.

## Yêu cầu chi tiết:

```
▪ Chức năng:
o Thực hiện được cả hai chức năng giải mã và mã hóa mã Morse (định nghĩa về mã
Morse trên Wiki).
o Kí tự "+" được là tự "dot", "===" là "dash" và " " (khoảng trắng - space) cho "space".
o Đối với chức năng Decoding, cần xác định được một chuỗi mã hóa Morse có được
gửi đúng chuẩn hay không.
▪ Phi chức năng:
o Chương trình chạy trên môi trường Linux và sử dụng ngôn ngữ lập trình C.
o Có sử dụng Makefile để tiến hành compile.
o Tên chương trình là: my_morse.
o Tham số điều khiển chương trình được đưa vào từ Terminal.
Ví dụ:
Encode một file: my_morse -e <input file directory>
Decode một file: my_more -d <input file directory>
o Kết quả encode/decode được đưa in ra màn hình và lưu vào file. Nếu số lượng kí tự
đầu ra lớn hơn 20 thì chỉ tiến hành in 20 kí tự đầu tiên ra màn hình.
▪ Input/ouput:
o Đối với mã hóa
▪ Đầu vào cho mã hóa là chuỗi các kí tự A->Z, a->z và 0->9 được lưu trữ ở một
file ở dạng text. Độ dài tối đa của chuỗi kí tự là 10.000.000.
Ví dụ như: "Viettel Network Technologies Center 2018"
▪ Đầu ra là chuỗi kí tự đã được mã hóa.
o Đối với giải mã:
▪ Đầu vào là chuỗi kí tự gồm các kí tự: "+", "===" và khoảng trắng, được lưu
trữ ở dạng một file text. Độ dài tối đa của chuỗi kí tự là 100.
Ví dụ như: +=== ===+++ ===+===+ (là chuỗi Morse cho chuỗi kí tự: A B C)
```
## Yêu cầu về kết quả cần gửi

```
▪ Source code, Makefile lưu trữ trên github cá nhân
▪ Kết quả test, test report
▪ Tài liệu mô tả thiết kế và kịch bản kiểm thử (ở dạng pdf)
```

