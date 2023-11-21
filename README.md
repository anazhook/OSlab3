# OSlab3
Operation Systems lab №3
Лабораторная работа No3.
Тема: «Синхронизация потоков при помощи критических секций и событий. Обработка тупиков».

Задача. Написать программу для консольного процесса, который состоит из потока main и
нескольких экземпляров потока marker.
Поток main должен выполнять следующие действия:
1. Захватить память под массив целых чисел, размерность которого вводится с консоли.
2. Инициализировать элементы массива нулями.
3. Запросить количество потоков marker, которые требуется запустить.
4. Запустить заданное количество экземпляров потока marker. В качестве параметра
каждому экземпляру потока marker передать его порядковый номер в запуске.
5. Дать сигнал на начало работы всех потоков marker.
6. Выполнять в цикле следующие действия:
6.1. Ждать, пока все потоки marker не подадут сигналы о невозможности
продолжения своей работы.
6.2. Вывести содержимое массива на консоль.
6.3. Запросить с консоли порядковый номер потока marker, которому будет подан
сигнал на завершение своей работы.
6.4. Подать потоку marker, номер которого получен в пункте 6.3, сигнал на
завершение работы.
6.5. Ждать завершение работы потока marker, которому был подан сигнал на
завершение работы в пункте 6.4.
6.6. Вывести содержимое массива на консоль.
6.7. Подать сигнал на продолжение работы, оставшимся потокам marker.
7. Завершить свою работу после завершения работы всех потоков marker.
Поток marker должен выполнять следующие действия:
1. Начать работу по сигналу от потока main.
2. Инициализировать генерацию последовательности случайных чисел. Для этого
использовать функцию srand, которой передать в качестве аргумента свой
порядковый номер.
3. Работать циклически, выполняя на каждом цикле следующие действия:
3.1. Генерировать случайное число, используя функцию rand.
3.2. Разделить это число по модулю на размерность массива.
3.3. Если элемент массива, индекс которого равен результату деления, равен нулю, то
выполнить следующие действия:
3.3.1. Поспать 5 миллисекунд.
3.3.2. Занести в элемент, индекс которого вычислен, свой порядковый номер.
3.3.3. Поспать 5 миллисекунд.
3.3.4. Продолжить исполнение цикла 3.
3.4. В противном случае:
3.4.1. Вывести на консоль следующую информацию:
- свой порядковый номер;
- количество помеченных элементов;
- индекс элемента массива, который невозможно пометить.
3.4.2. Дать сигнал потоку main на невозможность продолжения своей работы.
3.4.3. Ждать ответный сигнал на продолжение или завершение работы от потока
main.

4. Если получен сигнал на завершение работы, то выполнить следующие действия:
4.1. Заполнить нулями в массиве все элементы, которые он пометил.
4.2. Завершить работу.
5. Если получен сигнал на продолжение работы, то продолжить исполнение цикла из
пункта 3.
