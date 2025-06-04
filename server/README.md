# myRPC Server - README

## Описание

myRPC Server - серверная часть системы удаленного выполнения команд, принимающая запросы от клиентов и выполняющая указанные bash-команды после проверки прав доступа.

## Установка

### Из исходников

```bash
cd server/
make
sudo make install
```

### Из deb-пакета

```bash
sudo dpkg -i myrpc-server_1.0-1_amd64.deb
```

## Конфигурация

Основные файлы конфигурации:

1. `/etc/myRPC/myRPC.conf`:
```ini
port = 1234
socket_type = stream  # или "dgram" для UDP
```

2. `/etc/myRPC/users.conf` (список разрешенных пользователей):
```text
kali
archik
other
```

## Управление службой

Запуск и остановка сервера:

```bash
sudo systemctl start myRPC
sudo systemctl stop myRPC
sudo systemctl restart myRPC
```

Автозагрузка при старте системы:

```bash
sudo systemctl enable myRPC
```

Просмотр статуса:

```bash
sudo systemctl status myRPC
```

## Журналирование

Сервер записывает логи в:
1. `/var/log/myRPC.log` - основные события
2. `/tmp/myRPC_*.stdout` - stdout выполненных команд
3. `/tmp/myRPC_*.stderr` - stderr выполненных команд

Пример лога:
```
[Wed Nov 15 10:30:45 2023] [INFO] User admin executed: ls -la
[Wed Nov 15 10:31:12 2023] [WARN] Access denied for user test
```

## Безопасность

1. Сервер проверяет пользователей по white-листу
2. Все команды выполняются с правами пользователя, под которым работает сервер
3. Рекомендуется запускать сервер под отдельным непривилегированным пользователем

## Удаление

```bash
sudo apt remove myrpc-server
sudo rm -rf /etc/myRPC/
```
