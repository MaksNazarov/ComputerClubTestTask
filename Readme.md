В этом публичном репозитории лежит решение некоторого тестового задания.

При решении использовался стандарт С++20, в частности появившийся там метод contains для некоторых классов STL. 

Команда сборки решения под gcc:
g++ -std=gnu++20 -o task.exe main.cpp club.cpp util.cpp

Был реализован основной класс Club с методами, отвечающими входящим событиям, а также несколько служебных классов. С учётом формулировки задачи было принято решение не реализовывать выходящее событие "Ошибка" как полноценный класс исключений и ограничиться логами происходящего. Для парсинга входного файла использованы getline и istringstream.