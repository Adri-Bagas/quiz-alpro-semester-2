# 📚 Bookstore Management App

A simple, interactive terminal application to manage a bookstore. Written in **C++** using **Ncurses**. All data is automatically saved to JSON files.

## ✨ Features
* **Inventory:** Add, edit, search, and delete books.
* **Shopping Cart:** Add books to a cart and check out (stock updates automatically).
* **Reports:** View transaction history and see the Top 5 Best Selling books.
* **Currency Toggle:** Quickly switch prices between USD ($) and IDR (Rp).

## 🚀 How to Run
### 🐧 Linux / Mac / WSL (Windows Subsystem for Linux)
1. Install the required tools:
```
cmake ncurses
```

2. Build the app and run it:

```bash
cmake -B build
cmake --build build
./build/bookstore
```

## ⌨️ Controls

* **Arrow Keys (Up/Down)**: Move up and down lists.

* **Arrow Keys (Left/Right)**: Switch between menu actions (e.g., Edit, Delete).

* **Enter**: Select an option or submit text.

* **ESC**: Go back or cancel an action.