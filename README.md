# gt
Gerber-транслятор. Кроссплатформенное (windows/linux) многопоточное приложение, которое формирует из Gerber-файлов изображения фотошаблонов печатной платы в форматах bmp/png. Автоматизирует процесс выпуска документации на печатные платы.
По умолчанию ищет файл контура печатной платы и размещает его на всех остальных фотошаблонах. 
Размер изображения также вычисляется на основе контура, но может задаваться и в настройках.
При экспорте в png есть возможность формирования полупрозрачного изображения.

Приложение имеет свой апдейтер, который проверяет наличие обновлений при каждом запуске.
Для работы апдейтера gt_u необходимы 2 служебных файла в папке с программой:
1. path - файл содержит путь к хранилищу с актуальной версией. хранилище с
актуальной версией должно выглядеть, например, так ../windows/gerber_translator_32/
или ../linux/gerber_translator_64
2. gt_version - данный файл должен содержаться и в папке с программой пользователя,
и в папке с актуальной версией программы. пример содержимого:

version=1.6
platform=windows
bit=64
