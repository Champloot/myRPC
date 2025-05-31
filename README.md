# myRPC - Remote Procedure Call System

## Описание

myRPC - это система удаленного выполнения команд, позволяющая клиентам выполнять bash-команды на сервере с проверкой прав доступа и журналированием всех операций.

## Основные возможности

- Удаленное выполнение bash-команд
- Аутентификация пользователей через конфигурационный файл
- Журналирование всех операций с помощью libmysyslog
- Поддержка двух типов сокетов: потоковые (TCP) и датаграмные (UDP)
- Генерация временных файлов для stdout/stderr команд
- Формирование deb-пакетов для простого развертывания

## Требования

- Linux-система
- gcc (версия 9.4.0 или выше)
- make
- fakeroot (для сборки deb-пакетов)
- systemd (для работы сервера как службы)

## Установка

### Сборка из исходников

```bash
git clone https://github.com/yourusername/myRPC.git
cd myRPC
make all
sudo make install
```

### Установка из deb-пакетов

1. Сначала соберите пакеты:
```bash
make deb
```

2. Затем установите их:
```bash
# На сервере
sudo dpkg -i deb/myrpc-server_1.0-1_amd64.deb
sudo dpkg -i deb/libmysyslog_1.0-1_amd64.deb

# На клиенте
sudo dpkg -i deb/myrpc-client_1.0-1_amd64.deb
sudo dpkg -i deb/libmysyslog_1.0-1_amd64.deb
```

## Настройка

### Серверная часть

Основные конфигурационные файлы расположены в `/etc/myRPC/`:

1. `/etc/myRPC/myRPC.conf` - настройки сервера:
```ini
port = 1234
socket_type = stream
```

2. `/etc/myRPC/users.conf` - список разрешенных пользователей (по одному на строку):
```text
kali
archik
other
```

### Запуск сервера

```bash
sudo systemctl daemon-reload
sudo systemctl enable myRPC
sudo systemctl start myRPC
```

## Использование клиента

### Синтаксис

```bash
myRPC-client [options]
```

### Опции

| Опция              | Описание                              |
|--------------------|---------------------------------------|
| -c, --command      | Команда для выполнения на сервере     |
| -h, --host         | IP-адрес сервера (по умолчанию: 127.0.0.1) |
| -p, --port         | Порт сервера (по умолчанию: 1234)     |
| -s, --stream       | Использовать потоковый сокет (TCP)    |
| -d, --dgram        | Использовать датаграмный сокет (UDP)  |
| --help             | Показать справку                      |

### Примеры

Выполнить команду `ls -la` на сервере:
```bash
myRPC-client -h 127.0.0.1 -p 1234 -s -c "ls -la"
```


## Журналирование

Сервер записывает логи в `/var/log/myRPC.log` в формате:
```
[Timestamp] [Log Level] [Driver] [Message]
```

Пример:
```
Mon Oct 10 15:30:45 2023 INFO 0 User admin executed command: ls -la
```

## Структура проекта

```
myRPC/
├── client/              # Клиентская часть
├── server/              # Серверная часть
├── common/              # Общие компоненты (libmysyslog)
├── config/              # Примеры конфигурационных файлов
├── deb/                 # Собранные deb-пакеты
├── Makefile             # Общий Makefile
└── README.md            # Этот файл
```
