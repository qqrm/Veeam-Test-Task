# unsuccessful Veeam-Test-Task
task in task.txt

exe in release folder

clang, cmake, vscode, vcpkg 

feedback:
Возвращаюсь к Вам с обратной связью по задаче, обнаружились некоторые недочёты в решении:
- Нет обработки ошибок в потоках, любое исключение завалит приложение.
- Для хранения результатов используется преаллоцированный буфер, как следствие - потенциально большой расход по памяти. 
- Работа с файлами сделана на memory mapped files, что не является правильным решением. Ожидалось самобалансирующееся решение на базе Producer-Consumer с входными/выходными очередями блоков данных.

1 - не уверен

2 - согласен

3 - todo




