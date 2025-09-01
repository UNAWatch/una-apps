import os
import re
import shutil
from pathlib import Path

# 🔹 Константи
SDK_MACRO = "$(UnaSdkPath)"
SDK_PATTERN = re.compile(r'(<ClInclude Include=")([^"]*\\SDK\\)([^"]*)(")')  # Знаходимо Include="...SDK..."

# Отримуємо поточну папку, де знаходиться скрипт
CURRENT_DIR = Path(__file__).parent

# Знаходимо файли *.vcxproj та *.vcxproj.filters
vcxproj_files = list(CURRENT_DIR.glob("*.vcxproj"))
filters_files = list(CURRENT_DIR.glob("*.vcxproj.filters"))


def backup_file(file_path):
    """Робить резервну копію перед змінами"""
    backup_path = file_path.with_suffix(file_path.suffix + ".bak")
    shutil.copy(file_path, backup_path)


def replace_paths_in_file(file_path):
    """Замінює тільки шляхи в `Include=""`, не чіпаючи `<Filter>`"""
    with open(file_path, "r", encoding="utf-8") as f:
        content = f.read()

    # Заміна шляхів у атрибутах Include="", не чіпаючи фільтри
    updated_content = SDK_PATTERN.sub(lambda m: f'{m.group(1)}{SDK_MACRO}\\{m.group(3)}{m.group(4)}', content)

    if updated_content != content:
        backup_file(file_path)  # Створюємо резервну копію
        with open(file_path, "w", encoding="utf-8") as f:
            f.write(updated_content)
        print(f"Оновлено: {file_path.name}")
    else:
        print(f"ℹЗмін не потрібно: {file_path.name}")


if __name__ == "__main__":
    for file in vcxproj_files + filters_files:
        replace_paths_in_file(file)

    print("Оновлення завершено! Відкрийте Visual Studio.")
