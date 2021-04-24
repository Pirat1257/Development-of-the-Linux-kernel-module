# Development-of-the-Linux-kernel-module
Operating system security

Модуль ядра ОС Linux, имеющий функционал USB-sniffer.

Для перехвата USB траффика был использован модуль usbmon, являющийся частью Linux-ядра начиная с версии 2.6.11 и позволяющий осуществлять полный мониторинг обмена данными с USB-устройствами. Usbmon позволяет выбрать интерфейс для сниффинга usbmon0 или же usbmonN, где N – номер USB-шины, которую можно определить, например, утилитой lsusb.

Запуск usbmon производится при помощи команды modprobe, необходимой для подгрузки уже готовых модулей, включенных в дерево модулей текущей версии ядра.

Модуль ядра работает в kernel mode, следовательно, обычный fopen языка Си работать не будет, по причине того, что можно использовать только Linux API библиотеки, а наличие стандартных библиотек языка Си в коде приведёт к ошибкам компиляции модуля.

Суть реализованного модуля ядра заключается в сохранении USB-трафика в файл output.txt.

Полезные ссылки:
  - https://www.kernel.org/doc/html/v5.3/usb/usbmon.html;
  - https://github.com/MarcoBueno1/SmallSnif;
  - https://stackoverflow.com/questions/1184274/read-write-files-within-a-linux-kernel-module;
  - https://stackoverflow.com/questions/2888421/malloc-in-kernel;
