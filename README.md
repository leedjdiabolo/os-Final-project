# os-Final-project

Final Project of OS

----

### 重點摘要

請使用 “make” 指令來進行編譯，會產出 server.o

編譯完後，你可以透過 “./server.o” 來啟動 server，預設 server port = 8000

要更換 server port的話，可以用 “./server.o XXX” 來設定 （XXX 指你想要的 port number）

大家的程式進入點在 “start_while_loop_for_accept_input（）” 這個function，可以參考 pwd,ls,cat 等指令的部分 （大家一起加油xD ）

啟動server後，會進入 "/home/你登入的帳號的家目錄/" 中，也就是user的家目錄 ( 因為 chdir() function 的關係 ),

執行檔的文件夾 “bin/” 已經移動到 /tmp/ 中了 （要添加指令執行檔的人，記得加到 /tmp/bin/ 中喔 ）

(提醒：如果有換系統，請把你的指令執行檔案重新編譯過喔)

---

### v1.0.0

完整版本

cgi 與 httpserver 兩個文件夾是CGI ，也就是 API 的程式部分

os-project-html 是 GUI，也就是網頁的程式部分

目前可用指令：

1）簡單指令：pwd, ls, cat, mv, touch, cp, mkdir

2) 刪除，恢復指令：rm, recover

3）解壓縮指令：compress，extract

4）顯示隱藏指令：hide，show

5) 搜索指令：search

6) 移動指令: cd (可加強)

7）空間查詢指令： space

8）列印指令： echo

9）功能查詢指令： help

10）退出指令：exit

投影片網址：https://drive.google.com/open?id=1RIvuOKf9eUzM3DNupOSxAsM4CTZC0sIIqalIkraXmUs

---

### v0.3.1

更新 merge 出問題的情況：

調整 help 跟 unknown command 的顯示

---

### v0.3.0

目前可用指令：

1）簡單指令：pwd, ls, cat, mv, touch, cp mkdir

2) 刪除，恢復指令：rm, recover

3）解壓縮指令：compress，extract

4）顯示隱藏指令：hide，show

5) 搜索指令：search

6) 移動指令: cd (可加強)

7）空間查詢指令： space

8）列印指令： echo

9）功能查詢指令： help

10）退出指令：exit

---

### v0.2.0

新增內容：

1）/bin 移動到 /tmp 中

2）登陸後，會進入 user 的家目錄

增加指令：

mkdir，cd

---

### v0.1.0

需要登陸才能使用指令

登陸帳號為ubuntu上的任何一個帳號跟密碼

目前可用指令：

1）簡單指令：cat，cp，ls，mv，pwd，rm，touch

2）解壓縮指令：compress，extract

3）顯示隱藏指令：hide，show

4) 搜索指令：search （還未驗證）

5）退出指令：exit

可再強化的指令 ？

1）cd 的返回指令 (../) 路徑沒有完全擋完

2)hide,show 指令，或許可以加 --list 的參數，存粹顯示被隱藏的文件

3）大家的指令，應該都沒有做到權限的控管（比如 ls /,根目錄會看光光）