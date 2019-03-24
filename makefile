project = socker
srcdir = src
builddir = build
src = $(wildcard $(srcdir)/*.c)
obj = $(src:.c=.o)

$(builddir)/$(project): $(obj)
	mkdir -p $(@D)
	$(CC) -o $@ -DAPP_NAME=$(project) $^
	rm $(srcdir)/*.o

.PHONY: clean
clean:
	rm -rf $(builddir)
