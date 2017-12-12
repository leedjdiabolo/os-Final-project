# os-Final-project

Final Project of OS

----

### 我是題目

g++ 編譯完 main.cpp 後，你可以透過 “./a.out” 來啟動 server，預設 server port = 8000

要更換 server port的話，可以用 “./a.out XXX” 來設定 （XXX 指你想要的 port number）

大家的程式進入點在 “start_while_loop_for_accept_input（）” 這個function，可以參考 pwd()（大家一起加油xD ）

目前啟動server後，會進入 "user_profile/" 中 ( 因為 chdir("user_profile/") ),

當中的 “bin/” 是各個指令的執行檔 （等之後換多人模式時，再統一指令位置）

所以可以的話，請把你的指令執行檔放到其中的 "bin/" 中。

(提醒：如果有換系統，請把你的指令執行檔案重新編譯過喔)

---

#### 個人偷懶

你可以直接下 “sh sr.sh” 來編譯跟啟動server，預設port = 8000

PS：如果你要用 sr.sh ,而另外又 include 自己寫的 header cpp，請記得自己加東西喔
