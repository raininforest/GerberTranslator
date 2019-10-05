# gt
Gerber translator. Written using the Qt 5.13 library. Cross-platform (windows / linux) multi-threaded application that generates images of PCB from Gerber-files to bmp/png formats. Automates the process of issuing documentation for printed circuit boards.
By default, it searches for the outline file (that filename contains ".board") of the printed circuit board and put it to all other images.
The image size is also calculated based on the outline, but can also be specified in the settings.
When exporting to png, it is possible to form a semitransparent image.

Gerber-транслятор. Написан с использованием библиотеки Qt 5.13. Кроссплатформенное (windows/linux) многопоточное приложение, которое формирует из Gerber-файлов изображения фотошаблонов печатной платы в форматах bmp/png. Автоматизирует процесс выпуска документации на печатные платы.
По умолчанию ищет файл контура печатной платы и размещает его на всех остальных фотошаблонах. 
Размер изображения также вычисляется на основе контура, но может задаваться и в настройках.
При экспорте в png есть возможность формирования полупрозрачного изображения.
