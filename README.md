# Индивидуальное домашнее задание по "АВС" на тему "Построение многопоточных приложений" (на оценку 6)
Печик Ирина Юрьевна, БПИ-217, Вариант-13
## Условие задачи о гостинице-2 (умные клиенты):
В гостинице 10 номеров с ценой 2000 рублей, 10 номеров с ценой 4000 рублей и 5 номеров с ценой 6000
руб. Клиент, зашедший в гостиницу, обладает некоторой суммой и получает номер по своим финансовым возможностям, если тот свободен. При наличии денег и разных по стоимости номеров он выбирает случайный номер. Если доступных по цене свободных номеров нет, клиент уходит
искать ночлег в другое место. Клиенты порождаются динамически и уничтожаются при освобождении номера или уходе из гостиницы при невозможности оплаты. Создать многопоточное приложение, моделирующее работу
гостиницы.

## Как пользоваться программой
Пользователь вводит количество гостей n, планирующих заехать в гостиницу
с вместимостью в 25 мест, и для каждого гостя вводит его бюджет
```
n > 0
```
# Решение на 4 балла:
### 1.Приведено условие задачи.
В гостиницу нужно заселить n человек, у каждого есть свой бюджет b. У гостиницы нет интернет сайта, поэтому цены и количество свободных номеров известны только на ресепшн (некоторые номера на момент заезда этих посетителей могут быть недоступны для заселения (там может уже кто-то жить или могут проводиться ремонтные работы).
Поэтому, пользователь вводит сколько человек ему нужно заселить в гостиницу и каков бюджет каждого посетителя. Затем постепенно эти люди начинают приходить в гостиницу с целью заселиться в неё. Узнав, что самый дешёвый номер стоит 2000 руб., некоторые посетители уходят, ввиду нехватки их бюджета. Однако для тех, кому денег хватает, на ресепшн ищется номер. Номера есть разной категории: 
* 1 категории (номера за 2000 руб)
* 2 категории (номера за 4000 руб)
* 3 категории (номера за 6000 руб)
Если у пользователя бюджет позволяет заселиться в несколько категорий, он заселяется в любую. Пользователи должны приехать в определённый час к открытию, а выехать, по правилам отеля, в 0:00 ночи.
### 2. Описана модель параллельных вычислений, используемая при разработке многопоточной программы.
Была выбрана модель __"Итеративный параллелизм"__. 
В данной задаче для каждого потока посетителя итеративно ищется номер, подходящий для него по его бюджету. Если номер подходит этому посетителю, то он ему назначается, и поиск других номеров для него заканчивается, при этом теперь в списке занятых номеров, этот номер занят, и для других посетителей теперь недоступен.
### 3. Описаны входные данные программы, включающие вариативные диапазоны, возможные при многократных запусках.
количество гостей >= 1
бюджет каждого гостя может быть (-inf; +inf), ведь гость может быть должен этому отелю (не оплатил ранее). Однако тольк при бюджете >= 2000, посетитель может заселиться в гостиницу.
### 4. Реализовано консольное приложение, решающее поставленную задачу с использованием одного варианта синхропримитивов.
### 5. Ввод данных в приложение реализован с консоли.

# Решение на 5 баллов:
### 1. В программу добавлены комментарии, поясняющие выполняемые действия и описание используемых переменных.
### 2. Приведен сценарий, описывающий одновременное поведение представленных в условии задания сущеностей в терминах предметной области.
Два класса:
* Visitor - гость гостиницы
* Rooms - номера гостиницы
Каждый гость представляет собой поток.
В определённый момент только один гость имеет доступ к номерам, то есть код
с логикой взаимодействия гостей и номеров блокируется мьютексом
(pthread_mutex_lock), после данного взаимодействия мьютекс снимается
(phtread_mutex_unlock), что означает что гость либо нашёл номер, либо съехал из номера,
либо подходящего ему номера нет, и он ушёл из отеля.
На данном моменте выводится в консоль состояние гостя
и результат поиска номера для него (если нашел-выводит описание номера),
если не нашёл - выодится информация о том, что гость ушёл, если съехал - сообщение о том, что он освободил номер и ушёл из отеля.
Это описывается в методе VisitorFunc.
# Решение на 6 баллов:
### 1. Описан обобщённый алгоритм:

* Пользователь вводит количество гостей n (>0), планирующих заехать в гостиницу
с вместимостью в 25 мест, и для каждого гостя вводит его бюджет.
* Программа генерирует данные о том, какие номера для заезда на данный момент свободны.
* Запускается 25 потоков с номерами.
   Программа распределяет n гостей по 25 комнатам в порядке очереди и
   учитывая условия распределения по номерам

   ##### Условие распределения по 25 номерам:
    - Если денег (<2000) => уходит из отеля
    - Если денег (>= 2000) && (< 4000) => в номера для 1 категории** 
    - Если денег (>=4000 && (< 6000)) => в номера для 2 категории**
    - Если денег (>= 6000) => в номера для 3 категории**

   #### Что такое номера чей-то категории?
	Номера для 1 категории - номера [1; 10]
	Номера для 2 категории - номера [1; 20]
	Номера для 3 категории - все номера [1; 25] (т.к. денег им хватает на номер любой ценовой категории)
  
 * После того, как все посетители выехали из отеля (отель закрывается после того, как данные пользователи выедут), потоки посетителей завершаются.
  
 ### 2. Реализован ввод данных из командной строки.
