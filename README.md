# os-Final-project

Final Project of OS

----

### 重點摘要

請使用 “make” 指令來進行編譯，會產出 server.o

編譯完後，你可以透過 “./server.o” 來啟動 server，預設 server port = 8000

要更換 server port的話，可以用 “./server.o XXX” 來設定 （XXX 指你想要的 port number）

大家的程式進入點在 “start_while_loop_for_accept_input（）” 這個function，可以參考 pwd,ls,cat 等指令的部分 （大家一起加油xD ）

目前啟動server後，會進入 "user_profile/" 中 ( 因為 chdir("user_profile/") ),

當中的 “bin/” 是各個指令的執行檔 （等之後換多人模式時，再統一指令位置）

所以可以的話，請把你的指令執行檔放到其中的 "bin/" 中。

(提醒：如果有換系統，請把你的指令執行檔案重新編譯過喔)
