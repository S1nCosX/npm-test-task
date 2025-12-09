# npm-test-task
Тестовое задание НДМ Систем

По умолчанию запросы принимаются на localhost:8080, адрес и порт можно изменить аргументами

Использование:
* Создание пакета
```
make package
```
* Просто билд
```
make build
```
* Собрать и оставить объекты
```
make all
```
* Изменить параметры запуска:
  /etc/systemd/system/ndm_test_popov.service
```
ExecStart=/usr/local/bin/ndm_test [localhost] [8080]
```
