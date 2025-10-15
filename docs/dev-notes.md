# 開發心得：Todo CLI 小工具

## 開發動機

為了熟悉 C 語言實作、練習檔案處理與字串解析技巧，並同時製作一個可以放進履歷作品集的 CLI 工具，我設計並實作了一個命令列代辦清單工具 `todo-cli`。

這個工具支援：
- 新增代辦事項
- 完成事項標記
- 刪除代辦項目
- 清單列出（包含完成狀態）
- 清除已完成事項

## 使用說明

```bash
make
./todo add "買牛奶"
./todo done 1
./todo delete 2
./todo list
./todo clear
```

## 技術細節

### 使用語言與工具

- C (標準 C11)
- `fopen` / `fgets` / `strtok` 等標準 C 檔案與字串處理
- 自訂結構 `TodoItem` 來包裝每筆代辦
- Makefile 自動編譯（簡化版）

### 資料格式設計

每一行代辦事項以 `id|title|done` 儲存在文字檔 `todo.txt` 中，例如：

    1|買牛奶|0
    2|完成 side project|1

透過 `strtok()` 做欄位解析，再結構化處理。

### 程式設計風格

- 使用模組化架構（`todo.c`, `todo.h`）將邏輯與主程式分離
- 抽出通用小函式：如 `read_all_lines()`、`parse_line()`、`write_all_items()` 提高重用性
- 每個 CLI 指令（如 `add_todo()`、`delete_todo()`）為獨立函式，主程式只做指令解析與呼叫


## 開發過程重點

- 初期先寫出最小可執行版本（MVP），快速驗證流程
- 之後開始重構重複邏輯，逐步拆解小函式
- 為了避免出現「魔法數字」，使用 `MAX_ITEMS`、`MAX_TITLE_LEN` 等常數統一管理
- 使用 `strncpy()`、長度檢查、特殊字元過濾，確保檔案資料格式安全性
- 嘗試使用 Makefile 的萬用字元與路徑轉換，但為了先成功執行，改為簡化版 Makefile


## 錯誤處理策略

為了讓 `todo.txt` 檔案損壞時能友善顯示錯誤、不中斷 CLI 工具，
我將 `parse_line()` 中的 `atoi()` 改成 `safe_atoi()`，並加入欄位級別的錯誤訊息。

`strtol()` 無法處理換行符號 `\r` 因此在輸入之前先使用 `strip_newline()` 清除尾部換行符號。

這樣一來即使格式異常（如：欄位缺失、非整數字串），也能清楚知道是哪一欄出錯。


## 學習目標

- 熟悉 C 語言中檔案處理、字串切割、錯誤處理邏輯
- 用 C 寫純 CLI 工具，有自訂參數處理（用 getopt 或 argp）
- 學會怎麼拆出有重複性的函式、避免邏輯散落在多個地方
- Makefile 編寫技巧（簡單與進階的路徑處理）
- 如何從最簡的程式逐步整理成乾淨、模組化、可維護的架構


## 未來可改進 improve

- 支援代辦事項的截止日期與分類
- 將檔案格式改為 JSON 或 markdown 儲存
- 改為跨平台 GUI 工具（以 Python / Electron 為主）
- parse_line 讀取到的字串可能會超過 MAX_TITLE_LEN
- 清單項目數量會超過 MAX_ITEMS
- 參數可以接受多個指令依序執行
- 增加支援儲存目錄或檔案參數（例如：自訂儲存檔名）


## 檔案結構簡介

```
.
├── docs
│   └── dev-notes.md # 開發紀錄與心得（此檔案）
├── Makefile
├── README.md
├── src
│   ├── main.c # 主程式：處理 CLI 指令
│   ├── todo.c # 邏輯實作（add, delete, done, list, etc）
│   └── todo.h # 函式宣告與結構定義
├── test
│   ├── test.cpp # 測試用檔案
│   └── test.sh # shell 測試腳本
└── todo.txt # 資料儲存檔案
```


## 更新紀錄

### 2025/06/23

- 設計 TodoItem 結構（id, title, done 狀態）
- 建立基本 CLI 架構（main() + argc/argv 解析）
- 加入 add_todo 功能
- 支援 todo list（顯示所有項目）
    - 讀取空檔案要處理
    - todo list 是否排序正確、顯示正常？
    - 檔案不存在時會不會 crash？會說沒有代辦事項
- 儲存資料到本地檔案（todo.txt，每行一筆）
- 程式啟動時載入檔案

### 2025/06/26

- 加入 todo remove 2（刪除第 2 項）
    - 如何只修改一行，還有要只改 id
    - 步驟：讀取、修改、儲存
    - 直接存成新的
- add_todo 功能修正
    - 輸入空字串：todo add ""
    - 輸入太長的事項
    - 不能包含非可見字元以及`|`和`\n`
    - `title[i] < 32 || title[i] > 126`，只能用在英文
    - 改了先關讀檔，在開寫檔
    - 修改錯誤輸出 

### 2025/06/27

- todo list 功能修正
    - 讀取空檔案要處理
    - todo list 是否排序正確、顯示正常？
    - 檔案不存在時會不會 crash？會說沒有代辦事項
- delete todo 功能修正
    - id 輸入 0 或 負數：新增範圍偵測
    - 修改錯誤輸出
    - title 先另外取出放沒錯
        - 如果未來修改邏輯（例如不小心沒跳過那筆）
        - 有可能 strtok() 會破壞那一行，保險作法：先複製一份
        - 預測潛在風險

### 2025/07/05

- 修正 add_todo, delete_todo
- 加入 todo done
    - title 先另外取出放沒錯
    - 早已完成的也 OK

### 2025/07/09

- 重構程式碼
- 功能模組化
- 使用工具函式 utils
- 檔案結構分開，了解架構安排
- 已經 include stdio.h 那就會間接 include
- 專案資料夾結構重構
- 寫 README.md
- Makefile 簡化編譯流程？
- 完成 main.c 的基本 CLI 主程式

### 2025/07/10

- 建立 docs/dev-notes.md，用來寫作品集「開發心得」、「設計架構圖」
- 撰寫一個簡單的 shell script: test.sh，驗證 CLI 功能是否正常
- 讀取檔案名稱的邏輯，改寫成支援環境變數

### 2025/07/11

- 改進錯誤處理與輸入驗證（像是 strtol 取代 atoi）
    - atoi 是不安全的轉換方式，它無法檢查錯誤
    - 轉換失敗會回傳 0，無法區分錯誤與有效結果

### 2025/07/29

- 加入 getopt() 處理參數，支援 -a、-d、-m、-l，可與原本的 add、delete 等子命令並行使用
- 增加 print_usage() 提示錯誤參數輸入(使用 -h)
- 程式會在啟動時自動建立 todo.txt（若不存在）

### 2025/08/08

- 加入 clear_todos() 清理已完成的事項
- README.md 加入英文版
- 重構 read_all_lines，使用較直覺也方便測試

### 2025/09/15

感覺使用上應該要更直覺一點，互動式的看板那樣，弄一個新的架構看看：

#### 基本功能（MVP）
1. add
    - `add "task"` -> 加到 To-Do
    - `add-sub <task_id> "subTask"` -> 在某主項目下新增子任務
2. move
    - `move <task_id> in-progress`
    - `move <task_id> done`
3. check
    - `check <task_id>` -> 勾/反勾，如果是主項目，整個任務 Done
    - `done-sub <task_id> <sub_id>` -> 只完成子項目
4. toggle
    - `toggle <task_id>` -> 切換 `is_collapsed`
5. note
    - `note "content"` -> 新增到 Notes

#### 顯示

輸出時分區塊顯示，子項目依照是否折疊決定是否展開。

主進度條公式：
```bash
完成度 = （完成子項目數 / 總子項目數）* 100
```

CLI進度條顯示：
```bash
[====>     ] 40%
```
- 使用 `=` 與空格填充
- 可以用長度固定 10 格

---

我改了架構之後，會影響到原本的功能，如何將影響化為最小？

### 2025/10/07

#### 資料結構

因為有巢狀結構，使用 cJSON 結構來管理資料

```JSON
{
  "todos": [
    {
      "id": 1,
      "title": "學習 C 語言",
      "status": "todo",
      "is_collapsed": false,
      "subtasks": [
        {"title": "指標練習", "status": "done"},
        {"title": "檔案 I/O", "status": "todo"},
        {"title": "結構體", "status": "todo"},
        {"title": "動態記憶體", "status": "done"}
      ]
    }
  ],
  "in_progress": [
    {
      "id": 2,
      "title": "撰寫 todo-cli",
      "status": "in_progress",
      "subtasks": []
    }
  ],
  "done": [
    {"id": 3, "title": "安裝 Linux", "status": "done"}
  ],
  "notes": [
    "嘗試加入 TUI",
    "CLI 完成後再進階"
  ]
}
```

#### 使用 enum 有什麼優點？

- 程式結構化
- TaskStatus 變數只能是三種狀態，switch case 會檢查
- 之後排序、統計使用較方便
