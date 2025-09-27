# SimpleBashUtils — `cat` и `grep` на C

Реализация утилит командной строки **cat** и **grep** на C.  
Сборка выполняется с помощью **Makefile**.

---

## Сборка

```bash
cd src
make all
```

После сборки в подкаталогах появятся два исполняемых файла:

- `cat/s21_cat`
- `grep/s21_grep`

---

## Запуск

### `cat`
```bash
cd src/cat
./s21_cat "имя_файла"
```

### `grep`
```bash
cd src/grep
./s21_grep [флаг] "шаблон" [файл]
# пример:
./s21_grep -i "school" test_grep.txt
```
