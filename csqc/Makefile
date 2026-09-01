# Сборка клиентского модуля TF2003 (csprogs.dat).
# Используется fteqcc (из fteqw/_buildsv или fteqcc64 из PATH).
#
#   make            — собрать csprogs.dat
#   make install    — собрать и скопировать в qwtf/fortress/
#   make clean      — удалить csprogs.dat и .lno

FTEQCC ?= fteqcc64
DAT     := csprogs.dat
QWTF    := ../../qwtf/fortress

.PHONY: all install clean

all:
	$(FTEQCC) ./csprogs.src

install: all
	cp $(DAT) $(QWTF)/$(DAT)

clean:
	rm -f $(DAT) csprogs.lno
