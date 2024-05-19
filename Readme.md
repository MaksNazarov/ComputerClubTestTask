В этом частном репозитории лежит решение тестового задания на вакансию инженера по разработке ПО для базовых станций (С++).

При решении использовался стандарт С++20, в частности появившийся там метод contains для некоторых классов STL. 

Команда сборки решения под gcc:
g++ -std=gnu++20 -o task.exe main.cpp club.cpp util.cpp

Был реализован основной класс Club с методами, отвечающими входящим событиям, а также несколько служебных классов. С учётом формулировки задачи было принято решение не реализовывать выходящее событие 13 - "Ошибка" - как полноценный класс исключений и ограничиться логами происходящего. Для парсинга входного файла использованы getline и istringstream.