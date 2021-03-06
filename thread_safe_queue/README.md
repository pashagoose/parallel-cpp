## Потокобезопасная очередь

В данной задаче необходимо написать обертку над очередью стандартной библиотеки `std::queue`. Потокобезопасная очередь часто необходима, когда существуют несколько потоков, создающих объекты (называются producers), и несколько потоков, обрабатывающих эти объекты (называюстя consumers).  

В вашей реализации метод `Pop` должен быть блокирующим, то есть дожидаться объекта в очереди и извлекать его. А метод `TryPop` неблокирующим, то есть моментально возвращать объект, если он там был, а иначе возвращать `std::nullopt`.  

---

В этой задаче вам может пригодится [`std::condition_variable`](https://en.cppreference.com/w/cpp/thread/condition_variable).
